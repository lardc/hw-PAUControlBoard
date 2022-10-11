#ifndef __LOWLEVEL_H
#define __LOWLEVEL_H

// Include
#include "Board.h"
#include "stdinc.h"


// Functions
//
void LL_ToggleBoardLED();
void LL_SetStateExtLED(bool State);
void LL_SetStateSelfTestCurrent(bool State);
void LL_SelfTestChannel_IGTU(bool State);
void LL_SwitchMuxToIGTU();
void LL_SwitchMuxToLCTU();
void LL_SwitchMuxToDefault();
void LL_SwitchSyncToIGTU();
void LL_SwitchSyncToLCTU();
void LL_SwitchSyncOff();
void LL_SetStateCurrentDivider(bool State);
void LL_GenerateSyncToKeithley();

#endif //__LOWLEVEL_H
