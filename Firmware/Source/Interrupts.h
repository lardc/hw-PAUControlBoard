#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

// Includes
//
#include "stdinc.h"

// Variables
//
extern volatile bool FlagSyncFromLCTU;
extern volatile bool FlagSyncFromIGTU;
extern volatile bool FlagSyncToLCTU;
extern volatile bool FlagSyncToIGTU;
//
extern Int16S SyncCounter;

// Functions
//
void INT_ResetFlags();

#endif // __INTERRUPTS_H
