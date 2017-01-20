#include "hmi.h"

/*******************************************************************************
* ��������
*******************************************************************************/
// �ñ�����ʾ�Ƿ���һ��ͨ������ά��ģʽ
// ������ƣ�ͬһʱ�����ֻ����һ��ͨ������ά��ģʽ
// ������һ��ͨ������ά��ģʽʱ������ͨ������������
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
* ά��ģʽʧ����ʾ����
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
* ά��ģʽ����
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
* ά��ģʽ��������
*******************************************************************************/
void hmiMaintainCreate(void)
{
	HMI_CHAN_ARGS* pArgs;

	// ��ȡ����TCB�����Ĳ���
	pArgs = (HMI_CHAN_ARGS*)pvTaskGetThreadLocalStoragePointer(0, 0);

	// ��������
}

/*******************************************************************************
* ά��ģʽ�˳�
*******************************************************************************/
void hmiMaintainExit(void)
{
}
