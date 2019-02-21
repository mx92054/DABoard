#ifndef __SPI_DA_H
#define __SPI_DA_H

#include "stm32f10x.h"

// AD5754 Define
#define SYNC_PIN GPIO_Pin_8
#define SYNC_PORT GPIOA
#define SYNC_CLK RCC_APB2Periph_GPIOA

#define SCLK_PIN GPIO_Pin_9
#define SCLK_PORT GPIOA
#define SCLK_CLK RCC_APB2Periph_GPIOA

#define SDIN_PIN GPIO_Pin_10
#define SDIN_PORT GPIOA
#define SDIN_CLK RCC_APB2Periph_GPIOA

#define LDAC_PIN GPIO_Pin_11
#define LDAC_PORT GPIOA
#define LDAC_CLK RCC_APB2Periph_GPIOA

#define CLR_PIN GPIO_Pin_12
#define CLR_PORT GPIOA
#define CLR_CLK RCC_APB2Periph_GPIOA

#define SDO_PIN GPIO_Pin_15
#define SDO_PORT GPIOB
#define SDO_CLK RCC_APB2Periph_GPIOB

//-------------------SPI1 Define-----------------------------------------------
#define SYNC_H  GPIO_SetBits(SYNC_PORT, SYNC_PIN)
#define SYNC_L  GPIO_ResetBits(SYNC_PORT, SYNC_PIN)

#define SCLK_H  GPIO_SetBits(SCLK_PORT, SCLK_PIN)
#define SCLK_L  GPIO_ResetBits(SCLK_PORT, SCLK_PIN)

#define SDIN_H  GPIO_SetBits(SDIN_PORT, SDIN_PIN)
#define SDIN_L  GPIO_ResetBits(SDIN_PORT, SDIN_PIN)

#define LDAC_H  GPIO_SetBits(LDAC_PORT, LDAC_PIN)
#define LDAC_L  GPIO_ResetBits(LDAC_PORT, LDAC_PIN)

#define CLR_H  GPIO_SetBits(CLR_PORT, CLR_PIN)
#define CLR_L  GPIO_ResetBits(CLR_PORT, CLR_PIN)

#define SDO     GPIO_ReadInputDataBit(SDO_PORT,SDO_PIN)


//-------------Fucntion define---------------------------------------------
void AD5754_init(void) ;
void WriteToAD5754RViaSpi(u8 uCmd, u8 uChn, u16 uVal) ;

#endif 
/*----------------------------------END OF FILE------------------------*/
