/**********************************************************************************
**********************************************************************************/
#include "spi_da.h"
#include "stm32f10x.h"


static void delay(void)
{
  static int i;
  for (i = 0; i < 50; i++)
    ;
}
/********************************************************************************
 *	@brief	控制端口初始化化
 *	@param	None
 *	@retval	None
 ********************************************************************************/
void AD5754_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  //--------------Digital Output-------------------------------
  RCC_APB2PeriphClockCmd(SYNC_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = SYNC_PIN | SCLK_PIN | SDIN_PIN | LDAC_PIN | CLR_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(SYNC_PORT, &GPIO_InitStructure);

  //--------------Digital Input_-------------------------------
  RCC_APB2PeriphClockCmd(SDO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = SDO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(SDO_PORT, &GPIO_InitStructure);

  delay() ;
  CLR_H;
  LDAC_L;
}



/********************************************************************************
 *	@brief	控制端口初始化化
 *	@param	nChannel: 通道号
 *          sVal：需要写入的值
 *	@retval	读到的参数值
 ********************************************************************************/
void AD5754_Write(u8 *ptrTx, u8 *ptrRx)
{
  static u8 i, j;
  static u8 data;

  SYNC_L;
  SCLK_H;
  delay();

  for (j = 0; j < 3; j++)
  {
    data = *ptrTx ;
    for (i = 0; i < 8; i++)
    {
      if ( data & 0x80 == 0)
        SDIN_L ;
      else
        SDIN_H ;
      delay() ;
      SCLK_L;

      delay() ;

      if ( SDO == 0)
        *ptrRx &= 0xFE ;
      else
        *ptrRx |= 0x01 ;

      data <<= 1 ;
      *ptrRx <<= 1 ;
      SCLK_H;
    }
    ptrRx++ ;
    ptrTx++ ;
  }
  delay() ;

  SYNC_H ;
}



/*----------------------------------END OF FILE------------------------*/
