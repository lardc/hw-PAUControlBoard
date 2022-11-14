#ifndef CONTROLLER_KEITHLEY6485_H_
#define CONTROLLER_KEITHLEY6485_H_

// Includes
//
#include "Board.h"

// Definitions
//
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
#define NPLC_MAX						5
#define NPLC_TIME_DEF					60
#define PLC_TIME						60						// in ms
#define PLC_TIME_OFFSET					7.5						// in ms
#define PLC_TIME_COEFFICIENT			2

//
#define KEI_FIFO_LENGTH					100
#define KEI_RECEIVE_TIME				25						// in ms
#define KEI_CURRENT_MAX					20						// in mA

// Functions prototypes
//
void KEI_Config();
void KEI_SetRange(float Current);
bool KEI_ReadData(float* Data);
void KEI_ReceiveData(USART_TypeDef* USARTx);
void KEI_SwitchToSyncWaiting();
bool KEI_Measure(float* Data);
void KEI_SetADCRate(float Time);
void KEI_ResetRxConuter();

#endif /* CONTROLLER_KEITHLEY6485_H_ */
