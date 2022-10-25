#ifndef __DEV_OBJ_DIC_H
#define __DEV_OBJ_DIC_H

// Команды
#define ACT_ENABLE_POWER				1	// Включение блока
#define ACT_DISABLE_POWER				2	// Выключение блока
#define ACT_CLR_FAULT					3	// Очистка всех fault
#define ACT_CLR_WARNING					4	// Очистка всех warning
//
#define ACT_DBG_EXTERNAL_LED			10	// Проверка работы внешнего индикатора
#define ACT_DBG_ST_CURRENT				11	// Запус тестового тока
#define ACT_DBG_ST_CH_IGTU				12	// Проверка канала мультиплексора IGTU
#define ACT_DBG_SWITCH_AMUX_IGTU		13	// Переключить аналоговый мультиплексор в положение IGTU
#define ACT_DBG_SWITCH_AMUX_LCTU		14	// Переключить аналоговый мультиплексор в положение LCTU
#define ACT_DBG_SWITCH_SMUX_IGTU		15	// Переключить мультиплексор синхронизации в положение IGTU
#define ACT_DBG_SWITCH_SMUX_LCTU		16	// Переключить мультиплексор синхронизации в положение LCTU
#define ACT_DBG_CURRENT_DIVIDER			17	// 1 - включить делитель тока, 0 - выключить делитель тока
#define ACT_DBG_GENERATE_SYNC_KEI		18	// Формирование импульса синхронизации для Keithley 6485
#define ACT_DBG_SEND_DATA_TO_KEI		19	// Отправить данные в Keithley 6485
//
#define ACT_CONFIG						100	// Сконфигурировать блок
#define ACT_RUN_SELF_TEST				101	// Запуск самодиагностики

#define ACT_SAVE_TO_ROM					200	// Сохранение пользовательских данных во FLASH процессора
#define ACT_RESTORE_FROM_ROM			201	// Восстановление данных из FLASH
#define ACT_RESET_TO_DEFAULT			202	// Сброс DataTable в состояние по умолчанию

#define ACT_BOOT_LOADER_REQUEST			320	// Перезапуск процессора с целью перепрограммирования
// -----------------------------

// Регистры
// Сохраняемые регистры
#define REG_SFTST_R_SHUNT				0	// Сопротивление шунта системы самодиагностики (Ом)
#define REG_SFTST_I_ALOWED_ERR			1	// Допустимая ошибка измеренного значения тока (%)
#define REG_SFTST_KEI_I_MUX_OFF			2	// Эталонное значение тока с отключенным MUX (мА)
#define REG_SFTST_KEI_I_MUX_IGTU		3	// Эталонное значение тока при MUX в положении IGTU (мА)
#define REG_SFTST_KEI_I_MUX_LCTU		4	// Эталонное значение тока при MUX в положении LCTU без делителя (мА)
#define REG_SFTST_KEI_I_MUX_LCTU_DIV	5	// Эталонное значение тока при MUX в положении LCTU с делителем (мА)
//
#define REG_I_DIV_FACTOR				6	// Коэффициент деления делителя тока


// Несохраняемы регистры чтения-записи
#define REG_CHANNEL						128	// 0 - канал не выбран, 1 - LCTU, 2 - IGTU
#define REG_RANGE						129	// Выбор дипазона измерения тока (mA)
#define REG_MEASUREMENT_TIME			130	// Время преобразования (мс)
//
#define REG_DBG							150	// Отладочный регистр


// Регистры только чтение
#define REG_DEV_STATE					192	// Регистр состояния
#define REG_FAULT_REASON				193	// Регистр Fault
#define REG_DISABLE_REASON				194	// Регистр Disable
#define REG_WARNING						195	// Регистр Warning
#define REG_PROBLEM						196	// Регистр Problem
#define REG_OP_RESULT					197	// Регистр результата операции
#define REG_SELF_TEST_OP_RESULT			198	// Регистр результата самотестирования
#define REG_SUB_STATE					199	// Регистр вспомогательного состояния
// -----------------------------
#define REG_FWINFO_SLAVE_NID			256	// Device CAN slave node ID
#define REG_FWINFO_MASTER_NID			257	// Device CAN master node ID (if presented)
// 258 - 259
#define REG_FWINFO_STR_LEN				260	// Length of the information string record
#define REG_FWINFO_STR_BEGIN			261	// Begining of the information string record


// Operation results
#define OPRESULT_NONE					0	// No information or not finished
#define OPRESULT_OK						1	// Operation was successful
#define OPRESULT_FAIL					2	// Operation failed

//  Fault and disable codes
#define DF_NONE							0
#define DF_TEST_I_SET					1
#define DF_TEST_I_MEASURE				2

// Problem
#define PROBLEM_NONE					0

//  Warning
#define WARNING_NONE					0

//  User Errors
#define ERR_NONE						0
#define ERR_CONFIGURATION_LOCKED		1	//  Устройство защищено от записи
#define ERR_OPERATION_BLOCKED			2	//  Операция не может быть выполнена в текущем состоянии устройства
#define ERR_DEVICE_NOT_READY			3	//  Устройство не готово для смены состояния
#define ERR_WRONG_PWD					4	//  Неправильный ключ

// Endpoints
#define EP16_WR							1	// Ендпоинт для отладки взаимодействия с Keithley 6485

#endif //  __DEV_OBJ_DIC_H
