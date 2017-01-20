#define _BSP_

#include "bsp.h"

#define LED_Error		PFout(9)
#define LED_Running		PFout(10)

/* LED ≥ı ºªØ */
void ledInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	//GPIOF9,F10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	GPIO_SetBits(GPIOF, GPIO_Pin_9 | GPIO_Pin_10);
}

void ledRunning(u8 onoff)
{
	LED_Running = onoff?0:1;
}

void ledError(u8 onoff)
{
	LED_Error = onoff?0:1;
}
