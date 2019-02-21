/**********************************************************************************
**********************************************************************************/
#include "spi_da.h"
#include "stm32f10x.h"

//delay 1us------------------
static void delayus(void)
{
  int i;
  for (i = 0; i < 71; i++)
    ;
}

static void AD5754_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  //--------------Digital Output-------------------------------
  RCC_APB2PeriphClockCmd(SYNC_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = SYNC_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
  GPIO_Init(SYNC_PORT, &GPIO_InitStructure);

  RCC_APB2PeriphClockCmd(SDIN_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = SCLK_PIN | SDIN_PIN | LDAC_PIN | CLR_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
  GPIO_Init(SDIN_PORT, &GPIO_InitStructure);

  //--------------Digital Input_-------------------------------
  RCC_APB2PeriphClockCmd(SDO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = SDO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(SDO_PORT, &GPIO_InitStructure);
}
/********************************************************************************
 *	@brief	控制端口初始化化
 *	@param	None
 *	@retval	None
 ********************************************************************************/
void AD5754_init(void)
{
  u8 DATx[3];
  u8 DARx[3];

  AD5754_GPIO_Init();

  delayus();
  CLR_H;
  LDAC_L;

  //设置量程 0:5v 1:10v 2:10.8v 3:+-5v 4:+-10v 5:+-10.8v
  WriteToAD5754RViaSpi(0x01, 0x04, 0x0004);
  //打开电源
  WriteToAD5754RViaSpi(0x02, 0x00, 0x000F);
  //初始化输出为0v
  WriteToAD5754RViaSpi(0x00, 0x04, 0x0000);
}

/********************************************************************************
 *	@brief	写命令到AD5754
 *	@param	nCmd: 命令号
 *          nChn:通道号
 *          uVal：需要写入的值
 *	@retval	None
 ********************************************************************************/
void WriteToAD5754RViaSpi(u8 uCmd, u8 uChn, u16 uVal)
{

  uint32_t ValueToWrite;
  int i;

  ValueToWrite = 0;
  ValueToWrite |= (uCmd & 0x07) << 19;
  ValueToWrite |= (uChn & 0x07) << 16;
  ValueToWrite |= uVal;

  // SPI start
  SYNC_H;
  delay(3);
  SYNC_L; //bring CS low
  delay(1);

  //Write out the ControlWord
  for (i = 0; i < 24; i++)
  {
    SCLK_H;
    if (0x800000 == (ValueToWrite & 0x800000))
      SDIN_H; //Send one to SDI pin
    else
      SDIN_L; //Send zero to SDI pin
    delay(1);

    SCLK_L;
    delay(2);

    ValueToWrite <<= 1; //Rotate data
  }

  // SPI ends
  delay(4); 
  SYNC_H;
  delay(10);
}

/*----------------------------------END OF FILE------------------------*/
