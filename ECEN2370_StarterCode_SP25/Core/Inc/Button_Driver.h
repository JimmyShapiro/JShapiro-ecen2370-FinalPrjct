/*
 * Button_Driver.h
 *
 *  Created on: Feb 11, 2025
 *      Author: shapi
 */

#ifndef BUTTON_DRIVER_H_
#define BUTTON_DRIVER_H_

#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "InterruptControl.h"

#define Button_Port_Val  GPIOA
#define Button_Pin_Num   GPIO_PIN_0

void Button_Init();
bool Button_Pressed();
void Button_Int_Init();

#endif /* BUTTON_DRIVER_H_ */

