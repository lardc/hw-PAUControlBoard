#ifndef __CONTROLLER_H
#define __CONTROLLER_H

// Include
//
#include "stdinc.h"
#include "Global.h"

// Types
//
typedef enum __DeviceState
{
	DS_None = 0,
	DS_Fault = 1,
	DS_Disabled = 2,
	DS_Ready = 3,
	DS_InProcess = 4,
	DS_ConfigReady = 5
} DeviceState;

typedef enum __DeviceSubState
{
	SS_None = 0,
	SS_InitDelay,
	SS_PowerOn,
	SS_ConfigMUX,
	SS_ConfigKeithley,
	ST_WaitingKeithley,
	SS_ConfigDivider,
	SS_WaitCommutation,
	SS_ConfigSync,
	SS_Measurement,
	SS_SaveResults,
	//
	ST_Prepare = 20,
	ST_WaitingConfig,
	ST_CurrentCheck,
	ST_Keithley,
	ST_Measure,
	ST_IGTU_ChannelCheck,
	ST_LCTU_ChannelCheck,
	ST_CurrentDividerCheck
} DeviceSubState;

// Variables
//
extern volatile Int64U CONTROL_TimeCounter;
extern volatile DeviceState CONTROL_State;
extern volatile DeviceSubState CONTROL_SubState;
extern Int64U CONTROL_LEDTimeout;
extern Int64U Timeout;
//
extern Int16U MEMBUF_Values_Write[VALUES_x_SIZE];
extern Int16U MEMBUF_ValuesWrite_Counter;
extern bool CONTROL_SoftwareStartMeasure;

// Functions
//
void CONTROL_Init();
void CONTROL_Idle();
void CONTROL_DelayMs(uint32_t Delay);
void CONTROL_SwitchToFault(Int16U Reason);
void CONTROL_SetDeviceState(DeviceState NewState, DeviceSubState NewSubState);
void CONTROL_SetDeviceSubState(DeviceSubState NewSubState);
void CONTROL_HardwareDefaultState();
void CONTROL_HandleExternalLamp();

#endif // __CONTROLLER_H
