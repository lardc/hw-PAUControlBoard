// Header
#include "DebugActions.h"

// Include
//
#include "LowLevel.h"
#include "Board.h"
#include "Delay.h"
#include "Controller.h"
#include "DataTable.h"

// Functions
//
void DBGACT_ToggleExtLED()
{
	LL_SetStateExtLED(true);
	DELAY_MS(1000);
	LL_SetStateExtLED(false);
}
//-----------------------

void DBACT_SelfTestCurrent()
{
	LL_SetStateSelfTestCurrent(DataTable[REG_DBG]);
}
//-----------------------

void DBACT_SelfTestChannel_IGTU()
{
	LL_SelfTestChannel_IGTU(DataTable[REG_DBG]);
}
//-----------------------

void DBACT_SwitchMuxToIGTU()
{
	(DataTable[REG_DBG]) ? LL_SwitchMuxToIGTU() : LL_SwitchMuxToDefault();
}
//-----------------------

void DBACT_SwitchMuxToLCTU()
{
	(DataTable[REG_DBG]) ? LL_SwitchMuxToLCTU() : LL_SwitchMuxToDefault();
}
//-----------------------

void DBACT_SwitchSyncToIGTU()
{
	(DataTable[REG_DBG]) ? LL_SwitchSyncToIGTU() : LL_SwitchSyncOff();
}
//-----------------------

void DBACT_SwitchSyncToLCTU()
{
	(DataTable[REG_DBG]) ? LL_SwitchSyncToLCTU() : LL_SwitchSyncOff();
}
//-----------------------

void DBACT_SetStateCurrentDivider()
{
	LL_SetStateCurrentDivider(DataTable[REG_DBG]);
}
//-----------------------

void DBACT_GenerateSyncToKeithley()
{
	LL_GenerateSyncToKeithley();
}
//-----------------------

void DBACT_SendDataToKeithley()
{
	for(int i = 0; i < MEMBUF_ValuesWrite_Counter; i++)
		LL_SendByteToKeithley(MEMBUF_Values_Write[i]);
	
	MEMBUF_ValuesWrite_Counter = 0;
}
//-----------------------

void DBACT_KeithleyPower()
{
	LL_KeithleyPowered(DataTable[REG_DBG]);
}
//-----------------------
