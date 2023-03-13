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
//
Int16S SyncCounter = 0;
bool LastSyncFromIGTU = false;

// Functions prototypes
//
bool INT_CheckSyncFromLCTU();
bool INT_CheckSyncFromIGTU();
bool INT_CheckSyncToLCTU();
bool INT_CheckSyncToIGTU();
void INT_SyncInterruptProcess();
void INT_LCTUsyncProcess(bool FromLCTU, bool ToLCTU);
void INT_IGTUsyncProcess(bool FromIGTU, bool ToIGTU);

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
		KEI_ReceiveData(USART2);
	
	ZwSCI_RecieveFlagClear(USART2);
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
		
		CONTROL_HandleExternalLamp();
		
		TIM_StatusClear(TIM7);
	}
}
//-----------------------------------------

void EXTI4_IRQHandler()
{
	INT_SyncInterruptProcess();
}
//-----------------------------------------

void EXTI9_5_IRQHandler()
{
	INT_SyncInterruptProcess();
}
//-----------------------------------------

void INT_SyncInterruptProcess()
{
	bool IntFlag_fromLCTU = INT_CheckSyncFromLCTU();
	bool IntFlag_fromIGTU = INT_CheckSyncFromIGTU();
	bool IntFlag_toLCTU = INT_CheckSyncToLCTU();
	bool IntFlag_toIGTU = INT_CheckSyncToIGTU();

	if(DataTable[REG_CHANNEL] == CHANNEL_LCTU)
		INT_LCTUsyncProcess(IntFlag_fromLCTU, IntFlag_toLCTU);
	else
		INT_IGTUsyncProcess(IntFlag_fromIGTU, IntFlag_toIGTU);
}
//-----------------------------------------

void INT_LCTUsyncProcess(bool FromLCTU, bool ToLCTU)
{
	if(FromLCTU)
	{
		if(CONTROL_State == DS_ConfigReady)
			CONTROL_SetDeviceState(DS_InProcess, SS_Measurement);

		CONTROL_TimeoutCounter = CONTROL_TimeCounter + KEI_MEASURE_TIMEOUT;
	}
	
	if(ToLCTU && CONTROL_SubState == SS_Measurement)
		CONTROL_SetDeviceState(DS_InProcess, SS_SaveResults);
}
//-----------------------------------------

void INT_IGTUsyncProcess(bool FromIGTU, bool ToIGTU)
{
	if(FromIGTU)
	{
		if(CONTROL_State == DS_ConfigReady)
			CONTROL_SetDeviceState(DS_InProcess, SS_Measurement);

		LastSyncFromIGTU = true;
		CONTROL_TimeoutCounter = CONTROL_TimeCounter + KEI_MEASURE_TIMEOUT;
	}

	if(ToIGTU && CONTROL_SubState == SS_Measurement)
	{
		if(--SyncCounter <= 0)
			CONTROL_SetDeviceState(DS_InProcess, SS_SaveResults);

		LastSyncFromIGTU = false;
	}

	INT_ResetFlags();
}
//-----------------------------------------

bool INT_CheckSyncFromLCTU()
{
	FlagSyncFromLCTU |= LL_CheckSyncFromLCTU();
	return (DataTable[REG_CHANNEL] == CHANNEL_LCTU) && FlagSyncFromLCTU;
}
//-----------------------------------------

bool INT_CheckSyncFromIGTU()
{
	FlagSyncFromIGTU |= LL_CheckSyncFromIGTU();
	return (DataTable[REG_CHANNEL] == CHANNEL_IGTU) && FlagSyncFromIGTU;
}
//-----------------------------------------

bool INT_CheckSyncToLCTU()
{
	FlagSyncToLCTU |= LL_CheckSyncToLCTU();
	return (DataTable[REG_CHANNEL] == CHANNEL_LCTU) && FlagSyncToLCTU;
}
//-----------------------------------------

bool INT_CheckSyncToIGTU()
{
	FlagSyncToIGTU |= LL_CheckSyncToIGTU();
	return (DataTable[REG_CHANNEL] == CHANNEL_IGTU) && FlagSyncToIGTU;
}
//-----------------------------------------

void INT_ResetFlags()
{
	FlagSyncFromLCTU = false;
	FlagSyncFromIGTU = false;
	FlagSyncToLCTU = false;
	FlagSyncToIGTU = false;
}
//-----------------------------------------
