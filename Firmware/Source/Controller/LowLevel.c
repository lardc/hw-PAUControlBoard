﻿// Header
#include "LowLevel.h"
// Include
#include "Board.h"
#include "Delay.h"
#include "Global.h"
#include "DataTable.h"

// Functions
//
void LL_ToggleBoardLED()
{
	GPIO_Toggle(GPIO_LED);
}
//-----------------------------

void LL_SetStateExtLED(bool State)
{
	GPIO_SetState(GPIO_LED_EXT, State);
}
//-----------------------------

void LL_ToggleExtLED()
{
	GPIO_Toggle(GPIO_LED_EXT);
}
//-----------------------------

void LL_SetStateSelfTestCurrent(bool State)
{
	GPIO_SetState(GPIO_STST_CURRENT, State);
}
//-----------------------------

void LL_SelfTestChannel_IGTU(bool State)
{
	GPIO_SetState(GPIO_STST_IGTU_CH, State);
}
//-----------------------------

void LL_SwitchMuxToIGTU()
{
	GPIO_SetState(GPIO_LCTU_MUX, false);
	DELAY_MS(DELAY_COMMUTATION);
	GPIO_SetState(GPIO_IGTU_MUX, true);
}
//-----------------------------

void LL_SwitchMuxToLCTU()
{
	GPIO_SetState(GPIO_IGTU_MUX, false);
	DELAY_MS(DELAY_COMMUTATION);
	GPIO_SetState(GPIO_LCTU_MUX, true);
}
//-----------------------------

void LL_SwitchMuxToDefault()
{
	LL_SelfTestChannel_IGTU(false);
	LL_SetStateCurrentDivider(false);
	
	GPIO_SetState(GPIO_IGTU_MUX, false);
	GPIO_SetState(GPIO_LCTU_MUX, false);
}
//-----------------------------

void LL_SwitchSyncToIGTU()
{
	GPIO_SetState(GPIO_LCTU_SEL, true);
	GPIO_SetState(GPIO_IGTU_SI, true);
	GPIO_SetState(GPIO_IGTU_SEL, false);
	GPIO_SetState(GPIO_LCTU_SI, false);
}
//-----------------------------

void LL_SwitchSyncToLCTU()
{
	GPIO_SetState(GPIO_IGTU_SEL, true);
	GPIO_SetState(GPIO_LCTU_SI, true);
	GPIO_SetState(GPIO_LCTU_SEL, false);
	GPIO_SetState(GPIO_IGTU_SI, false);
}
//-----------------------------

void LL_SwitchSyncOff()
{
	GPIO_SetState(GPIO_LCTU_SEL, true);
	GPIO_SetState(GPIO_LCTU_SI, true);
	GPIO_SetState(GPIO_IGTU_SEL, true);
	GPIO_SetState(GPIO_IGTU_SI, true);
}
//-----------------------------

void LL_GenerateSyncToKeithley()
{
	GPIO_SetState(GPIO_LCTU_SI, false);
	GPIO_SetState(GPIO_IGTU_SI, false);
	DELAY_US(5);
	GPIO_SetState(GPIO_LCTU_SI, true);
	GPIO_SetState(GPIO_IGTU_SI, true);
}
//-----------------------------

bool LL_CheckSyncFromLCTU()
{
	return EXTI_FlagCheck(EXTI_5);
}
//-----------------------------

bool LL_CheckSyncFromIGTU()
{
	return EXTI_FlagCheck(EXTI_4);
}
//-----------------------------

bool LL_CheckSyncToLCTU()
{
	return EXTI_FlagCheck(EXTI_9);
}
//-----------------------------

bool LL_CheckSyncToIGTU()
{
	return EXTI_FlagCheck(EXTI_7);
}
//-----------------------------

void LL_SetStateCurrentDivider(bool State)
{
	GPIO_SetState(GPIO_IDIV2, State);
}
//-----------------------------

void LL_SendByteToKeithley(uint8_t Byte)
{
	ZwSCI_xSendChar(USART2, Byte);
}
//-----------------------------

void LL_KeithleyPowered(bool State)
{
	GPIO_SetState(GPIO_KEI_PWR, !State);
}
//-----------------------------
