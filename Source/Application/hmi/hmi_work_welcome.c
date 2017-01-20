#include "hmi.h"

#define ID_IMAGE_1     (GUI_ID_USER + 0x01)

static const GUI_WIDGET_CREATE_INFO _aWindowCreate[] =
{
	{ WINDOW_CreateIndirect, "", 0, 0, 0, 320, 240, 0, 0x0, 0 },
    { IMAGE_CreateIndirect, "", ID_IMAGE_1, 20, 20, 240, 64, 0, 0x0, 0 },
};

static void _cbCallback(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	u8 ch;
	HMI_CHAN_ARGS* pArgs;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		// 控件初始化
        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_1);
        IMAGE_SetBitmap(hItem, resGetBitmap(BMP_LOGO));
		// 创建定时器，定时器ID即为通道号
		pArgs = (HMI_CHAN_ARGS*)pvTaskGetThreadLocalStoragePointer(0, 0);
		WM_CreateTimer(pMsg->hWin, pArgs->chan, 2000, 0);
		// 屏蔽开关键
		pArgs->offFlg = HMI_SW_OFF_FLG_NACK;
		break;
	case WM_TIMER:
		// 获取通道号
		ch = WM_GetTimerId(pMsg->Data.v);
		// 删除定时器
		WM_DeleteTimer(pMsg->Data.v);
		// 响应开关键
		// 此处获取任务相关参数时，不能采用当前任务句柄，
		// 因为定时器消息产生时，回调函数通过系统定时任务调用，而不是通道任务
		pArgs = (HMI_CHAN_ARGS*)pvTaskGetThreadLocalStoragePointer(panelTaskGetChanHandle(ch), 0);
		pArgs->offFlg = HMI_SW_OFF_FLG_EN;
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

WM_HWIN hmiWorkWelcomeCreate(WM_HWIN hParent)
{
	return GUI_CreateDialogBox(_aWindowCreate, GUI_COUNTOF(_aWindowCreate), _cbCallback, hParent, 0, 0);
}
