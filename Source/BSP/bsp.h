#ifndef _BSP_H_
#define _BSP_H_

/* STM32F4xx */
#include "stm32f4xx.h"
#include "bsp_bitband.h"

// 以下板级支持函数，只有在BSP、驱动或main文件中可用
#if defined(_BSP_) || defined(_DRIVER_) || defined(_MAIN_)

/* Delay */
void delay_us(u16 us);
void delay_ms(u16 ms);

/* Beep */
enum
{
	BEEP_OFF = 0,
	BEEP_ON  = 1,
};
void beepInit(void);
void beepOnOff(u8 onoff);

/* LED */
enum
{
	LED_OFF = 0,
	LED_ON  = 1,
};
void ledInit(void);
void ledRunning(u8 onoff);
void ledError(u8 onoff);

/* LCD */
void lcdBspInit(void);

// Touch
void touchI2CInit(void);
void touchI2CStart(void);
void touchI2CStop(void);
u8   touchI2CWaitAck(void);
void touchI2CAck(void);
void touchI2CNAck(void);
void touchI2CSendByte(u8 txd);
u8   touchI2CReadByte(unsigned char ack);

// Key

/* BSP */
#define bspInit()	\
{	\
	/* 优先级分组设置为4，可配置0-15级抢占式优先级，0级子优先级，即不存在子优先级。*/	\
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	\
	beepInit();	\
	ledInit();	\
	lcdBspInit();	\
	touchI2CInit();	\
}

#endif

#endif
