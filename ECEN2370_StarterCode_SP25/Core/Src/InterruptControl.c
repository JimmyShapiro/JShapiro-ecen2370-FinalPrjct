/*
 * InterruptControl.c
 *
 *  Created on: Feb 18, 2025
 *      Author: shapi
 */

#include "InterruptControl.h"

void IRQ_INTR_Enable(uint8_t IRQ_num)
{
	if(IRQ_num < 32)
	{
		*NVIC_ISER0 |= (1 << IRQ_num);
	}
}

void IRQ_INTR_Disable(uint8_t IRQ_num)
{
	if(IRQ_num < 32)
	{
		*NVIC_ISER0 |= (1 << IRQ_num);
	}
}

void IRQ_INTRP_Clear(uint8_t IRQ_num)
{
	if(IRQ_num < 32)
	{
		*NVIC_ICPR0 |= (1 << IRQ_num);
	}
}

//void EXTI_INTRP_Clear(uint8_t Pin_num)
//{
	//EXTI->PR |= (1 << Pin_num);
	//__HAL_GPIO_EXTI_CLEAR_FLAG(Pin_num);
//}

