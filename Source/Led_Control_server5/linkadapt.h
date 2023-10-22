//*****************************************************************************
//	linkadapt.h
//
// This module adapts generic slave link functions to use multiple
// physical links.
//
//*****************************************************************************

//Max of raw return, that is return value plus Up Bytes if any
#define MAX_LINK_RETURN_SIZE 255

//Max of FIdx and param bytes and any BlockDown bytes (if any)
#define MAX_LINK_SEND_SIZE 256

#define LINK_BUFF_SIZE 300
#define MAX_IDLE_COUNT 3
bool DoLinkReceive();

