#ifndef __DEBUGACTIONS_H
#define __DEBUGACTIONS_H

// Include
//
#include "ZwBase.h"

// Functions
//
void DBGACT_ToggleExtLED();
void DBACT_SelfTestCurrent();
void DBACT_SelfTestChannel_IGTU();
void DBACT_SwitchMuxToIGTU();
void DBACT_SwitchMuxToLCTU();
void DBACT_SwitchSyncToIGTU();
void DBACT_SwitchSyncToLCTU();
void DBACT_SetStateCurrentDivider();
void DBACT_GenerateSyncToKeithley();
void DBACT_SendDataToKeithley();

#endif //__DEBUGACTIONS_H
