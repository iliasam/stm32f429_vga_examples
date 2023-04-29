/* Includes ------------------------------------------------------------------*/
#include "stm32f429_lcd.h"
#include "main.h"


#define LCD_FRAME_BUFFER        ((uint32_t)0xD0000000)

#define LCD_DEFAULT_FONT        Font16x24


LTDC_HandleTypeDef hltdc;
extern DMA2D_HandleTypeDef hdma2d;

static sFONT *LCD_Currentfonts;

static uint16_t CurrentTextColor   = 0x0000;
static uint16_t CurrentBackColor   = 0xFFFF;


/* LTDC init function */
void MX_LTDC_Init(void)
{
  LTDC_LayerCfgTypeDef pLayerCfg;

  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Init.HorizontalSync = LCD_TIMING_HSYNC;
  hltdc.Init.VerticalSync = LCD_TIMING_VSYNC;
  hltdc.Init.AccumulatedHBP = (LCD_TIMING_HSYNC + LCD_TIMING_HBP); // HSync+HBP;
  hltdc.Init.AccumulatedVBP = (LCD_TIMING_VSYNC + LCD_TIMING_VBP);  // VSync+VBP;
  hltdc.Init.AccumulatedActiveW = (LCD_TIMING_HSYNC + LCD_TIMING_HBP + LCD_PIXEL_WIDTH); // HSync+HBP+W;
  hltdc.Init.AccumulatedActiveH = (LCD_TIMING_VSYNC + LCD_TIMING_VBP + LCD_PIXEL_HEIGHT); // VSync+VBP+H;
  hltdc.Init.TotalWidth = (LCD_TIMING_HSYNC + LCD_TIMING_HBP + LCD_PIXEL_WIDTH + LCD_TIMING_HFP);  // HSync+HBP+W+HFP;
  hltdc.Init.TotalHeigh = (LCD_TIMING_VSYNC + LCD_TIMING_VBP + LCD_PIXEL_HEIGHT + LCD_TIMING_VFP);  // VSync+VBP+H+VFP;
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;
  if (HAL_LTDC_Init(&hltdc) != HAL_OK)
  {
    Error_Handler();
  }

  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = LCD_PIXEL_WIDTH;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = LCD_PIXEL_HEIGHT;
  
#if (LCD_BYTES_IN_PIXEL == 2)
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
#elif (LCD_BYTES_IN_PIXEL == 1)
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_L8;
#else
  #error "ERROR!"
#endif

  pLayerCfg.Alpha = 255;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg.FBStartAdress = LCD_FRAME_BUFFER;
  pLayerCfg.ImageWidth = LCD_PIXEL_WIDTH;
  pLayerCfg.ImageHeight = LCD_PIXEL_HEIGHT;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
  {
    Error_Handler();
  }
  
  //HAL_LTDC_EnableDither(&hltdc);

  LCD_SetFont(&LCD_DEFAULT_FONT); /* Set default font */  
}

/**
  * @brief  Displays a full rectangle.
  * @param  Xpos: specifies the X position, can be a value from 0 to 240.
  * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
  * @param  Height: rectangle height.
  * @param  Width: rectangle width.
  * @retval None
  */
void LCD_DrawFullRect(uint16_t xpos, uint16_t ypos, uint16_t width, uint16_t height, uint16_t color)
{
  uint32_t color32 =  lcd_convert_to_color32(color);

  uint32_t addr = 0;
  addr = (hltdc.LayerCfg[0].FBStartAdress) + LCD_BYTES_IN_PIXEL * (ypos * hltdc.LayerCfg[0].ImageWidth + xpos);
  hdma2d.Init.Mode = DMA2D_R2M;
  //смещение = ширина экрана минус ширина прямоугольника
  hdma2d.Init.OutputOffset = hltdc.LayerCfg[0].ImageWidth - width;
  if(HAL_DMA2D_Init(&hdma2d) == HAL_OK)
  {
    if (HAL_DMA2D_Start(&hdma2d, color32, addr, width, height) == HAL_OK)
    {
      HAL_DMA2D_PollForTransfer(&hdma2d, 10);
    }
  }
}


/**
  * @brief  Clears the whole LCD.
  * @param  Color: the color of the background.
  * @retval None
  */
void lcd_clear(uint16_t color)
{
  uint32_t color32 =  lcd_convert_to_color32(color);
    
  hdma2d.Init.Mode = DMA2D_R2M;
  hdma2d.Init.OutputOffset = 0;
  if(HAL_DMA2D_Init(&hdma2d) == HAL_OK)
  {
    if (HAL_DMA2D_Start(&hdma2d, color32, LCD_FRAME_BUFFER,
        LCD_PIXEL_WIDTH, LCD_PIXEL_HEIGHT) == HAL_OK)
    {
      HAL_DMA2D_PollForTransfer(&hdma2d, 10);
    }
  }
}

/**
  * @brief  Sets the LCD Text and Background colors.
  * @param  TextColor: specifies the Text Color.
  * @param  BackColor: specifies the Background Color.
  * @retval None
  */
void LCD_SetColors(uint16_t TextColor, uint16_t BackColor)
{
  CurrentTextColor = TextColor; 
  CurrentBackColor = BackColor;
}

/**
  * @brief  Gets the LCD Text and Background colors.
  * @param  TextColor: pointer to the variable that will contain the Text 
            Color.
  * @param  BackColor: pointer to the variable that will contain the Background 
            Color.
  * @retval None
  */
void LCD_GetColors(uint16_t *TextColor, uint16_t *BackColor)
{
  *TextColor = CurrentTextColor;
  *BackColor = CurrentBackColor;
}

/**
  * @brief  Sets the Text color.
  * @param  Color: specifies the Text color code RGB(5-6-5).
  * @retval None
  */
void LCD_SetTextColor(uint16_t Color)
{
  CurrentTextColor = Color;
}

/**
  * @brief  Sets the Background color.
  * @param  Color: specifies the Background color code RGB(5-6-5).
  * @retval None
  */
void LCD_SetBackColor(uint16_t Color)
{
  CurrentBackColor = Color;
}

/**
  * @brief  Sets the Text Font.
  * @param  fonts: specifies the font to be used.
  * @retval None
  */
void LCD_SetFont(sFONT *fonts)
{
  LCD_Currentfonts = fonts;
}

/**
  * @brief  Gets the Text Font.
  * @param  None.
  * @retval the used font.
  */
sFONT *LCD_GetFont(void)
{
  return LCD_Currentfonts;
}

/**
  * @brief  Draws a character on LCD.
  * @param  Xpos: the Line where to display the character shape.
  * @param  Ypos: start column address.
  * @param  c: pointer to the character data.
  * @retval None
  */
void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const uint16_t *c)
{
  uint32_t index = 0, counter = 0, xpos =0;
  uint32_t  Xaddress = 0;
  
  xpos = Xpos*LCD_PIXEL_WIDTH*2;
  Xaddress += Ypos;
  
  for(index = 0; index < LCD_Currentfonts->Height; index++)
  {
    for(counter = 0; counter < LCD_Currentfonts->Width; counter++)
    {
      if((((c[index] & ((0x80 << ((LCD_Currentfonts->Width / 12 ) * 8 ) ) >> counter)) == 0x00) &&(LCD_Currentfonts->Width <= 12))||
        (((c[index] & (0x1 << counter)) == 0x00)&&(LCD_Currentfonts->Width > 12 )))
      {
          /* Write data value to all SDRAM memory */
         *(__IO uint16_t*) (LCD_FRAME_BUFFER + (2*Xaddress) + xpos) = CurrentBackColor;
      }
      else
      {
          /* Write data value to all SDRAM memory */
         *(__IO uint16_t*) (LCD_FRAME_BUFFER + (2*Xaddress) + xpos) = CurrentTextColor;         
      }
      Xaddress++;
    }
      Xaddress += (LCD_PIXEL_WIDTH - LCD_Currentfonts->Width);
  }
}

/**
  * @brief  Displays one character (16dots width, 24dots height).
  * @param  Line: the Line where to display the character shape .
  *   This parameter can be one of the following values:
  *     @arg Linex: where x can be 0..29
  * @param  Column: start column address.
  * @param  Ascii: character ascii code, must be between 0x20 and 0x7E.
  * @retval None
  */
void LCD_DisplayChar(uint16_t Line, uint16_t Column, uint8_t Ascii)
{
  Ascii -= 32;

  LCD_DrawChar(Line, Column, &LCD_Currentfonts->table[Ascii * LCD_Currentfonts->Height]);
}

/**
  * @brief  Displays a maximum of 20 char on the LCD.
  * @param  Line: the Line where to display the character shape .
  *   This parameter can be one of the following values:
  *     @arg Linex: where x can be 0..9
  * @param  *ptr: pointer to string to display on LCD.
  * @retval None
  */
void LCD_DisplayStringLine(uint16_t Line, uint8_t *ptr)
{  
  uint16_t refcolumn = 0;
  /* Send the string character by character on lCD */
  while ((refcolumn < LCD_PIXEL_WIDTH) && ((*ptr != 0) & 
      (((refcolumn + LCD_Currentfonts->Width) & 0xFFFF) >= LCD_Currentfonts->Width)))
  {
    /* Display one character on LCD */
    LCD_DisplayChar(Line, refcolumn, *ptr);
    /* Decrement the column position by width */
    refcolumn += LCD_Currentfonts->Width;
    /* Point on the next character */
    ptr++;
  }
}

void lcd_draw_vertical_line(uint16_t x, uint16_t color)
{
  for (uint16_t i = 0; i < LCD_PIXEL_HEIGHT; i++)
  {
    lcd_set_pixel(x, i, color);
  }
}

//Convert from 565 to 888
uint32_t lcd_convert_to_color32(uint16_t color)
{
  uint8_t r5 = color >> 11;
  uint8_t g6 = (color & 0x7E0) >> 5;
  uint8_t b5 = color & 0x1F;
  
  uint8_t r8 = ( r5 << 3 ) | (r5 >> 2);
  uint8_t g8 = ( g6 << 2 ) | (g6 >> 4);
  uint8_t b8 = ( b5 << 3 ) | (b5 >> 2);
  
  return ((uint32_t)r8 << 16) | ((uint32_t)g8 << 8) | (uint32_t)b8;
}

void lcd_set_pixel(uint16_t x, uint16_t y, uint16_t color)
{
  uint32_t pos = (y * LCD_PIXEL_WIDTH + x) * LCD_BYTES_IN_PIXEL;
  *(__IO uint16_t*)(LCD_FRAME_BUFFER + pos) = color;
}

uint16_t grayscale_to_rgb565(uint8_t value)
{
  uint16_t red = (uint8_t)(value >> 3);
  uint16_t green = (uint8_t)(value >> 2);
  uint16_t blue = (uint8_t)(value >> 3);

  uint16_t result = (red << 11) | (green << 5) | (blue << 0);
  return result;
}

void lcd_draw_test_picture(void)
{
  uint16_t* test_ptr = (uint16_t*)LCD_FRAME_BUFFER;
  
  uint16_t x;
  uint16_t y;
  
  for (y=0; y<LCD_PIXEL_HEIGHT; y++)
  {
    for (x=0; x < LCD_PIXEL_WIDTH; x++)
    {
      if (x & 32)
        *test_ptr = 0xFFFF;
      else
        *test_ptr = 0;
      test_ptr++;
    }
  }
}