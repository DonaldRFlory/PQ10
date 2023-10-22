//*****************************************************************************
//	linkadapt.cpp
//
// This module adapts generic slave link functions to use multiple
// physical links.
//
//*****************************************************************************
#include <arduino.h>
#include "type.h"
#include "link.h"
#include "slink.h"
#include "linkadapt.h"
#include "linkctrl.h"

void TraceBytes(U8 * Ptr, int Count);
uint16_t ComputeCRC(uint8_t * Buff, uint32_t CharCount);
U8 LinkBuffer[LINK_BUFF_SIZE];


extern const int8_t TELL_TALE_1;
extern const int8_t TELL_TALE_2;
extern const int8_t TELL_TALE_3;

static U8 LinkErrorCode = LE_NO_ERROR;
static int8_t IdleCount;

void PostLogicError(U16 ErrorIdentifier)
{
    //TODO: flesh this out
}


//This is designed to be called in background once per millisecond.
bool DoLinkReceive()
{
    U16 CRC, CompCRC;
    static int MaxRcvIndex, RcvIndex;

    if(LinkErrorCode != LE_NO_ERROR)
    {
        if(Serial.available() != 0)
        {
            IdleCount = 0;
            while(Serial.available() != 0)
            {
                Serial.read();//flush incoming data til we see a gap of several msec
            }
        }
        else if(++IdleCount > MAX_IDLE_COUNT)
        {//have timed out interchar spacing so flushing of incoming bad packet is complete
            PostLinkResult(0, LinkErrorCode, 0);
            LinkErrorCode = LE_NO_ERROR;
        }
        return false;
    }

    int CurAvail = Serial.available();
    if(RcvIndex)
    {
        //we are actively receiving a packet
        if(CurAvail)
        {
            IdleCount = 0;
            while(Serial.available() != 0)
            {
                LinkBuffer[RcvIndex++] = Serial.read();
                if(RcvIndex > MaxRcvIndex)
                { //we have the whole expected packet
                    RcvIndex = 0;
                    CompCRC = ComputeCRC(LinkBuffer, MaxRcvIndex - 1);
                    CRC = LinkBuffer[MaxRcvIndex - 1];
                    CRC <<= 8;
                    CRC |= LinkBuffer[MaxRcvIndex];
                    if(CRC == CompCRC)
                    {//have a valid packet so tell them to execute it
                        return true;
                    }
                    else
                    {
                        LinkErrorCode = LE_BAD_CRC_CD;
                    }
                }
            }
        }
        else
        {
            ++IdleCount;

        }
        if(IdleCount >= MAX_IDLE_COUNT)
        {//nothing has arrived in 3 msec so packet is complete
         //but not long enough, so generate error
            RcvIndex = 0;
            LinkErrorCode = LE_BAD_FORMAT;
            //and intentionally no resetting IdleCount as we have already timed gap
        }
    }
    else
    {
        //We are not actively receiving a packet (RcvIndex == 0)
        if(CurAvail != 0)
        {
            //There are bytes in buffer so we are starting to receive a packet now
            LinkBuffer[RcvIndex++] = Serial.read(); //save length byte for CRC check
            MaxRcvIndex = ((int)LinkBuffer[0]) + 3;
        }
    }
    return false;
}


//Modify this to return number of physical links
int GetNumLinks(void)
{
    return 1;
}

//static U8 LinkIndex;//Set during LinkServe processing to indicate the link we are
	//servicing. It will allow us to possibly return function def from a different
	//LinkDef table for each link in GetFDef().

void SetLinkIndex(U8 Index)
{
//    LinkIndex = Index;
}


extern U8 LinkBuffer[];
static U8 LinkReturnStatus;

//This should be modified to initialize all implemented slave links
void InitSlaveLink(void)
{
}

U32 GetMaxLinkReturnSize(U8 Channel)
{
    return 0;//This should be setup based on physical transport
        //it is the maximum allowed size of raw link return packet
        //dictated by the physical layer buffering and packe tsize
}

//Multiple links may share the same linklist
//or they may have distinct linklists.
//Modify this to handle various LinkDef structures
extern int LinkCount;
extern struct LinkDef  SDef[];

U32 GetCurFDef(struct LinkDef **FDefP)
{
	*FDefP = &(SDef[0]);
	return (U32)LinkCount;
}


//Link processing functions use this to get pointers to command packet and raw response buffer
//They may point to the same buffer area as all command data is consumed before producing
//any response data
int GetLinkCommandPacket(U8 **CmdPtrAddress, U8 **RspPtrAddress, U32 *MaxReturnSizeAddress)
{
	int Length;
	*CmdPtrAddress = &(LinkBuffer[1]); //we put length byte then raw packet in LinkBuffer.
	                                    //So skip length byte and point to raw packet.
    *RspPtrAddress  = &(LinkBuffer[2]);//leave room for length byte and return status
    *MaxReturnSizeAddress = 255;//size of max raw return packet, the physical scheme with length-byte prepended
                                //allows 256 bytes of payload. In addition we have 1 length byte plus 2 CRC bytes.
                                //But.. we always return a status byte leaving 255 for raw return.
                                //LinkBuffer must thus hold 256 + 3 = 259 bytes.
    Length = LinkBuffer[0];
    ++Length; //length byte is Length + 1 since minimum raw packet is 1 byte
    LinkBuffer[0] = 0;//clear length in command packet so we can see if a result gets posted
    return Length;
}

void PostLinkResult(U32 ByteCount, U8 ReturnStatus, U8 FIdx)
{
    //In this implementation, we are not returning FIdx along with response
    U16 CRC;

    LinkBuffer[0] = (U8)(ByteCount);//length byte is return byte_count -1 but we add Return Status which makes it correct
    LinkBuffer[1] = ReturnStatus;
    CRC = ComputeCRC(LinkBuffer, ByteCount + 2); //compute CRC on raw packet and bytecount plus status byte
    LinkBuffer[ByteCount + 2] = (U8)(CRC >> 8) ; //Big-endian as always
    LinkBuffer[ByteCount + 3] = (U8)(CRC) ; //Big-endian as always
    Serial.write(LinkBuffer, ByteCount + 4);
}
