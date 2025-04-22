/*
 * LCD_Driver.c
 *
 *  Created on: Sep 28, 2023
 *      Author: Xavion
 */

#include "LCD_Driver.h"

/**
  * @brief LTDC Initialization Function
  * @param None
  * @retval None
  */

static LTDC_HandleTypeDef hltdc;
static RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
static FONT_t *LCD_Currentfonts;
static uint16_t CurrentTextColor   = 0xFFFF;


/*
 * fb[y*W+x] OR fb[y][x]
 * Alternatively, we can modify the linker script to have an end address of 20013DFB instead of 2002FFFF, so it does not place variables in the same region as the frame buffer. In this case it is safe to just specify the raw address as frame buffer.
 */
//uint32_t frameBuffer[(LCD_PIXEL_WIDTH*LCD_PIXEL_WIDTH)/2] = {0};		//16bpp pixel format. We can size to uint32. this ensures 32 bit alignment


//Someone from STM said it was "often accessed" a 1-dim array, and not a 2d array. However you still access it like a 2dim array,  using fb[y*W+x] instead of fb[y][x].
uint16_t frameBuffer[LCD_PIXEL_WIDTH*LCD_PIXEL_HEIGHT] = {0};			//16bpp pixel format.


void LCD_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable the LTDC clock */
  __HAL_RCC_LTDC_CLK_ENABLE();

  /* Enable GPIO clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /* GPIO Config
   *
    LCD pins
   LCD_TFT R2 <-> PC.10
   LCD_TFT G2 <-> PA.06
   LCD_TFT B2 <-> PD.06
   LCD_TFT R3 <-> PB.00
   LCD_TFT G3 <-> PG.10
   LCD_TFT B3 <-> PG.11
   LCD_TFT R4 <-> PA.11
   LCD_TFT G4 <-> PB.10
   LCD_TFT B4 <-> PG.12
   LCD_TFT R5 <-> PA.12
   LCD_TFT G5 <-> PB.11
   LCD_TFT B5 <-> PA.03
   LCD_TFT R6 <-> PB.01
   LCD_TFT G6 <-> PC.07
   LCD_TFT B6 <-> PB.08
   LCD_TFT R7 <-> PG.06
   LCD_TFT G7 <-> PD.03
   LCD_TFT B7 <-> PB.09
   LCD_TFT HSYNC <-> PC.06
   LCDTFT VSYNC <->  PA.04
   LCD_TFT CLK   <-> PG.07
   LCD_TFT DE   <->  PF.10
  */

  /* GPIOA configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6 |
                           GPIO_PIN_11 | GPIO_PIN_12;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
  GPIO_InitStructure.Alternate= GPIO_AF14_LTDC;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

 /* GPIOB configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_8 | \
                           GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

 /* GPIOC configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_10;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

 /* GPIOD configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_3 | GPIO_PIN_6;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);

 /* GPIOF configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_10;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);

 /* GPIOG configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_6 | GPIO_PIN_7 | \
                           GPIO_PIN_11;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);

  /* GPIOB configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1;
  GPIO_InitStructure.Alternate= GPIO_AF9_LTDC;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* GPIOG configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_10 | GPIO_PIN_12;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
}

void LTCD_Layer_Init(uint8_t LayerIndex)
{
	LTDC_LayerCfgTypeDef  pLayerCfg;

	pLayerCfg.WindowX0 = 0;	//Configures the Window HORZ START Position.
	pLayerCfg.WindowX1 = LCD_PIXEL_WIDTH;	//Configures the Window HORZ Stop Position.
	pLayerCfg.WindowY0 = 0;	//Configures the Window vertical START Position.
	pLayerCfg.WindowY1 = LCD_PIXEL_HEIGHT;	//Configures the Window vertical Stop Position.
	pLayerCfg.PixelFormat = LCD_PIXEL_FORMAT_1;  //INCORRECT PIXEL FORMAT WILL GIVE WEIRD RESULTS!! IT MAY STILL WORK FOR 1/2 THE DISPLAY!!! //This is our buffers pixel format. 2 bytes for each pixel
	pLayerCfg.Alpha = 255;
	pLayerCfg.Alpha0 = 0;
	pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
	pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
	if (LayerIndex == 0){
		pLayerCfg.FBStartAdress = (uintptr_t)frameBuffer;
	}
	pLayerCfg.ImageWidth = LCD_PIXEL_WIDTH;
	pLayerCfg.ImageHeight = LCD_PIXEL_HEIGHT;
	pLayerCfg.Backcolor.Blue = 0;
	pLayerCfg.Backcolor.Green = 0;
	pLayerCfg.Backcolor.Red = 0;
	if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, LayerIndex) != HAL_OK)
	{
		LCD_Error_Handler();
	}
}

void clearScreen(void)
{
  LCD_Clear(0,LCD_COLOR_WHITE);
}

void LTCD__Init(void)
{
	hltdc.Instance = LTDC;
	/* Configure horizontal synchronization width */
	hltdc.Init.HorizontalSync = ILI9341_HSYNC;
	/* Configure vertical synchronization height */
	hltdc.Init.VerticalSync = ILI9341_VSYNC;
	/* Configure accumulated horizontal back porch */
	hltdc.Init.AccumulatedHBP = ILI9341_HBP;
	/* Configure accumulated vertical back porch */
	hltdc.Init.AccumulatedVBP = ILI9341_VBP;
	/* Configure accumulated active width */
	hltdc.Init.AccumulatedActiveW = 269;
	/* Configure accumulated active height */
	hltdc.Init.AccumulatedActiveH = 323;
	/* Configure total width */
	hltdc.Init.TotalWidth = 279;
	/* Configure total height */
	hltdc.Init.TotalHeigh = 327;
	/* Configure R,G,B component values for LCD background color */
	hltdc.Init.Backcolor.Red = 0;
	hltdc.Init.Backcolor.Blue = 0;
	hltdc.Init.Backcolor.Green = 0;

	/* LCD clock configuration */
	/* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
	/* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 192 Mhz */
	/* PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 192/4 = 48 Mhz */
	/* LTDC clock frequency = PLLLCDCLK / LTDC_PLLSAI_DIVR_8 = 48/4 = 6Mhz */

	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
	PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
	PeriphClkInitStruct.PLLSAI.PLLSAIR = 4;
	PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_8;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
	/* Polarity */
	hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
	hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
	hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
	hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;

	LCD_GPIO_Init();

	if (HAL_LTDC_Init(&hltdc) != HAL_OK)
	 {
	   LCD_Error_Handler();
	 }

	ili9341_Init();
}

/* START Draw functions */


/*
 * This is really the only function needed.
 * All drawing consists of is manipulating the array.
 * Adding input sanitation should probably be done.
 */
void LCD_Draw_Pixel(uint16_t x, uint16_t y, uint16_t color)
{
	frameBuffer[y*LCD_PIXEL_WIDTH+x] = color;  //You cannot do x*y to set the pixel.
}

/*
 * These functions are simple examples. Most computer graphics like OpenGl and stm's graphics library use a state machine. Where you first call some function like SetColor(color), SetPosition(x,y), then DrawSqure(size)
 * Instead all of these are explicit where color, size, and position are passed in.
 * There is tons of ways to handle drawing. I dont think it matters too much.
 */
void LCD_Draw_Circle_Fill(uint16_t Xpos, uint16_t Ypos, uint16_t radius, uint16_t color)
{
    for(int16_t y=-radius; y<=radius; y++)
    {
        for(int16_t x=-radius; x<=radius; x++)
        {
            if(x*x+y*y <= radius*radius)
            {
            	LCD_Draw_Pixel(x+Xpos, y+Ypos, color);
            }
        }
    }
}

void LCD_Draw_Box_Fill(uint16_t X_strt_pos, uint16_t Y_strt_pos, uint16_t Xlen, uint16_t Ylen, uint16_t color)
{
	for(uint16_t i = 0; i < Xlen; i++)
	{
		LCD_Draw_Vertical_Line(i+X_strt_pos, Y_strt_pos, Ylen, color);
	}
}

void LCD_Draw_Vertical_Line(uint16_t x, uint16_t y, uint16_t len, uint16_t color)
{
  for (uint16_t i = 0; i < len; i++)
  {
	  LCD_Draw_Pixel(x, i+y, color);
  }
}

void LCD_Clear(uint8_t LayerIndex, uint16_t Color)
{
	if (LayerIndex == 0){
		for (uint32_t i = 0; i < LCD_PIXEL_WIDTH * LCD_PIXEL_HEIGHT; i++){
			frameBuffer[i] = Color;
		}
	}
  // TODO: Add more Layers if needed
}

//This was taken and adapted from stm32's mcu code
void LCD_SetTextColor(uint16_t Color)
{
  CurrentTextColor = Color;
}

//This was taken and adapted from stm32's mcu code
void LCD_SetFont(FONT_t *fonts)
{
  LCD_Currentfonts = fonts;
}

//This was taken and adapted from stm32's mcu code
void LCD_Draw_Char(uint16_t Xpos, uint16_t Ypos, const uint16_t *c)
{
  uint32_t index = 0, counter = 0;
  for(index = 0; index < LCD_Currentfonts->Height; index++)
  {
    for(counter = 0; counter < LCD_Currentfonts->Width; counter++)
    {
      if((((c[index] & ((0x80 << ((LCD_Currentfonts->Width / 12 ) * 8 ) ) >> counter)) == 0x00) && (LCD_Currentfonts->Width <= 12)) || (((c[index] & (0x1 << counter)) == 0x00)&&(LCD_Currentfonts->Width > 12 )))
      {
         //Background If want to overrite text under then add a set color here
      }
      else
      {
    	  LCD_Draw_Pixel(counter + Xpos,index + Ypos,CurrentTextColor);
      }
    }
  }
}

//This was taken and adapted from stm32's mcu code
void LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii)
{
  Ascii -= 32;
  LCD_Draw_Char(Xpos, Ypos, &LCD_Currentfonts->table[Ascii * LCD_Currentfonts->Height]);
}

void visualDemo(void)
{
	uint16_t x;
	uint16_t y;
	// This for loop just illustrates how with using logic and for loops, you can create interesting things
	// this may or not be useful ;)
	for(y=0; y<LCD_PIXEL_HEIGHT; y++){
		for(x=0; x < LCD_PIXEL_WIDTH; x++){
			if (x & 32)
				frameBuffer[x*y] = LCD_COLOR_WHITE;
			else
				frameBuffer[x*y] = LCD_COLOR_BLACK;
		}
	}

	HAL_Delay(1500);
	LCD_Clear(0, LCD_COLOR_GREEN);
	HAL_Delay(1500);
	LCD_Clear(0, LCD_COLOR_RED);
	HAL_Delay(1500);
	LCD_Clear(0, LCD_COLOR_WHITE);
	LCD_Draw_Vertical_Line(10,10,250,LCD_COLOR_MAGENTA);
	HAL_Delay(1500);
	LCD_Draw_Vertical_Line(230,10,250,LCD_COLOR_MAGENTA);
	HAL_Delay(1500);

	LCD_Draw_Circle_Fill(125,150,20,LCD_COLOR_BLACK);
	HAL_Delay(2000);

	// Test the Draw filled box
	LCD_Clear(0, LCD_COLOR_WHITE);
	LCD_Draw_Box_Fill(20, 260, 200, 40, LCD_COLOR_BLACK);
	HAL_Delay(2000);

	LCD_Clear(0,LCD_COLOR_BLUE);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font16x24);

	LCD_DisplayChar(100,140,'H');
	LCD_DisplayChar(115,140,'e');
	LCD_DisplayChar(125,140,'l');
	LCD_DisplayChar(130,140,'l');
	LCD_DisplayChar(140,140,'o');

	LCD_DisplayChar(100,160,'W');
	LCD_DisplayChar(115,160,'o');
	LCD_DisplayChar(125,160,'r');
	LCD_DisplayChar(130,160,'l');
	LCD_DisplayChar(140,160,'d');
}

void startScreen(void)
{
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font16x24);

	LCD_Clear(0, LCD_COLOR_WHITE);
	LCD_Draw_Box_Fill(20, 260, 200, 40, LCD_COLOR_GREY);

	LCD_DisplayChar(60,275,'T');
	LCD_DisplayChar(73,275,'w');
	LCD_DisplayChar(88,275,'o');

	LCD_DisplayChar(105,275,'P');
	LCD_DisplayChar(115,275,'l');
	LCD_DisplayChar(123,275,'a');
	LCD_DisplayChar(133,275,'y');
	LCD_DisplayChar(145,275,'e');
	LCD_DisplayChar(156,275,'r');


	LCD_Draw_Box_Fill(20, 200, 200, 40, LCD_COLOR_GREY);

	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font16x24);

	LCD_DisplayChar(60,215,'O');
	LCD_DisplayChar(76,215,'n');
	LCD_DisplayChar(88,215,'e');

	LCD_DisplayChar(105,215,'P');
	LCD_DisplayChar(115,215,'l');
	LCD_DisplayChar(123,215,'a');
	LCD_DisplayChar(133,215,'y');
	LCD_DisplayChar(145,215,'e');
	LCD_DisplayChar(156,215,'r');

	LCD_Draw_Box_Fill(20, 45, 200, 135, LCD_COLOR_BLACK);
	LCD_Draw_Box_Fill(35, 55, 170, 115, LCD_COLOR_BLUE2);

	LCD_Draw_Circle_Fill(48, 68, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(48, 86, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(48, 104, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(48, 122, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(48, 140, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(48, 158, 7, LCD_COLOR_BLACK);

	LCD_Draw_Circle_Fill(72, 68, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(72, 86, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(72, 104, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(72, 122, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(72, 140, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(72, 158, 7, LCD_COLOR_BLACK);

	LCD_Draw_Circle_Fill(96, 68, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(96, 86, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(96, 104, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(96, 122, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(96, 140, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(96, 158, 7, LCD_COLOR_BLACK);

	LCD_Draw_Circle_Fill(120, 68, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(120, 86, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(120, 104, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(120, 122, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(120, 140, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(120, 158, 7, LCD_COLOR_BLACK);

	LCD_Draw_Circle_Fill(144, 68, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(144, 86, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(144, 104, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(144, 122, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(144, 140, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(144, 158, 7, LCD_COLOR_BLACK);

	LCD_Draw_Circle_Fill(168, 68, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(168, 86, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(168, 104, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(168, 122, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(168, 140, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(168, 158, 7, LCD_COLOR_BLACK);

	LCD_Draw_Circle_Fill(192, 68, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(192, 86, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(192, 104, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(192, 122, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(192, 140, 7, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(192, 158, 7, LCD_COLOR_BLACK);

	LCD_DisplayChar(50,20,'C');
	LCD_DisplayChar(62,20,'o');
	LCD_DisplayChar(74,20,'n');
	LCD_DisplayChar(86,20,'n');
	LCD_DisplayChar(98,20,'e');
	LCD_DisplayChar(110,20,'c');
	LCD_DisplayChar(118,20,'t');

	LCD_DisplayChar(135,20,'F');
	LCD_DisplayChar(147,20,'o');
	LCD_DisplayChar(159,20,'u');
	LCD_DisplayChar(171,20,'r');

	HAL_Delay(5000);

}

void singlePlayerScreen(void)
{
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font16x24);

	LCD_DisplayChar(68,25,'O');
	LCD_DisplayChar(84,25,'n');
	LCD_DisplayChar(96,25,'e');

	LCD_DisplayChar(113,25,'P');
	LCD_DisplayChar(123,25,'l');
	LCD_DisplayChar(131,25,'a');
	LCD_DisplayChar(141,25,'y');
	LCD_DisplayChar(153,25,'e');
	LCD_DisplayChar(164,25,'r');

	LCD_Draw_Box_Fill(15, 120, 210, 180, LCD_COLOR_BLUE2);

	LCD_Draw_Circle_Fill(45, 146, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(45, 172, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(45, 198, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(45, 224, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(45, 250, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(45, 276, 8, LCD_COLOR_BLACK);

	LCD_Draw_Circle_Fill(70, 146, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(70, 172, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(70, 198, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(70, 224, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(70, 250, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(70, 276, 8, LCD_COLOR_BLACK);

	LCD_Draw_Circle_Fill(95, 146, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(95, 172, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(95, 198, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(95, 224, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(95, 250, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(95, 276, 8, LCD_COLOR_BLACK);

	LCD_Draw_Circle_Fill(120, 146, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(120, 172, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(120, 198, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(120, 224, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(120, 250, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(120, 276, 8, LCD_COLOR_BLACK);

	LCD_Draw_Circle_Fill(145, 146, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(145, 172, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(145, 198, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(145, 224, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(145, 250, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(145, 276, 8, LCD_COLOR_BLACK);

	LCD_Draw_Circle_Fill(170, 146, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(170, 172, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(170, 198, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(170, 224, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(170, 250, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(170, 276, 8, LCD_COLOR_BLACK);

	LCD_Draw_Circle_Fill(195, 146, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(195, 172, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(195, 198, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(195, 224, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(195, 250, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(195, 276, 8, LCD_COLOR_BLACK);
}


void twoPlayerScreen(void)
{
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font16x24);

	LCD_DisplayChar(68,25,'T');
	LCD_DisplayChar(83,25,'w');
	LCD_DisplayChar(96,25,'o');

	LCD_DisplayChar(113,25,'P');
	LCD_DisplayChar(123,25,'l');
	LCD_DisplayChar(131,25,'a');
	LCD_DisplayChar(141,25,'y');
	LCD_DisplayChar(153,25,'e');
	LCD_DisplayChar(164,25,'r');

	LCD_Draw_Box_Fill(15, 120, 210, 180, LCD_COLOR_BLUE2);

	LCD_Draw_Circle_Fill(45, 146, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(45, 172, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(45, 198, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(45, 224, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(45, 250, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(45, 276, 8, LCD_COLOR_BLACK);

	LCD_Draw_Circle_Fill(70, 146, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(70, 172, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(70, 198, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(70, 224, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(70, 250, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(70, 276, 8, LCD_COLOR_BLACK);

	LCD_Draw_Circle_Fill(95, 146, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(95, 172, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(95, 198, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(95, 224, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(95, 250, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(95, 276, 8, LCD_COLOR_BLACK);

	LCD_Draw_Circle_Fill(120, 146, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(120, 172, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(120, 198, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(120, 224, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(120, 250, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(120, 276, 8, LCD_COLOR_BLACK);

	LCD_Draw_Circle_Fill(145, 146, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(145, 172, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(145, 198, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(145, 224, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(145, 250, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(145, 276, 8, LCD_COLOR_BLACK);

	LCD_Draw_Circle_Fill(170, 146, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(170, 172, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(170, 198, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(170, 224, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(170, 250, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(170, 276, 8, LCD_COLOR_BLACK);

	LCD_Draw_Circle_Fill(195, 146, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(195, 172, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(195, 198, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(195, 224, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(195, 250, 8, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(195, 276, 8, LCD_COLOR_BLACK);
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void LCD_Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

// Touch Functionality   //

#if COMPILE_TOUCH_FUNCTIONS == 1

void InitializeLCDTouch(void)
{
  if(STMPE811_Init() != STMPE811_State_Ok)
  {
	 for(;;); // Hang code due to error in initialzation
  }
}

STMPE811_State_t returnTouchStateAndLocation(STMPE811_TouchData * touchStruct)
{
	return STMPE811_ReadTouch(touchStruct);
}

void DetermineTouchPosition(STMPE811_TouchData * touchStruct)
{
	STMPE811_DetermineTouchPosition(touchStruct);
}

uint8_t ReadRegisterFromTouchModule(uint8_t RegToRead)
{
	return STMPE811_Read(RegToRead);
}

void WriteDataToTouchModule(uint8_t RegToWrite, uint8_t writeData)
{
	STMPE811_Write(RegToWrite, writeData);
}

#endif // COMPILE_TOUCH_FUNCTIONS
