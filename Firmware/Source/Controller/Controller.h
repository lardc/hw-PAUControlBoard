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
	DS_SelfTest = 5,
	DS_ConfigReady = 6
} DeviceState;

typedef enum __DeviceSubState
{
	SS_None = 0,
	//
	SS_Prepare = 10,
	SS_Measure,
	SS_IGTU_ChannelCheck,
	SS_LCTU_ChannelCheck,
	SS_CurrentDeviderCheck
} DeviceSubState;

// Variables
//
extern volatile Int64U CONTROL_TimeCounter;
extern volatile DeviceState CONTROL_State;
extern volatile DeviceSubState CONTROL_SubState;
extern Int64U CONTROL_LEDTimeout;
//
extern Int16U MEMBUF_Values_Write[VALUES_x_SIZE];
extern Int16U MEMBUF_ValuesWrite_Counter;


// Functions
//
void CONTROL_Init();
void CONTROL_Idle();
void CONTROL_DelayMs(uint32_t Delay);
void CONTROL_SwitchToFault(Int16U Reason);
void CONTROL_SetDeviceState(DeviceState NewState, DeviceSubState NewSubState);
void CONTROL_SetDeviceSubState(DeviceSubState NewSubState);

#endif // __CONTROLLER_H
