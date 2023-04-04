#ifndef __BOARD_H
#define __BOARD_H

#include "stm32f30x.h"

#include "ZwRCC.h"
#include "ZwGPIO.h"
#include "ZwNCAN.h"
#include "ZwTIM.h"
#include "ZwDAC.h"
#include "ZwDMA.h"
#include "ZwADC.h"
#include "ZwEXTI.h"
#include "ZwSCI.h"
#include "ZwIWDG.h"
#include "ZwNFLASH.h"
#include "ZwSPI.h"

// Определения для выходных портов
//
GPIO_PortPinSettingMacro GPIO_LED = {GPIOA, Pin_6}; //
GPIO_PortPinSettingMacro GPIO_LED_EXT = {GPIOB, Pin_0}; //
GPIO_PortPinSettingMacro GPIO_STST_CURRENT = {GPIOA, Pin_4}; //
GPIO_PortPinSettingMacro GPIO_STST_IGTU_CH = {GPIOA, Pin_8}; //
GPIO_PortPinSettingMacro GPIO_IGTU_SEL = {GPIOB, Pin_6}; //
GPIO_PortPinSettingMacro GPIO_IGTU_SI = {GPIOB, Pin_4}; //
GPIO_PortPinSettingMacro GPIO_IGTU_MUX = {GPIOC, Pin_13}; //
GPIO_PortPinSettingMacro GPIO_LCTU_SEL = {GPIOB, Pin_8}; //
GPIO_PortPinSettingMacro GPIO_LCTU_SI = {GPIOB, Pin_5}; //
GPIO_PortPinSettingMacro GPIO_LCTU_MUX = {GPIOC, Pin_14}; //
GPIO_PortPinSettingMacro GPIO_IDIV2 = {GPIOC, Pin_15}; //
GPIO_PortPinSettingMacro GPIO_KEI_PWR = {GPIOA, Pin_5}; //

// Определения для входных портов
//
GPIO_PortPinSettingMacro GPIO_IGTU_SO = {GPIOB, Pin_7}; //
GPIO_PortPinSettingMacro GPIO_LCTU_SO = {GPIOB, Pin_9}; //

// Определения для портов альтернативных функций
//
GPIO_PortPinSettingMacro GPIO_ALT_UART1_TX = {GPIOA, Pin_9};
GPIO_PortPinSettingMacro GPIO_ALT_UART1_RX = {GPIOA, Pin_10};
GPIO_PortPinSettingMacro GPIO_ALT_CAN1_TX = {GPIOA, Pin_12};
GPIO_PortPinSettingMacro GPIO_ALT_CAN1_RX = {GPIOA, Pin_11};
GPIO_PortPinSettingMacro GPIO_ALT_UART2_TX = {GPIOB, Pin_3};
GPIO_PortPinSettingMacro GPIO_ALT_UART2_RX = {GPIOA, Pin_15};

// Определения для портов внешних прерываний
//
EXTI_PortChannelSettingMacro EXTI_IGTU_SO = {EXTI_PB, EXTI_7};
EXTI_PortChannelSettingMacro EXTI_LCTU_SO = {EXTI_PB, EXTI_9};
EXTI_PortChannelSettingMacro EXTI_IGTU_SI = {EXTI_PB, EXTI_4};
EXTI_PortChannelSettingMacro EXTI_LCTU_SI = {EXTI_PB, EXTI_5};

#endif // __BOARD_H
