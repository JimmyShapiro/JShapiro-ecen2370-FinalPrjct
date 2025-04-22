/*
 * ApplicationCode.c
 *
 *  Created on: Dec 30, 2023 (updated 11/12/2024) Thanks Donavon! 
 *      Author: Xavion
 */

#include "ApplicationCode.h"

/* Static variables */


extern void initialise_monitor_handles(void); 

#if COMPILE_TOUCH_FUNCTIONS == 1
static STMPE811_TouchData StaticTouchData;
#endif // COMPILE_TOUCH_FUNCTIONS

void ApplicationInit(void)
{
	initialise_monitor_handles(); // Allows printf functionality
    LTCD__Init();
    LTCD_Layer_Init(0);
    LCD_Clear(0,LCD_COLOR_WHITE);

    #if COMPILE_TOUCH_FUNCTIONS == 1
	InitializeLCDTouch();

	// This is the orientation for the board to be direclty up where the buttons are vertically above the screen
	// Top left would be low x value, high y value. Bottom right would be low x value, low y value.
	StaticTouchData.orientation = STMPE811_Orientation_Portrait_2;

	#endif // COMPILE_TOUCH_FUNCTIONS
}

void LCD_Visual_Demo(void)
{
	visualDemo();
}

void LCD_startScreen(void)
{
	startScreen();
}

void LCD_singlePlayerScreen(void)
{
	singlePlayerScreen();
}

void LCD_twoPlayerScreen(void)
{
	twoPlayerScreen();
}

void LCD_touchedButtonPolling(void)
{
	bool buttonChosen = 0;
	while (buttonChosen != 1)
	{
		startScreen();
		if (returnTouchStateAndLocation(&StaticTouchData) == STMPE811_State_Pressed)
		{
			/* Touch valid */
			printf("\nX: %03d\nY: %03d\n", StaticTouchData.x, StaticTouchData.y);
			if(StaticTouchData.x >= 20 && StaticTouchData.x <= 20)
			{
				if(StaticTouchData.y >= 200 && StaticTouchData.x <= 240)
				{
					singlePlayerScreen();
					printf("One Player Button Pressed");
					buttonChosen = 1;
				}
				else if(StaticTouchData.y >= 260 && StaticTouchData.x <= 300)
				{
					twoPlayerScreen();
					printf("Two Player Button Pressed");
					buttonChosen = 1;
				}
				else
				{
					printf("Touch Outside of Target Areas");
					startScreen();
				}
			}
		}
		else
		{
			/* Touch not pressed */
			printf("Not Pressed\n\n");
			startScreen();
		}
	}
}

#if COMPILE_TOUCH_FUNCTIONS == 1
void LCD_Touch_Polling_Demo(void)
{
	LCD_Clear(0,LCD_COLOR_GREEN);
	while (1) {
		/* If touch pressed */
		if (returnTouchStateAndLocation(&StaticTouchData) == STMPE811_State_Pressed) {
			/* Touch valid */
			printf("\nX: %03d\nY: %03d\n", StaticTouchData.x, StaticTouchData.y);
			LCD_Clear(0, LCD_COLOR_RED);
		} else {
			/* Touch not pressed */
			printf("Not Pressed\n\n");
			LCD_Clear(0, LCD_COLOR_GREEN);
		}
	}
}
#endif // COMPILE_TOUCH_FUNCTIONS

