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
  GPIO_InitStructure.GPIO_Pin = SDIN_PIN | LDAC_PIN | CLR_PIN;
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

  delayus();
  DATx[0] = 0x0C; //设置量程
  DATx[1] = 0x00;
  DATx[2] = 0x04; //0:5v 1:10v 2:10.8v 3:+-5v 4:+-10v 5:+-10.8v
  AD5754_Write(DATx, DARx);
  WriteToAD5754RViaSpi(0x01, 0x04, 0x0004);

  delayus();
  DATx[0] = 0x10; //打开电源
  DATx[1] = 0x00;
  DATx[2] = 0x0F;
  AD5754_Write(DATx, DARx);
  WriteToAD5754RViaSpi(0x02, 0x04, 0x000F);

  delayus();
  DATx[0] = 0x04; //初始化输出为0v
  DATx[1] = 0x00;
  DATx[2] = 0x00;
  AD5754_Write(DATx, DARx);
  WriteToAD5754RViaSpi(0x00, 0x04, 0x0000);
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
  __NOP();

  for (j = 0; j < 3; j++)
  {
    data = *ptrTx;
    for (i = 0; i < 8; i++)
    {
      if (data & 0x80)
        SDIN_H; //数据信号为高
      else
        SDIN_L;   //数据信号为低
      __NOP();    //插入等待周期
      SCLK_L;     //时钟信号低电平
      data <<= 1; //数据左移1位

      if (SDO)
        *ptrRx |= 0x01;
      else
        *ptrRx &= 0xFE;
      *ptrRx <<= 1;

      SCLK_H; //时钟信号高电平
    }
    ptrRx++;
    ptrTx++;
    __NOP();
  }

  SDIN_H;
  SYNC_H;
}

//Delay;

void delay(int length)
{
  while (length > 0)
    length--;
}

/********************************************************************************
Function that writes to the AD5754R via the SPI port.
********************************************************************************/
void WriteToAD5754RViaSpi(u8 uCmd, u8 uChn, u16 uVal)
{

  int ValueToWrite;
  int i;

  ValueToWrite = (uCmd & 0x07) << 3;
  ValueToWrite |= (uChn & 0x07) << 16;
  ValueToWrite |= uVal;

  // SPI start
  SYNC_H;
  delay(10);
  SYNC_L; //bring CS low
  delay(1);

  //Write out the ControlWord
  for (i = 0; i < 24; i++)
  {
    SCLK_H;
    delay(5);
    if (0x800000 == (ValueToWrite & 0x800000))
      SDIN_H; //Send one to SDI pin
    else
      SDIN_L; //Send zero to SDI pin

    delay(5);
    SCLK_L;
    delay(5);

    ValueToWrite <<= 1; //Rotate data
    delay(5);
  }

  // SPI ends
  SYNC_H;
  delay(10);
}

/*----------------------------------END OF FILE------------------------*/
