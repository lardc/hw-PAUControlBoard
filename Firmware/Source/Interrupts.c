// Include
#include "Interrupts.h"
//
#include "LowLevel.h"
#include "Keithley6485.h"
#include "Constraints.h"

// Variables
//
struct SyncFlagsStruct SyncFlags;
//
Int16S SyncCounter = 0;

// Functions prototypes
//
void INT_SyncProcess();
void INT_LCTUsyncProcess();
void INT_IGTUsyncProcess();
void INT_ClearInterrupts();

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
	INT_SyncProcess();
}
//-----------------------------------------

void EXTI9_5_IRQHandler()
{
	INT_SyncProcess();
}
//-----------------------------------------

void INT_SyncProcess()
{
	SyncFlags.FromIGTU |= LL_CheckSyncFromIGTU();
	SyncFlags.FromLCTU |= LL_CheckSyncFromLCTU();
	SyncFlags.ToIGTU |= LL_CheckSyncToIGTU();
	SyncFlags.ToLCTU |= LL_CheckSyncToLCTU();

	INT_LCTUsyncProcess(SyncFlags);
	INT_IGTUsyncProcess(SyncFlags);

	INT_ClearInterrupts();
}
//-----------------------------------------

void INT_ClearInterrupts()
{
	EXTI_FlagReset(EXTI_4);
	EXTI_FlagReset(EXTI_5);
	EXTI_FlagReset(EXTI_7);
	EXTI_FlagReset(EXTI_9);
}
//-----------------------------------------

void INT_ResetFlags()
{
	SyncFlags.FromIGTU = false;
	SyncFlags.FromLCTU = false;
	SyncFlags.ToIGTU = false;
	SyncFlags.ToLCTU = false;
}
//-----------------------------------------

void INT_LCTUsyncProcess()
{
	if(DataTable[REG_CHANNEL] == CHANNEL_LCTU)
	{
		if(SyncFlags.FromLCTU)
		{
			if(CONTROL_State == DS_ConfigReady)
				CONTROL_SetDeviceState(DS_InProcess, SS_Measurement);

			CONTROL_TimeoutCounter = CONTROL_TimeCounter + KEI_MEASURE_TIMEOUT;
		}

		if(SyncFlags.ToLCTU && CONTROL_SubState == SS_Measurement)
			CONTROL_SetDeviceState(DS_InProcess, SS_SaveResults);
	}
}
//-----------------------------------------

void INT_IGTUsyncProcess()
{
	if(DataTable[REG_CHANNEL] == CHANNEL_IGTU)
	{
		if(SyncFlags.FromIGTU)
		{
			if(CONTROL_State == DS_ConfigReady)
				CONTROL_SetDeviceState(DS_InProcess, SS_Measurement);

			CONTROL_TimeoutCounter = CONTROL_TimeCounter + KEI_MEASURE_TIMEOUT;
		}

		if(SyncFlags.ToIGTU && CONTROL_SubState == SS_Measurement)
		{
			if(--SyncCounter <= 0)
				CONTROL_SetDeviceState(DS_InProcess, SS_SaveResults);
		}

		SyncFlags.FromIGTU = false;
		SyncFlags.ToIGTU = false;
	}
}
//-----------------------------------------
