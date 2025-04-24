/*
 * ApplicationCode.h
 *
 *  Created on: Dec 30, 2023
 *      Author: Xavion
 */

#include "LCD_Driver.h"
#include "Button_Driver.h"
#include "Scheduler.h"
#include "stm32f4xx_hal.h"

#include <stdio.h>


#ifndef INC_APPLICATIONCODE_H_
#define INC_APPLICATIONCODE_H_

#define PlayerOne 0
#define PlayerTwo 1
#define USE_INTERRUPT_FOR_BUTTON 1

void ApplicationInit(void);
void LCD_Visual_Demo(void);
void LCD_startScreen(void);
void LCD_singlePlayerScreen(void);
void LCD_twoPlayerScreen(void);
void LCD_touchedButtonPolling(void);
bool LCD_touchedLeftRight(void);
void displayCurrentDropCol(void);
void moveChipLeftRight(void);
void dropChip(void);
bool checkIfOver(void);
void twoPlayerMode(void);

// Button functions
void appDelay(uint32_t time_in);
void appButtonInit();
void executeButtonPollingRoutine();
void appButton_Int_Init();

#if (COMPILE_TOUCH_FUNCTIONS == 1)
void LCD_Touch_Polling_Demo(void);
#endif // (COMPILE_TOUCH_FUNCTIONS == 1)

#endif /* INC_APPLICATIONCODE_H_ */
