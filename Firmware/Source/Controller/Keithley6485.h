#ifndef CONTROLLER_KEITHLEY6485_H_
#define CONTROLLER_KEITHLEY6485_H_

// Includes
//
#include "Board.h"

// Definitions
//
#define TRIG_WITHOUT_BUFFER				1

// Current ranges
#define RANGE_2nA						2.1e-6					// in mA
#define RANGE_20nA						21e-6					// in mA
#define RANGE_200nA						210e-6					// in mA
#define RANGE_2uA						2.1e-3					// in mA
#define RANGE_20uA						21e-3					// in mA
#define RANGE_200uA						210e-3					// in mA
#define RANGE_2mA						2.1						// in mA
#define RANGE_20mA						21						// in mA

// Number of power line cycles (NPLC)
#define NPLC_MIN						0.01
#define NPLC_MAX						10

//
#define KEI_FIFO_LENGTH					100
#define KEI_RECEIVE_TIME				50						// in ms
#define KEI_READ_DELAY					10						// in ms
#define KEI_CURRENT_MAX					20						// in mA
#define KEI_MEASURE_TIMEOUT				300						// in ms
#define KEI_SYNC_INIT_DELAY				50						// in ms

// Functions prototypes
//
void KEI_SimpleConfig();
void KEI_SetRange(float Current);
bool KEI_ReadData(float* Data);
void KEI_ReceiveData(USART_TypeDef* USARTx);
void KEI_SwitchToSyncWaiting();
bool KEI_Measure(float* Data);
void KEI_SetNPLC(float Value);
void KEI_ResetRxCounter();
void KEI_BufferConfig(Int16U Size);
void KEI_EnableAverage(Int16U Size);
void KEI_EnableMedianFilter();
void KEI_TriggerLinkConfig(Int16U N);
void KEI_Reset();
void KEI_ZeroCorrect();
void KEI_AbortMeasure();

#endif /* CONTROLLER_KEITHLEY6485_H_ */
