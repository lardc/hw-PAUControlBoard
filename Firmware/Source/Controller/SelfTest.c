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
void SELFTEST_FaultMUX(Int16U MUXState);

// Functions
//
void SELFTEST_Process()
{
	static Int16U SelfTestSwitch = 0;
	float KEI_Data = 0;

	switch(CONTROL_SubState)
	{
	case ST_Prepare:
		KEI_Config();
		LL_SwitchSyncToLCTU();
		LL_SwitchMuxToDefault();
		LL_SetStateSelfTestCurrent(true);

		RequiredTestCurrent = DataTable[REG_SFTST_KEI_I_MUX_OFF];
		DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_NONE;
		CommutationDelayCounter = CONTROL_TimeCounter + DELAY_KEI_CONFIG;

		CONTROL_SetDeviceSubState(ST_CurrentCheck);
		break;

	case ST_CurrentCheck:
		if(CONTROL_TimeCounter >= CommutationDelayCounter)
			{
			if(SELFTEST_TestCurrentCheck())
				CONTROL_SetDeviceSubState(ST_Interface);
			else
			{
				DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_FAIL;
				CONTROL_SwitchToFault(DF_ST_KEI_LINE_OPEN);
			}
		}
		break;

	case ST_Interface:
		if(KEI_ReadData(&KEI_Data))
			CONTROL_SetDeviceSubState(ST_Measure);
		break;

	case ST_Measure:
		if(CONTROL_TimeCounter >= CommutationDelayCounter)
		{
			if(KEI_Measure(&KEI_Data))
			{
				float Error = fabs((KEI_Data - RequiredTestCurrent) / RequiredTestCurrent * 100);

				if(Error <= DataTable[REG_SFTST_I_ALOWED_ERR])
				{
					switch(SelfTestSwitch)
					{
					case 0:
						SelfTestSwitch++;
						CONTROL_SetDeviceSubState(ST_IGTU_ChannelCheck);
						break;

					case 1:
						SelfTestSwitch++;
						LL_SelfTestChannel_IGTU(false);
						CONTROL_SetDeviceSubState(ST_LCTU_ChannelCheck);
						break;

					case 2:
						SelfTestSwitch++;
						CONTROL_SetDeviceSubState(ST_CurrentDeviderCheck);
						break;

					case 3:
						SELFTEST_StopProcess();
						SelfTestSwitch = 0;
						DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_OK;
						CONTROL_SetDeviceState(DS_Ready, SS_None);
						break;
					}
				}
				else
				{
					SELFTEST_FaultMUX(SelfTestSwitch);
					SelfTestSwitch = 0;
				}
			}
		}
		break;

	case ST_IGTU_ChannelCheck:
		LL_SwitchSyncToIGTU();
		LL_SwitchMuxToIGTU();
		LL_SelfTestChannel_IGTU(true);
		RequiredTestCurrent = DataTable[REG_SFTST_KEI_I_MUX_IGTU];
		CommutationDelayCounter = CONTROL_TimeCounter + DELAY_COMMUTATION;
		CONTROL_SetDeviceSubState(ST_Measure);
		break;

	case ST_LCTU_ChannelCheck:
		LL_SwitchSyncToLCTU();
		LL_SwitchMuxToLCTU();
		RequiredTestCurrent = DataTable[REG_SFTST_KEI_I_MUX_LCTU];
		CommutationDelayCounter = CONTROL_TimeCounter + DELAY_COMMUTATION;
		CONTROL_SetDeviceSubState(ST_Measure);
		break;

	case ST_CurrentDeviderCheck:
		LL_SetStateCurrentDivider(true);
		RequiredTestCurrent = DataTable[REG_SFTST_KEI_I_MUX_LCTU_DIV];
		CommutationDelayCounter = CONTROL_TimeCounter + DELAY_COMMUTATION;
		CONTROL_SetDeviceSubState(ST_Measure);
		break;

	default:
		break;
	}
}
//-------------------------

void SELFTEST_FaultMUX(Int16U MUXState)
{
	DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_FAIL;

	switch(MUXState)
	{
	case 0:
		CONTROL_SwitchToFault(DF_ST_KEI_LINE_SHORT);
		break;

	case 1:
		CONTROL_SwitchToFault(DF_ST_MUX_IGTU);
		break;

	case 2:
		CONTROL_SwitchToFault(DF_ST_MUX_LCTU);
		break;

	case 3:
		CONTROL_SwitchToFault(DF_ST_DIVIDER);
		break;
	}
}
//-------------------------

bool SELFTEST_TestCurrentCheck()
{
	float CalculatedTestCurrent, MeasuredTestCurrent, Error;

	CalculatedTestCurrent = ADC_REF_VOLTAGE / (2 * DataTable[REG_SFTST_R_SHUNT] + DataTable[REG_SFTST_R_KEI]);
	MeasuredTestCurrent = (float)ADC_Measure(ADC1, ADC_I_CHANNEL) * ADC_REF_VOLTAGE / ADC_RESOLUTION / DataTable[REG_SFTST_R_SHUNT];

	Error = fabs((MeasuredTestCurrent - CalculatedTestCurrent) / CalculatedTestCurrent * 100);

	return (Error >= DataTable[REG_SFTST_I_ALOWED_ERR]) ? false : true;
}
//-------------------------

void SELFTEST_StopProcess()
{
	CONTROL_HardwareDefaultState();
}
//-------------------------

