/**
  ******************************************************************************
  * @file    DCMI/OV7670_Camera/dcmi_OV7670.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-April-2011
  * @brief   Header for dcmi_OV7670.c module
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DCMI_OV7670_H
#define __DCMI_OV7670_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "camera/SCCB.h"
#include "config.h"

#define DCMI_DR_ADDRESS     0x50050028

//#define QQVGA_CAMERA //160x120

#define PICT_WIDTH      (640)
#define PICT_HEIGHT     (480)

#define PICTURE_SIZE (PICT_WIDTH*PICT_HEIGHT)

/* Exported types ------------------------------------------------------------*/

typedef enum
{
    LED_DUMMY = 0,
    LED_DISABLED,
    LED_ENABLED,
    
} LedStateType;

typedef struct
{
  uint8_t Manufacturer_ID1;
  uint8_t Manufacturer_ID2;
  uint8_t Version;
  uint8_t PID; 
}OV7670_IDTypeDef; 

/*
typedef struct
{
  uint16_t *buf_pointer;
  uint8_t is_on_flag;
}RawImageBufType;
*/


/* Exported constants --------------------------------------------------------*/

/* Use this define to set the maximum delay timeout for the I2C DCMI_SingleRandomWrite()
   and DCMI_SingleRandomRead() operations. Exeeding this timeout delay, the read/write 
   functions will be aborted and return error code (0xFF).
   The period of the delay will depend on the system operating frequency. The following
   value has been set for system running at 120MHz. */
#define DCMI_TIMEOUT_MAX  10000

/* Uncomment the line corresponding to the camera resolution */
//#define QVGA_SIZE  /*320x240 */
//#define VGA_SIZE  /* 640x480 */

								 
/* OV7670 Registers definition */
#define OV7670_AGC        0x00
#define OV7670_BLUE       0x01
#define OV7670_RED        0x02
#define OV7670_VREF       0x03  
#define OV7670_COM1       0x04  
#define OV7670_BAVE       0x05
#define OV7670_GbAVE      0x06
#define OV7670_AECHH      0x07
#define OV7670_RAVE       0x08
#define OV7670_COM2       0x09  
#define OV7670_PID        0x0A  
#define OV7670_VER        0x0B  
#define OV7670_COM3       0x0C 
#define OV7670_COM4       0x0D  
#define OV7670_COM5       0x0E  
#define OV7670_COM6       0x0F  
#define OV7670_AECH       0x10
#define OV7670_CLKRC      0x11 
#define OV7670_COM7       0x12 
#define OV7670_COM8       0x13 
#define OV7670_COM9       0x14  
#define OV7670_COM10      0x15 
//#define OV7670_RSVD       0x16 
#define OV7670_HSTART     0x17 
#define OV7670_HSTOP      0x18 
#define OV7670_VSTART     0x19 
#define OV7670_VSTOP      0x1A 
#define OV7670_PSHFT      0x1B
#define OV7670_MIDH       0x1C 
#define OV7670_MIDL       0x1D 
#define OV7670_MVFP       0x1E
#define OV7670_LAEC		  0x1F
#define OV7670_ADCCTR0    0x20
#define OV7670_ADCCTR1    0x21
#define OV7670_ADCCTR2    0x22
#define OV7670_ADCCTR3    0x23
#define OV7670_AEW        0x24
#define OV7670_AEB        0x25
#define OV7670_VPT        0x26
#define OV7670_BBIAS      0x27
#define OV7670_GbBIAS     0x28
//#define OV7670_RSVD    	  0x29
#define OV7670_EXHCH      0x2A 
#define OV7670_EXHCL      0x2B 
#define OV7670_RBIAS      0x2C
#define OV7670_ADVFL      0x2D
#define OV7670_ADVFH      0x2E
#define OV7670_YAVE       0x2F
#define OV7670_HSYST      0x30
#define OV7670_HSYEN      0x31
#define OV7670_HREF       0x32  
#define OV7670_CHLF       0x33
#define OV7670_ARBLM      0x34
//#define OV7670_RSVD       0x35
//#define OV7670_RSVD       0x36
#define OV7670_ADC        0x37
#define OV7670_ACOM       0x38
#define OV7670_OFON       0x39
#define OV7670_TSLB       0x3A
#define OV7670_COM11      0x3B  
#define OV7670_COM12      0x3C  
#define OV7670_COM13      0x3D  
#define OV7670_COM14      0x3E  
#define OV7670_EDGE       0x3F
#define OV7670_COM15      0x40 
#define OV7670_COM16      0x41 
#define OV7670_COM17      0x42 
#define OV7670_AWBC1      0x43 
#define OV7670_AWBC2      0x44 
#define OV7670_AWBC3      0x45 
#define OV7670_AWBC4      0x46 
#define OV7670_AWBC5      0x47 
#define OV7670_AWBC6      0x48 
//#define OV7670_RSVD       0x49 
//#define OV7670_RSVD       0x4A 
#define OV7670_REG4B      0x4B 
#define OV7670_DNSTH      0x4C 
//#define OV7670_RSVD       0x4D 
//#define OV7670_RSVD       0x4E 
#define OV7670_MTX1       0x4F 
#define OV7670_MTX2       0x50
#define OV7670_MTX3       0x51
#define OV7670_MTX4       0x52
#define OV7670_MTX5       0x53
#define OV7670_MTX6       0x54 
#define OV7670_BRTN       0x55
#define OV7670_CONTRAS    0x56
#define OV7670_CONTRASCENTER    0x57
#define OV7670_MTXS       0x58
//#define OV7670_RSVD       0x59
//#define OV7670_RSVD       0x5A
//#define OV7670_RSVD       0x5B
//#define OV7670_RSVD       0x5C
//#define OV7670_RSVD       0x5D
//#define OV7670_RSVD       0x5E
//#define OV7670_RSVD       0x5F
//#define OV7670_RSVD       0x60
//#define OV7670_RSVD       0x61
#define OV7670_LCC1       0x62
#define OV7670_LCC2       0x63
#define OV7670_LCC3       0x64
#define OV7670_LCC4       0x65
#define OV7670_LCC5       0x66
#define OV7670_MANU       0x67
#define OV7670_MANV       0x68
#define OV7670_GFIX       0x69
#define OV7670_GGAIN      0x6A
#define OV7670_DBLV       0x6B
#define OV7670_AWBCTR3    0x6C
#define OV7670_AWBCTR2    0x6D
#define OV7670_AWBCTR1    0x6E
#define OV7670_AWBCTR0    0x6F
#define OV7670_SCALING_XSC      0x70
#define OV7670_SCALING_YSC      0x71
#define OV7670_SCALING_DCWCTR   0x72 
#define OV7670_SCALING_PC       0x73 
#define OV7670_REG74      0x74
#define OV7670_REG75      0x75
#define OV7670_REG76      0x76
#define OV7670_REG77      0x77
//#define OV7670_RSVD       0x78
//#define OV7670_RSVD       0x79
#define OV7670_SLOP       0x7A
#define OV7670_GAM1       0x7B
#define OV7670_GAM2       0x7C
#define OV7670_GAM3       0x7D
#define OV7670_GAM4       0x7E
#define OV7670_GAM5       0x7F
#define OV7670_GAM6       0x80
#define OV7670_GAM7       0x81
#define OV7670_GAM8       0x82
#define OV7670_GAM9       0x83
#define OV7670_GAM10      0x84
#define OV7670_GAM11      0x85
#define OV7670_GAM12      0x86
#define OV7670_GAM13      0x87
#define OV7670_GAM14      0x88
#define OV7670_GAM15      0x89
//#define OV7670_RSVD       0x8A
//#define OV7670_RSVD       0x8B
#define OV7670_RGB444     0x8C
//#define OV7670_RSVD       0x8D
//#define OV7670_RSVD       0x8E
//#define OV7670_RSVD       0x8F
//#define OV7670_RSVD       0x90
//#define OV7670_RSVD       0x91
#define OV7670_DM_LNL     0x92
#define OV7670_DM_LNH     0x93
#define OV7670_LCC6       0x94
#define OV7670_LCC7       0x95
//#define OV7670_RSVD       0x96
//#define OV7670_RSVD       0x97
//#define OV7670_RSVD       0x98
//#define OV7670_RSVD       0x99
//#define OV7670_RSVD       0x9A
//#define OV7670_RSVD       0x9B
//#define OV7670_RSVD       0x9C
#define OV7670_BD50ST     0x9D
#define OV7670_BD60ST     0x9E
#define OV7670_HAECC1     0x9F
#define OV7670_HAECC2     0xA0
//#define OV7670_RSVD       0xA1
#define OV7670_SCALING_PCLK_DELAY       0xA2
//#define OV7670_RSVD       0xA3
#define OV7670_NT_CTRL    0xA4 
#define OV7670_BD50MAX    0xA5 
#define OV7670_HAECC3     0xA6
#define OV7670_HAECC4     0xA7
#define OV7670_HAECC5     0xA8
#define OV7670_HAECC6     0xA9
#define OV7670_HAECC7     0xAA
#define OV7670_BD60MAX    0xAB
#define OV7670_STR_OPT    0xAC
#define OV7670_STR_R      0xAD
#define OV7670_STR_G      0xAE
#define OV7670_STR_B      0xAF
#define OV7670_RSVD       0xB0
#define OV7670_ABLC1      0xB1
//#define OV7670_RSVD       0xB2
#define OV7670_THL_DLT    0xB3
//#define OV7670_RSVD       0xB4
#define OV7670_THL_DLT_AREA    0xB5
//#define OV7670_RSVD       0xB6
//#define OV7670_RSVD       0xB7
//#define OV7670_RSVD       0xB8
//#define OV7670_RSVD       0xBC
//#define OV7670_RSVD       0xBD
#define OV7670_AD_CHB     0xBE
#define OV7670_AD_CHR     0xBF
#define OV7670_AD_CHGb    0xC0
#define OV7670_AD_CHGr    0xC1
//#define OV7670_RSVD       0xC2
//#define OV7670_RSVD       0xC3
//#define OV7670_RSVD       0xC4 
//#define OV7670_RSVD       0xC5
//#define OV7670_RSVD       0xC6
//#define OV7670_RSVD       0xC7 
//#define OV7670_RSVD       0xC8
#define OV7670_SATCTR     0xC9

/* Registers bit definition */
/* COM1 Register */
#define CCIR656_FORMAT  0x40
#define HREF_SKIP_0     0x00
#define HREF_SKIP_1     0x04
#define HREF_SKIP_3     0x08

/* COM2 Register */
#define SOFT_SLEEP_MODE  0x10	
#define ODCAP_1x         0x00	
#define ODCAP_2x         0x01	
#define ODCAP_3x         0x02	
#define ODCAP_4x         0x03
	
/* COM3 Register */
#define COLOR_BAR_OUTPUT         0x80
#define OUTPUT_MSB_LAS_SWAP      0x40
#define PIN_REMAP_RESETB_EXPST   0x08 
#define RGB565_FORMAT            0x00 
#define RGB_OUTPUT_AVERAGE       0x04 
#define SINGLE_FRAME             0x01

/* COM5 Register */
#define SLAM_MODE_ENABLE      0x40
#define EXPOSURE_NORMAL_MODE  0x01

/* COM7 Register */
#define SCCB_REG_RESET                       0x80
#define FORMAT_CTRL_15fpsVGA                 0x00
#define FORMAT_CTRL_30fpsVGA_NoVArioPixel    0x50
#define FORMAT_CTRL_30fpsVGA_VArioPixel      0x60
#define OUTPUT_FORMAT_RAWRGB                 0x00
#define OUTPUT_FORMAT_RAWRGB_DATA            0x00
#define OUTPUT_FORMAT_RAWRGB_INTERP          0x01
#define OUTPUT_FORMAT_YUV                    0x02
#define OUTPUT_FORMAT_RGB                    0x03

/* COM9 Register */
#define GAIN_2x         0x00	
#define GAIN_4x         0x10	
#define GAIN_8x         0x20	
#define GAIN_16x        0x30	
#define GAIN_32x        0x40	
#define GAIN_64x        0x50	
#define GAIN_128x       0x60	
#define DROP_VSYNC      0x04	
#define DROP_HREF       0x02
	
/* COM10 Register */
#define RESETb_REMAP_SLHS    0x80
#define HREF_CHANGE_HSYNC    0x40
#define PCLK_ON              0x00
#define PCLK_OFF             0x20
#define PCLK_POLARITY_REV    0x10
#define HREF_POLARITY_REV    0x08
#define RESET_ENDPOINT       0x04
#define VSYNC_NEG            0x02
#define HSYNC_NEG            0x01

/* TSLB Register */
#define PCLK_DELAY_0         0x00
#define PCLK_DELAY_2         0x40
#define PCLK_DELAY_4         0x80
#define PCLK_DELAY_6         0xC0
#define OUTPUT_BITWISE_REV   0x20
#define UV_NORMAL            0x00
#define UV_FIXED             0x10
#define YUV_SEQ_YUYV         0x00
#define YUV_SEQ_YVYU         0x02
#define YUV_SEQ_VYUY         0x04
#define YUV_SEQ_UYVY         0x06
#define BANDING_FREQ_50      0x02

#define RGB_NORMAL   0x00 
#define RGB_565      0x10 
#define RGB_555      0x30 

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void DCMI_Config(void);
void MCO1_Init(void);
uint8_t DCMI_OV7670_Init(void);
uint8_t DCMI_OV7670_ReadID(OV7670_IDTypeDef* OV7670ID);


void start_new_frame_capture(void);
void process_camera_event_flag(void);

#endif /* __DCMI_OV7670_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
