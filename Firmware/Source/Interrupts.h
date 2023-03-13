#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

// Includes
//
#include "stdinc.h"

// Variables
//
extern bool FlagSyncFromLCTU;
extern bool FlagSyncFromIGTU;
extern bool FlagSyncToLCTU;
extern bool FlagSyncToIGTU;
extern bool LastSyncFromIGTU;
//
extern Int16S SyncCounter;

// Functions
//
void INT_ResetFlags();

#endif // __INTERRUPTS_H
