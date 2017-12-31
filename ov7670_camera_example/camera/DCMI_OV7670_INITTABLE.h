#include "camera/dcmi_OV7670.h"

#ifndef _DCMI_OV7670_INITABLE_H
#define _DCMI_OV7670_INITABLE_H


#define  COM3_SCALEEN	  0x08	  /* Enable scaling */
#define  COM3_DCWEN	  0x04	  /* Enable downsamp/crop/window */
#define  COM14_DCWEN	  0x10	  /* DCW/PCLK-scale enable */
#include <stm32f4xx.h>



const uint8_t change_reg2[][2]=
{
  {OV7670_COM7, (1<<2)},//b2+b0 - rgb
  {OV7670_COM15,  0xC0 | (1<<4)},//full range + rgb565
  
  
  {OV7670_CLKRC, 0x02+128},//ext clk / ((1+1)) + 128(x2 clk)
  
  {OV7670_MVFP, 0x07 + (1<<4)},//mirror/flip
  
  {OV7670_RSVD, 0x84},//need for true color!
  
  {OV7670_COM10, 0x18},//polarity configure
  {OV7670_COM2, 0x03},//drive capability - 4x
  

};



#endif
