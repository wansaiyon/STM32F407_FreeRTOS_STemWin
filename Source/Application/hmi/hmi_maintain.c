#include "hmi.h"

/*******************************************************************************
* 变量定义
*******************************************************************************/
// 该变量表示是否有一个通道处于维护模式
// 根据设计，同一时刻最多只允许一个通道进入维护模式
// 且在有一个通道处于维护模式时，其他通道将不允许开机
static u32 hmiAnyChanInMaintain = 0;
#define hmiChanInMaintainInc()  {hmiAnyChanInMaintain++;}
#define hmiChanInMaintainDec()  {if(hmiAnyChanInMaintain)hmiAnyChanInMaintain--;}

/*******************************************************************************
* hmiIsAnyChanInMaintain
*******************************************************************************/
u8 hmiIsAnyChanInMaintain(void)
{
	return hmiAnyChanInMaintain? 1 : 0;
}

/*******************************************************************************
* 维护模式失败提示界面
*******************************************************************************/
#define ID_TEXT_1   (GUI_ID_USER + 0x01)

static const GUI_WIDGET_CREATE_INFO _aCanNotMaintainCreate[] =
{
	{ WINDOW_CreateIndirect, "", 0, 0, 0, 320, 240, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "", ID_TEXT_1, 32, 0, 320-32, 240, 0, 0x0, 0 },
};

static void _cbCanNotMaintain(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
//      WINDOW_SetBkColor(pMsg->hWin, GUI_WHITE);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
		TEXT_SetFont(hItem, LangGetFont(sysGetLang(), 24));
        TEXT_SetText(hItem, LangGetStr(sysGetLang(), STR_INFO_CAN_NOT_INTO_MAINTAIN));
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

/*******************************************************************************
* 维护模式界面
*******************************************************************************/
static const GUI_WIDGET_CREATE_INFO _aMaintainCreate[] =
{
	{ WINDOW_CreateIndirect, "", 0, 0, 0, 320, 240, 0, 0x0, 0 },
};

static void _cbMaintain(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		WINDOW_SetBkColor(pMsg->hWin, GUI_RED);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

/*******************************************************************************
* 维护模式创建界面
*******************************************************************************/
void hmiMaintainCreate(void)
{
	HMI_CHAN_ARGS* pArgs;

	// 获取任务TCB关联的参数
	pArgs = (HMI_CHAN_ARGS*)pvTaskGetThreadLocalStoragePointer(0, 0);

	// 创建界面
}

/*******************************************************************************
* 维护模式退出
*******************************************************************************/
void hmiMaintainExit(void)
{
}
