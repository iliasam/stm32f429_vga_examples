#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdio.h>
#include "stm32f4xx.h"

/* DCMI Communication boards Interface */

//sda - pb11
//scl - ph4

/**
 * @connected to I2C2
 */
 /* Configure I2C2 pins*/ 
#define BOARD_I2C                        	I2C2
#define BOARD_I2C_CLK                    	RCC_APB1Periph_I2C2

#define BOARD_I2C_SDA_PIN                 	GPIO_Pin_11
#define BOARD_I2C_SDA_GPIO_PORT           	GPIOB
#define BOARD_I2C_SDA_GPIO_CLK            	RCC_AHB1Periph_GPIOB
#define BOARD_I2C_SDA_SOURCE              	GPIO_PinSource11
#define BOARD_I2C_SDA_AF                  	GPIO_AF_I2C2

#define BOARD_I2C_SCL_PIN                 	GPIO_Pin_4
#define BOARD_I2C_SCL_GPIO_PORT           	GPIOH
#define BOARD_I2C_SCL_GPIO_CLK            	RCC_AHB1Periph_GPIOH
#define BOARD_I2C_SCL_SOURCE              	GPIO_PinSource4
#define BOARD_I2C_SCL_AF                  	GPIO_AF_I2C2

#endif	  /*_CONFIG_H*/

