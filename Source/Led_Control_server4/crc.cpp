// Load Wi-Fi library
#include <Arduino.h>
#include "type.h"
uint16_t ComputeCRC(U8 * Buff, U32 CharCount);

#define INIT_CRC  		0  // CCITT: 0xFFFF
#define CRCPOLY  		0xA001  // ANSI CRC-16
                                // CCITT: 0x8408

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
static void MakeCRCTable()
{
  	static bool CRCTabInited = false;
	unsigned short i, j, r;

  	if(CRCTabInited)
    	return;
  	CRCTabInited = true;

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


//For some as yet undiscovered reason, the compiler rejects U16, U8, and U32 in
//definition of Compute CRC() below !!???
//Compute complete CRC on a buffer.
uint16_t ComputeCRC(uint8_t * Buff, uint32_t CharCount)
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
