// Header
//
#include "SelfTest.h"
#include "DeviceObjectDictionary.h"
#include "Global.h"
#include "DataTable.h"

// Includes
//
#include "Board.h"
#include "LowLevel.h"
#include "Keithley6485.h"
#include "math.h"

// Variables
//
Int64U DelayCounter = 0;

// Functions prototypes
//
bool SELFTEST_TestCurrentCheck();
void SELFTEST_StopProcess();

// Functions
//
void SELFTEST_Process()
{
	float KEI_Data = 0;
	
	switch(CONTROL_SubState)
	{
		case ST_PowerUpWaiting:
			if(!DelayCounter)
				DelayCounter = CONTROL_TimeCounter + DELAY_POWER_UP;
			else if(CONTROL_TimeCounter >= DelayCounter)
				CONTROL_SetDeviceSubState(ST_Prepare);
			break;
			
		case ST_Prepare:
			KEI_SimpleConfig();
			LL_SwitchSyncToLCTU();
			LL_SwitchMuxToDefault();
			LL_SetStateSelfTestCurrent(true);
			
			DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_NONE;
			DelayCounter = CONTROL_TimeCounter + DELAY_KEI_CONFIG;
			
			CONTROL_SetDeviceSubState(ST_WaitingConfig);
			break;
			
		case ST_WaitingConfig:
			if(CONTROL_TimeCounter >= DelayCounter)
			{
				DelayCounter = 0;
				CONTROL_SetDeviceSubState(ST_CurrentCheck);
			}
			break;
			
		case ST_CurrentCheck:
			if(SELFTEST_TestCurrentCheck())
			{
				DelayCounter = CONTROL_TimeCounter + DELAY_KEI_CONFIG;
				CONTROL_SetDeviceSubState(ST_Keithley);
			}
			else
			{
				DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_FAIL;
				CONTROL_SwitchToFault(DF_ST_KEI_LINE_OPEN);
			}
			break;
			
		case ST_Keithley:
			if(KEI_Measure(&KEI_Data))
			{
				DelayCounter = 0;
				
				if(KEI_Data >= DataTable[REG_SFTST_MUX_OFF_THRE])
					CONTROL_SetDeviceSubState(ST_IGTU_ChannelCheck);
				else
				{
					DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_FAIL;
					CONTROL_SwitchToFault(DF_ST_KEI_WRONG_DATA);
				}
			}
			break;
			
		case ST_IGTU_ChannelCheck:
			LL_SwitchSyncToIGTU();
			LL_SwitchMuxToIGTU();
			LL_SelfTestChannel_IGTU(true);
			
			if(!DelayCounter)
				DelayCounter = CONTROL_TimeCounter + DELAY_COMMUTATION;
			
			if(CONTROL_TimeCounter >= DelayCounter)
			{
				if(KEI_Measure(&KEI_Data))
				{
					DelayCounter = 0;
					
					if(KEI_Data >= DataTable[REG_SFTST_MUX_IGTU_THRE])
						CONTROL_SwitchToFault(DF_ST_MUX_IGTU);
					else
						CONTROL_SetDeviceSubState(ST_LCTU_ChannelCheck);
				}
			}
			break;
			
		case ST_LCTU_ChannelCheck:
			LL_SwitchSyncToLCTU();
			LL_SwitchMuxToLCTU();
			
			if(!DelayCounter)
				DelayCounter = CONTROL_TimeCounter + DELAY_COMMUTATION;
			
			if(CONTROL_TimeCounter >= DelayCounter)
			{
				if(KEI_Measure(&KEI_Data))
				{
					DelayCounter = 0;
					
					if(KEI_Data >= DataTable[REG_SFTST_MUX_LCTU_THRE])
						CONTROL_SwitchToFault(DF_ST_MUX_LCTU);
					else
						CONTROL_SetDeviceSubState(ST_CurrentDividerCheck);
				}
			}
			break;
			
		case ST_CurrentDividerCheck:
			LL_SetStateCurrentDivider(true);
			
			if(!DelayCounter)
				DelayCounter = CONTROL_TimeCounter + DELAY_COMMUTATION;
			
			if(CONTROL_TimeCounter >= DelayCounter)
			{
				if(KEI_Measure(&KEI_Data))
				{
					DelayCounter = 0;
					
					if(KEI_Data >= DataTable[REG_SFTST_MUX_LCTU_DIV_THRE])
						CONTROL_SwitchToFault(DF_ST_DIVIDER);
					else
					{
						SELFTEST_StopProcess();
						
						DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_OK;
						CONTROL_SetDeviceState(DS_Ready, SS_None);
					}
				}
			}
			break;
			
		default:
			break;
	}
}
//-------------------------

bool SELFTEST_TestCurrentCheck()
{
	float CalculatedTestCurrent, MeasuredTestCurrent, Error;
	
	CalculatedTestCurrent = ADC_REF_VOLTAGE / (2 * DataTable[REG_SFTST_R_SHUNT] + DataTable[REG_SFTST_R_KEI]);
	MeasuredTestCurrent = (float)ADC_Measure(ADC1, ADC_I_CHANNEL) * ADC_REF_VOLTAGE / ADC_RESOLUTION
			/ DataTable[REG_SFTST_R_SHUNT];
	
	Error = fabs((MeasuredTestCurrent - CalculatedTestCurrent) / CalculatedTestCurrent * 100);
	
	return (Error >= DataTable[REG_SFTST_I_ALOWED_ERR]) ? false : true;
}
//-------------------------

void SELFTEST_StopProcess()
{
	CONTROL_HardwareDefaultState();
}
//-------------------------

