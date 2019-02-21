#include "stm32f10x.h"
#include <math.h>

#include "..\bsp\bsp_led.h"
#include "..\bsp\SysTick.h"
#include "..\bsp\bsp_exti.h"
#include "..\bsp\bsp_innerflash.h"
#include "..\bsp\spi_da.h"

#include "modbus_svr.h"

//--------------------------------------------------------------------------------
int main(void)
{
	u8 nChn = 0;
	u32 tick, oldtick = 0;
	short daval[4] = {0, 0, 0, 0};

	SysTick_Init();
	InternalFlashRead(wReg, 200);
	BOOTNUM++;
	bSaved = 1;

	LED_GPIO_Config();
	Modbus_init();

	SetTimer(1, 500);  //喂狗服务程序定时器
	SetTimer(2, 1000); //参数定时保存进程
	SetTimer(3, 100);  //DA口定时刷新进程

	AD5754_init();

	IWDG_Configuration();
	LED1_OFF;

	wReg[0] = 0x8000;
	wReg[1] = 0x8000;
	wReg[2] = 0x8000;
	wReg[3] = 0x8000;

	while (1)
	{
		Modbus_task();

		//------------------------------------------------------------------
		if (GetTimer(1))
		{
			LED1_TOGGLE;
			IWDG_Feed();
		}

		if (GetTimer(2) && bSaved)
		{
			InternalFlashWrite(wReg, 200);
			bSaved = 0;
		}

		if (GetTimer(3)) //DA输出
		{
			if (daval[nChn] != wReg[nChn])
			{
				WriteToAD5754RViaSpi(0x00, nChn, wReg[nChn]);
				daval[nChn] = wReg[nChn];
			}
			tick = GetCurTick();
			nChn = (nChn + 1) % 4;
			wReg[4] = tick - oldtick;
			oldtick = tick;
		}
	}
}
