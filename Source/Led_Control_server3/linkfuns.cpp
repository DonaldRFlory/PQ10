//#include "project.h"
#include <EEPROM.h>
#include <stdio.h>
#include "type.h"
#include "slvmainll.h"
#include "slink.h"
#include "slavparm.h"

#define TRACE_BUFF_LEN 1024
U8 TraceBuffer[TRACE_BUFF_LEN];
U16 TBInIndex; //where next incoming byte goes
U16 TBOutIndex; //where next byte read out comes from
U16 TBCount;//Characters in trace buffer. Can be used to distinguish full from empty
//Since both full and empty have TBInIndex == TBOutIndex

//Note: Writes will happily take any length string and overwrite the oldest stuff in trace
//buffer. Users needing to ensure data is read should check for space before writing.
void WriteTraceBuffer(U8* InBuff, U16 Count);
//This version is only for text and requires zero terminated string
void WriteTraceBuffer(U8* InBuff)
{
    int Len = strlen((char *)InBuff) + 1;//we write the null terminator also
    WriteTraceBuffer(InBuff, Len);
}


void WriteTraceBuffer(U8* InBuff, U16 Count)
{
    while(Count)
    {
        --Count;
        TraceBuffer[TBInIndex++] = *InBuff++;
        TBInIndex = (TBInIndex >= TRACE_BUFF_LEN) ? 0 : TBInIndex;
        if(TBCount < TRACE_BUFF_LEN)
        {
            ++TBCount;
        }
        else
        {
            ++TBOutIndex;//overwriting the oldest byte in FIFO
            TBOutIndex = (TBOutIndex >= TRACE_BUFF_LEN) ? 0 : TBOutIndex;
        }
    }
}

//DFDEBUG
//This is a link function implementing the Slave side of
//the host block transfer function:
//U8 ReadTraceBuffer(UP_PTR_U8 TraceValsPtr, U8 TraceValCount);
//If more bytes requested than are in buffer, returns what is available.
//Returns count of bytes actually sent.
U8 ReadTraceBuffer(U8 ByteCount)
{
    U8 ValidBytesSent = 0;
//so we send all bytes asked for if we have them, otherwise
// we pad with zeros until we have sent what they asked for
//but we return count of legit bytes.
    while(ByteCount)
    {
        if(TBCount)
        {
            ++ValidBytesSent;
            SendU8(TraceBuffer[TBOutIndex++]);
            TBOutIndex = (TBOutIndex >= TRACE_BUFF_LEN) ? 0 : TBOutIndex;
            --TBCount;
        }
        else
        {
            SendU8(0);
        }
        --ByteCount;
    }
    return ValidBytesSent;
}

U16 TraceBufferCount()
{
    return TBCount;
}

U16 TraceBufferFree()
{
    return TRACE_BUFF_LEN - TBCount;
}


U8 MasterBlockDown(U8 Count, U16 DestAddress)
{
    //Not implemented yet for ESP32
    return 0;
}

U8 MasterBlockUp(U16 SrcAddress, U8 Count)
{
    //Not implemented yet for ESP32
    return 0;

}

U32 GetSlaveParameter(U8 ParCode, U8 Index)
{
    //Not implemented yet for ESP32
    switch (ParCode)
    {
        default:
            return 0;
    }
}

U32 SetSlaveParameter(U8 ParCode, U16 Param1, U16 Param2)
{
    //Not implemented yet for ESP32
    switch (ParCode)
    {
        default:
            return 0;
    }
}
