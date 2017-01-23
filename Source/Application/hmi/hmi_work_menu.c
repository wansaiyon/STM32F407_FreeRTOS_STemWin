#include "hmi.h"

#define ID_LISTBOX_1     (GUI_ID_USER + 0x01)

typedef struct
{
	u16 textId;
	u16 nextWinId;
}menuListInfo_t;
static const menuListInfo_t menuListInfo[] =
{
	{ STR_MENU_MODE_SELECTION,		0},
	{ STR_MENU_BRAND_SELECTION,		0},
	{ STR_MENU_SYSTEM_CONFIG,		0},
	{ STR_MENU_ADVANCED_SETTING,	0},
	{ STR_MENU_ABOUT,				0},
};
#define WORK_MENU_ITEM_NUM	(sizeof(menuListInfo)/sizeof(menuListInfo[0]))

static const GUI_WIDGET_CREATE_INFO _aWindowCreate[] =
{
	{ WINDOW_CreateIndirect, "", 0, 0, 48, 320, 240, 0, 0x0, 0 },
    { LISTBOX_CreateIndirect, "", ID_LISTBOX_1, 0, 0, 320, 240-48, 0, 0x0, 0 },
};

static void _cbCallback(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	WM_MESSAGE msg;
	u32 i;
	HMI_CHAN_ARGS* pArgs;

	switch (pMsg->MsgId)
	{
	case HMI_MSG_KEY:
		if(pMsg->Data.v & HMI_KEY_PRESSED)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_1);
			switch(pMsg->Data.v)
			{
			case HMI_KEY_UP:
			case HMI_KEY_FUP:
				LISTBOX_DecSel(hItem);
				break;
			case HMI_KEY_DN:
			case HMI_KEY_FDN:
				LISTBOX_IncSel(hItem);
				break;
			case HMI_KEY_ENTER:
				pArgs = (HMI_CHAN_ARGS*)pvTaskGetThreadLocalStoragePointer(0, 0);
				hItem = pArgs->hWinList[menuListInfo[LISTBOX_GetSel(hItem)].nextWinId];
				if(WM_IsWindow(hItem))
				{
					// 更新状态栏信息
					msg.MsgId = HMI_MSG_STAT_INFO;
					msg.Data.p = LangGetStr(sysGetLang(), STR_MENU);
					WM_SendMessage(pArgs->hWinStat, &msg);
					// 隐藏自身
					WM_HideWindow(pMsg->hWin);
					// 显示下一个界面
					WM_ShowWindow(hItem);
				}
				break;
			default:
				break;
			}
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
			LISTBOX_AddString(hItem, LangGetStr(sysGetLang(), menuListInfo[i].textId));
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
