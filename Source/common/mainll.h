//---------------------------------------------------------------------------
//
//      File: mainll.H
//
//      Purpose: This file is the master list of functions callable over a
//               link between Master and Slave processors
//               It is processed by the ldfutil program to
//               produce headers including function stubs and data tables
//               used by link communication programs at each processor.
//               This version for ESP32 PQ10
//
//      Note:    Use only C++ style comments in this file
//               due to simple minded ldfutil.cpp parser.
//
//
//---------------------------------------------------------------------------



//
// Legitimate functions based on slave function calling routine limitations:
// 1) Long, int, char, or void functions with zero to four parameters
//          of types any types which will pass as an integer, (char, short, int).
// 2) Any signed or unsigned variations on above.
//
// 3) Block tranfer. Each of the four parameters can and the host side consist of
//   two parameters in the host side stub, an UP_BLOCK_XX type or a DOWN_POINTER_XX type
//   followed by a count parameter. The XX represents U8, U16 or U32. Each pointer/count
//  pair counts as one parameter. These pairs may be mixed in master side functions
//	with standard arguments with the only restriction being that only four effective
//  arguments are allowed.
// There are no longer any built-in  built in link functions.
// linklist preprocessor program. Prototypes are in link.h
//

//Following three define turn on exporting of DLL functions in Modes 2 and 3 and 9 of LDFUtil. The idea
//is that only some of the functions in a linklist will be exported from the DLL and this allows
//turning exporting on and off during linklist processing. In this example we export everything
#define DRF_LDF_EXPORT 1	   //for benefit of LDFUTIL.EXE
//This define only applies to DLL exports
#define DRF_LDF_Prefix PQ10_API

#include <basicll.h>

//EEPROM.write(0, 9);
//EEPROM.commit(); to actually program data written with EEPROM.write
//512 bytes available.
//U8 ReadEEPROM(U16 Address);
//EEPROM.write(address, value);
//U8 ReadTraceBuffer(UP_PTR_U8 TraceValsPtr, U8 TraceValCount);
//U8 DataUpdate(DOWN_PTR_U16 USecsPtr, U8 USecCount, UP_PTR_U16 ADValsPtr, U8 ADValCount);
U16 TraceBufferCount();
U8 ReadTraceBuffer(UP_PTR_U8 TraceValsPtr, U8 TraceValCount);
