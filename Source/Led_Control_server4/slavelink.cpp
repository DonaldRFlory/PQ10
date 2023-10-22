// ..\..\Output\LCS_arduino\slavelink.cpp**************************************
// This file is machine generated from the link definition file
// ..\..\Output\LCS_arduino\slvmainll.h by LDFUTIL V2_022
// Processed in Mode: 0 - Standard stubs Mode
// It should not be directly edited.

#include "slink.h"
#include "slvmainll.h"


struct LinkDef  SDef[6]=
{
	{(void (*)())MasterBlockDown, 1, 33, 4, 0, 0},
	{(void (*)())MasterBlockUp, 1, 4, 161, 0, 0},
	{(void (*)())GetSlaveParameter, 4, 1, 1, 0, 0},
	{(void (*)())SetSlaveParameter, 1, 1, 4, 0, 0},
	{(void (*)())TraceBufferCount, 2, 0, 0, 0, 0},
	{(void (*)())ReadTraceBuffer, 1, 161, 0, 0, 0}
};

int  LinkCount = 6;
