#ifndef CONTROLLER_KEITHLEY6485_H_
#define CONTROLLER_KEITHLEY6485_H_

// Includes
//
#include "Board.h"

// Definitions
//
// Current ranges
#define RANGE_2nA						2e-6					// in mA
#define RANGE_20nA						20e-6					// in mA
#define RANGE_200nA						200e-6					// in mA
#define RANGE_2uA						2e-3					// in mA
#define RANGE_20uA						20e-3					// in mA
#define RANGE_200uA						200e-3					// in mA
#define RANGE_2mA						2						// in mA
#define RANGE_20mA						20						// in mA

// Number of power line cycles (NPLC)
#define NPLC_MIN						0.01
#define NPLC_MAX						5
#define NPLC_DEF						1
#define PLC_TIME						20						// in ms
#define PLC_TIME_OFFSET					PLC_TIME * 0.1

//
#define KEI_FIFO_LENGTH					20
#define KEI_RECEIVE_TIME				2						// in ms

// Functions prototypes
//
void KEI_Config();
void KEI_SetRange(float Current);
float KEI_ReadData();
void KEI_ReceiveData(USART_TypeDef* USARTx);
void KEI_SwitchToSyncWaiting();
float KEI_Measure();

#endif /* CONTROLLER_KEITHLEY6485_H_ */
