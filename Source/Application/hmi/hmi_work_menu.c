#include "hmi.h"

#define ID_LISTBOX_1     (GUI_ID_USER + 0x01)

static const u32 menuListTextId[] =
{
	STR_MENU_MODE_SELECTION,
	STR_MENU_BRAND_SELECTION,
	STR_MENU_SYSTEM_CONFIG,
	STR_MENU_ADVANCED_SETTING,
	STR_MENU_ABOUT,
};
#define WORK_MENU_ITEM_NUM	(sizeof(menuListTextId)/sizeof(menuListTextId[0]))

static const GUI_WIDGET_CREATE_INFO _aWindowCreate[] =
{
	{ WINDOW_CreateIndirect, "", 0, 0, 48, 320, 240, 0, 0x0, 0 },
    { LISTBOX_CreateIndirect, "", ID_LISTBOX_1, 0, 0, 320, 240-48, 0, 0x0, 0 },
};

static void _cbCallback(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	u32 i;

	switch (pMsg->MsgId)
	{
	case HMI_MSG_KEY:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_1);
		switch(pMsg->Data.v)
		{
		case HMI_KEY_UP|HMI_KEY_PRESSED:
			LISTBOX_DecSel(hItem);
			break;
		case HMI_KEY_DN|HMI_KEY_PRESSED:
			LISTBOX_IncSel(hItem);
			break;
		default:
			break;
		}
		break;
	case WM_INIT_DIALOG:
		// 控件初始化
        hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_1);
		LISTBOX_SetBkColor(hItem, LISTBOX_CI_UNSEL, GUI_BLACK);			// 未选中项背景色
		LISTBOX_SetBkColor(hItem, LISTBOX_CI_SEL, GUI_WHITE);			// 选中非焦点项背景色
		LISTBOX_SetBkColor(hItem, LISTBOX_CI_SELFOCUS, GUI_WHITE);		// 选中焦点项背景色
		LISTBOX_SetTextColor(hItem, LISTBOX_CI_UNSEL, GUI_WHITE);		// 未选中项文字色
		LISTBOX_SetTextColor(hItem, LISTBOX_CI_SEL, GUI_BLACK);			// 选中非焦点项文字色
		LISTBOX_SetTextColor(hItem, LISTBOX_CI_SELFOCUS, GUI_BLACK);	// 选中焦点项文字色
		LISTBOX_SetAutoScrollV(hItem, 1);
		LISTBOX_SetFont(hItem, LangGetFont(sysGetLang(), 32));
		for(i=0; i<WORK_MENU_ITEM_NUM; i++)
		{
			LISTBOX_AddString(hItem, LangGetStr(sysGetLang(), menuListTextId[i]));
		}
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

WM_HWIN hmiWorkMenuCreate(WM_HWIN hParent)
{
	return GUI_CreateDialogBox(_aWindowCreate, GUI_COUNTOF(_aWindowCreate), _cbCallback, hParent, 0, 0);
}
