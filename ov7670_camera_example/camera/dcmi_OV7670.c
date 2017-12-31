//Functions for initialize OV7670 camera and capturing data from it
//DMA can do maximun 0xFFFF transfers, thais less than picture size
//so several transfers are needed

/* Includes ------------------------------------------------------------------*/
#include "dcmi_OV7670.h"
#include "DCMI_OV7670_INITTABLE.h"
#include "main.h"
#include "32f429_lcd.h"
    
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CAMERA_INIT_I2C

#define CAMERA_DMA_PNUM         3 //Number of DMA packets

#define PIXELS_IN_TRANSFER      2 //One transfet is 32 bit = 4 bytes = 2 colr pixels
#define CAMERA_DMA_TBYTES       4 //Number of bytes in one transfer

#define CAMERA_DMA_TNUM         (PICTURE_SIZE / PIXELS_IN_TRANSFER / CAMERA_DMA_PNUM) //Number of single transfers

#if ((PICTURE_SIZE / PIXELS_IN_TRANSFER % CAMERA_DMA_PNUM) > 0)
#error "Wrong CAMERA_DMA_PNUM"
#endif

#if (CAMERA_DMA_TNUM > 0xFFFF)
#error "CAMERA_DMA_TNUM too long."
#endif

#define CAMERA_DMA_ADR(x) (temp_buffer_adr + CAMERA_DMA_TNUM*CAMERA_DMA_TBYTES*(x+1)) //X - number of complete packet transfered


/* Private variables ---------------------------------------------------------*/
DMA_InitTypeDef  DMA_InitStructure;
uint32_t temp_buffer_adr = LCD_FRAME_BUFFER;  
uint8_t camera_event_flag = 0;
uint8_t camera_dma_packets_transfered = 0;
extern uint32_t ms_tick;
uint32_t camera_frame_duration;//debug only
uint16_t camera_fps = 0;//debug only

uint8_t lcd_buffer_num = 0;//current lcd buffer to write
    
/* Private function prototypes -----------------------------------------------*/
void dcmi_dma_init(void);
void DCMI_0V7670_PWDN_Init(void);
void DCMI_IRQHandler(void);
void DMA2_Stream1_IRQHandler(void);
void switch_dma_buffer(void);


/* Private functions ---------------------------------------------------------*/


void DCMI_IRQHandler(void)
{
  static uint32_t prev_frame_time  = 0;
  
  if (DCMI_GetITStatus(DCMI_IT_VSYNC) != RESET) 
  {	 		   
    DCMI_ClearITPendingBit(DCMI_IT_VSYNC);
  }
  
  if (DCMI_GetITStatus(DCMI_IT_LINE) != RESET) 
  {
    DCMI_ClearITPendingBit(DCMI_IT_LINE); 			  
  }
  
  if (DCMI_GetITStatus(DCMI_IT_FRAME) != RESET) //new frame
  {
    DCMI_ClearITPendingBit(DCMI_IT_FRAME);
    camera_event_flag = 3;
    camera_frame_duration = ms_tick - prev_frame_time;
    camera_fps = 1000 / camera_frame_duration;
  }
  if (DCMI_GetITStatus(DCMI_IT_ERR) != RESET) 
  {
    DCMI_ClearITPendingBit(DCMI_IT_ERR);
    camera_event_flag = 1;
    GPIOI->ODR&= ~GPIO_Pin_7;//debug led
  }
  
  if (DCMI_GetITStatus(DCMI_IT_OVF) != RESET) 
  {
    DCMI_ClearITPendingBit(DCMI_IT_OVF);
    camera_event_flag = 2;
    GPIOI->ODR&= ~GPIO_Pin_7;//debug led
  }
  
  prev_frame_time = ms_tick;

}

//DCMI DMA
void DMA2_Stream1_IRQHandler(void)
{
  if(DMA_GetITStatus(DMA2_Stream1, DMA_IT_TCIF1)) //packet tranfer complete
  {
    DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);
    
    camera_dma_packets_transfered++;
    switch_dma_buffer();
    
    if ( camera_dma_packets_transfered == CAMERA_DMA_PNUM)//full frame captured
    {
      DCMI_CaptureCmd(DISABLE);
      DMA_Cmd(DMA2_Stream1, DISABLE);
      GPIOI->ODR&= ~GPIO_Pin_7;
    } 
  }
  
  if(DMA_GetITStatus(DMA2_Stream1, DMA_IT_TEIF1))
  {
    DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TEIF1);
  }
}

//We need to change address of NOT active buffer
void switch_dma_buffer(void)
{
  //Returns the current memory target used by double buffer transfer
  if (DMA_GetCurrentMemoryTarget(DMA2_Stream1) == 0)
  {
    DMA2_Stream1->M1AR = CAMERA_DMA_ADR(camera_dma_packets_transfered);
  }
  else
  {
    DMA2_Stream1->M0AR = CAMERA_DMA_ADR(camera_dma_packets_transfered);
  }
}

void process_camera_event_flag(void)
{
  camera_event_flag = 0;
  start_new_frame_capture();
}


/**
* @brief  Configures the DCMI to interface with the OV7670 camera module.
* @param  None
* @retval None
*/
void DCMI_Config(void)
{
  
  
  DCMI_InitTypeDef DCMI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Enable DCMI GPIOs clocks */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOH, ENABLE); 
  
  /* Enable DCMI clock */
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI, ENABLE);
  
  /* Connect DCMI pins ************************************************/
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_DCMI);//HSYNC 
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_DCMI);//PCLK
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_DCMI);//D1
  
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource3, GPIO_AF_DCMI);//D5
  
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource4, GPIO_AF_DCMI);//d4
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_DCMI);//d6
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource6, GPIO_AF_DCMI);//d7
  
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource9, GPIO_AF_DCMI);//VSYNC
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource10, GPIO_AF_DCMI);//d2
  
  GPIO_PinAFConfig(GPIOH, GPIO_PinSource9, GPIO_AF_DCMI);//d0
  GPIO_PinAFConfig(GPIOH, GPIO_PinSource12, GPIO_AF_DCMI);//d3
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;  
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_12;
  GPIO_Init(GPIOH, &GPIO_InitStructure);
  
  
  /* DCMI configuration *******************************************************/ 
  
  DCMI_DeInit();
  
  DCMI_InitStructure.DCMI_CaptureMode = DCMI_CaptureMode_SnapShot;
  DCMI_InitStructure.DCMI_SynchroMode = DCMI_SynchroMode_Hardware;
  DCMI_InitStructure.DCMI_PCKPolarity = DCMI_PCKPolarity_Falling;
  DCMI_InitStructure.DCMI_VSPolarity = DCMI_VSPolarity_High;
  DCMI_InitStructure.DCMI_HSPolarity = DCMI_HSPolarity_High;
  DCMI_InitStructure.DCMI_CaptureRate = DCMI_CaptureRate_All_Frame;
  DCMI_InitStructure.DCMI_ExtendedDataMode = DCMI_ExtendedDataMode_8b;//8bit - camera interface
  
  DCMI_Init(&DCMI_InitStructure);
  
  DCMI_ITConfig(DCMI_IT_OVF | DCMI_IT_ERR | DCMI_IT_FRAME, ENABLE);
  NVIC_InitStructure.NVIC_IRQChannel = DCMI_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  dcmi_dma_init();
}

void dcmi_dma_init(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Configures the DMA2 to transfer Data from DCMI to the LCD ****************/
  /* Enable DMA2 clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);  
  
  /* DMA2 Stream1 Configuration */  
  DMA_DeInit(DMA2_Stream1);
  
  DMA_StructInit(&DMA_InitStructure);
  DMA_InitStructure.DMA_Memory0BaseAddr = temp_buffer_adr;//memory
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//memory
  DMA_InitStructure.DMA_Channel = DMA_Channel_1;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = DCMI_DR_ADDRESS;	
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = (uint32_t)(PICTURE_SIZE / 2);
  
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;//32 bit - dcmi (8x4)
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;//32 bit - memory

  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;

  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  
  DMA_DoubleBufferModeConfig(DMA2_Stream1, CAMERA_DMA_ADR(0), CAMERA_DMA_ADR(-1));
  DMA_DoubleBufferModeCmd(DMA2_Stream1, ENABLE);
  
  DMA_Init(DMA2_Stream1, &DMA_InitStructure);
  
  DMA_ITConfig(DMA2_Stream1, DMA_IT_TC, ENABLE);
  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


void start_new_frame_capture(void)
{
  DCMI_Cmd(DISABLE);
  camera_dma_packets_transfered = 0;
  lcd_buffer_num^= 1;//switch current write lcd buffer
  
  if (lcd_buffer_num == 0)
    temp_buffer_adr = LCD_FRAME_BUFFER;
  else
    temp_buffer_adr = LCD_FRAME_BUFFER + PICTURE_SIZE*2;//2 bytes in pixel
  
  DMA_Cmd(DMA2_Stream1, DISABLE);
  DMA2_Stream1->CR &= ~(uint32_t)(DMA_SxCR_CT);//Set Memory 0 as current memory address
  DMA2_Stream1->M0AR = CAMERA_DMA_ADR(-1);
  DMA2_Stream1->M1AR = CAMERA_DMA_ADR(0);
  DMA2_Stream1->NDTR = CAMERA_DMA_TNUM;
  DMA_Cmd(DMA2_Stream1, ENABLE);
  
  DCMI_Cmd(ENABLE);
  
  DCMI_ClearITPendingBit(DCMI_IT_OVF);
  DCMI_ClearITPendingBit(DCMI_IT_ERR);
  DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TEIF1);
  
  DCMI_CaptureCmd(ENABLE);
  GPIOI->ODR|= GPIO_Pin_7;//debug led
}


/**
  * @brief  Set PA8 Output
  * @param  None
  * @retval None
  */
void MCO1_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_ClockSecuritySystemCmd(ENABLE);
  
  /* Enable GPIOs clocks */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO);
  
  /* Configure MCO (PA8) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  RCC_MCO1Config(RCC_MCO1Source_PLLCLK, RCC_MCO1Div_3);
}

void DCMI_0V7670_PWDN_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable GPIOs clocks */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  /*PWDN*/
  GPIO_ResetBits(GPIOA, GPIO_Pin_2);
  delay_ms(10);
  GPIO_SetBits(GPIOA, GPIO_Pin_2);	
}

/**
  * @brief  Set the VGA size(640*320).
  * @param  None
  * @retval None
  */
uint8_t DCMI_OV7670_Init(void)
{
  uint8_t i;
  uint16_t length = sizeof(change_reg2) / 2;//(register + value ) / 2 = table length
  SCCB_GPIO_Config();
  DCMI_Config();
  MCO1_Init();
  
  delay_ms(400);
  if(DCMI_SingleRandomWrite(OV7670_COM7, SCCB_REG_RESET)!= 0)
  while (1)
  {
    //error
  }
    
  delay_ms(400);
  
#ifdef CAMERA_INIT_I2C
  for(i=0;i<length;i++)
  {
    if(DCMI_SingleRandomWrite(change_reg2[i][0],change_reg2[i][1])!=0)
    {
      return 0xff;
    }
  }
#endif
  
  delay_ms(100);
  return 0;//Init ok
}

/**
* @brief  Read the OV7670 Manufacturer identifier.
* @param  OV7670ID: pointer to the OV7670 Manufacturer identifier. 
* @retval None
*/
uint8_t DCMI_OV7670_ReadID(OV7670_IDTypeDef* OV7670ID)
{
  uint8_t temp;
  if(DCMI_SingleRandomRead(OV7670_MIDH,&temp)!=0)
    return 0xff;
  OV7670ID->Manufacturer_ID1 = temp;
  if(DCMI_SingleRandomRead(OV7670_MIDL,&temp)!=0)
    return 0xff;
  OV7670ID->Manufacturer_ID2 = temp;
  if(DCMI_SingleRandomRead(OV7670_VER,&temp)!=0)
    return 0xff;
  OV7670ID->Version = temp;
  if(DCMI_SingleRandomRead(OV7670_PID,&temp)!=0)
    return 0xff;
  OV7670ID->PID = temp;
  
  return 0;
}
