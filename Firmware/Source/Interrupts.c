// Include
#include "Interrupts.h"
//
#include "Controller.h"
#include "LowLevel.h"
#include "Board.h"
#include "SysConfig.h"
#include "Global.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "Keithley6485.h"

// Variables
//
bool FlagSyncFromLCTU = false;
bool FlagSyncFromIGTU = false;
bool FlagSyncToLCTU = false;
bool FlagSyncToIGTU = false;

// Functions
//
void USART1_IRQHandler()
{
	if(ZwSCI_RecieveCheck(USART1))
	{
		ZwSCI_RegisterToFIFO(USART1);
		ZwSCI_RecieveFlagClear(USART1);
	}
}
//-----------------------------------------

void USART2_IRQHandler()
{
	if(ZwSCI_RecieveCheck(USART2))
	{
		KEI_ReceiveData(USART2);
		ZwSCI_RecieveFlagClear(USART2);
	}
}
//-----------------------------------------

void USB_LP_CAN_RX0_IRQHandler()
{
	if(NCAN_RecieveCheck())
	{
		NCAN_RecieveData();
		NCAN_RecieveFlagReset();
	}
}
//-----------------------------------------

void TIM7_IRQHandler()
{
	static uint16_t LED_BlinkTimeCounter = 0;

	if(TIM_StatusCheck(TIM7))
	{
		CONTROL_TimeCounter++;
		if(++LED_BlinkTimeCounter > TIME_LED_BLINK)
		{
			LL_ToggleBoardLED();
			LED_BlinkTimeCounter = 0;
		}

		TIM_StatusClear(TIM7);
	}
}
//-----------------------------------------

void EXTI9_5_IRQHandler()
{
	if(LL_CheckSyncFromLCTU())
		FlagSyncFromLCTU = true;

	if(LL_CheckSyncToLCTU())
		FlagSyncToLCTU = true;

	if(LL_CheckSyncFromIGTU())
		FlagSyncFromIGTU = true;

	if(LL_CheckSyncToIGTU())
		FlagSyncToIGTU = true;
}
//-----------------------------------------
