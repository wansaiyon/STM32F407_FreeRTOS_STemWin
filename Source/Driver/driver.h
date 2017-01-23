#ifndef _DRIVER_H_
#define _DRIVER_H_

/* BSP */
#include "bsp.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

/* emWIN */
#include "GUI.h"
#include "DIALOG.h"

/* Beep */
void beepOnce(void);
void beepCycle(u8 onoff);

/* LED */
void ledRunningIndicator(void);
void ledErrorIndicator(void);

/* LCD */
#define BSP_LCD_SWAP_RB		1
#define BSP_LCD_SWAP_XY		1
#define BSP_LCD_MIRROR_X	0
#define BSP_LCD_MIRROR_Y	1
#if BSP_LCD_SWAP_XY
	#define LCD_XSIZE		800
	#define LCD_YSIZE		480
#else
	#define LCD_XSIZE		480
	#define LCD_YSIZE		800
#endif
void lcdControllerInit(void);
void lcdSetBacklight(u8 bl);
void lcdDrawPoint(u16 x, u16 y, u16 c);
u16  lcdGetPoint(u16 x, u16 y);
void lcdFillRect(u16 x0, u16 y0, u16 x1, u16 y1, u16 c);
void lcdSetRamLine(u16 x, u16 y, uc16* pDat, u16 cnt);

/* Touch */
void touchGt9147Init(void);
u32  touchGt9147Scan(void);

/* Key */
typedef struct
{
	// ÿλ��ʾһ�����������ɱ�ʾ32������
	u32 key;	// �Ƿ��а���������1-�ǣ�0-��
	u32 pre;	// ���������ǰ��»����ɿ���1-���£�0-�ɿ�
}KEY;
KEY  keyGetKey(void);

/* System Parameters */
#define sysGetLang()	(1)

/* Init */
#define drvInit()	\
{	\
	bspInit();	\
	lcdControllerInit();	\
	touchGt9147Init();	\
}

#endif
