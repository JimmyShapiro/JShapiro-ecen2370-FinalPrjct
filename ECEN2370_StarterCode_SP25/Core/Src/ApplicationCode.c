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
static int currentGameMode;
static int currentPlayer;
static int winner;
static bool turnComplete;
static bool gameComplete;
static uint8_t redWinCnt;
static uint8_t yellowWinCnt;
static uint32_t gameStrtTime;
static uint32_t gameEndTime;

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

void printGameBoard(void)
{
    printf("Board:\n");
    for (int i = 0; i < 6; i++)  // Rows
    {
        for (int j = 0; j < 7; j++)  // Columns
        {
            printf("%d ", gameBoardArr[i][j]);
        }
        printf("\n");  // Newline after each row
    }
}

void LCD_singlePlayerScreen(void)
{
	singlePlayerScreen();
	currentGameMode = ONEPLAYER;
	currentColum[3] = 1;
	currentPlayer = PlayerOne;
	displayCurrentDropCol();
}

void LCD_twoPlayerScreen(void)
{
	twoPlayerScreen();
	currentGameMode = TWOPLAYER;
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
					onePlayerMode();
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
	while (gameComplete != 1)
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
	}
	return 0;
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

void displayCurrentDropColOnePlayer(void)
{
	for(int i=0; i < 7; i++)
	{
		uint16_t XPosFill = 0;
		XPosFill = (45 + (i*25));
		if(currentColum[i] == 1)
		{
			LCD_Draw_Circle_Fill(XPosFill, 100, 8, LCD_COLOR_RED);
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

void moveChipLeftRightOnePlayer(void)
{

	if(currentPlayer == PlayerOne)
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
		displayCurrentDropColOnePlayer();
	}
	else
	{
		currentPlayer = PlayerTwo;
	}
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
				gameBoardArr[5-j][currentColIndx] = 1;
				if(currentPlayer == PlayerOne)
				{
					LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_RED);

					if(currentGameMode == TWOPLAYER)
					{
						LCD_Draw_Circle_Fill(XPosFill, 100, 8, LCD_COLOR_YELLOW);
						currentPlayer = PlayerTwo;
					}
					else
					{
						AIplayer();
					}

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
	//bool gameOver = 0;
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
					gameEndTime = HAL_GetTick();
					currentPlayer = 3; // This prevents dropChip effectively disabling the button
					winner = compChip;
					turnComplete = 1;
					gameComplete = 1;
					break;
				}
				else if(i <= 2 && (compChip == gameBoardArr[i+1][j]) &&
						(compChip == gameBoardArr[i+2][j]) &&
						(compChip == gameBoardArr[i+3][j]))
				{
					printf("Triggered Vertical\n");
					gameEndTime = HAL_GetTick();
					currentPlayer = 3; // This prevents dropChip effectively disabling the button
					winner = compChip;
					turnComplete = 1;
					gameComplete = 1;
					break;
				}
				else if(j <= 3 && i <= 2 && (compChip == gameBoardArr[i+1][j+1]) &&
						(compChip == gameBoardArr[i+2][j+2]) &&
						(compChip == gameBoardArr[i+3][j+3]))
				{
					printf("Triggered Down Right\n");
					gameEndTime = HAL_GetTick();
					currentPlayer = 3; // This prevents dropChip effectively disabling the button
					winner = compChip;
					turnComplete = 1;
					gameComplete = 1;
					break;
				}
				else if(j <= 3 && i >= 3 && (compChip == gameBoardArr[i-1][j+1]) &&
						(compChip == gameBoardArr[i-2][j+2]) &&
						(compChip == gameBoardArr[i-3][j+3]))
				{
					printf("Triggered Up Right\n");
					gameEndTime = HAL_GetTick();
					currentPlayer = 3; // This prevents dropChip effectively disabling the button
					winner = compChip;
					turnComplete = 1;
					gameComplete = 1;
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
		gameEndTime = HAL_GetTick();
		currentPlayer = 3;
		winner = 0;
		turnComplete = 1;
		gameComplete = 1;
	}

	return gameComplete;
}

void twoPlayerMode(void)
{
	gameStrtTime = HAL_GetTick();
	// printf("Start Time: %lu ", gameStrtTime);
	while(gameComplete != 1)
	{
		turnComplete = 0;
		while(turnComplete != 1)
		{
			moveChipLeftRight();
		}
	}
	if(winner == 1)
	{
		// printf("Start Time: %lu ", gameStrtTime);
		// printf("End Time: %lu ", gameEndTime);

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

		redWinCnt += 1;

		HAL_Delay(2000);
		ScoreBoardScreen();
		pollingForNewGame();
	}
	else if(winner == 2)
	{
		// printf("Start Time: %lu ", gameStrtTime);
		// printf("End Time: %lu ", gameEndTime);

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

		yellowWinCnt += 1;

		HAL_Delay(2000);
		ScoreBoardScreen();
		pollingForNewGame();
	}
	else
	{
		// printf("Start Time: %lu ", gameStrtTime);
		// printf("End Time: %lu ", gameEndTime);

		LCD_Clear(0,LCD_COLOR_WHITE);
		LCD_SetTextColor(LCD_COLOR_BLACK);
		LCD_SetFont(&Font16x24);

		LCD_DisplayChar(58,160,'T');
		LCD_DisplayChar(73,160,'i');
		LCD_DisplayChar(83,160,'e');

		HAL_Delay(2000);
		ScoreBoardScreen();
		pollingForNewGame();
	}
}

void ScoreBoardScreen(void)
{
	LCD_Clear(0,LCD_COLOR_WHITE);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font16x24);

	LCD_DisplayChar(58,100,'R');
	LCD_DisplayChar(73,100,'e');
	LCD_DisplayChar(86,100,'d');
	LCD_DisplayChar(95,100,':');

	char Rbuff[8] = {0};
	sprintf(Rbuff, "%d", redWinCnt);
	//LCD_DisplayChar(140, 100, Rbuff[0]);

	for (int i = 0; Rbuff[i] != '\0'; i++)
	{
		LCD_DisplayChar(140 + (i * 10), 100, Rbuff[i]);
	}

	LCD_DisplayChar(58,120,'Y');
	LCD_DisplayChar(73,120,'e');
	LCD_DisplayChar(83,120,'l');
	LCD_DisplayChar(90,120,'l');
	LCD_DisplayChar(98,120,'o');
	LCD_DisplayChar(112,120,'w');
	LCD_DisplayChar(124,120,':');

	char Ybuff[8]  = {0};
	sprintf(Ybuff, "%d", yellowWinCnt);
	//LCD_DisplayChar(140,120, Ybuff[0]);

	for (int i = 0; Ybuff[i] != '\0'; i++)
	{
		LCD_DisplayChar(140 + (i * 10), 120, Ybuff[i]);
	}

	LCD_Draw_Box_Fill(20, 260, 200, 40, LCD_COLOR_GREY);

	LCD_DisplayChar(66,275,'N');
	LCD_DisplayChar(79,275,'e');
	LCD_DisplayChar(94,275,'w');

	LCD_DisplayChar(120,275,'G');
	LCD_DisplayChar(135,275,'a');
	LCD_DisplayChar(149,275,'m');
	LCD_DisplayChar(165,275,'e');

	LCD_DisplayChar(58,140,'T');
	LCD_DisplayChar(66,140,'i');
	LCD_DisplayChar(76,140,'m');
	LCD_DisplayChar(90,140,'e');
	LCD_DisplayChar(100,140,':');

	uint32_t gameTime = 0;
	gameTime = (gameEndTime - gameStrtTime) / 1000;
	char Tbuff[8]  = {0};
	sprintf(Tbuff, "%lu", gameTime);
	//LCD_DisplayChar(140,140, Tbuff[0]);

	for (int i = 0; Tbuff[i] != '\0'; i++)
	{
		LCD_DisplayChar(140 + (i * 10), 140, Tbuff[i]);
	}
}

void pollingForNewGame(void)
{
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
				if(StaticTouchData.y >= 10 && StaticTouchData.y <= 60)
				{
					printf("New Game Button Pressed\n");
					LCD_Clear(0,LCD_COLOR_WHITE);
					for(int j = 0; j < 6; j++)
					{
						for(int i = 0; i < 7; i++)
						{
							currentColum[i] = 0;
							gameBoardArr[j][i] = 0;
						}
					}
					//LCD_twoPlayerScreen();
					buttonChosen = 1;
					gameComplete = 0;
					if(currentGameMode == ONEPLAYER)
					{
						LCD_singlePlayerScreen();
						onePlayerMode();
					}
					else
					{
						LCD_twoPlayerScreen();
						twoPlayerMode();
					}
				}
				else
				{
					printf("Touch Outside of Target Areas\n");
					//startScreen();
				}
			}
			printf("\nX: %03d\nY: %03d\n", StaticTouchData.x, StaticTouchData.y);
		}
	}
}

void AIplayer(void)
{
	uint16_t XPosFill = 0;
	uint16_t YPosFill = 0;
	bool chipDropped = 0;
	while(chipDropped != 1)
	{
		for(int i=0; i < 6; i++)
		{
			for(int j=0; j < 7; j++)
			{
				int compChip = gameBoardArr[i][j];
				if(compChip == 1)
				{
					// Horizontal 3 in a row check
					if(j <= 4 && compChip == gameBoardArr[i][j+1] && compChip == gameBoardArr[i][j+2])
					{
						// drop left
						if(j > 0 && gameBoardArr[i][j-1] == 0) // is left space open
						{
							if(i == 5 || gameBoardArr[i+1][j-1] != 0) // gravity
							{
								XPosFill = (45 + ((j-1)*25));
								YPosFill = (146 + (i*26));
								LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
								gameBoardArr[i][j-1] = 2;
								currentPlayer = PlayerOne;
								checkIfOver();
								chipDropped = 1;
								return;
							}
						}
						else if(j+3 < 7 && gameBoardArr[i][j+3] == 0) // is right space open
						{
							if(i == 5 || gameBoardArr[i+1][j+3] != 0) // gravity
							{
								XPosFill = (45 + ((j+3)*25));
								YPosFill = (146 + (i*26));
								LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
								gameBoardArr[i][j+3] = 2;
								currentPlayer = PlayerOne;
								checkIfOver();
								chipDropped = 1;
								return;
							}
						}
					}
					// Vertical 3 in a row check
					else if(i <= 3 && compChip == gameBoardArr[i+1][j] && compChip == gameBoardArr[i+2][j])
					{
						// can only drop on top
						if(i > 0 && gameBoardArr[i-1][j] == 0) // is space open
						{
							XPosFill = (45 + (j*25));
							YPosFill = (146 + ((i-1)*26));
							LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
							gameBoardArr[i-1][j] = 2;
							currentPlayer = PlayerOne;
							checkIfOver();
							chipDropped = 1;
							return;
						}
					}
					// Up Right 3 in a row check
					else if(i >= 3 && j <= 3 && compChip == gameBoardArr[i-1][j+1] && compChip == gameBoardArr[i-2][j+2])
					{
						if(gameBoardArr[i-3][j+3] == 0 && gameBoardArr[i-2][j+3] != 0) // is up right space open and has something below it
						{
							XPosFill = (45 + ((j+3)*25));
							YPosFill = (146 + ((i-3)*26));
							LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
							gameBoardArr[i-3][j+3] = 2;
							currentPlayer = PlayerOne;
							checkIfOver();
							chipDropped = 1;
							return;
						}
						else if(i < 5 && j > 0 && gameBoardArr[i+1][j-1] == 0 && (i+1 == 5 || gameBoardArr[i+2][j-1] != 0)) // is down left open and has something below it
						{
							XPosFill = (45 + ((j-1)*25));
							YPosFill = (146 + ((i+1)*26));
							LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
							gameBoardArr[i+1][j-1] = 2;
							currentPlayer = PlayerOne;
							checkIfOver();
							chipDropped = 1;
							return;
						}
					}
					// Down Right 3 in a row check
					else if(i <= 1 && j <= 3 && compChip == gameBoardArr[i+1][j+1] && compChip == gameBoardArr[i+2][j+2])
					{
						if(gameBoardArr[i+3][j+3] == 0 && (i+3 == 5 || gameBoardArr[i+4][j+3] != 0)) // is down right space open and has something below it
						{
							XPosFill = (45 + ((j+3)*25));
							YPosFill = (146 + ((i+3)*26));
							LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
							gameBoardArr[i+3][j+3] = 2;
							currentPlayer = PlayerOne;
							checkIfOver();
							chipDropped = 1;
							return;
						}
						else if(i > 0 && j > 0 && gameBoardArr[i-1][j-1] == 0 && gameBoardArr[i][j-1] != 0) // is up left open and has something below it
						{
							XPosFill = (45 + ((j-1)*25));
							YPosFill = (146 + ((i-1)*26));
							LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
							gameBoardArr[i-1][j-1] = 2;
							currentPlayer = PlayerOne;
							checkIfOver();
							chipDropped = 1;
							return;
						}
					}
				}
				else if(compChip == 2)
				{
					// Horizontal 3 in a row check
					if(j <= 4 && compChip == gameBoardArr[i][j+1] && compChip == gameBoardArr[i][j+2])
					{
						// drop left
						if(j > 0 && gameBoardArr[i][j-1] == 0) // is left space open
						{
							if(i == 5 || gameBoardArr[i+1][j-1] != 0) // gravity
							{
								XPosFill = (45 + ((j-1)*25));
								YPosFill = (146 + (i*26));
								LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
								gameBoardArr[i][j-1] = 2;
								currentPlayer = PlayerOne;
								checkIfOver();
								chipDropped = 1;
								return;
							}
						}
						else if(j+3 < 7 && gameBoardArr[i][j+3] == 0) // is right space open
						{
							if(i == 5 || gameBoardArr[i+1][j+3] != 0) // gravity
							{
								XPosFill = (45 + ((j+3)*25));
								YPosFill = (146 + (i*26));
								LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
								gameBoardArr[i][j+3] = 2;
								currentPlayer = PlayerOne;
								checkIfOver();
								chipDropped = 1;
								return;
							}
						}
					}
					// Vertical 3 in a row check
					else if(i <= 3 && compChip == gameBoardArr[i+1][j] && compChip == gameBoardArr[i+2][j])
					{
						// can only drop on top
						if(i > 0 && gameBoardArr[i-1][j] == 0) // is space open
						{
							XPosFill = (45 + (j*25));
							YPosFill = (146 + ((i-1)*26));
							LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
							gameBoardArr[i-1][j] = 2;
							currentPlayer = PlayerOne;
							checkIfOver();
							chipDropped = 1;
							return;
						}
					}
					// Up Right 3 in a row check
					else if(i >= 3 && j <= 3 && compChip == gameBoardArr[i-1][j+1] && compChip == gameBoardArr[i-2][j+2])
					{
						if(gameBoardArr[i-3][j+3] == 0 && gameBoardArr[i-2][j+3] != 0) // is up right space open and has something below it
						{
							XPosFill = (45 + ((j+3)*25));
							YPosFill = (146 + ((i-3)*26));
							LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
							gameBoardArr[i-3][j+3] = 2;
							currentPlayer = PlayerOne;
							checkIfOver();
							chipDropped = 1;
							return;
						}
						else if(i < 5 && j > 0 && gameBoardArr[i+1][j-1] == 0 && (i+1 == 5 || gameBoardArr[i+2][j-1] != 0)) // is down left open and has something below it
						{
							XPosFill = (45 + ((j-1)*25));
							YPosFill = (146 + ((i+1)*26));
							LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
							gameBoardArr[i+1][j-1] = 2;
							currentPlayer = PlayerOne;
							checkIfOver();
							chipDropped = 1;
							return;
						}
					}
					// Down Right 3 in a row check
					else if(i <= 1 && j <= 3 && compChip == gameBoardArr[i+1][j+1] && compChip == gameBoardArr[i+2][j+2])
					{
						if(gameBoardArr[i+3][j+3] == 0 && (i+3 == 5 || gameBoardArr[i+4][j+3] != 0)) // is down right space open and has something below it
						{
							XPosFill = (45 + ((j+3)*25));
							YPosFill = (146 + ((i+3)*26));
							LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
							gameBoardArr[i+3][j+3] = 2;
							currentPlayer = PlayerOne;
							checkIfOver();
							chipDropped = 1;
							return;
						}
						else if(i > 0 && j > 0 && gameBoardArr[i-1][j-1] == 0 && gameBoardArr[i][j-1] != 0) // is up left open and has something below it
						{
							XPosFill = (45 + ((j-1)*25));
							YPosFill = (146 + ((i-1)*26));
							LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
							gameBoardArr[i-1][j-1] = 2;
							currentPlayer = PlayerOne;
							checkIfOver();
							chipDropped = 1;
							return;
						}
					}

				}
			}
		}
		for(int k=0; k < 6; k++)
		{
			for(int p=0; p < 7; p++)
			{
				if(gameBoardArr[k][p] == 1)
				{
					// Horizontal 2 in a row check
					int compChip = gameBoardArr[k][p];
					if(p <= 5 && compChip == gameBoardArr[k][p+1])
					{
						// drop left
						if(p > 0 && gameBoardArr[k][p-1] == 0) // is left space open
						{
							if(k == 5 || gameBoardArr[k+1][p-1] != 0) // gravity
							{
								XPosFill = (45 + ((p-1)*25));
								YPosFill = (146 + (k*26));
								LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
								gameBoardArr[k][p-1] = 2;
								currentPlayer = PlayerOne;
								checkIfOver();
								chipDropped = 1;
								return;
							}
						}
						else if(p+2 < 7 && gameBoardArr[k][p+2] == 0) // is right space open
						{
							if(k == 5 || gameBoardArr[k+1][p+2] != 0) // gravity
							{
								XPosFill = (45 + ((p+2)*25));
								YPosFill = (146 + (k*26));
								LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
								gameBoardArr[k][p+2] = 2;
								currentPlayer = PlayerOne;
								checkIfOver();
								chipDropped = 1;
								return;
							}
						}
					}
					// Vertical 2 in a row check
					else if(k <= 4 && compChip == gameBoardArr[k+1][p])
					{
						// can only drop on top
						if(k > 0 && gameBoardArr[k-1][p] == 0) // is space open
						{
							XPosFill = (45 + (p*25));
							YPosFill = (146 + ((k-1)*26));
							LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
							gameBoardArr[k-1][p] = 2;
							currentPlayer = PlayerOne;
							checkIfOver();
							chipDropped = 1;
							return;
						}
					}
					// Up Right 2 in a row check
					else if(k >= 2 && p <= 4 && compChip == gameBoardArr[k-1][p+1])
					{
						if(gameBoardArr[k-2][p+2] == 0 && gameBoardArr[k-1][p+2] != 0) // is up right space open and has something below it
						{
							XPosFill = (45 + ((p+2)*25));
							YPosFill = (146 + ((k-2)*26));
							LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
							gameBoardArr[k-2][p+2] = 2;
							currentPlayer = PlayerOne;
							checkIfOver();
							chipDropped = 1;
							return;
						}
						else if(k < 5 && p > 0 && gameBoardArr[k+1][p-1] == 0 && (k+1 == 5 || gameBoardArr[k+2][p-1] != 0)) // is down left open and has something below it
						{
							XPosFill = (45 + ((p-1)*25));
							YPosFill = (146 + ((k+1)*26));
							LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
							gameBoardArr[k+1][p-1] = 2;
							currentPlayer = PlayerOne;
							checkIfOver();
							chipDropped = 1;
							return;
						}
					}
					// Down Right 2 in a row check
					else if(k <= 2 && p <= 3 && compChip == gameBoardArr[k+1][p+1])
					{
						if(gameBoardArr[k+2][p+2] == 0 && (k+2 == 5 || gameBoardArr[k+3][p+3] != 0)) // is down right space open and has something below it
						{
							XPosFill = (45 + ((p+2)*25));
							YPosFill = (146 + ((k+2)*26));
							LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
							gameBoardArr[k+2][p+2] = 2;
							currentPlayer = PlayerOne;
							checkIfOver();
							chipDropped = 1;
							return;
						}
						else if(k > 0 && p > 0 && gameBoardArr[k-1][p-1] == 0 && gameBoardArr[k][p-1] != 0) // is up left open and has something below it
						{
							XPosFill = (45 + ((p-1)*25));
							YPosFill = (146 + ((k-1)*26));
							LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
							gameBoardArr[k-1][p-1] = 2;
							currentPlayer = PlayerOne;
							checkIfOver();
							chipDropped = 1;
							return;
						}
					}
				}
			}
		}
		for(int r=0; r < 6; r++)
		{
			for(int q=0; q < 7; q++)
			{
				if(gameBoardArr[r][q] == 2)
				{
					// Horizontal 2 in a row check
					int compChip = gameBoardArr[r][q];
					if(q <= 5 && compChip == gameBoardArr[r][q+1])
					{
						// drop left
						if(q > 0 && gameBoardArr[r][q-1] == 0) // is left space open
						{
							if(r == 5 || gameBoardArr[r+1][q-1] != 0) // gravity
							{
								XPosFill = (45 + ((q-1)*25));
								YPosFill = (146 + (r*26));
								LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
								gameBoardArr[r][q-1] = 2;
								currentPlayer = PlayerOne;
								checkIfOver();
								chipDropped = 1;
								return;
							}
						}
						else if(q+2 < 7 && gameBoardArr[r][q+2] == 0) // is right space open
						{
							if(r == 5 || gameBoardArr[r+1][q+2] != 0) // gravity
							{
								XPosFill = (45 + ((q+2)*25));
								YPosFill = (146 + (r*26));
								LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
								gameBoardArr[r][q+2] = 2;
								currentPlayer = PlayerOne;
								checkIfOver();
								chipDropped = 1;
								return;
							}
						}
					}
					// Vertical 2 in a row check
					else if(r <= 4 && compChip == gameBoardArr[r+1][q])
					{
						// can only drop on top
						if(r > 0 && gameBoardArr[r-1][q] == 0) // is space open
						{
							XPosFill = (45 + (q*25));
							YPosFill = (146 + ((r-1)*26));
							LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
							gameBoardArr[r-1][q] = 2;
							currentPlayer = PlayerOne;
							checkIfOver();
							chipDropped = 1;
							return;
						}
					}
					// Up Right 2 in a row check
					else if(r >= 2 && q <= 4 && compChip == gameBoardArr[r-1][q+1])
					{
						if(gameBoardArr[r-2][q+2] == 0 && gameBoardArr[r-1][q+2] != 0) // is up right space open and has something below it
						{
							XPosFill = (45 + ((q+2)*25));
							YPosFill = (146 + ((r-2)*26));
							LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
							gameBoardArr[r-2][q+2] = 2;
							currentPlayer = PlayerOne;
							checkIfOver();
							chipDropped = 1;
							return;
						}
						else if(r < 5 && q > 0 && gameBoardArr[r+1][q-1] == 0 && (r+1 == 5 || gameBoardArr[r+2][q-1] != 0)) // is down left open and has something below it
						{
							XPosFill = (45 + ((q-1)*25));
							YPosFill = (146 + ((r+1)*26));
							LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
							gameBoardArr[r+1][q-1] = 2;
							currentPlayer = PlayerOne;
							checkIfOver();
							chipDropped = 1;
							return;
						}
					}
					// Down Right 2 in a row check
					else if(r <= 2 && q <= 3 && compChip == gameBoardArr[r+1][q+1])
					{
						if(gameBoardArr[r+2][q+2] == 0 && (r+2 == 5 || gameBoardArr[r+3][q+3] != 0)) // is down right space open and has something below it
						{
							XPosFill = (45 + ((q+2)*25));
							YPosFill = (146 + ((r+2)*26));
							LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
							gameBoardArr[r+2][q+2] = 2;
							currentPlayer = PlayerOne;
							checkIfOver();
							chipDropped = 1;
							return;
						}
						else if(r > 0 && q > 0 && gameBoardArr[r-1][q-1] == 0 && gameBoardArr[r][q-1] != 0) // is up left open and has something below it
						{
							XPosFill = (45 + ((q-1)*25));
							YPosFill = (146 + ((r-1)*26));
							LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
							gameBoardArr[r-1][q-1] = 2;
							currentPlayer = PlayerOne;
							checkIfOver();
							chipDropped = 1;
							return;
						}
					}
				}
			}
		}
		for(int m=0; m < 6; m++)
		{
			for(int n=0; n < 7; n++)
			{
				if(gameBoardArr[m][n] != 0)
				{
					//int compChip = gameBoardArr[m][n];
					if(m > 0 && gameBoardArr[m-1][n] == 0)
					{
						XPosFill = (45 + (n*25));
						YPosFill = (146 + ((m-1)*26));
						LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
						gameBoardArr[m-1][n] = 2;
						currentPlayer = PlayerOne;
						checkIfOver();
						chipDropped = 1;
						return;
					}
					else if(n < 6 && gameBoardArr[m][n+1] == 0 && (m == 5 || gameBoardArr[m+1][n+1] != 0))
					{
						XPosFill = (45 + ((n+1)*25));
						YPosFill = (146 + (m*26));
						LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
						gameBoardArr[m][n+1] = 2;
						currentPlayer = PlayerOne;
						checkIfOver();
						chipDropped = 1;
						return;
					}
					else if(n > 0 && gameBoardArr[m][n-1] == 0 && (m == 5 || gameBoardArr[m+1][n-1] != 0))
					{
						XPosFill = (45 + ((n-1)*25));
						YPosFill = (146 + (m*26));
						LCD_Draw_Circle_Fill(XPosFill, YPosFill, 8, LCD_COLOR_YELLOW);
						gameBoardArr[m][n-1] = 2;
						currentPlayer = PlayerOne;
						checkIfOver();
						chipDropped = 1;
						return;
					}
				}
			}
		}
	}
}

void onePlayerMode(void)
{
	gameStrtTime = HAL_GetTick();
	while(gameComplete != 1)
	{

		if(currentPlayer == PlayerOne)
		{
			moveChipLeftRightOnePlayer();
		}
//		else
//		{
//			AIplayer();
//		}
	}
	if(winner == 1)
	{
		// printf("Start Time: %lu ", gameStrtTime);
		// printf("End Time: %lu ", gameEndTime);

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

		redWinCnt += 1;

		HAL_Delay(2000);
		ScoreBoardScreen();
		currentGameMode = ONEPLAYER;
		pollingForNewGame();
	}
	else if(winner == 2)
	{
		// printf("Start Time: %lu ", gameStrtTime);
		// printf("End Time: %lu ", gameEndTime);

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

		yellowWinCnt += 1;

		HAL_Delay(2000);
		ScoreBoardScreen();
		pollingForNewGame();
	}
	else
	{
		// printf("Start Time: %lu ", gameStrtTime);
		// printf("End Time: %lu ", gameEndTime);

		LCD_Clear(0,LCD_COLOR_WHITE);
		LCD_SetTextColor(LCD_COLOR_BLACK);
		LCD_SetFont(&Font16x24);

		LCD_DisplayChar(58,160,'T');
		LCD_DisplayChar(73,160,'i');
		LCD_DisplayChar(83,160,'e');

		HAL_Delay(2000);
		ScoreBoardScreen();
		pollingForNewGame();
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
	checkIfOver();
	//printGameBoard();
	__HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_0);
	IRQ_INTR_Enable(EXTI0_IRQ_NUMBER);
}


#endif // COMPILE_TOUCH_FUNCTIONS

