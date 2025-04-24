/*
 * InterruptControl.h
 *
 *  Created on: Feb 18, 2025
 *      Author: shapi
 */

//#include "STM32F429i.h"

#ifndef INTERRUPTCONTROL_H_
#define INTERRUPTCONTROL_H_

#include <stdint.h>

//NVIC Registers
#define NVIC_ISER0   ((volatile uint32_t*) 0xE000E100)
#define NVIC_ICER0   ((volatile uint32_t*) 0xE000E180)
#define NVIC_ISPR0   ((volatile uint32_t*) 0xE000E200)
#define NVIC_ICPR0   ((volatile uint32_t*) 0xE000E280)
#define NVIC_IABR0   ((volatile uint32_t*) 0xE000E300)
#define NVIC_IPR0   ((volatile uint32_t*) 0xE000E400)
#define NVIC_STIR   ((volatile uint32_t*) 0xE000EF00)

#define EXTI0_IRQ_NUMBER 6

void IRQ_INTR_Enable(uint8_t IRQ_num);
void IRQ_INTR_Disable(uint8_t IRQ_num);
void IRQ_INTRP_Clear(uint8_t IRQ_num);
void IRQ_INTRP_Set(uint8_t IRQ_num);
//void EXTI_INTRP_Clear(uint8_t Pin_num);

#endif /* INTERRUPTCONTROL_H_ */
