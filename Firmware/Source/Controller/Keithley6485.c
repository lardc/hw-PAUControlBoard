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
#define KEI_AVERAGE_COUNT_MAX			100

// Variables
//
Int16U KEI_RXcount = 0;
Int8U KEI_Fifo[KEI_FIFO_LENGTH];

// Functions prototypes
//
void KEI_SendData(char* Data);
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

	sprintf(Sizestr, "%f", (float)Size);
	snprintf(Comm2, sizeof Comm2, "%s%s", Comm1, Sizestr);

	KEI_SendData("SYST:ZCH OFF");
	KEI_SendData("SYST:AZER:STAT OFF");
	KEI_SendData("*CLS");
	KEI_SendData(Comm2);
	KEI_SendData("TRAC:CLE");
	KEI_SendData("TRAC:FEED:CONT NEXT");
	KEI_SendData("STAT:MEAS:ENAB 512");
	KEI_SendData("*SRE 1");
	KEI_SendData("*OPC?");
}
//----------------------------------
void KEI_EnableAverage(Int16U Size)
{
	static char Sizestr[4] = {0};
	static char Comm1[10] = {"AVER:COUN "};
	static char Comm2[14] = {0};

	if(Size > KEI_AVERAGE_COUNT_MAX)
		Size = KEI_AVERAGE_COUNT_MAX;

	sprintf(Sizestr, "%f", (float)Size);
	snprintf(Comm2, sizeof Comm2, "%s%s", Comm1, Sizestr);

	KEI_SendData(Comm2);
	KEI_SendData("AVER:TCON MOV");
	KEI_SendData("AVER:ADV:NTOL 0");//2..100
	KEI_SendData("AVER:ADV ON");
	KEI_SendData("AVER ON");
}
//----------------------------------

void KEI_EnableMedianFilter()
{
	KEI_SendData("MED:RANK 5");//1..5
	KEI_SendData("MED ON");
}
//----------------------------------

void KEI_AbortMeasure()
{
	KEI_SendData("ABOR");
}
//----------------------------------

void KEI_Reset()
{
	KEI_AbortMeasure();
	KEI_SendData("*CLS");
	KEI_SendData("*RST");
	KEI_SendData("TRAC:FEED:CONT NEV");
}
//----------------------------------

void KEI_ZeroCorrect()
{
	KEI_SendData("SYST:ZCH ON");
	//
	KEI_SendData("CURR:RANG 2E-9");
	KEI_SendData("INIT");
	KEI_SendData("SYST:ZCOR:ACQ");
	//
	KEI_SendData("SYST:ZCH OFF");
	KEI_SendData("SYST:ZCOR ON");
}
//----------------------------------

void KEI_SetRange(float Current)
{
	if(Current <= RANGE_2nA)
		KEI_SendData("CURR:RANG 2E-9"); 	// 2nA
	else if(Current <= RANGE_20nA)
		KEI_SendData("CURR:RANG 20E-9"); 	// 20nA
	else if(Current <= RANGE_200nA)
		KEI_SendData("CURR:RANG 200E-9"); 	// 200nA
	else if(Current <= RANGE_2uA)
		KEI_SendData("CURR:RANG 2E-6"); 	// 2uA
	else if(Current <= RANGE_20uA)
		KEI_SendData("CURR:RANG 20E-6"); 	// 20uA
	else if(Current <= RANGE_200uA)
		KEI_SendData("CURR:RANG 200E-6");	// 200uA
	else if(Current <= RANGE_2mA)
		KEI_SendData("CURR:RANG 2E-3"); 	// 2mA
	else
		KEI_SendData("CURR:RANG 20E-3"); 	// 20mA
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
	sprintf(PLCStr, "%f", Value);
	snprintf(Comm2, sizeof Comm2, "%s%s", Comm1, PLCStr);

	KEI_SendData(Comm2);
}
//----------------------------------

void KEI_TriggerLinkConfig(Int16U N)
{
	static char Nstr[4] = {0};
	static char Comm1[10] = {"TRIG:COUN "};
	static char Comm2[14] = {0};

	sprintf(Nstr, "%f", (float)N);
	snprintf(Comm2, sizeof Comm2, "%s%s", Comm1, Nstr);

	// Input trigger link
	KEI_SendData("TRIG:CLE");
	KEI_SendData("TRIG:DEL 0");
	KEI_SendData("TRIG:SOUR TLINk");
	KEI_SendData(Comm2);
	KEI_SendData("TRIG:ASYN:ILIN 1");
	
	// Output trigger link
	KEI_SendData("TRIG:ASYN:OLIN 2");
	KEI_SendData("TRIG:ASYN:OUTP SENS");
	KEI_SendData("TRIG:SOUR TLIN");
}
//----------------------------------

void KEI_SwitchToSyncWaiting()
{
	KEI_SendData("INIT");
	DELAY_MS(KEI_SYNC_INIT_DELAY);
}
//----------------------------------

bool KEI_ReadData(float* Data)
{
	DELAY_MS(KEI_READ_DELAY);

	if(DataTable[REG_SAMPLES_NUMBER] > 1)
	{
		KEI_SendData("CALC3:FORM MEAN");
		KEI_SendData("CALC3:DATA?");
	}
	else
		KEI_SendData("SENS:DATA?");

	DELAY_MS(KEI_RECEIVE_TIME);
	
	if(KEI_RXcount)
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
		if(SyncFlags.ToLCTU || SyncFlags.ToIGTU)
			return KEI_ReadData(Data);

		IWDG_Refresh();
	}

	CONTROL_SwitchToFault(DF_KEI_SYNC_TIMEOUT);
	return 0;
}
//----------------------------------

void KEI_SendData(char* Data)
{
	char KeithleyData = 0;
	Int16U SymbolCounter = 0;

	do
	{
		KeithleyData = *(Data + SymbolCounter++);

		if(KeithleyData)
			LL_SendByteToKeithley(KeithleyData);
	}while(KeithleyData);
	
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
	char Mantissa[KEI_FIFO_LENGTH] = {};
	char Exponenta[KEI_FIFO_LENGTH] = {};
	Int16U ExpStartAddress = 0;
	Int16U MantissaStartAddress = 0;
	float M, E;
	
	KEI_ResetRxCounter();
	
	for(int i = 0; i < KEI_FIFO_LENGTH; i++)
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
		for(int i = 0; i < KEI_FIFO_LENGTH; i++)
		{
			if(Mantissa[i] == '+' || Mantissa[i] == '-')
			{
				MantissaStartAddress = i;
				break;
			}
		}
	}

	M = atoff(&Mantissa[MantissaStartAddress]);
	E = atoff(Exponenta);
	
	return (M * powf(10, E) * 1000);
}
//----------------------------------

void KEI_ResetRxCounter()
{
	KEI_RXcount = 0;
}
//----------------------------------
