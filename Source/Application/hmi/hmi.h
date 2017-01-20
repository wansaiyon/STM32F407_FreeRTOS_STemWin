#ifndef _HMI_H_
#define _HMI_H_

#include "includes.h"
#include "language.h"
#include "resource.h"

/*******************************************************************************
* 界面创建函数类型定义
*******************************************************************************/
typedef WM_HWIN(*pWinCreate)(WM_HWIN);

/*******************************************************************************
* 通道号定义
*******************************************************************************/
enum
{
	CHAN_1,
	CHAN_2,
	CHAN_NUM,
};

/*******************************************************************************
* 通道号模式定义
*******************************************************************************/
enum
{
	CHAN_MODE_WORK,
	CHAN_MODE_MAINTAIN,
};

/*******************************************************************************
* 自定义消息
*******************************************************************************/
enum
{
	HMI_MSG_ARGS_COPY_DONE = WM_USER,
	HMI_MSG_KEY,
    HMI_MSG_SW_OFF,
    HMI_MSG_ALARM,
};

/*******************************************************************************
* 按键定义
*******************************************************************************/
// 按键状态定义
#define HMI_KEY_PRESSED		0x80
#define HMI_KEY_RELEASED	0x00

// 获取按键值
#define HMI_KEY_VALUE(k)    ((k)&0x7F)

// 按键值定义
enum
{
	HMI_KEY_SW,
	HMI_KEY_MENU,
	HMI_KEY_BACK,
	HMI_KEY_S_P,
	HMI_KEY_FUP,
	HMI_KEY_UP,
	HMI_KEY_DN,
	HMI_KEY_FDN,
	HMI_KEY_ENTER,
};

/*******************************************************************************
* 报警定义
*******************************************************************************/
enum
{
    HMI_ALM_SW_OFF = 0x00,
};

/*******************************************************************************
* Panel
*******************************************************************************/
void panelWinStoreMsg(WM_MESSAGE* msg);
void panelTaskStoreArgsCopyDone(void);
TaskHandle_t panelTaskGetChanHandle(u8 chan);

/*******************************************************************************
* HMI
*******************************************************************************/
BaseType_t hmiStoreMsg(u8 chan, WM_MESSAGE* pMsg, u32 delay);
BaseType_t hmiGetMsg(u8 chan, WM_MESSAGE* pMsg, u32 delay);
void hmiTask(void* pArgs);

/*******************************************************************************
* HMI Switch
*******************************************************************************/
enum
{
	// 关机使能项
	HMI_SW_OFF_FLG_EN = 0,		// 允许关机
	HMI_SW_OFF_FLG_DIS_WIN,		// 禁止关机并显示提示界面
	HMI_SW_OFF_FLG_NACK,		// 禁止关机且不响应开关键
	
	// 关机函数参数
	HMI_SW_OFF_START = HMI_KEY_SW|HMI_KEY_PRESSED,	// 开始关机
	HMI_SW_OFF_ONGOING = 0X55,						// 正在关机
	HMI_SW_OFF_STOP = HMI_KEY_SW|HMI_KEY_RELEASED,	// 停止关机

	// 关机函数返回值
	HMI_SW_OFF_RET_ONGOING = 0,		// 正在关机
	HMI_SW_OFF_RET_ABORT,			// 关机终止
	HMI_SW_OFF_RET_DONE,			// 关机成功
	HMI_SW_OFF_RET_ERROR,			// 关机出错
};
u8 hmiSwitchOff(u8 opt);

/*******************************************************************************
* HMI Work
*******************************************************************************/
enum	// 工作界面定义
{
	HMI_WORK_WIN_CAN_NOT_INTO_WORK = 0,
	HMI_WORK_WIN_WELCOME,
//	HMI_WORK_WIN_WORK,
	HMI_WORK_WIN_NUM,
};
WM_HWIN hmiWorkNotCreate(WM_HWIN hParent);
WM_HWIN hmiWorkWelcomeCreate(WM_HWIN hParent);
u8   hmiIsAnyChanInWork(void);
void hmiWorkCreate(void);
void hmiWorkExit(void);

/*******************************************************************************
* HMI Maintain
*******************************************************************************/
enum	// 维护界面定义
{
	HMI_MT_WIN_CAN_NOT_INTO_MAINTAIN = 0,
	HMI_MT_WIN_MAINTAIN,
	HMI_MT_WIN_NUM,
};
u8   hmiIsAnyChanInMaintain(void);
void hmiMaintainCreate(void);
void hmiMaintainExit(void);

/*******************************************************************************
* 界面任务参数结构
*******************************************************************************/
typedef struct
{
	u8			chan;		// 任务对应的通道号。0或 1
	u8			mode;		// 任务模式。0-工作模式，1-维护模式
	u8			offFlg;		// 通道关机标志。0-允许关机，1-禁止关机并弹出界面，2-不响应关机按钮
	WM_HWIN		hWin;		// 通道对应的窗口句柄
	WM_HWIN		hWinOff;	// 通道关机窗口句柄
	WM_HWIN		hWinStat;	// 通道状态栏窗口句柄
	WM_HWIN		hWinClient;	// 通道主窗口句柄
	WM_HWIN		hWinAlarm;	// 通道报警窗口句柄
#if HMI_WORK_WIN_NUM > HMI_MT_WIN_NUM
	WM_HWIN		hWinList[HMI_WORK_WIN_NUM];
#else
	WM_HWIN		hWinList[HMI_MT_WIN_NUM];
#endif
	TickType_t	tick;		// 当任务需要定时，但又不能阻塞时，该变量记录定时起点的系统时钟
}HMI_CHAN_ARGS;

#endif
