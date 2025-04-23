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

static bool currentColum[6];
//static bool gameBoardArr[5][6];
static bool currentPlayer;

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
	currentColum[3] = 1;
	currentPlayer = PlayerOne;
	displayCurrentDropCol();
}

void LCD_twoPlayerScreen(void)
{
	twoPlayerScreen();
	currentColum[3] = 1;
	currentPlayer = PlayerOne;
	displayCurrentDropCol();
}

#if COMPILE_TOUCH_FUNCTIONS == 1
void LCD_touchedButtonPolling(void)
{
	startScreen();
	bool buttonChosen = 0;
	while (buttonChosen != 1)
	{
		//startScreen();
		if (returnTouchStateAndLocation(&StaticTouchData) == STMPE811_State_Pressed)
		{
			/* Touch valid */
			//printf("\nX: %03d\nY: %03d\n", StaticTouchData.x, StaticTouchData.y);
			if(StaticTouchData.x >= 20 && StaticTouchData.x <= 200)
			{
				if(StaticTouchData.y >= 70 && StaticTouchData.y <= 120)
				{
					LCD_Clear(0, LCD_COLOR_WHITE);
					LCD_singlePlayerScreen();
					printf("One Player Button Pressed");
					buttonChosen = 1;
				}
				else if(StaticTouchData.y >= 10 && StaticTouchData.y <= 60)
				{
					LCD_Clear(0, LCD_COLOR_WHITE);
					LCD_twoPlayerScreen();
					printf("Two Player Button Pressed");
					buttonChosen = 1;
				}
				else
				{
					printf("Touch Outside of Target Areas");
					//startScreen();
				}
			}
			printf("\nX: %03d\nY: %03d\n", StaticTouchData.x, StaticTouchData.y);
		}
		else
		{
			/* Touch not pressed */
			//printf("Not Pressed\n\n");
			//startScreen();
		}
	}
}

bool LCD_touchedLeftRight(void)
{
	bool sideChosen = 0;
	printf("In LEFT RIGHT");
	LCD_Clear(0, LCD_COLOR_BLACK);
	while (1)
	{
		//startScreen();
		if (returnTouchStateAndLocation(&StaticTouchData) == STMPE811_State_Pressed)
		{
			LCD_Clear(0, LCD_COLOR_WHITE);
			/* Touch valid */
			if(StaticTouchData.x <= 120)
			{
				printf("Left Side chosen");
				printf("\nX: %03d\nY: %03d\n", StaticTouchData.x, StaticTouchData.y);
				sideChosen = 0;
				return sideChosen;
			}
			else if(StaticTouchData.x > 120)
			{
				printf("Right Side chosen");
				printf("\nX: %03d\nY: %03d\n", StaticTouchData.x, StaticTouchData.y);
				sideChosen = 1;
				return sideChosen;
			}
			else
			{
				printf("Something is very wrong");
				printf("\nX: %03d\nY: %03d\n", StaticTouchData.x, StaticTouchData.y);
			}
		}
		else
		{
			/* Touch not pressed */
			printf("Not Pressed\n\n");
			LCD_Clear(0, LCD_COLOR_BLACK);
			HAL_Delay(500);
			LCD_Clear(0, LCD_COLOR_RED);
			HAL_Delay(500);
			//startScreen();
		}
	}
}

//#if COMPILE_TOUCH_FUNCTIONS == 1
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

void displayCurrentDropCol(void)
{
	for(int i=0; i < 6; i++)
	{
		uint16_t XPosFill = 0;
		XPosFill = (45 + (i*25));
		if(currentColum[i] == 1)
		{
			if(currentPlayer == PlayerOne)
			{
				LCD_Draw_Circle_Fill(XPosFill, 100, 8, LCD_COLOR_RED);
			}
			else
			{
				LCD_Draw_Circle_Fill(XPosFill, 100, 8, LCD_COLOR_YELLOW);
			}
		}
		else
		{
			LCD_Draw_Circle_Fill(XPosFill, 100, 8, LCD_COLOR_WHITE);
		}
	}
}

void moveChipLeftRight(void)
{
	bool direction = LCD_touchedLeftRight();
	if(direction == 0) //left
	{
		for(int i=0; i < 6; i++)
		{
			if(currentColum[i] == 1)
			{
				currentColum[i] = 0;
				currentColum[i-1] = 1;
			}
		}
	}
	else //right
	{
		for(int i=0; i < 6; i++)
		{
			if(currentColum[i] == 1)
			{
				currentColum[i] = 0;
				currentColum[i+1] = 1;
			}
		}
	}
	displayCurrentDropCol();
}
#endif // COMPILE_TOUCH_FUNCTIONS

