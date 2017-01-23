#include "hmi.h"

#define ID_TEXT_INFO   (GUI_ID_USER + 0x01)
#define ID_IMAGE_BED   (GUI_ID_USER + 0x02)

static const GUI_WIDGET_CREATE_INFO _aWindowCreate[] =
{
	{ WINDOW_CreateIndirect,	"", 0,			  0,  0, 320, 48, 0, 0x0, 0 },
	{ TEXT_CreateIndirect,		"", ID_TEXT_INFO, 0, 24, 320, 24, 0, 0x0, 0 },
	{ IMAGE_CreateIndirect,		"",	ID_IMAGE_BED, 0,  0,  24, 24, 0,   0, 0 },
};

static void _cbCallback(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;

	switch (pMsg->MsgId)
	{
	case HMI_MSG_STAT_INFO:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_INFO);
		TEXT_SetText(hItem, (const char*)(pMsg->Data.p));
		break;
	case WM_INIT_DIALOG:
		WINDOW_SetBkColor(pMsg->hWin, GUI_LIGHTBLUE);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_BED);
        IMAGE_SetBitmap(hItem, resGetBitmap(BMP_BED_24PX));
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_INFO);
        TEXT_SetFont(hItem, LangGetFont(sysGetLang(), 24));
		TEXT_SetTextColor(hItem, GUI_WHITE);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

WM_HWIN hmiWorkStatusCreate(WM_HWIN hParent)
{
	return GUI_CreateDialogBox(_aWindowCreate, GUI_COUNTOF(_aWindowCreate), _cbCallback, hParent, 0, 0);
}
