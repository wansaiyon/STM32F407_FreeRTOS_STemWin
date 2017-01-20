#include "hmi.h"

/*******************************************************************************
* ��ֹ�ػ�����
*******************************************************************************/
#define ID_TEXT_CAN_NOT_OFF         (GUI_ID_USER + 0x01)

static const GUI_WIDGET_CREATE_INFO _aCanNotOffCreate[] =
{
    { WINDOW_CreateIndirect, "", 0, 320/6, 240/6, 320*2/3, 240*2/3, WM_CF_STAYONTOP, 0x0, 0 },
    { TEXT_CreateIndirect, "", ID_TEXT_CAN_NOT_OFF, 0, 0, 320*2/3, 240*2/3, 0, 0x0, 0 },
};

static void _cbCanNotOff(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
//		WINDOW_SetBkColor(pMsg->hWin, GUI_WHITE);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CAN_NOT_OFF);
        TEXT_SetTextAlign(hItem, TEXT_CF_HCENTER | TEXT_CF_VCENTER);
		TEXT_SetFont(hItem, LangGetFont(sysGetLang(), 24));
        TEXT_SetText(hItem, LangGetStr(sysGetLang(), STR_INFO_CAN_NOT_SWITCH_OFF));
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

/*******************************************************************************
* �ػ�����
*******************************************************************************/
#define ID_TEXT_SWITCHING_OFF       (GUI_ID_USER + 0x01)
#define ID_PROGBAR_SWITCHING_OFF    (GUI_ID_USER + 0x02)

static const GUI_WIDGET_CREATE_INFO _aSwitchOffCreate[] =
{
    { WINDOW_CreateIndirect, "", 0, 320/6, 240/6, 320*2/3, 240*2/3, WM_CF_STAYONTOP, 0x0, 0 },
    { TEXT_CreateIndirect, "", ID_TEXT_SWITCHING_OFF, 0, 240*2/3/5, 320*2/3, 240*2/3/5, 0, 0x0, 0 },
    { PROGBAR_CreateIndirect, "", ID_PROGBAR_SWITCHING_OFF, 320*2/3/6, 240*2/3/5*3, 320*4/9, 240*2/3/5, 0, 0x0, 0 },
};

static void _cbSwitchOff(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;

	// ��Ϣ����
	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
//		WINDOW_SetBkColor(pMsg->hItem, GUI_WHITE);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SWITCHING_OFF);
		TEXT_SetFont(hItem, LangGetFont(sysGetLang(), 24));
		TEXT_SetText(hItem, LangGetStr(sysGetLang(), STR_INFO_SWITCHING_OFF));
		TEXT_SetTextAlign(hItem, TEXT_CF_HCENTER);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_SWITCHING_OFF);
        PROGBAR_SetMinMax(hItem, 0, 3000);
        PROGBAR_SetText(hItem, ""); // ����ʹ����������ʾ�κ�����
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

/*******************************************************************************
* �����ػ�����
*******************************************************************************/
static u8 hmiSwOffNormal(u8 opt)
{
	HMI_CHAN_ARGS* pArgs;
	TickType_t tickcnt;
	u8 ret = HMI_SW_OFF_RET_ONGOING;

	// ��ȡ����TCB�����Ĳ���
	pArgs = (HMI_CHAN_ARGS*)pvTaskGetThreadLocalStoragePointer(0, 0);

	// �ػ��߼�
	switch(opt)
	{
	case HMI_SW_OFF_START:
		if(!WM_IsWindow(pArgs->hWinOff))
		{
			pArgs->tick = xTaskGetTickCount();
			pArgs->hWinOff = GUI_CreateDialogBox(_aSwitchOffCreate, GUI_COUNTOF(_aSwitchOffCreate), _cbSwitchOff, pArgs->hWin, 0, 0);
		}
		break;
	case HMI_SW_OFF_ONGOING:
		if(WM_IsWindow(pArgs->hWinOff))
		{
			tickcnt = xTaskGetTickCount()-pArgs->tick;
			if(tickcnt > 3000)
			{
				WM_DeleteWindow(pArgs->hWinOff);
				pArgs->hWinOff = 0;	// GUI���ظ����þ���ţ����Դ˴���Ҫ�������ŷ�ֹ�����
				ret = HMI_SW_OFF_RET_DONE;
			}
			else
			{
				PROGBAR_SetValue(WM_GetDialogItem(pArgs->hWin, ID_PROGBAR_SWITCHING_OFF), tickcnt);
			}
		}
		break;
	case HMI_SW_OFF_STOP:
		if(WM_IsWindow(pArgs->hWinOff))
		{
			WM_DeleteWindow(pArgs->hWinOff);
			pArgs->hWinOff = 0;	// GUI���ظ����þ���ţ����Դ˴���Ҫ�������ŷ�ֹ�����
			ret = HMI_SW_OFF_RET_ABORT;
		}
		break;
	default:
		break;
	}

	return ret;
}

/*******************************************************************************
* ��ֹ�ػ�����ʾ��ʾ���溯��
*******************************************************************************/
static u8 hmiSwOffDisWin(u8 opt)
{
	HMI_CHAN_ARGS* pArgs;
	u8 ret = HMI_SW_OFF_RET_ONGOING;

	// ��ȡ����TCB�����Ĳ���
	pArgs = (HMI_CHAN_ARGS*)pvTaskGetThreadLocalStoragePointer(0, 0);

	// �ػ��߼�
	switch(opt)
	{
	case HMI_SW_OFF_START:
		if(!WM_IsWindow(pArgs->hWinOff))
		{
			pArgs->tick = xTaskGetTickCount();
			pArgs->hWinOff = GUI_CreateDialogBox(_aCanNotOffCreate, GUI_COUNTOF(_aCanNotOffCreate), _cbCanNotOff, pArgs->hWin, 0, 0);
			beepCycle(1);	// ��������ʾ
		}
		break;
	case HMI_SW_OFF_ONGOING:
		if(WM_IsWindow(pArgs->hWinOff))
		{
			if((xTaskGetTickCount()-pArgs->tick) > 10000)	// ��ֹ�ػ�������³����ػ��� 10s���򷵻ش����־
			{
				WM_DeleteWindow(pArgs->hWinOff);
				pArgs->hWinOff = 0;	// GUI���ظ����þ���ţ����Դ˴���Ҫ�������ŷ�ֹ�����
				beepCycle(0);		// �رշ�������ʾ
				ret = HMI_SW_OFF_RET_ERROR;
			}
		}
		break;
	case HMI_SW_OFF_STOP:
		if(WM_IsWindow(pArgs->hWinOff))
		{
			WM_DeleteWindow(pArgs->hWinOff);
			pArgs->hWinOff = 0;	// GUI���ظ����þ���ţ����Դ˴���Ҫ�������ŷ�ֹ�����
			beepCycle(0);		// �رշ�������ʾ
			ret = HMI_SW_OFF_RET_ABORT;
		}
		break;
	default:
		break;
	}

	return ret;
}

/*******************************************************************************
* �ػ�
*******************************************************************************/
u8 hmiSwitchOff(u8 opt)
{
	HMI_CHAN_ARGS* pArgs;
	u8 ret = HMI_SW_OFF_RET_ONGOING;

	// ��ȡ����TCB�����Ĳ���
	pArgs = (HMI_CHAN_ARGS*)pvTaskGetThreadLocalStoragePointer(0, 0);

	// �ػ��߼�
	switch(pArgs->offFlg)
	{
	case HMI_SW_OFF_FLG_EN:			// ��ǰ����ػ�����ʾ�ػ�����
		ret = hmiSwOffNormal(opt);
		break;
	case HMI_SW_OFF_FLG_DIS_WIN:	// ��ǰ������ػ�������ʾ������ػ���ʾ����
		ret = hmiSwOffDisWin(opt);
		break;
	default:
		break;
	}

	return ret;
}
