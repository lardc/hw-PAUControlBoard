// Include
#include "Interrupts.h"
//
#include "LowLevel.h"
#include "Keithley6485.h"
#include "Constraints.h"

// Variables
//
bool FlagSyncFromLCTU = false;
bool FlagSyncFromIGTU = false;
bool FlagSyncToLCTU = false;
bool FlagSyncToIGTU = false;


// Functions prototypes
//
bool INT_CheckSyncFromLCTU();
bool INT_CheckSyncFromIGTU();
bool INT_CheckSyncToLCTU();
bool INT_CheckSyncToIGTU();


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
	if(CONTROL_State == DS_ConfigReady && (INT_CheckSyncFromLCTU() || INT_CheckSyncFromIGTU()))
	{
		CONTROL_TimeoutCounter = CONTROL_TimeCounter + DataTable[REG_KEI_MEASURE_TIMEOUT];
		CONTROL_SetDeviceState(DS_InProcess, SS_Measurement);
	}


	if(INT_CheckSyncToLCTU() || INT_CheckSyncToIGTU())
		CONTROL_SetDeviceState(DS_InProcess, SS_SaveResults);
}
//-----------------------------------------

bool INT_CheckSyncFromLCTU()
{
	if(LL_CheckSyncFromLCTU() && DataTable[REG_CHANNEL] == CHANNEL_LCTU)
		FlagSyncFromLCTU = true;
	else
		FlagSyncFromLCTU = false;

	return FlagSyncFromLCTU;
}
//-----------------------------------------

bool INT_CheckSyncFromIGTU()
{
	if(LL_CheckSyncFromIGTU() && DataTable[REG_CHANNEL] == CHANNEL_IGTU)
		FlagSyncFromIGTU = true;
	else
		FlagSyncFromIGTU = false;

	return FlagSyncFromIGTU;
}
//-----------------------------------------

bool INT_CheckSyncToLCTU()
{
	if(LL_CheckSyncToLCTU() && DataTable[REG_CHANNEL] == CHANNEL_LCTU)
		FlagSyncToLCTU = true;
	else
		FlagSyncToLCTU = false;

	return FlagSyncToLCTU;
}
//-----------------------------------------

bool INT_CheckSyncToIGTU()
{
	if(LL_CheckSyncToIGTU() && DataTable[REG_CHANNEL] == CHANNEL_IGTU)
		FlagSyncToIGTU = true;
	else
		FlagSyncToIGTU = false;

	return FlagSyncToIGTU;
}
//-----------------------------------------
