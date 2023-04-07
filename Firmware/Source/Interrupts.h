#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

// Includes
//
#include "stdinc.h"

struct SyncFlagsStruct
{
	bool FromIGTU;
	bool FromLCTU;
	bool ToIGTU;
	bool ToLCTU;
};

// Variables
//
extern struct SyncFlagsStruct SyncFlags;
//
extern Int16S SyncCounter;

// Functions
//
void INT_ResetFlags();

#endif // __INTERRUPTS_H
