// API.cpp : Defines the exported functions for the DLL application.
//
#include "api.h"
#include "apilinkadapt.h"

DFLOGFPTR LogCB = NULL;
LINK_STAT_CB_P   LinkStatCB = NULL;

//One to one with errors in LINKERR.H at time of this writing, just added leading STAT_
//to produce the corresponding API_STAT value
const API_STAT  LinkToAPIStat[LE_NUM_ERRORS] =
{
  STAT_OK, //LE_NO_ERROR
  STAT_LE_BAD_PARAM,
  STAT_LE_BAD_FUNCODE,
  STAT_LE_BAD_COMMAND,
  STAT_LE_BAD_FORMAT,
  STAT_LE_BAD_TABLE,
  STAT_LE_BAD_LT_PACKET_LEN,
  STAT_LE_BAD_LT_CALL,
  STAT_LE_BAD_SEND,
  STAT_LE_RESPONSE_TIMEOUT,
  STAT_LE_BAD_RESP,
  STAT_LE_BAD_RESP_LEN,
  STAT_LE_BAD_RESP_CS,
  STAT_LE_BAD_STATUS,
  STAT_LE_SLAVE_STATUS,
  STAT_LE_COMM_FAIL,
  STAT_LE_BAD_CMN_CHAN,
  STAT_LE_BAD_CHAN,
  STAT_LE_SHORT_PACKET,
  STAT_LE_BAD_CHECK,
  STAT_LE_BAD_RETURN,
  STAT_LE_BAD_PARAM_FLAG,
  STAT_LE_BAD_BLOCK,
  STAT_LE_BLOCK_SIZE,
  STAT_LE_BLOCK_DOWN,
  STAT_LE_BLOCK_UP,
  STAT_LE_FUN_RETURN,
  STAT_LE_UNEXP_RESP,
  STAT_LE_SEND_BYTE_TO,
  STAT_LE_BABBLE,
  STAT_LE_OVERRUN,
  STAT_LE_UNDERRUN,
  STAT_BAD_API_HANDLE,
  STAT_LE_UNKNOWN,
};


//---------------------------------------
//  FUNCTION:     RegisterLoggingCallback
//
//  DESCRIPTION:  Supplies pointer to function to call for logging from API to main program
//
//---------------------------------------
DECL_SPEC void CALL_CONV  RegisterLoggingCallback(DFLOGFPTR CallbackPointer)
{
	LogCB = CallbackPointer;
} //endof RegisterLoggingCallback

//---------------------------------------
//  FUNCTION:     RegisterLinkErrorCallback
//
//  DESCRIPTION:  Supplies pointer to function to call for logging from API to main program
//
//---------------------------------------

DECL_SPEC void CALL_CONV  RegisterLinkStatusCallback(LINK_STAT_CB_P CallbackPointer)
{
    LinkStatCB = CallbackPointer;
} //endof RegisterLinkStatusCallback


DECL_SPEC bool CALL_CONV Disconnect(API_DEVICE_HANDLE Handle)
{
	if(CloseAPIHandle(Handle))//First checks if it is a valid connection handle
	{
		return true;
	}
	return false;
}

DECL_SPEC bool CALL_CONV SerialConnect(API_DEVICE_HANDLE &Handle, U8 CommIndex)
{
	//check for room in handle table, if so try connect. If successful
	//enter connection in table and return true, handle returned in Handle.
	if(ConnectToSerial(Handle, CommIndex))
	{
		return true;
	}
	return false;
}


//Just to test block transmission of various length blocks up to about 300 chars.
//For communication with a PQ10 ESP32 which is set up to echo any block received.
//In loop/echo mode, the ESP32 looks at serial receive every msec. When something
//is in  receive buffer  and no new chars are received in 3 msec, the
//whole block is read and echoed back on serial. This function generates
//a block of incremental characters of length Count with starting value
//of StartValue. It sends it out, then waits 10 msec and reads whatever comes back.
//Ideally, this is the same block that was just sent out. If it matches,
//the function returns true.
DECL_SPEC bool CALL_CONV PQ10APILoopTest(API_DEVICE_HANDLE Handle, U8 StartValue, U16 Count)
{
	int ReceiveCount;
	U8 CharVal;
	U8 TestBuff[512], RefBuff[512];
	if(Count > 500)
	{	//we are only allowing up to 512 char blocks
        return false;
	}
	CharVal = StartValue;
	for(int i = 0; i < Count; ++i)
	{
        TestBuff[i] = RefBuff[i] = CharVal++;
	}


	ReceiveCount = SerialLoopTransact(Handle, TestBuff, Count);
	if(ReceiveCount != Count)
	{
	    return false;
	}

	for(int i = 0; i < Count; ++i)
	{
        if(TestBuff[i] != RefBuff[i])
        {
            return false;

        }
	}
    return true;
}


DECL_SPEC U32 CALL_CONV GetMaxLinkReturnSize(API_DEVICE_HANDLE Handle)
{
	U32 Size;
	if( GetMaxLReturnSize(Handle, Size) )
	{
		return Size;
	}
	return 0;
}

DECL_SPEC U32 CALL_CONV GetLinkMaxSendSize(API_DEVICE_HANDLE Handle)
{
	U32 Size;
	if( GetMaxLSendSize(Handle, Size) )
	{
		return Size;
	}
	return 0;
}


U32  PunFloatToLong(float Float)
{
	U32 *LP;
	float *FP;
	FP =  &Float;
	LP = (U32*)FP;
	return *LP;
}

float PunLongToFloat(U32 Long)
{
	U32 *LP;
	float *FP;
	LP = &Long;
	FP = (float *)LP;
	return *FP;
}
