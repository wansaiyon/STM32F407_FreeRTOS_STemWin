/*******************************************************************************
* ���ļ������ṩ�������ʹ�����
* �ڿ������������ڴ���Ļ�Ϲ������ⰴ����2��ͨ����ʾ��
* �ڻ��������ڽ�������Ļ�ֱ��Ӧ��2��ͨ����ʾ��
*******************************************************************************/

#include "hmi.h"

// ȫ�ֱ���
static WM_HWIN			hPanel = 0;						// ��崰�ھ��
static WM_HTIMER		hPanelTmr[CHAN_NUM] = {0};		// ��ʱ�����
static TaskHandle_t		hChannel[CHAN_NUM] = {0};		// ͨ��������
static WM_HWIN			hWinChan[CHAN_NUM] = {0};		// ͨ�����ھ��
static u8				chanMode[CHAN_NUM] = {0};		// ͨ������ģʽ
static QueueHandle_t	hPanelQueue = 0;				// ���������Ϣ����

// �ֲ���������
static void panelKeyProcess(WM_MESSAGE * pMsg);
static void panelTaskWaitArgsCopyDone(void);

// ͨ���������궨��
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
* ͨ�����ڶ���
*******************************************************************************/
// ���ڶ����
static const GUI_WIDGET_CREATE_INFO _aChannelCreate[] =
{
	{ WINDOW_CreateIndirect,	"Channel",		0,		  		0,   0, 320, 240, 0, 0x0, 0 },
};

/*******************************************************************************
* ��嶨��
*******************************************************************************/
// ���ؼ�ID
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

// ��嶨���
static const GUI_WIDGET_CREATE_INFO _aPanelCreate[] =
{
	{ WINDOW_CreateIndirect,	"Panel",		ID_WINDOW_PANEL,		  0,   0, 800, 480, 0, 0x0, 0 },
	{ TEXT_CreateIndirect,		"Key Value:",	ID_TEXT_KEY_VALUE,		  0,   0, 800,  32, 0, 0x0, 0 },
    { TEXT_CreateIndirect,		"Key State:",	ID_TEXT_KEY_STA,		  0,  32, 800,  32, 0, 0x0, 0 },
	{ IMAGE_CreateIndirect,		"",				ID_IMAGE_LOGO,			  5,  80, 240,  64, 0, 0x0, 0 },
	// ͨ��һ
	{ BUTTON_CreateIndirect,	"ON/OFF",		ID_BUTTON_CH1_SW,		  5, 190,  40,  40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,	"Menu",			ID_BUTTON_CH1_MENU,		  5, 290,  40,  40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,	"Back",			ID_BUTTON_CH1_BACK,		  5, 340,  40,  40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,	">/ |",			ID_BUTTON_CH1_SP,		  5, 390,  40,  40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,	"^^",			ID_BUTTON_CH1_FUP,		 70, 435,  40,  40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,	"^",			ID_BUTTON_CH1_UP,		130, 435,  40,  40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,	"v",			ID_BUTTON_CH1_DN,		190, 435,  40,  40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,	"vv",			ID_BUTTON_CH1_FDN,		250, 435,  40,  40, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect,	"Enter",		ID_BUTTON_CH1_ENTER,	310, 435,  40,  40, 0, 0x0, 0 },
	// ͨ����
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

// ���ص�����
static void _cbPanel(WM_MESSAGE * pMsg)
{
	HMI_CHAN_ARGS taskArgs;
	WM_HWIN hItem;
	u8 ch;

	switch (pMsg->MsgId)
	{
	// �Զ�����Ϣ����
	case HMI_MSG_KEY:
		panelKeyProcess(pMsg);
		break;
	case HMI_MSG_SW_OFF:
		ch = pMsg->Data.v;
		WM_DeleteWindow(hWinChan[ch]);	// ɾ��ͨ������
		hWinChan[ch] = 0;
		hChannel[ch] = 0;
		break;
	// GUI��Ϣ����
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
		ch = WM_GetTimerId(hItem);	// ��ȡ��ʱ��ID,��ID��ͨ����
		if(hPanelTmr[ch] != hItem)	// ����ʱ������뱣��ľ����һ�£��򱨴�
		{
			ledErrorIndicator();
		}
		WM_DeleteTimer(hItem);		// ɾ����ʱ��
		hPanelTmr[ch] = 0;			// �����ʱ�����
		
		// ����ͨ������
		hWinChan[ch] = GUI_CreateDialogBox(_aChannelCreate, GUI_COUNTOF(_aChannelCreate), 0, hPanel, chanPos[ch].x, chanPos[ch].y);
		if(!WM_IsWindow(hWinChan[ch]))
		{
			ledErrorIndicator();
		}
		// ����ͨ������
		taskArgs.chan = ch;
		taskArgs.mode = chanMode[ch];
		taskArgs.hWin = hWinChan[ch];
		if(xTaskCreate(hmiTask, "Chan Task", 1024, &taskArgs, 2, &hChannel[ch]) != pdPASS)
		{
			ledErrorIndicator();
		}
		// �ȴ���������������
		panelTaskWaitArgsCopyDone();
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

// �ص�������������
static void panelKeyProcess(WM_MESSAGE * pMsg)
{
	WM_MESSAGE msg;
	u32 tmp, key, ch;
	
	tmp = pMsg->Data.v;			// ��״̬�ļ�ֵ
	key = HMI_KEY_VALUE(tmp);	// ����״̬�ļ�ֵ
	
	if(key < 18)	// ͨ����������
	{
		// �жϰ��������ĸ�ͨ������ת����ͳһ����
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

		// ��������
		if(hChannel[ch] == 0)
		{
			// ͨ��δ����״̬���������ؼ���Ŀ¼��
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
				chanMode[ch] = (tmp & HMI_KEY_PRESSED) ? CHAN_MODE_MAINTAIN : CHAN_MODE_WORK;   // ��Ⲣ��¼Ŀ¼��״̬
				break;
			default:
				break;
			}
		}
		else
		{			
			// ͨ������״̬��������ϢͶ�ݵ�ͨ������
			msg.MsgId = HMI_MSG_KEY;
			msg.Data.v = tmp;
			hmiStoreMsg(ch, &msg, 0);
		}
	}
	else	// ������������ todo
	{
	}
}

// �������
void panelTask(void* pArgs)
{
	WM_MESSAGE msg;
	char buf[40];
	KEY  tmp;
	u8	 id;

	// �������������Ϣ����
	if((hPanelQueue = xQueueCreate(5, sizeof(WM_MESSAGE))) == 0)
	{
		ledErrorIndicator();
	}

	// ��ʼ�� GUI
	GUI_Init();
    WM_SetCreateFlags(WM_CF_MEMDEV);
	WIDGET_SetDefaultEffect(&WIDGET_Effect_None);
	GUI_UC_SetEncodeUTF8();

	// ������崰��
	hPanel = GUI_CreateDialogBox(_aPanelCreate, GUI_COUNTOF(_aPanelCreate), _cbPanel, WM_HBKWIN, 0, 0);
	if(!WM_IsWindow(hPanel))
	{
		ledErrorIndicator();
	}

	// �������
	while(1)
	{
		// ��ȡ��Ļ��������������
		tmp = keyGetKey();

        // ��ʾ����״̬
        if(tmp.key)
        {
            sprintf(buf, "Key Value: 0x%08X", tmp.key);
		    TEXT_SetText(WM_GetDialogItem(hPanel, ID_TEXT_KEY_VALUE), buf);
            sprintf(buf, "Key State: 0x%08X", tmp.pre);
		    TEXT_SetText(WM_GetDialogItem(hPanel, ID_TEXT_KEY_STA), buf);
        }

        // ���м����£���������һ���������ɿ�����  todo
        if(tmp.key & tmp.pre)
        {
//			beepOnce();
        }

        // Ͷ�ݰ�����Ϣ
		while(tmp.key)
		{
			id = 31 - __CLZ(tmp.key);

			// Ͷ���û�������Ϣ
			msg.MsgId = HMI_MSG_KEY;
			msg.Data.v = id+((tmp.pre&(1<<id))?HMI_KEY_PRESSED:HMI_KEY_RELEASED);
			WM_SendMessage(hPanel, &msg);

			// ����ѭ����ֵ
			tmp.key &= ~(1<<id);
		}

		// ˢ����ʾ
		GUI_Delay(50);
	}
}

// ������Ϣ�����洰��
void panelWinStoreMsg(WM_MESSAGE* pMsg)
{
	WM_SendMessage(hPanel, pMsg);
}

// ������������ѱ�����Ϣ����������
void panelTaskStoreArgsCopyDone(void)
{
	WM_MESSAGE msg;

	if(hPanelQueue)
	{
		msg.MsgId = HMI_MSG_ARGS_COPY_DONE;
		xQueueSend(hPanelQueue, &msg, portMAX_DELAY);
	}
}

// �ȴ���������������
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

// ��ȡͨ��������
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
