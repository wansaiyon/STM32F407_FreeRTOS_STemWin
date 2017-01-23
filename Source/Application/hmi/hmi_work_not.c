#include "hmi.h"

#define ID_TEXT_1   (GUI_ID_USER + 0x01)

static const GUI_WIDGET_CREATE_INFO _aWindowCreate[] =
{
	{ WINDOW_CreateIndirect, "", 0, 0, 0, 320, 240, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "", ID_TEXT_1, 32, 0, 320-32, 240, 0, 0x0, 0 },
};

static void _cbCallback(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;

	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
//		WINDOW_SetBkColor(pMsg->hWin, GUI_WHITE);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
        TEXT_SetFont(hItem, LangGetFont(sysGetLang(), 24));
        TEXT_SetText(hItem, LangGetStr(sysGetLang(), STR_INFO_CAN_NOT_INTO_WORK));
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

WM_HWIN hmiWorkNotCreate(WM_HWIN hParent)
{
	return GUI_CreateDialogBox(_aWindowCreate, GUI_COUNTOF(_aWindowCreate), _cbCallback, hParent, 0, 0);
}
