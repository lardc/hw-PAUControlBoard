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

// Definitions
//

//

// Variables
//
Int64U CommutationDelayCounter = 0;
float RequiredTestCurrent = 0;


// Functions prototypes
//
bool SELFTEST_TestCurrentCheck();
void SELFTEST_StopProcess();

// Functions
//
void SELFTEST_Process()
{
	switch(CONTROL_SubState)
	{
	case ST_Prepare:
		KEI_Config();
		LL_SwitchSyncOff();
		LL_SwitchMuxToDefault();
		LL_SetStateSelfTestCurrent(true);

		RequiredTestCurrent = DataTable[REG_SFTST_KEI_I_MUX_OFF];
		DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_NONE;
		CommutationDelayCounter = CONTROL_TimeCounter + COMMUTATION_DELAY_MS;

		CONTROL_SetDeviceSubState(ST_Measure);
		break;

	case ST_Measure:
		if(CONTROL_TimeCounter >= CommutationDelayCounter)
		{
			if(SELFTEST_TestCurrentCheck())
			{
				float KEI_Data = KEI_Measure();
				float Error = (KEI_Data - RequiredTestCurrent) / RequiredTestCurrent * 100;

				if(Error <= DataTable[REG_SFTST_I_ALOWED_ERR])
				{
					static Int16U SelfTestSwitch = 0;

					switch(SelfTestSwitch)
					{
					case 0:
						CONTROL_SetDeviceSubState(ST_IGTU_ChannelCheck);
						break;

					case 1:
						LL_SelfTestChannel_IGTU(false);
						CONTROL_SetDeviceSubState(ST_LCTU_ChannelCheck);
						break;

					case 2:
						CONTROL_SetDeviceSubState(ST_CurrentDeviderCheck);
						break;

					case 3:
						SelfTestSwitch = 0;
						SELFTEST_StopProcess();
						DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_OK;
						CONTROL_SetDeviceState(DS_Ready, SS_None);
						break;
					}

					SelfTestSwitch++;
				}
				else
				{
					DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_FAIL;
					CONTROL_SwitchToFault(DF_TEST_I_MEASURE);
				}
			}
			else
			{
				DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_FAIL;
				CONTROL_SwitchToFault(DF_TEST_I_SET);
			}
		}
		break;

	case ST_IGTU_ChannelCheck:
		LL_SwitchSyncToIGTU();
		LL_SwitchMuxToIGTU();
		LL_SelfTestChannel_IGTU(true);
		RequiredTestCurrent = DataTable[REG_SFTST_KEI_I_MUX_IGTU];
		CommutationDelayCounter = CONTROL_TimeCounter + COMMUTATION_DELAY_MS;
		CONTROL_SetDeviceSubState(ST_Measure);
		break;

	case ST_LCTU_ChannelCheck:
		LL_SwitchSyncToLCTU();
		LL_SwitchMuxToLCTU();
		RequiredTestCurrent = DataTable[REG_SFTST_KEI_I_MUX_LCTU];
		CommutationDelayCounter = CONTROL_TimeCounter + COMMUTATION_DELAY_MS;
		CONTROL_SetDeviceSubState(ST_Measure);
		break;

	case ST_CurrentDeviderCheck:
		LL_SetStateCurrentDivider(true);
		RequiredTestCurrent = DataTable[REG_SFTST_KEI_I_MUX_LCTU_DIV];
		CommutationDelayCounter = CONTROL_TimeCounter + COMMUTATION_DELAY_MS;
		CONTROL_SetDeviceSubState(ST_Measure);
		break;

	default:
		break;
	}
}
//-------------------------

bool SELFTEST_TestCurrentCheck()
{
	float CalculatedTestCurrent, MeasuredTestCurrent, Error;

	CalculatedTestCurrent = ADC_REF_VOLTAGE / (2 * DataTable[REG_SFTST_R_SHUNT]);
	MeasuredTestCurrent = (float)ADC_Measure(ADC1, ADC_I_CHANNEL) * ADC_REF_VOLTAGE / ADC_RESOLUTION / DataTable[REG_SFTST_R_SHUNT];

	Error = (MeasuredTestCurrent - CalculatedTestCurrent) / CalculatedTestCurrent * 100;

	return (Error >= DataTable[REG_SFTST_I_ALOWED_ERR]) ? false : true;
}
//-------------------------

void SELFTEST_StopProcess()
{
	CONTROL_HardwareDefaultState();
}
//-------------------------

