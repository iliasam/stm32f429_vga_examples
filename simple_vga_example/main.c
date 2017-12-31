//Example of using STM32F429 for generating VGA picture.
//LTDC + SDRAM are used.
//By iliasam

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "32f429_sdram.h"
#include "32f429_lcd.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t uwTimingDelay;
RCC_ClocksTypeDef RCC_Clocks;
char test_string[64];


/* Private function prototypes -----------------------------------------------*/
static void Delay(__IO uint32_t nTime);
void init_gpio(void);
void draw_random_circle(void);
uint16_t grayscale_to_rgb565(uint8_t value);


/* Private functions ---------------------------------------------------------*/

int main(void)
{
  uint16_t i;
  //Clock is configured at SetSysClock()
  //SYSCLK is 168MHz

  /* SysTick end of count event each 1ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
  
  init_gpio();
  SDRAM_Init();
  Delay(5);
  
  LCD_Init();
  LCD_LayerInit();
  LTDC_Cmd(ENABLE);
  LCD_SetLayer(LCD_BACKGROUND_LAYER);
  //LCD_SetLayer(LCD_FOREGROUND_LAYER);
  
  
  LCD_Clear(LCD_COLOR_BLACK);
  LCD_SetFont(&Font16x24);
  
  Delay(2000);
  
  //red bars
  LCD_SetTextColor(LCD_COLOR_RED);
  for (i=0; i< 640; i+= 30)
  {
    LCD_DrawFullRect(i, 0, 15, 480);
  }
  
  Delay(2000);
  
  //grayscale bars
  for (i=0; i< 600; i+= 30)
  {
    uint16_t color = grayscale_to_rgb565((uint8_t)(i/3));
    LCD_SetTextColor(color);
    LCD_DrawFullRect(i, 0, 30, 480);
  }
  
  Delay(2000);
  
  
  LCD_SetTextColor(LCD_COLOR_BLACK);
  LCD_DisplayStringLine(LINE(2), (uint8_t*)"ILIASAM VGA TEST");
  
  LCD_SetLayer(LCD_FOREGROUND_LAYER);//Picture would be drawn at "shadow" memory
  LCD_Clear(LCD_COLOR_WHITE);
  
  GPIO_SetBits(GPIOI, GPIO_Pin_7);//LED
  
  /* Infinite loop */
  while (1)
  {
    static uint16_t cnt = 0;
    Delay(40);
    sprintf(test_string, "TEST CNT: %d", cnt);
    cnt++;
    draw_random_circle();

    LCD_SetTextColor(LCD_COLOR_BLACK);
    LCD_DisplayStringLine(LINE(0), (uint8_t*)test_string);

    if (cnt == 200) //Switch to another buffer
    {
      LTDC_Layer1->CFBAR = LCD_FRAME_BUFFER + BUFFER_OFFSET;//LCD_FOREGROUND_LAYER
      LTDC_ReloadConfig(LTDC_IMReload);
    }
  }
}

void draw_random_circle(void)
{
  int random = rand();
  
  uint32_t x_pos = (random & 0xFFFF) % 550 + 50;
  uint32_t y_pos = ((random >> 16) & 0xFFFF) % 300 + 50;
  
  LCD_SetTextColor((uint16_t)(random >> 8));
  
  LCD_DrawFullCircle((uint16_t)x_pos, (uint16_t)y_pos, 40);
}

uint16_t grayscale_to_rgb565(uint8_t value)
{
  uint16_t red = (uint8_t)(value >> 3);
  uint16_t green = (uint8_t)(value >> 2);
  uint16_t blue = (uint8_t)(value >> 3);

  uint16_t result = (red << 11) | (green << 5) | (blue << 0);
  return result;
}



void init_gpio(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);
  
  GPIO_StructInit(&GPIO_InitStructure);
  //LEDS
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOI, &GPIO_InitStructure);
}


void Delay(__IO uint32_t nTime)
{ 
  uwTimingDelay = nTime;

  while(uwTimingDelay != 0);
}

void delay_ms(__IO uint32_t nTime)
{
  Delay(nTime);
}


void TimingDelay_Decrement(void)
{
  if (uwTimingDelay != 0x00)
  { 
    uwTimingDelay--;
  }
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
