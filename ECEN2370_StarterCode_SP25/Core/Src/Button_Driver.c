/*
 * Button_Driver.c
 *
 *  Created on: Feb 11, 2025
 *      Author: shapi
 */

#include "Button_Driver.h"

void Button_Init()
{
	GPIO_InitTypeDef Button_Conf = {0};
	Button_Conf.Pin = GPIO_PIN_0;
	Button_Conf.Mode = GPIO_MODE_OUTPUT_PP;
	Button_Conf.Speed = GPIO_SPEED_FREQ_HIGH;
	Button_Conf.Pull = GPIO_NOPULL;

	__HAL_RCC_GPIOA_CLK_ENABLE();

	HAL_GPIO_Init(GPIOA, &Button_Conf);
}

bool Button_Pressed()
{
	if(HAL_GPIO_ReadPin(GPIOA, Button_Pin_Num) == GPIO_PIN_SET)
	{
		return true;
	}else
	{
		return false;
	}
}

void Button_Int_Init()
{
	GPIO_InitTypeDef Button_Conf = {0};
	Button_Conf.Pin = GPIO_PIN_0;
	Button_Conf.Mode = GPIO_MODE_IT_FALLING;
	Button_Conf.Speed = GPIO_SPEED_FREQ_HIGH;
	Button_Conf.Pull = GPIO_NOPULL;
	//Button_Conf.PinInterrupt = INTR_FALL_RISE_EDGE;

	//NEED TO FIGURE OUT THE INTERUPT SHIT HERE

	__HAL_RCC_GPIOA_CLK_ENABLE();

	HAL_GPIO_Init(GPIOA, &Button_Conf);

	//NVIC_GPIO_INTRP_EnDis(EXTI0_IRQ_NUMBER, ENABLE);
	//IRQ_INTR_Enable(EXTI0_IRQ_NUMBER);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
}

