//--------------------------------------------------------------------------*
//                Copyright (C) Don Flory 2022
//
//  MODULE :     apilog.cpp
//  Purpose:     Functions for API error logging
//  Created by:  Don Flory
//  Cr. Date:    2/7/2022
//--------------------------------------------------------------------------*/
#include "api.h"
#include <stdio.h>
#include <string.h>
#include "apilinkadapt.h"
#include "LnkMutex.h"  //for critical section functions

extern void(*LogCB)(char *, U32, BOOL);

void APILogMessage(U8 MessageType, const char *Message, bool ContinueEntry)//MessageType is used to allow filtering various categories
{                                                                          //of message or allow different handling of different types.
        APILogMessage((U8*)Message, ContinueEntry);
}


#define LOG_BUFFER_LENGTH 2000
bool LogBuffInited = false;
int LogBuffChars, LogInIndex, LogOutIndex;

static char LogBuffer[LOG_BUFFER_LENGTH], LogTransferBuffer[LOG_BUFFER_LENGTH+1];

//If Message fits in FIFO buffer, it is copied in completely
//and return is true. If it does not fit, nothing is copied
//and return is false.
bool LogMessage(char * Message, U32 Length)
{
    bool ReturnVal;

    EnterCritSection();
    if(!LogBuffInited)
    {
        LogBuffChars = 0;
//        LogBuffSpace = LOG_BUFFER_LENGTH;
        LogInIndex = LogOutIndex = 0;
        LogBuffInited = true;
    }
    if((int)Length > (LOG_BUFFER_LENGTH - LogBuffChars) )//if free space is not adequate
    {
        ReturnVal = false;
    }
    else
    {
        ReturnVal = true;
        LogBuffChars += Length;
        while(Length)
        {
            LogBuffer[LogInIndex ++] = *Message++;
            if(LogInIndex >= LOG_BUFFER_LENGTH)
            {
                LogInIndex = 0;
            }
            --Length;
        }
    }
    LeaveCritSection();
    return ReturnVal;
}

static void PrintToFile(char * String, char const * Path)
{
	static FILE *FHand;
	char fullFileName[250];
	fullFileName[0] = 0;
	strncpy (fullFileName, DATA_LOG_PATH, 249);
	fullFileName[249] = 0;
	strcat(fullFileName, Path);

	if ((FHand = fopen(fullFileName, "a+")) != NULL)
	{
		fputs(String, FHand);
		fclose(FHand);
		return;
	}
	return;
}

void LoggingServe()
{
    int i = 0;

    EnterCritSection();
    while(LogBuffChars)
    {
        LogTransferBuffer[i++] = LogBuffer[LogOutIndex++];
        LogOutIndex = (LogOutIndex >= LOG_BUFFER_LENGTH) ? 0 : LogOutIndex;
        --LogBuffChars;
    }
    LeaveCritSection();

    LogTransferBuffer[i] = 0;//null terminate it, (transfer buffer is one byte longer than LogBuffer)
    if(i != 0)
    {
        if(LogCB != NULL)
        {
	        (LogCB)(LogTransferBuffer,  i, TRUE);
        }
        //DFDEBUG  for now we are going to write all log messages to local debug log
        PrintToFile(LogTransferBuffer, "PQ10APIDbg.log");
    }

}

//These are the manually generated DLL exports and helper functions. See also ExpLink.cpp.
//
//=============================================================
//		EXPORTED FUNCTIONS
//=============================================================
#ifdef __cplusplus
extern "C" {
#endif
//---------------------------------------
//  FUNCTION:     APILogMessage
//
//  DESCRIPTION:  Write a message to the API log
//
//  PARAMETERS:   	char* Message: string to be logged
//  				bool ContinueEntry: true, continue previous message, false,
// 										start a new message. Use up to logging function.
//  RETURNS:	void
//
//---------------------------------------
void CALL_CONV APILogMessage(U8 MessageType, U8 *Message, BOOL ContinueEntry)
{
	int Length = strlen((char*)Message) & 0XFFFF;

	LogMessage((char*)Message, Length);
}


#ifdef __cplusplus
}	//extern "C"

#endif
