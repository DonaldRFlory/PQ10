//--------------------------------------------------------------------------
//                  Copyright (C)
//
//  MODULE :        LINKTRAN.C
//  Purpose:        Master link communication LinkTransact and
//					related functions. Adapter between packet
//					forming logic in MASTER.C and communication
//					channel functions (physical layer).
//--------------------------------------------------------------------------
#include <Windows.h>
#include "mlink.h"
#include "apilinkadapt.h"
#include "mastadapt.h"

#include <limits.h>
#define INIT_CRC  		0  /* CCITT: 0xFFFF */
#define CRCPOLY  		0xA001  // ANSI CRC-16  CCITT: 0x8408

static bool CheckCRC(U8 * Buff, U32 CharCount);
static U8 GetSerialResponse(LINK_CTRL & LCtrl);
static U32 SlowTransactMillisec = 0;

static LINK_STAT LinkSend(LINK_CTRL &LCtrl);
static LINK_STAT LinkGetResponse(LINK_CTRL &LCtrl);

//------------------------------------------------------------------------------
//  FUNCTION:     MakeCRCTable
//
//  DESCRIPTION:  This Function is used to make CRC Table
//
//  PARAMETERS: None
//
//  RETURNS:    None
//
//  Notes:
//------------------------------------------------------------------------------
static U16 CRCTable[UCHAR_MAX + 1];    //Holds Value of CRCTable
static void MakeCRCTable(void)
{
  	static short CRCTabInited = FALSE;
	unsigned short i, j, r;

  	if(CRCTabInited)
    	return;
  	CRCTabInited = TRUE;

  	for (i = 0; i <= UCHAR_MAX; i++)
  	{
		r = i;
		for (j = 0; j < CHAR_BIT; j++)
		{
      		if (r & 1)
      		{
        		r = (r >> 1) ^ CRCPOLY;
      		}
      		else
      		{
        		r >>= 1;
      		}
    	}
    	CRCTable[i] = r;
	}
}



//Compute complete CRC on a buffer in one call.
U16 ComputeCRC(U8 * Buff, U32 CharCount)
{
	U32 u;
	U16 CRC;
	MakeCRCTable();
	CRC = INIT_CRC;
	for(u=0; u < CharCount; ++u)
	{
		CRC = CRCTable[(CRC ^ (Buff[u] & 0xFF)) & 0xFF] ^ (CRC >> CHAR_BIT);
	}
	return CRC;
}


void SetSlowTransact(U32 Milliseconds)
{
	SlowTransactMillisec = Milliseconds;
}

//true return means that control byte indicates successful function
//call at slave end
static bool GoodSlaveStatus(UCHAR CtrlByte)
{
	//TODO: flesh this out according to final slave design
	return true;
}



typedef enum
{
	RTN_0 = 0,
	RTN_1,
	RTN_2,
	RTN_3,
	RTN_4,
	RTN_5,
	RTN_6,
	RTN_7,
	RTN_8,
	RTN_9
} RETURN_ENUM;

#define FLUSH_BUFF_SIZE 2000
static U8 FlushBuff[FLUSH_BUFF_SIZE];
//This version is for communication with ESP32.
//Using CRC in this implementation and one byte LengthByte.
//On entry, LCtrl contains the raw packet, starting with at
//least FIdx, and 0-255 parameter and/or down-bytes.
LINK_STAT SerialTransact(LINK_CTRL &LCtrl)
{
	ULONG BytesWritten;
	U16 CRC;
	LINK_STAT Stat;
	U16 PayloadLength = (U16)(LCtrl.NextIndex - LCtrl.StartIndex);
	int WriteCount;

	Stat.FIdx = LCtrl.FIdx;
	if(LCtrl.FIdx > MAX_FIDX)
	{
		Stat.Stat = LE_BAD_LT_CALL_D;
		return Stat;
	}

	if((PayloadLength  < 1) || (PayloadLength > GetMaxLinkSendSize(LCtrl.LSel)))
	{
		Stat.Stat = LE_BAD_LT_CALL_D;
		return Stat;
	}
	//length byte doesn't count FIdx as it must always be there, only variable bytes in LB count
	LCtrl.Buffer[--LCtrl.StartIndex] = (U8)(PayloadLength - 1); //tuck length_byte in before start of packet

	CRC = ComputeCRC(&(LCtrl.Buffer[LCtrl.StartIndex]), PayloadLength + 1);//computed on LB + raw packet
	LCtrl.Buffer[LCtrl.NextIndex++] = (U8)(CRC >> 8); //as every multi-byte val, send big-endian
	LCtrl.Buffer[LCtrl.NextIndex++] = (U8)CRC;		  //and place it after raw packet
	FlushRead(LCtrl.LSel.LHand, FlushBuff, FLUSH_BUFF_SIZE);

	WriteCount = PayloadLength + 3; //Length_Byte, 2 CRC_BYTES

	WriteFile(LCtrl.LSel.LHand, &LCtrl.Buffer[LCtrl.StartIndex], WriteCount, &BytesWritten, NULL);
	if(BytesWritten  != WriteCount)
	{
		Stat.Stat = LE_BAD_SEND_D;
		return Stat;
	}

	Sleep(10);

	Stat.Stat = GetSerialResponse(LCtrl);
	return Stat;
}


//This was written to test ability of serial link to handle various sizes
//of packet transmission with back to back characters.
int SerialLoopTransact(LINK_HANDLE Handle, U8 * Buffer, U16 Count)
{
    U32 BytesWritten, BytesRead;

    if(Count > FLUSH_BUFF_SIZE)
    {
        return -1;
    }

    FlushRead(Handle, FlushBuff, FLUSH_BUFF_SIZE);

    WriteFile(Handle, Buffer, Count, &BytesWritten, NULL);
    if(BytesWritten != Count)
    {
        return -1;
    }

    Sleep(40);
    for(int i = 0; i < Count; ++i)
    {
        Buffer[i] = 0;
    }
    ReadFile(Handle, Buffer, Count, &BytesRead, NULL);

      	return BytesRead;
}

//So we are going to get response into LCtrl buffer at beginning
//We will then check length byte or bytes and set StartIndex and NextIndex
//to bracket the raw packet. We will also validate the returned status
// as well as length byte against the following CRC.
//---
//So it slave detects an error, it may send back an error status which
//may be shorter than what we expect from the function we are calling.
//We should probe short response packets to see if they are indeed
//properly formed error responses and pass the error code back.
//So for an error return, we expect:

//1) length byte
//2) Status byte
//3) CRC_BYTE1
//4) CRC_BYTE2
//for a total of 4 bytes and a length byte of one.
//In addition, the status byte must be non-zero, and the length-byte
//value should be 1. In addition, we should have read exactly four bytes.
static L_STAT GetSerialResponse(LINK_CTRL & LCtrl)
{
	U8 Status;
	U16 LenVal, ReadLength, PayloadLength;
	ULONG  BytesRead;

	PayloadLength = (U16)LCtrl.RtnSize + 1;//status byte, raw return value
	ReadLength = PayloadLength + 3; //len-byte , 2-byte CRC

	ReadFile(LCtrl.LSel.LHand, LCtrl.Buffer, ReadLength, &BytesRead, NULL);
	if(BytesRead != ReadLength)
	{
		if(BytesRead == 4)
		{
		    //It might be a valid error status packet
		    if((LCtrl.Buffer[0] == 1) && (LCtrl.Buffer[1] != LE_NO_ERROR))
		    {//Looks like legit error packet
		        if(CheckCRC(LCtrl.Buffer, 2))
		        { //and CRC ok
		            return LCtrl.Buffer[1]; //return the error code.
		        }
	        }
		}
		return LE_SHORT_PACKET_RD;
	}
	LCtrl.StartIndex = 0; //we are putting things right at beginning of LCtrl Buffer
	LCtrl.NextIndex = ReadLength;
	LenVal = ((U16)LCtrl.Buffer[0]) + 1;
	if(PayloadLength != LenVal)
	{
		return LE_BAD_RETURN;
	}
	LCtrl.StartIndex = 1;//they are not expecting length byte
	Status = LCtrl.Buffer[LCtrl.StartIndex++];//nor status byte
	if(CheckCRC(LCtrl.Buffer, PayloadLength + 1))
	{
	    return Status;
	}
	return LE_BAD_CHECK_D;
 }

//So we want a function to check CRC of Buffer
//CRC bytes are in Buff in big-endian order after CharCount packet characters
static bool CheckCRC(U8 * Buff, U32 CharCount)
{
    U16 ReceivedCRC;
    ReceivedCRC = Buff[CharCount];
    ReceivedCRC <<= 8;
    ReceivedCRC |= Buff[CharCount + 1];
    return (ReceivedCRC == ComputeCRC(Buff, CharCount));
}
