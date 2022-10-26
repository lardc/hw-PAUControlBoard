// Header
#include "Controller.h"
//
// Includes
#include "Board.h"
#include "Delay.h"
#include "DeviceProfile.h"
#include "Interrupts.h"
#include "LowLevel.h"
#include "SysConfig.h"
#include "DebugActions.h"
#include "Diagnostic.h"
#include "BCCIxParams.h"
#include "SelfTest.h"
#include "Keithley6485.h"
#include "Constraints.h"

// Definitions
//

// Types
//
typedef void (*FUNC_AsyncDelegate)();

// Variables
//
volatile DeviceState CONTROL_State = DS_None;
volatile DeviceSubState CONTROL_SubState = SS_None;
static Boolean CycleActive = false;
Int16U MEMBUF_Values_Write[VALUES_x_SIZE];
Int16U MEMBUF_ValuesWrite_Counter = 0;
//
volatile Int64U CONTROL_TimeCounter = 0;
Int64U CONTROL_CommutationDelayCounter = 0;
Int64U CONTROL_TimeoutCounter = 0;
//
float CurrentDividerRatio = 1;
bool CONTROL_SoftwareStartMeasure = false;
//

// Forward functions
//
static Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U pUserError);
void CONTROL_UpdateWatchDog();
void CONTROL_ResetToDefaultState();
void CONTROL_LogicProcess();
void CONTROL_SaveTestResult();
void CONTROL_ResetOutputRegisters();

// Functions
//
void CONTROL_Init()
{
	Int16U EPWriteIndexes[EP_WRITE_COUNT] = { EP16_WR };
	Int16U EPWriteSized[EP_WRITE_COUNT] = { VALUES_x_SIZE };
	pInt16U EPWriteCounters[EP_WRITE_COUNT] = { (pInt16U)&MEMBUF_ValuesWrite_Counter };
	pInt16U EPWriteDatas[EP_WRITE_COUNT] = { MEMBUF_Values_Write };

	// Конфигурация сервиса работы Data-table и EPROM
	EPROMServiceConfig EPROMService = {(FUNC_EPROM_WriteValues)&NFLASH_WriteDT, (FUNC_EPROM_ReadValues)&NFLASH_ReadDT};
	// Инициализация data table
	DT_Init(EPROMService, false);
	DT_SaveFirmwareInfo(CAN_SLAVE_NID, 0);
	// Инициализация device profile
	DEVPROFILE_Init(&CONTROL_DispatchAction, &CycleActive);
	DEVPROFILE_InitEPWriteService(EPWriteIndexes, EPWriteSized, EPWriteCounters, EPWriteDatas);

	// Сброс значений
	DEVPROFILE_ResetControlSection();
	CONTROL_ResetToDefaultState();
}
//------------------------------------------

void CONTROL_ResetToDefaultState()
{
	CONTROL_ResetOutputRegisters();
	CONTROL_HardwareDefaultState();
	CONTROL_SetDeviceState(DS_None, SS_None);
}
//------------------------------------------

void CONTROL_Idle()
{
	CONTROL_LogicProcess();

	DEVPROFILE_ProcessRequests();
	CONTROL_UpdateWatchDog();
}
//------------------------------------------

static Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U pUserError)
{
	*pUserError = ERR_NONE;
	
	switch (ActionID)
	{
		case ACT_ENABLE_POWER:
			if(CONTROL_State == DS_None)
				CONTROL_SetDeviceState(DS_InProcess, ST_Prepare);
			else if(CONTROL_State != DS_Ready)
				*pUserError = ERR_OPERATION_BLOCKED;
			break;

		case ACT_DISABLE_POWER:
			if(CONTROL_State == DS_Ready)
				CONTROL_ResetToDefaultState();
			else if(CONTROL_State != DS_None)
					*pUserError = ERR_OPERATION_BLOCKED;
			break;

		case ACT_CLR_FAULT:
			if (CONTROL_State == DS_Fault)
			{
				CONTROL_SetDeviceState(DS_None, SS_None);
				DataTable[REG_FAULT_REASON] = DF_NONE;
			}
			break;

		case ACT_CLR_WARNING:
			DataTable[REG_WARNING] = WARNING_NONE;
			break;

		case ACT_RUN_SELF_TEST:
			if(CONTROL_State == DS_None || CONTROL_State == DS_Ready)
				CONTROL_SetDeviceState(DS_InProcess, ST_Prepare);
			else
				*pUserError = ERR_OPERATION_BLOCKED;
			break;

		case ACT_CONFIG:
			if(CONTROL_State == DS_Ready || CONTROL_State == DS_ConfigReady)
			{
				CONTROL_SoftwareStartMeasure = false;
				CONTROL_SetDeviceState(DS_InProcess, SS_ConfigKeithley);
			}
			else
				if (CONTROL_State == DS_InProcess)
					*pUserError = ERR_OPERATION_BLOCKED;
				else
					*pUserError = ERR_DEVICE_NOT_READY;
			break;

		case ACT_START_MEASURE:
			if(CONTROL_State == DS_Ready)
			{
				CONTROL_SoftwareStartMeasure = true;
				CONTROL_SetDeviceState(DS_InProcess, SS_ConfigKeithley);
			}
			else
				if (CONTROL_State == DS_InProcess)
					*pUserError = ERR_OPERATION_BLOCKED;
				else
					*pUserError = ERR_DEVICE_NOT_READY;
			break;

		default:
			return DIAG_HandleDiagnosticAction(ActionID, pUserError);
			
	}
	return true;
}
//-----------------------------------------------

void CONTROL_LogicProcess()
{
	if(CONTROL_State == DS_InProcess)
	{
		switch(CONTROL_SubState)
		{
		case SS_ConfigKeithley:
			KEI_SetRange(DataTable[REG_RANGE]);
			KEI_SetADCRate(DataTable[REG_MEASUREMENT_TIME] / PLC_TIME);
			CONTROL_SetDeviceState(DS_InProcess, SS_ConfigMUX);
			break;

		case SS_ConfigMUX:
			(DataTable[REG_CHANNEL] == CHANNEL_LCTU) ? LL_SwitchMuxToLCTU() : LL_SwitchMuxToIGTU();
			CONTROL_SetDeviceState(DS_InProcess, SS_ConfigDevider);
			break;

		case SS_ConfigDevider:
			CurrentDividerRatio = 1;

			if(DataTable[REG_CHANNEL] == CHANNEL_LCTU && DataTable[REG_RANGE] > KEI_CURRENT_MAX)
			{
				CurrentDividerRatio = DataTable[REG_I_DIV_FACTOR];
				LL_SetStateCurrentDivider(true);
			}
			CONTROL_CommutationDelayCounter = CONTROL_TimeCounter + COMMUTATION_DELAY_MS;
			CONTROL_SetDeviceState(DS_InProcess, SS_WaitCommutation);
			break;

		case SS_WaitCommutation:
			if(CONTROL_TimeCounter >= CONTROL_CommutationDelayCounter)
				CONTROL_SetDeviceState(DS_InProcess, SS_ConfigSync);
			break;

		case SS_ConfigSync:
			if(!CONTROL_SoftwareStartMeasure)
			{
				(DataTable[REG_CHANNEL] == CHANNEL_LCTU) ? LL_SwitchSyncToLCTU() : LL_SwitchSyncToIGTU();
				KEI_SwitchToSyncWaiting();
				CONTROL_TimeoutCounter = CONTROL_TimeCounter + DataTable[REG_SYNC_WAIT_TIMEOUT];
				CONTROL_SetDeviceState(DS_ConfigReady, SS_None);
			}
			else
				CONTROL_SetDeviceState(DS_InProcess, SS_Measurement);
			break;

		case SS_Measurement:
			if(!CONTROL_SoftwareStartMeasure)
			{
				if(CONTROL_TimeCounter >= CONTROL_TimeoutCounter)
					CONTROL_SwitchToFault(DF_MEASURE_TIMEOUT);
			}
			else
			{
				LL_GenerateSyncToKeithley();
				CONTROL_SetDeviceState(DS_InProcess, SS_SaveResults);
			}
			break;

		case SS_SaveResults:
			CONTROL_SaveTestResult();
			CONTROL_HardwareDefaultState();
			CONTROL_SetDeviceState(DS_Ready, SS_None);
			break;

		default:
			SELFTEST_Process();
			break;
		}
	}

	if(CONTROL_State == DS_ConfigReady && CONTROL_TimeCounter >= CONTROL_TimeoutCounter)
	{
		CONTROL_HardwareDefaultState();
		DataTable[REG_WARNING] = WARNING_SYNC_WAIT_TIMEOUT;
		CONTROL_SetDeviceState(DS_Ready, SS_None);
	}
}
//-----------------------------------------------

void CONTROL_HardwareDefaultState()
{
	LL_SetStateExtLED(false);
	LL_SetStateCurrentDivider(false);
	LL_SetStateSelfTestCurrent(false);
	LL_SwitchMuxToDefault();
	LL_SwitchSyncOff();
}
//-----------------------------------------------

void CONTROL_ResetOutputRegisters()
{
	DataTable[REG_FAULT_REASON] = DF_NONE;
	DataTable[REG_DISABLE_REASON] = DF_NONE;
	DataTable[REG_WARNING] = WARNING_NONE;
	DataTable[REG_PROBLEM] = PROBLEM_NONE;
	DataTable[REG_OP_RESULT] = OPRESULT_NONE;
	//
	DEVPROFILE_ResetScopes(0);
	DEVPROFILE_ResetEPReadState();
}
//-----------------------------------------------

void CONTROL_SaveTestResult()
{
	DataTable[REG_RESULT_CURRENT] = KEI_ReadData() * CurrentDividerRatio;
	DataTable[REG_OP_RESULT] = OPRESULT_OK;
}
//-----------------------------------------------

void CONTROL_SwitchToFault(Int16U Reason)
{
	CONTROL_ResetOutputRegisters();
	CONTROL_HardwareDefaultState();

	CONTROL_SetDeviceState(DS_Fault, SS_None);
	DataTable[REG_FAULT_REASON] = Reason;
}
//------------------------------------------

void CONTROL_SetDeviceState(DeviceState NewState, DeviceSubState NewSubState)
{
	CONTROL_State = NewState;
	DataTable[REG_DEV_STATE] = NewState;

	CONTROL_SetDeviceSubState(NewSubState);
}
//------------------------------------------

void CONTROL_SetDeviceSubState(DeviceSubState NewSubState)
{
	CONTROL_SubState = NewSubState;
	DataTable[REG_SUB_STATE] = NewSubState;
}
//------------------------------------------

void CONTROL_UpdateWatchDog()
{
	if(BOOT_LOADER_VARIABLE != BOOT_LOADER_REQUEST)
		IWDG_Refresh();
}
//------------------------------------------

