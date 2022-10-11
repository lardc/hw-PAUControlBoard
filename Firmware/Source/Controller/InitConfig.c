#include "InitConfig.h"
#include "Board.h"
#include "SysConfig.h"
#include "BCCIxParams.h"

// Functions
//
Boolean INITCFG_ConfigSystemClock()
{
	return RCC_PLL_HSE_Config(QUARTZ_FREQUENCY, PREDIV_4, PLL_14);
}
//------------------------------------------------

void INITCFG_ConfigIO()
{
	// Включение тактирования портов
	RCC_GPIO_Clk_EN(PORTA);
	RCC_GPIO_Clk_EN(PORTB);
	RCC_GPIO_Clk_EN(PORTC);
	
	// Выходы
	GPIO_InitPushPullOutput(GPIO_LED);
	GPIO_InitPushPullOutput(GPIO_LED_EXT);
	GPIO_InitPushPullOutput(GPIO_STST_CURRENT);
	GPIO_InitPushPullOutput(GPIO_STST_IGTU_CH);
	GPIO_InitPushPullOutput(GPIO_IGTU_MUX);
	GPIO_InitPushPullOutput(GPIO_LCTU_MUX);
	GPIO_InitPushPullOutput(GPIO_IDIV2);
	//
	GPIO_InitOpenDrainOutput(GPIO_IGTU_SEL, NoPull);
	GPIO_InitOpenDrainOutput(GPIO_LCTU_SEL, NoPull);
	GPIO_InitOpenDrainOutput(GPIO_IGTU_SI, NoPull);
	GPIO_InitOpenDrainOutput(GPIO_LCTU_SI, NoPull);


	// Начальная установка состояний выводов
	GPIO_SetState(GPIO_LED, false);
	GPIO_SetState(GPIO_LED_EXT, false);
	GPIO_SetState(GPIO_STST_CURRENT, false);
	GPIO_SetState(GPIO_STST_IGTU_CH, false);
	GPIO_SetState(GPIO_IGTU_SEL, false);
	GPIO_SetState(GPIO_IGTU_MUX, false);
	GPIO_SetState(GPIO_IGTU_SI, false);
	GPIO_SetState(GPIO_LCTU_SEL, false);
	GPIO_SetState(GPIO_LCTU_MUX, false);
	GPIO_SetState(GPIO_LCTU_SI, false);
	GPIO_SetState(GPIO_IDIV2, false);

	// Альтернативные функции
	GPIO_InitAltFunction(GPIO_ALT_UART1_RX, AltFn_7);
	GPIO_InitAltFunction(GPIO_ALT_UART1_TX, AltFn_7);
	GPIO_InitAltFunction(GPIO_ALT_CAN1_RX, AltFn_9);
	GPIO_InitAltFunction(GPIO_ALT_CAN1_TX, AltFn_9);
	GPIO_InitAltFunction(GPIO_ALT_SPI1_CLK, AltFn_5);
	GPIO_InitAltFunction(GPIO_ALT_SPI1_DAT, AltFn_5);
	GPIO_InitAltFunction(GPIO_ALT_UART2_TX, AltFn_7);
	GPIO_InitAltFunction(GPIO_ALT_UART2_RX, AltFn_7);
}

//------------------------------------------------

void INITCFG_ConfigExtInterrupt()
{
	EXTI_Init(EXTI_IGTU_SO, RISE_TRIG);
	EXTI_Init(EXTI_LCTU_SO, RISE_TRIG);
	EXTI_Init(EXTI_IGTU_SI, FALL_TRIG);
	EXTI_Init(EXTI_LCTU_SI, FALL_TRIG);
}
//------------------------------------------------

void INITCFG_ConfigUART()
{
	USART_Init(USART1, SYSCLK, USART_BAUDRATE);
	USART_Recieve_Interupt(USART1, 0, true);

	USART_Init(USART2, SYSCLK, KEITHLEY_RS232_BAUDRATE);
	USART_Recieve_Interupt(USART2, 0, true);
}
//------------------------------------------------

void INITCFG_ConfigTimer7()
{
	TIM_Clock_En(TIM_7);
	TIM_Config(TIM7, SYSCLK, TIMER7_uS);
	TIM_Interupt(TIM7, 2, true);
	TIM_Start(TIM7);
}
//------------------------------------------------

void INITCFG_ConfigWatchDog()
{
	IWDG_Config();
	IWDG_ConfigureSlowUpdate();
}
//------------------------------------------------
