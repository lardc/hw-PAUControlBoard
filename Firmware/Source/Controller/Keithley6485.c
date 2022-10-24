// Header
//
#include "Keithley6485.h"

// Includes
//
#include "LowLevel.h"
#include "Delay.h"
#include "Math.h"
#include "stdio.h"

// Variables
//
Int16U KEI_RXcount = 0;
Int8U KEI_Fifo[KEI_FIFO_LENGTH];

// Functions prototypes
//
void KEI_Reset();
void KEI_ZeroCorrect();
void KEI_TriggerLinkConfig();
void KEI_SetADCRate(float Rate);
void KEI_SendData(char* Data, Int16U Bytes);
float KEI_ExtractData();



// Functions
//
void KEI_Config()
{
	KEI_Reset();
	KEI_ZeroCorrect();
	KEI_SetRange(RANGE_20mA);
	KEI_TriggerLinkConfig();
	KEI_SetADCRate(NPLC_DEF);
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
	else if(Current <= RANGE_20mA)
		KEI_SendData("CURR:RANG 20E-3", 15); 	// 20mA
}
//----------------------------------

void KEI_SetADCRate(float Rate)
{
	float RoundedRate;
	char Temp[13] = {'C','U','R','R',':','N','P','L','C',' '};
	char RateStr[3];

	if(Rate < NPLC_MIN)
		Rate = NPLC_MIN;
	else if(Rate > NPLC_MAX)
		Rate = NPLC_MAX;

	RoundedRate = roundf(Rate * 100) / 100;
	sprintf(RateStr, "%f", RoundedRate);

	for(int i = 0; i < sizeof(RateStr); i++)
		Temp[10 + i] = RateStr[i];

	KEI_SendData(&Temp[0], 13);
}
//----------------------------------

void KEI_TriggerLinkConfig()
{
	// Input trigger link
	KEI_SendData("TRIG:ASYN:ILIN 1", 16);
	KEI_SendData("TRIG:ASYN:OUTP SENS", 19);
	KEI_SendData("TRIG:DEL 0", 10);
	KEI_SendData("TRIG:SOUR TLINk", 15);
	KEI_SendData("TRIG:COUN  1", 11);

	// Output trigger link
	KEI_SendData("TRIG:ASYN:OLIN 2", 16);
}
//----------------------------------

void KEI_SwitchToSyncWaiting()
{
	KEI_SendData("INIT", 4);
}
//----------------------------------

float KEI_ReadData()
{
	KEI_SendData("SENS:DATA?", 10);
	DELAY_MS(KEI_RECEIVE_TIME);

	return KEI_ExtractData();
}
//----------------------------------

void KEI_SendData(char* Data, Int16U Bytes)
{
	for(int i = 0; i < Bytes; i++)
		LL_SendByteToKeithley(*(Data + i));

	LL_SendByteToKeithley(0x0D);
	LL_SendByteToKeithley(0x0A);
}
//----------------------------------

void KEI_ReceiveData(USART_TypeDef* USARTx)
{
	if (KEI_RXcount < KEI_FIFO_LENGTH)
		KEI_Fifo[KEI_RXcount++] = USARTx->RDR;
}
//----------------------------------

float KEI_ExtractData()
{
	KEI_RXcount = 0;
	//KEI_Fifo[0];

	return 0;
}
//----------------------------------
