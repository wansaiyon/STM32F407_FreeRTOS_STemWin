/*******************************************************************************
* 该文件用于提供虚拟面板和处理按键
* 在开发板上用于在大屏幕上构建虚拟按键和2个通道显示区
* 在机器中用于将两块屏幕分别对应到2个通道显示区
*******************************************************************************/

#include "hmi.h"

// 全局变量
static WM_HWIN			hPanel = 0;						// 面板窗口句柄
static WM_HTIMER		hPanelTmr[CHAN_NUM] = {0};		// 定时器句柄
static TaskHandle_t		hChannel[CHAN_NUM] = {0};		// 通道任务句柄
static WM_HWIN			hWinChan[CHAN_NUM] = {0};		// 通道窗口句柄
static u8				chanMode[CHAN_NUM] = {0};		// 通道工作模式
static QueueHandle_t	hPanelQueue = 0;				// 面板任务消息队列

// 局部函数声明
static void panelKeyProcess(WM_MESSAGE * pMsg);
static void panelTaskWaitArgsCopyDone(void);

// 通道界面坐标定义
struct _ChanPos
{
	u32 x, y;
	u32 w, h;
};
static const struct _ChanPos chanPos[CHAN_NUM] =
{
	{  50, 190, 320, 240 },
	{ 430, 190, 320, 240 },
};

/*******************************************************************************
* 通道窗口定义
*******************************************************************************/
// 窗口定义表
static const GUI_WIDGET_CREATE_INFO _aChannelCreate[] =
{
	{ WINDOW_CreateIndirect,	"Channel",		0,		  		0,   0, 320, 240, 0, 0x0, 0 },
};

/*******************************************************************************
* 面板定义
*******************************************************************************/
// 面板控件ID
#define ID_WINDOW_PANEL		(GUI_ID_USER + 0x00)
#define ID_BUTTON_CH1_SW	(GUI_ID_USER + 0x01)
#define ID_BUTTON_CH1_MENU	(GUI_ID_USER + 0x02)
#define ID_BUTTON_CH1_BACK	(GUI_ID_USER + 0x03)
#define ID_BUTTON_CH1_SP	(GUI_ID_USER + 0x04)
#define ID_BUTTON_CH1_FUP	(GUI_ID_USER + 0x05)
#define ID_BUTTON_CH1_UP	(GUI_ID_USER + 0x06)
#define ID_BUTTON_CH1_DN	(GUI_ID_USER + 0x07)
#define ID_BUTTON_CH1_FDN	(GUI_ID_USER + 0x08)
#define ID_BUTTON_CH1_ENTER	(GUI_ID_USER + 0x09)
#define ID_BUTTON_CH2_SW	(GUI_ID_USER + 0x11)
#define ID_BUTTON_CH2_MENU	(GUI_ID_USER + 0x12)
#define ID_BUTTON_CH2_BACK	(GUI_ID_USER + 0x13)
#define ID_BUTTON_CH2_SP	(GUI_ID_USER + 0x14)
#define ID_BUTTON_CH2_FUP	(GUI_ID_USER + 0x15)
#define ID_BUTTON_CH2_UP	(GUI_ID_USER + 0x16)
#define ID_BUTTON_CH2_DN	(GUI_ID_USER + 0x17)
#define ID_BUTTON_CH2_FDN	(GUI_ID_USER + 0x18)
#define ID_BUTTON_CH2_ENTER	(GUI_ID_USER + 0x19)
#define ID_TEXT_KEY_VALUE	(GUI_ID_USER + 0x21)
#define ID_TEXT_KEY_STA     (GUI_ID_USER + 0x22)
#define ID_IMAGE_LOGO		(GUI_ID_USER + 0x23)

// 面板定义表
static const GUI_WIDGET_CREATE_INFO _aPanelCreate[] =
{
	{ WINDOW_CreateIndirect,	"Panel",		ID_WINDOW_PANEL,		  0,   0, 800, 480, 0, 0x0, 0 },
	{ TEXT_CreateIndirect,		"Key Value:",	ID_TEXT_KEY_VALUE,		  0,   0, 800,  32, 0, 0x0, 0 },
    { TEXT_CreateIndirect,		"Key State:",	ID_TEXT_KEY_STA,		  0,  32, 800,  32, 0, 0x0, 0 },
	{ IMAGE_CreateIndirect,		"",				ID_IMAGE_LOGO,			  5,  80, 240,  64, 0, 0x0, 0 },
	// 通道一
	{ BUTTON_CreateIndirect,	"ON/OFF",		ID_BUTTON_CH1_SW,		  5, 190,  40,  40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,	"Menu",			ID_BUTTON_CH1_MENU,		  5, 290,  40,  40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,	"Back",			ID_BUTTON_CH1_BACK,		  5, 340,  40,  40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,	">/ |",			ID_BUTTON_CH1_SP,		  5, 390,  40,  40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,	"^^",			ID_BUTTON_CH1_FUP,		 70, 435,  40,  40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,	"^",			ID_BUTTON_CH1_UP,		130, 435,  40,  40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,	"v",			ID_BUTTON_CH1_DN,		190, 435,  40,  40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,	"vv",			ID_BUTTON_CH1_FDN,		250, 435,  40,  40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,	"Enter",		ID_BUTTON_CH1_ENTER,	310, 435,  40,  40, 0, 0x0, 0 },
	// 通道二
	{ BUTTON_CreateIndirect,	"ON/OFF",		ID_BUTTON_CH2_SW,		755, 190,  40,  40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,	"Menu",			ID_BUTTON_CH2_MENU,		755, 290,  40,  40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,	"Back",			ID_BUTTON_CH2_BACK,		755, 340,  40,  40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,	">/ |",			ID_BUTTON_CH2_SP,		755, 390,  40,  40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,	"^^",			ID_BUTTON_CH2_FUP,		450, 435,  40,  40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,	"^",			ID_BUTTON_CH2_UP,		510, 435,  40,  40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,	"v",			ID_BUTTON_CH2_DN,		570, 435,  40,  40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,	"vv",			ID_BUTTON_CH2_FDN,		630, 435,  40,  40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,	"Enter",		ID_BUTTON_CH2_ENTER,	690, 435,  40,  40, 0, 0x0, 0 },
};

// 面板回调函数
static void _cbPanel(WM_MESSAGE * pMsg)
{
	HMI_CHAN_ARGS taskArgs;
	WM_HWIN hItem;
	u8 ch;

	switch (pMsg->MsgId)
	{
	// 自定义消息处理
	case HMI_MSG_KEY:
		panelKeyProcess(pMsg);
		break;
	case HMI_MSG_SW_OFF:
		ch = pMsg->Data.v;
		WM_DeleteWindow(hWinChan[ch]);	// 删除通道窗口
		hWinChan[ch] = 0;
		hChannel[ch] = 0;
		break;
	// GUI消息处理
	case WM_INIT_DIALOG:
		WINDOW_SetBkColor(pMsg->hWin, GUI_GRAY_50);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_KEY_VALUE);
		TEXT_SetFont(hItem, GUI_FONT_8X16X2X2);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_KEY_STA);
		TEXT_SetFont(hItem, GUI_FONT_8X16X2X2);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_LOGO);
		IMAGE_SetBitmap(hItem, resGetBitmap(BMP_LOGO));
		break;
	case WM_PAINT:
		GUI_SetColor(GUI_BLACK);
		for(ch=0; ch<CHAN_NUM; ch++)
		{
			GUI_FillRect(chanPos[ch].x, chanPos[ch].y, chanPos[ch].x+chanPos[ch].w-1, chanPos[ch].y+chanPos[ch].h-1);
		}
		break;
	case WM_TIMER:
		hItem = pMsg->Data.v;
		ch = WM_GetTimerId(hItem);	// 获取定时器ID,此ID即通道号
		if(hPanelTmr[ch] != hItem)	// 若定时器句柄与保存的句柄不一致，则报错
		{
			ledErrorIndicator();
		}
		WM_DeleteTimer(hItem);		// 删除定时器
		hPanelTmr[ch] = 0;			// 清除定时器句柄
		
		// 创建通道窗口
		hWinChan[ch] = GUI_CreateDialogBox(_aChannelCreate, GUI_COUNTOF(_aChannelCreate), 0, hPanel, chanPos[ch].x, chanPos[ch].y);
		if(!WM_IsWindow(hWinChan[ch]))
		{
			ledErrorIndicator();
		}
		// 创建通道任务
		taskArgs.chan = ch;
		taskArgs.mode = chanMode[ch];
		taskArgs.hWin = hWinChan[ch];
		if(xTaskCreate(hmiTask, "Chan Task", 1024, &taskArgs, 2, &hChannel[ch]) != pdPASS)
		{
			ledErrorIndicator();
		}
		// 等待任务参数保存完成
		panelTaskWaitArgsCopyDone();
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

// 回调函数按键处理
static void panelKeyProcess(WM_MESSAGE * pMsg)
{
	WM_MESSAGE msg;
	u32 tmp, key, ch;
	
	tmp = pMsg->Data.v;			// 带状态的键值
	key = HMI_KEY_VALUE(tmp);	// 不带状态的键值
	
	if(key < 18)	// 通道按键处理
	{
		// 判断按键属于哪个通道，并转换成统一按键
		if(key < 9)
		{
			ch = 0;
		}
		else
		{
			ch = 1;
			tmp -= 9;
			key -= 9;
		}

		// 按键处理
		if(hChannel[ch] == 0)
		{
			// 通道未开启状态，仅处理开关键和目录键
			switch(key)
			{
			case HMI_KEY_SW:
				if(tmp & HMI_KEY_PRESSED)
				{
					if(hPanelTmr[ch] == 0)
					{
						hPanelTmr[ch] = WM_CreateTimer(pMsg->hWin, ch, 1000, 0);
					}
				}
				else
				{
					if(hPanelTmr[ch])
					{
						WM_DeleteTimer(hPanelTmr[ch]);
						hPanelTmr[ch] = 0;
					}
				}
				break;
			case HMI_KEY_MENU:
				chanMode[ch] = (tmp & HMI_KEY_PRESSED) ? CHAN_MODE_MAINTAIN : CHAN_MODE_WORK;   // 检测并记录目录键状态
				break;
			default:
				break;
			}
		}
		else
		{			
			// 通道开启状态将按键消息投递到通道任务
			msg.MsgId = HMI_MSG_KEY;
			msg.Data.v = tmp;
			hmiStoreMsg(ch, &msg, 0);
		}
	}
	else	// 公共按键处理 todo
	{
	}
}

// 面板任务
void panelTask(void* pArgs)
{
	WM_MESSAGE msg;
	char buf[40];
	KEY  tmp;
	u8	 id;

	// 创建面板任务消息队列
	if((hPanelQueue = xQueueCreate(5, sizeof(WM_MESSAGE))) == 0)
	{
		ledErrorIndicator();
	}

	// 初始化 GUI
	GUI_Init();
    WM_SetCreateFlags(WM_CF_MEMDEV);
	WIDGET_SetDefaultEffect(&WIDGET_Effect_None);
	GUI_UC_SetEncodeUTF8();

	// 创建面板窗口
	hPanel = GUI_CreateDialogBox(_aPanelCreate, GUI_COUNTOF(_aPanelCreate), _cbPanel, WM_HBKWIN, 0, 0);
	if(!WM_IsWindow(hPanel))
	{
		ledErrorIndicator();
	}

	// 面板任务
	while(1)
	{
		// 获取屏幕按键及按键处理
		tmp = keyGetKey();

        // 显示按键状态
        if(tmp.key)
        {
            sprintf(buf, "Key Value: 0x%08X", tmp.key);
		    TEXT_SetText(WM_GetDialogItem(hPanel, ID_TEXT_KEY_VALUE), buf);
            sprintf(buf, "Key State: 0x%08X", tmp.pre);
		    TEXT_SetText(WM_GetDialogItem(hPanel, ID_TEXT_KEY_STA), buf);
        }

        // 若有键按下，蜂鸣器响一声。按键松开不响  todo
        if(tmp.key & tmp.pre)
        {
//			beepOnce();
        }

        // 投递按键消息
		while(tmp.key)
		{
			id = 31 - __CLZ(tmp.key);

			// 投递用户按键消息
			msg.MsgId = HMI_MSG_KEY;
			msg.Data.v = id+((tmp.pre&(1<<id))?HMI_KEY_PRESSED:HMI_KEY_RELEASED);
			WM_SendMessage(hPanel, &msg);

			// 更新循环键值
			tmp.key &= ~(1<<id);
		}

		// 刷新显示
		GUI_Delay(50);
	}
}

// 发送消息到界面窗口
void panelWinStoreMsg(WM_MESSAGE* pMsg)
{
	WM_SendMessage(hPanel, pMsg);
}

// 发送任务参数已保存消息到界面任务
void panelTaskStoreArgsCopyDone(void)
{
	WM_MESSAGE msg;

	if(hPanelQueue)
	{
		msg.MsgId = HMI_MSG_ARGS_COPY_DONE;
		xQueueSend(hPanelQueue, &msg, portMAX_DELAY);
	}
}

// 等待任务参数保存完成
static void panelTaskWaitArgsCopyDone(void)
{
	WM_MESSAGE msg;
	
	while(1)
	{
		xQueueReceive(hPanelQueue, &msg, portMAX_DELAY);
		if(msg.MsgId == HMI_MSG_ARGS_COPY_DONE)
		{
			break;
		}
	}
}

// 获取通道任务句柄
TaskHandle_t panelTaskGetChanHandle(u8 chan)
{
	if(chan < CHAN_NUM)
	{
		return hChannel[chan];
	}
	else
	{
		ledErrorIndicator();
		return 0;
	}
}
