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

static bool currentColum[7];
static int gameBoardArr[6][7];
static int currentPlayer;
static int winner;
static bool turnComplete;
static bool gameComplete;

void ApplicationInit(void)
{
	initialise_monitor_handles(); // Allows printf functionality
    LTCD__Init();
    LTCD_Layer_Init(0);
    LCD_Clear(0,LCD_COLOR_WHITE);

    appButton_Int_Init();

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
					printf("One Player Button Pressed\n");
					LCD_Clear(0, LCD_COLOR_WHITE);
					LCD_singlePlayerScreen();
					buttonChosen = 1;
				}
				else if(StaticTouchData.y >= 10 && StaticTouchData.y <= 60)
				{
					printf("Two Player Button Pressed\n");
					LCD_Clear(0, LCD_COLOR_WHITE);
					LCD_twoPlayerScreen();
					twoPlayerMode();
					buttonChosen = 1;
				}
				else
				{
					printf("Touch Outside of Target Areas\n");
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
	printf("In LEFT RIGHT\n");
	//LCD_Clear(0, LCD_COLOR_BLACK);
	while (1)
	{
		//startScreen();
		if (returnTouchStateAndLocation(&StaticTouchData) == STMPE811_State_Pressed)
		{
			//LCD_Clear(0, LCD_COLOR_WHITE);
			/* Touch valid */
			if(StaticTouchData.x <= 120)
			{
				printf("Left Side chosen\n");
				printf("\nX: %03d\nY: %03d\n", StaticTouchData.x, StaticTouchData.y);
				sideChosen = 0;
				return sideChosen;
			}
			else if(StaticTouchData.x > 120)
			{
				printf("Right Side chosen\n");
				printf("\nX: %03d\nY: %03d\n", StaticTouchData.x, StaticTouchData.y);
				sideChosen = 1;
				return sideChosen;
			}
			else
			{
				printf("Something is very wrong\n");
				printf("\nX: %03d\nY: %03d\n", StaticTouchData.x, StaticTouchData.y);
			}
		}
		//else
		//{
			/* Touch not pressed */
		//	printf("Not Pressed\n\n");
		//	LCD_Clear(0, LCD_COLOR_BLACK);
		//	HAL_Delay(500);
		//	LCD_Clear(0, LCD_COLOR_RED);
		//	HAL_Delay(500);
			//startScreen();
		//}
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
	for(int i=0; i < 7; i++)
	{
		uint16_t XPosFill = 0;
		XPosFill = (45 + (i*25));
		if(currentColum[i] == 1)
		{
			if(currentPlayer == PlayerOne)
			{
				LCD_Draw_Circle_Fill(XPosFill, 100, 8, LCD_COLOR_RED);
			}
			else if(currentPlayer == PlayerTwo)
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
		for(int i=0; i < 7; i++)
		{
			if(currentColum[i] == 1 && i != 0)
			{
				currentColum[i] = 0;
				currentColum[i-1] = 1;
			}
		}
	}
	else //right
	{
		for(int i=0; i < 7; i++)
		{
			if(currentColum[i] == 1 && i != 6)
			{
				currentColum[i] = 0;
				currentColum[i+1] = 1;
				i++;
			}
		}
	}
	displayCurrentDropCol();
}

void dropChip(void)
{
	bool chipDropped = 0;
	bool fullCol = 1;
	int currentColIndx = 0;
	uint16_t XPosFill = 0;
	uint16_t YPosFill = 0;

	for(int i=0; i < 7; i++)
	{
		if(currentColum[i] == 1)
		{
			currentColIndx = i;
			XPosFill = (45 + (i*25));
		}
	}
	while(chipDropped != 1)
	{
		for(int j=0; j < 6; j++)
		{
			if(gameBoardArr[5-j][currentColIndx] == 0)
			{
				fullCol = 0;
				YPosFill = (146 + ((5-j)*26));
				if(currentPlayer == PlayerOne)
				{
					LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_RED);
					LCD_Draw_Circle_Fill(XPosFill, 100, 8, LCD_COLOR_YELLOW);
					currentPlayer = PlayerTwo;
					gameBoardArr[5-j][currentColIndx] = 1;
					chipDropped = 1;
					break;
				}
				else if(currentPlayer == PlayerTwo)
				{
					LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
					LCD_Draw_Circle_Fill(XPosFill, 100, 8, LCD_COLOR_RED);
					currentPlayer = PlayerOne;
					gameBoardArr[5-j][currentColIndx] = 2;
					chipDropped = 1;
					break;
				}
				else
				{
					chipDropped = 1;
					break;
				}
			}
		}
		if(fullCol)
		{
			chipDropped = 1;
		}
	}
}

bool checkIfOver(void)
{
	bool gameOver = 0;
	bool full = 1;
	for(int i=0; i < 6; i++)
	{
		for(int j=0; j < 7; j++)
		{
			int compChip = gameBoardArr[i][j];
			if(compChip != 0)
			{
				if(j <= 3 && (compChip == gameBoardArr[i][j+1]) &&
					(compChip == gameBoardArr[i][j+2]) &&
					(compChip == gameBoardArr[i][j+3]))
				{
					printf("Triggered Horizontal\n");
					currentPlayer = 3; // This prevents dropChip effectively disabling the button
					winner = compChip;
					turnComplete = 1;
					gameOver = 1;
					break;
				}
				else if(i <= 2 && (compChip == gameBoardArr[i+1][j]) &&
						(compChip == gameBoardArr[i+2][j]) &&
						(compChip == gameBoardArr[i+3][j]))
				{
					printf("Triggered Vertical\n");
					currentPlayer = 3; // This prevents dropChip effectively disabling the button
					winner = compChip;
					turnComplete = 1;
					gameOver = 1;
					break;
				}
				else if(j <= 3 && i <= 2 && (compChip == gameBoardArr[i+1][j+1]) &&
						(compChip == gameBoardArr[i+2][j+2]) &&
						(compChip == gameBoardArr[i+3][j+3]))
				{
					printf("Triggered Down Right\n");
					currentPlayer = 3; // This prevents dropChip effectively disabling the button
					winner = compChip;
					turnComplete = 1;
					gameOver = 1;
					break;
				}
				else if(j <= 3 && i >= 3 && (compChip == gameBoardArr[i-1][j+1]) &&
						(compChip == gameBoardArr[i-2][j+2]) &&
						(compChip == gameBoardArr[i-3][j+3]))
				{
					printf("Triggered Up Right\n");
					currentPlayer = 3; // This prevents dropChip effectively disabling the button
					winner = compChip;
					turnComplete = 1;
					gameOver = 1;
					break;
				}
			}
			else
			{
				full = 0;
			}
		}
	}

	if(full)
	{
		printf("Triggered Tie\n");
		currentPlayer = 3;
		winner = 0;
		turnComplete = 1;
		gameOver = 1;
	}

	return gameOver;
}

void twoPlayerMode(void)
{
	while(gameComplete != 1)
	{
		turnComplete = 0;
		while(turnComplete != 1)
		{
			//end = checkIfOver();
			moveChipLeftRight();
		}
	}
	if(winner == 1)
	{
		LCD_Clear(0,LCD_COLOR_RED);
		LCD_SetTextColor(LCD_COLOR_BLACK);
		LCD_SetFont(&Font16x24);

		LCD_DisplayChar(68,160,'R');
		LCD_DisplayChar(83,160,'e');
		LCD_DisplayChar(96,160,'d');

		LCD_DisplayChar(113,160,'W');
		LCD_DisplayChar(123,160,'i');
		LCD_DisplayChar(131,160,'n');
		LCD_DisplayChar(141,160,'s');
		LCD_DisplayChar(149,160,'!');
	}
	else if(winner == 2)
	{
		LCD_Clear(0,LCD_COLOR_YELLOW);
		LCD_SetTextColor(LCD_COLOR_BLACK);
		LCD_SetFont(&Font16x24);

		LCD_DisplayChar(58,160,'Y');
		LCD_DisplayChar(73,160,'e');
		LCD_DisplayChar(83,160,'l');
		LCD_DisplayChar(90,160,'l');
		LCD_DisplayChar(98,160,'o');
		LCD_DisplayChar(111,160,'w');

		LCD_DisplayChar(130,160,'W');
		LCD_DisplayChar(143,160,'i');
		LCD_DisplayChar(151,160,'n');
		LCD_DisplayChar(163,160,'s');
		LCD_DisplayChar(170,160,'!');
	}
	else
	{
		LCD_Clear(0,LCD_COLOR_WHITE);
		LCD_SetTextColor(LCD_COLOR_BLACK);
		LCD_SetFont(&Font16x24);

		LCD_DisplayChar(58,160,'T');
		LCD_DisplayChar(73,160,'i');
		LCD_DisplayChar(83,160,'e');
	}
}


//below is button stuff

//#if USE_INTERRUPT_FOR_BUTTON == RESET
//void appButtonInit()
//{
//	Button_Init();
//}
//
//void executeButtonPollingRoutine()
//{
//	if(Button_Pressed() == 1)
//	{
//		TurnOnLED(GPIO_PIN_NUM_13);
//	}else
//	{
//		TurnOffLED(GPIO_PIN_NUM_13);
//	}
//}
//
//#else
void appButton_Int_Init()
{
	Button_Int_Init();
}
//#endif

void EXTI0_IRQHandler()
{
	IRQ_INTR_Disable(EXTI0_IRQ_NUMBER);
	//ToggleLED(GPIO_PIN_NUM_13);
	//EXTI_INTRP_Clear(Button_Pin_Num);
	addSchedulerEvent(DROP_CHIP);       //MAYBE NEED THIS?
	dropChip();
	turnComplete = 1;
	gameComplete = checkIfOver();
	__HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_0);
	IRQ_INTR_Enable(EXTI0_IRQ_NUMBER);
}


#endif // COMPILE_TOUCH_FUNCTIONS

