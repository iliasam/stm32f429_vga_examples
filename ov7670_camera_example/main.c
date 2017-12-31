//Example of using OV7670 camera with STM32F429
//Data from camera are captured by DCMI and saved to SDRAM
//LTDC is used for generating VGA signal
//Double buffering is used for display an image
//By iliasam


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "32f429_sdram.h"
#include "dcmi_OV7670.h"
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

extern uint8_t camera_event_flag;
extern uint8_t lcd_buffer_num;//current lcd buffer to write
extern uint16_t camera_fps;//debug only

/* Private function prototypes -----------------------------------------------*/
static void Delay(__IO uint32_t nTime);
void init_gpio(void);
void draw_random_circle(void);


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
  
  LCD_DisplayStringLine(LINE(2), (uint8_t*)"ILIASAM CAMERA TEST");

  GPIO_SetBits(GPIOI, GPIO_Pin_7);//LED
  
  DCMI_OV7670_Init();
  start_new_frame_capture();
  
  /* Infinite loop */
  while (1)
  {
    if (camera_event_flag != 0)
    {
      process_camera_event_flag();//start new capture
      
      //switch to buffer that is already captured
      if (lcd_buffer_num == 0)
      {
        LCD_SetLayer(LCD_FOREGROUND_LAYER);
        LTDC_Layer1->CFBAR = LCD_FRAME_BUFFER + BUFFER_OFFSET;
      }
      else
      {
        LCD_SetLayer(LCD_BACKGROUND_LAYER);
        LTDC_Layer1->CFBAR = LCD_FRAME_BUFFER;
      }
      //draw FPS text
      sprintf(test_string, "FPS: %d", camera_fps);
      LCD_DisplayStringLine(LINE(3), (uint8_t*)test_string);
      
      LTDC_ReloadConfig(LTDC_VBReload);
    }
  }
  
  
}//end of main



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
