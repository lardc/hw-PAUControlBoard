#ifndef __GLOBAL_H
#define __GLOBAL_H

// Definitions
// 
#define	SCCI_TIMEOUT_TICKS						1000	// Таймаут интерфейса SCCI (в мс)
#define EP_WRITE_COUNT							1		// Количество массивов для записи
#define EP_COUNT								0		// Количество массивов для чтения
#define FEP_COUNT								0		// Количество массивов для чтения типа float
#define VALUES_x_SIZE							20		// Размер массивов
#define ENABLE_LOCKING							FALSE	// Защита NV регистров паролем

// Временные параметры
#define TIME_LED_BLINK							500		// Мигание светодиодом (в мс)
#define TIME_FAULT_LED_BLINK					250		// Мигание светодиодом в состоянии Fault
#define TIME_EXT_LAMP_ON_STATE					500		// Время загорания внешнего индикатора

// Параметры оцифровки
#define ADC_REF_VOLTAGE							3300.0f	// Опорное напряжение (в В)
#define ADC_RESOLUTION							4095	// Разрешение АЦП
#define ADC_I_CHANNEL							1		// Номер аналогового канала оцифровки тока

//
#define DELAY_COMMUTATION						10		// Время коммутации (мс)
#define DELAY_KEI_CONFIG						500		// Время конфигурации Keithley (мс)
#define DELAY_POWER_UP							1500	// Время необходимое для полного запуска Keithley 6485 (мс)

#endif //  __GLOBAL_H
