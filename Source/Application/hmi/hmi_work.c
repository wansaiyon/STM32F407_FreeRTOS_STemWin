#include "hmi.h"

/*******************************************************************************
* 界面创建函数列表定义
*******************************************************************************/
static const pWinCreate hmiWinCreateTbl[HMI_WORK_WIN_NUM] =
{
	hmiWorkNotCreate,
	hmiWorkWelcomeCreate,
	hmiWorkMenuCreate,
	hmiWorkStatusCreate,
};

/*******************************************************************************
* 变量定义
*******************************************************************************/
// 该变量表示是否有一个通道处于用户工作模式
// 根据设计，同一时刻最多只允许一个通道进入维护模式
// 所以当有通道处于用户工作模式时，其他通道不允许进入维护模式
static u32 hmiAnyChanInWork = 0;
#define hmiChanInWorkInc()  {hmiAnyChanInWork++;}
#define hmiChanInWorkDec()  {if(hmiAnyChanInWork)hmiAnyChanInWork--;}

/*******************************************************************************
* hmiIsAnyChanInWork
*******************************************************************************/
u8 hmiIsAnyChanInWork(void)
{
	return hmiAnyChanInWork? 1 : 0;
}

/*******************************************************************************
* 用户工作模式创建
*******************************************************************************/
void hmiWorkCreate(void)
{
	u32 i;
	HMI_CHAN_ARGS* pArgs;

	// 获取任务TCB关联的参数
	pArgs = (HMI_CHAN_ARGS*)pvTaskGetThreadLocalStoragePointer(0, 0);

	// 工作模式通道数加一
	hmiChanInWorkInc();

	// 创建界面
    for(i=0; i<HMI_WORK_WIN_NUM; i++)
    {
		if(!WM_IsWindow(pArgs->hWinList[i] = hmiWinCreateTbl[i](pArgs->hWin)))
		{
			ledErrorIndicator();
		}
		WM_HideWindow(pArgs->hWinList[i]);
	}

	// 显示第一个界面
	if(hmiIsAnyChanInMaintain())
	{
		WM_ShowWindow(pArgs->hWinList[HMI_WORK_WIN_CAN_NOT_INTO_WORK]);
	}
	else
	{
		WM_CreateTimer(pArgs->hWinList[HMI_WORK_WIN_WELCOME], pArgs->chan, 2000, 0);	// 创建定时器，定时器ID即为通道号
		pArgs->offFlg = HMI_SW_OFF_FLG_NACK;											// 屏蔽开关键
		WM_ShowWindow(pArgs->hWinList[HMI_WORK_WIN_WELCOME]);							// 显示界面
	}
}

/*******************************************************************************
* 用户工作模式退出
*******************************************************************************/
void hmiWorkExit(void)
{
	u32 i;
	HMI_CHAN_ARGS* pArgs;

	// 获取任务TCB关联的参数
	pArgs = (HMI_CHAN_ARGS*)pvTaskGetThreadLocalStoragePointer(0, 0);

	// 删除界面
    for(i=0; i<HMI_WORK_WIN_NUM; i++)
    {
		if(!WM_IsWindow(pArgs->hWinList[i]))
		{
			ledErrorIndicator();
		}
		WM_DeleteWindow(pArgs->hWinList[i]);
	}

	// 工作模式通道数减一
	hmiChanInWorkDec();
}
