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
#include "DataTable.h"

// Definitions
//
#define KEI_MSR_PACKAGE_LENGTH			16
#define TRIG_WITHOUT_BUFFER				1
#define KEI_AVERAGE_COUNT_MAX			100

// Variables
//
Int16U KEI_RXcount = 0;
Int8U KEI_Fifo[KEI_FIFO_LENGTH];

// Functions prototypes
//
void KEI_SendData(char* Data, Int16U Bytes);
float KEI_ExtractData();

// Functions
//
void KEI_SimpleConfig()
{
	KEI_Reset();
	KEI_ZeroCorrect();
	KEI_SetRange(RANGE_20mA);
	KEI_SetNPLC(DataTable[REG_KEI_NPLC_VALUE]);
	KEI_TriggerLinkConfig(TRIG_WITHOUT_BUFFER);
}
//----------------------------------

void KEI_BufferConfig(Int16U Size)
{
	static char Sizestr[4] = {0};
	static char Comm1[10] = {"TRAC:POIN "};
	static char Comm2[14] = {0};

	sprintf(&Sizestr[0], "%f", (float)Size);
	snprintf(Comm2, sizeof Comm2, "%s%s", Comm1, Sizestr);

	KEI_SendData("SYST:ZCH OFF", 12);
	KEI_SendData("SYST:AZER:STAT OFF", 18);
	KEI_SendData("*CLS", 4);
	KEI_SendData(&Comm2[0], sizeof(Comm2));
	KEI_SendData("TRAC:CLE", 8);
	KEI_SendData("TRAC:FEED:CONT NEXT", 19);
	KEI_SendData("STAT:MEAS:ENAB 512", 18);
	KEI_SendData("*SRE 1", 6);
	KEI_SendData("*OPC?", 5);
}
//----------------------------------
void KEI_EnableAverage(Int16U Size)
{
	static char Sizestr[4] = {0};
	static char Comm1[10] = {"AVER:COUN "};
	static char Comm2[14] = {0};

	if(Size > KEI_AVERAGE_COUNT_MAX)
		Size = KEI_AVERAGE_COUNT_MAX;

	sprintf(&Sizestr[0], "%f", (float)Size);
	snprintf(Comm2, sizeof Comm2, "%s%s", Comm1, Sizestr);

	KEI_SendData(&Comm2[0], sizeof(Comm2));
	KEI_SendData("AVER:TCON MOV", 13);
	KEI_SendData("AVER:ADV:NTOL 0", 16);//2..100
	KEI_SendData("AVER:ADV ON", 11);
	KEI_SendData("AVER ON", 8);
}
//----------------------------------

void KEI_EnableMedianFilter()
{
	KEI_SendData("MED:RANK 5", 10);//1..5
	KEI_SendData("MED ON", 6);
}
//----------------------------------

void KEI_AbortMeasure()
{
	KEI_SendData("ABOR", 4);
}
//----------------------------------

void KEI_Reset()
{
	KEI_AbortMeasure();
	KEI_SendData("*CLS", 4);
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

void KEI_SetNPLC(float Value)
{
	static const char Comm1[10] = {"CURR:NPLC "};
	static char PLCStr[4] = {0};
	static char Comm2[14] = {0};

	if(Value < NPLC_MIN)
		Value = NPLC_MIN;
	else if(Value > NPLC_MAX)
		Value = NPLC_MAX;
	
	Value = roundf(Value * 100) / 100;
	sprintf(&PLCStr[0], "%f", Value);
	snprintf(Comm2, sizeof Comm2, "%s%s", Comm1, PLCStr);

	KEI_SendData(&Comm2[0], sizeof(Comm2));
}
//----------------------------------

void KEI_TriggerLinkConfig(Int16U N)
{
	static char Nstr[4] = {0};
	static char Comm1[10] = {"TRIG:COUN "};
	static char Comm2[14] = {0};

	sprintf(&Nstr[0], "%f", (float)N);
	snprintf(Comm2, sizeof Comm2, "%s%s", Comm1, Nstr);

	// Input trigger link
	KEI_SendData("TRIG:CLE", 8);
	KEI_SendData("TRIG:DEL 0", 10);
	KEI_SendData("TRIG:SOUR TLINk", 15);
	KEI_SendData(&Comm2[0], sizeof(Comm2));
	KEI_SendData("TRIG:ASYN:ILIN 1", 16);
	
	// Output trigger link
	KEI_SendData("TRIG:ASYN:OLIN 2", 16);
	KEI_SendData("TRIG:ASYN:OUTP SENS", 19);
	KEI_SendData("TRIG:SOUR TLIN", 14);
}
//----------------------------------

void KEI_SwitchToSyncWaiting()
{
	KEI_SendData("INIT", 4);
}
//----------------------------------

bool KEI_ReadData(float* Data)
{
	if(DataTable[REG_SAMPLES_NUMBER] > 1)
	{
		KEI_SendData("CALC3:FORM MEAN", 15);
		KEI_SendData("CALC3:DATA?", 11);
	}
	else
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
	Int64U TimeCounter = CONTROL_TimeCounter + KEI_MEASURE_TIMEOUT;
	
	INT_ResetFlags();
	KEI_SwitchToSyncWaiting();
	LL_GenerateSyncToKeithley();

	while(CONTROL_TimeCounter < TimeCounter)
	{
		if(FlagSyncToLCTU || FlagSyncToIGTU)
			return KEI_ReadData(Data);
	}

	CONTROL_SwitchToFault(DF_KEI_SYNC_TIMEOUT);
	return 0;
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
	Int16U MantissaStartAddress = 0;
	float M, E;
	
	KEI_ResetRxCounter();
	
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
	
	if(Mantissa[0] != '+' || Mantissa[0] != '-')
	{
		for(int i = 0; i < KEI_MSR_PACKAGE_LENGTH; i++)
		{
			if(Mantissa[i] == '+' || Mantissa[i] == '-')
			{
				MantissaStartAddress = i;
				break;
			}
		}
	}

	M = atoff(&Mantissa[MantissaStartAddress]);
	E = atoff(&Exponenta[0]);
	
	return (M * powf(10, E) * 1000);
}
//----------------------------------

void KEI_ResetRxCounter()
{
	KEI_RXcount = 0;
}
//----------------------------------
