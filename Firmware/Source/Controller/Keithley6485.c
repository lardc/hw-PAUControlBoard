// Header
//
#include "Keithley6485.h"

// Includes
//
#include "LowLevel.h"
#include "Delay.h"
#include "Math.h"
#include "stdio.h"
#include "stdlib.h"
#include "Interrupts.h"
#include "Controller.h"
#include "DeviceObjectDictionary.h"

// Definitions
//
#define KEI_MSR_PACKAGE_LENGTH			43

// Variables
//
Int16U KEI_RXcount = 0;
Int8U KEI_Fifo[KEI_FIFO_LENGTH];
Int16U KEI_ConversionTimeout = 0;

// Functions prototypes
//
void KEI_Reset();
void KEI_ZeroCorrect();
void KEI_TriggerLinkConfig();
void KEI_SendData(char* Data, Int16U Bytes);
float KEI_ExtractData();

// Functions
//
void KEI_Config()
{
	KEI_Reset();
	KEI_ZeroCorrect();
	KEI_SetRange(RANGE_20mA);
	KEI_SetADCRate(NPLC_TIME_DEF);
	KEI_TriggerLinkConfig();
}
//----------------------------------

void KEI_Reset()
{
	KEI_SendData("*RST", 4);
}
//----------------------------------

void KEI_ZeroCorrect()
{
	KEI_SendData("SYST:ZCH ON", 11);
	//
	KEI_SendData("CURR:RANG 2E-9", 14);
	KEI_SendData("INIT", 4);
	KEI_SendData("SYST:ZCOR:ACQ", 13);
	//
	KEI_SendData("SYST:ZCH OFF", 12);
	KEI_SendData("SYST:ZCOR ON", 12);
}
//----------------------------------

void KEI_SetRange(float Current)
{
	if(Current <= RANGE_2nA)
		KEI_SendData("CURR:RANG 2E-9", 14); 	// 2nA
	else if(Current <= RANGE_20nA)
		KEI_SendData("CURR:RANG 20E-9", 15); 	// 20nA
	else if(Current <= RANGE_200nA)
		KEI_SendData("CURR:RANG 200E-9", 16); 	// 200nA
	else if(Current <= RANGE_2uA)
		KEI_SendData("CURR:RANG 2E-6", 14); 	// 2uA
	else if(Current <= RANGE_20uA)
		KEI_SendData("CURR:RANG 20E-6", 15); 	// 20uA
	else if(Current <= RANGE_200uA)
		KEI_SendData("CURR:RANG 200E-6", 16);	// 200uA
	else if(Current <= RANGE_2mA)
		KEI_SendData("CURR:RANG 2E-3", 14); 	// 2mA
	else
		KEI_SendData("CURR:RANG 20E-3", 15); 	// 20mA
}
//----------------------------------

void KEI_SetADCRate(float Time)
{
	float RoundedRate, Rate;
	char Temp[14] = {0};
	char RateStr[4] = {0};
	
	Rate = (Time - PLC_TIME_OFFSET) / PLC_TIME;

	if(Rate < NPLC_MIN)
		Rate = NPLC_MIN;
	else if(Rate > NPLC_MAX)
		Rate = NPLC_MAX;
	
	RoundedRate = roundf(Rate * 100) / 100;
	sprintf(&RateStr[0], "%f", RoundedRate);
	
	Temp[0] = 'C';
	Temp[1] = 'U';
	Temp[2] = 'R';
	Temp[3] = 'R';
	Temp[4] = ':';
	Temp[5] = 'N';
	Temp[6] = 'P';
	Temp[7] = 'L';
	Temp[8] = 'C';
	Temp[9] = ' ';
	
	for(int i = 0; i < sizeof(RateStr); i++)
		Temp[10 + i] = RateStr[i];

	KEI_SendData(&Temp[0], sizeof(Temp));

	KEI_ConversionTimeout = (RoundedRate * PLC_TIME) * PLC_TIME_COEFFICIENT;
}
//----------------------------------

void KEI_TriggerLinkConfig()
{
	// Input trigger link
	KEI_SendData("TRIG:DEL 0", 10);
	KEI_SendData("TRIG:SOUR TLINk", 15);
	KEI_SendData("TRIG:COUN 1", 11);
	KEI_SendData("TRIG:ASYN:ILIN 1", 16);
	
	// Output trigger link
	KEI_SendData("TRIG:ASYN:OLIN 2", 16);
	KEI_SendData("TRIG:ASYN:OUTP SENS", 19);
	KEI_SendData("TRIG:SOUR TLIN", 14);
}
//----------------------------------

void KEI_SwitchToSyncWaiting()
{
	FlagSyncFromLCTU = false;
	FlagSyncFromIGTU = false;
	FlagSyncToLCTU = false;
	FlagSyncToIGTU = false;
	
	KEI_SendData("INIT", 4);
}
//----------------------------------

bool KEI_ReadData(float* Data)
{
	KEI_SendData("SENS:DATA?", 10);
	DELAY_MS(KEI_RECEIVE_TIME);
	
	if(KEI_RXcount >= KEI_MSR_PACKAGE_LENGTH)
	{
		*Data = KEI_ExtractData();
		return true;
	}
	else
	{
		CONTROL_SwitchToFault(DF_KEI_INTERFACE_TIMEOUT);
		return false;
	}
}
//----------------------------------

bool KEI_Measure(float* Data)
{
	Int64U TimeCounter = 0;
	
	KEI_SwitchToSyncWaiting();
	LL_GenerateSyncToKeithley();
	
	TimeCounter = CONTROL_TimeCounter + KEI_ConversionTimeout;
	
	while(CONTROL_TimeCounter < TimeCounter)
	{
	}
	
	if(FlagSyncToLCTU || FlagSyncToIGTU)
		return KEI_ReadData(Data);
	else
	{
		CONTROL_SwitchToFault(DF_KEI_SYNC_TIMEOUT);
		return 0;
	}
}
//----------------------------------

void KEI_SendData(char* Data, Int16U Bytes)
{
	for(int i = 0; i < Bytes; i++)
		LL_SendByteToKeithley(*(Data + i));
	
	LL_SendByteToKeithley(0x0D);
	LL_SendByteToKeithley(0x0A);
	
	DELAY_MS(1);
}
//----------------------------------

void KEI_ReceiveData(USART_TypeDef* USARTx)
{
	if(KEI_RXcount < KEI_FIFO_LENGTH)
		KEI_Fifo[KEI_RXcount++] = USARTx->RDR;
}
//----------------------------------

float KEI_ExtractData()
{
	char Mantissa[KEI_MSR_PACKAGE_LENGTH] = {};
	char Exponenta[KEI_MSR_PACKAGE_LENGTH] = {};
	Int16U ExpStartAddress = 0;
	float M, E;
	
	KEI_ResetRxConuter();
	
	for(int i = 0; i < KEI_MSR_PACKAGE_LENGTH; i++)
	{
		if(!ExpStartAddress)
		{
			if(KEI_Fifo[i] == 'E')
				ExpStartAddress = i + 1;
			else
				Mantissa[i] = KEI_Fifo[i];
		}
		else
		{
			if(KEI_Fifo[i] == 'A')
				break;
			else
				Exponenta[i - ExpStartAddress] = KEI_Fifo[i];
		}
	}
	
	M = atoff(&Mantissa[0]);
	E = atoff(&Exponenta[0]);
	
	return (M * powf(10, E) * 1000);
}
//----------------------------------

void KEI_ResetRxConuter()
{
	KEI_RXcount = 0;
}
//----------------------------------
