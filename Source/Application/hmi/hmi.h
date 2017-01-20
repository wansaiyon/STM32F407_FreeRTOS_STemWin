#ifndef _HMI_H_
#define _HMI_H_

#include "includes.h"
#include "language.h"
#include "resource.h"

/*******************************************************************************
* ���洴���������Ͷ���
*******************************************************************************/
typedef WM_HWIN(*pWinCreate)(WM_HWIN);

/*******************************************************************************
* ͨ���Ŷ���
*******************************************************************************/
enum
{
	CHAN_1,
	CHAN_2,
	CHAN_NUM,
};

/*******************************************************************************
* ͨ����ģʽ����
*******************************************************************************/
enum
{
	CHAN_MODE_WORK,
	CHAN_MODE_MAINTAIN,
};

/*******************************************************************************
* �Զ�����Ϣ
*******************************************************************************/
enum
{
	HMI_MSG_ARGS_COPY_DONE = WM_USER,
	HMI_MSG_KEY,
    HMI_MSG_SW_OFF,
    HMI_MSG_ALARM,
};

/*******************************************************************************
* ��������
*******************************************************************************/
// ����״̬����
#define HMI_KEY_PRESSED		0x80
#define HMI_KEY_RELEASED	0x00

// ��ȡ����ֵ
#define HMI_KEY_VALUE(k)    ((k)&0x7F)

// ����ֵ����
enum
{
	HMI_KEY_SW,
	HMI_KEY_MENU,
	HMI_KEY_BACK,
	HMI_KEY_S_P,
	HMI_KEY_FUP,
	HMI_KEY_UP,
	HMI_KEY_DN,
	HMI_KEY_FDN,
	HMI_KEY_ENTER,
};

/*******************************************************************************
* ��������
*******************************************************************************/
enum
{
    HMI_ALM_SW_OFF = 0x00,
};

/*******************************************************************************
* Panel
*******************************************************************************/
void panelWinStoreMsg(WM_MESSAGE* msg);
void panelTaskStoreArgsCopyDone(void);
TaskHandle_t panelTaskGetChanHandle(u8 chan);

/*******************************************************************************
* HMI
*******************************************************************************/
BaseType_t hmiStoreMsg(u8 chan, WM_MESSAGE* pMsg, u32 delay);
BaseType_t hmiGetMsg(u8 chan, WM_MESSAGE* pMsg, u32 delay);
void hmiTask(void* pArgs);

/*******************************************************************************
* HMI Switch
*******************************************************************************/
enum
{
	// �ػ�ʹ����
	HMI_SW_OFF_FLG_EN = 0,		// ����ػ�
	HMI_SW_OFF_FLG_DIS_WIN,		// ��ֹ�ػ�����ʾ��ʾ����
	HMI_SW_OFF_FLG_NACK,		// ��ֹ�ػ��Ҳ���Ӧ���ؼ�
	
	// �ػ���������
	HMI_SW_OFF_START = HMI_KEY_SW|HMI_KEY_PRESSED,	// ��ʼ�ػ�
	HMI_SW_OFF_ONGOING = 0X55,						// ���ڹػ�
	HMI_SW_OFF_STOP = HMI_KEY_SW|HMI_KEY_RELEASED,	// ֹͣ�ػ�

	// �ػ���������ֵ
	HMI_SW_OFF_RET_ONGOING = 0,		// ���ڹػ�
	HMI_SW_OFF_RET_ABORT,			// �ػ���ֹ
	HMI_SW_OFF_RET_DONE,			// �ػ��ɹ�
	HMI_SW_OFF_RET_ERROR,			// �ػ�����
};
u8 hmiSwitchOff(u8 opt);

/*******************************************************************************
* HMI Work
*******************************************************************************/
enum	// �������涨��
{
	HMI_WORK_WIN_CAN_NOT_INTO_WORK = 0,
	HMI_WORK_WIN_WELCOME,
//	HMI_WORK_WIN_WORK,
	HMI_WORK_WIN_NUM,
};
WM_HWIN hmiWorkNotCreate(WM_HWIN hParent);
WM_HWIN hmiWorkWelcomeCreate(WM_HWIN hParent);
u8   hmiIsAnyChanInWork(void);
void hmiWorkCreate(void);
void hmiWorkExit(void);

/*******************************************************************************
* HMI Maintain
*******************************************************************************/
enum	// ά�����涨��
{
	HMI_MT_WIN_CAN_NOT_INTO_MAINTAIN = 0,
	HMI_MT_WIN_MAINTAIN,
	HMI_MT_WIN_NUM,
};
u8   hmiIsAnyChanInMaintain(void);
void hmiMaintainCreate(void);
void hmiMaintainExit(void);

/*******************************************************************************
* ������������ṹ
*******************************************************************************/
typedef struct
{
	u8			chan;		// �����Ӧ��ͨ���š�0�� 1
	u8			mode;		// ����ģʽ��0-����ģʽ��1-ά��ģʽ
	u8			offFlg;		// ͨ���ػ���־��0-����ػ���1-��ֹ�ػ����������棬2-����Ӧ�ػ���ť
	WM_HWIN		hWin;		// ͨ����Ӧ�Ĵ��ھ��
	WM_HWIN		hWinOff;	// ͨ���ػ����ھ��
	WM_HWIN		hWinStat;	// ͨ��״̬�����ھ��
	WM_HWIN		hWinClient;	// ͨ�������ھ��
	WM_HWIN		hWinAlarm;	// ͨ���������ھ��
#if HMI_WORK_WIN_NUM > HMI_MT_WIN_NUM
	WM_HWIN		hWinList[HMI_WORK_WIN_NUM];
#else
	WM_HWIN		hWinList[HMI_MT_WIN_NUM];
#endif
	TickType_t	tick;		// ��������Ҫ��ʱ�����ֲ�������ʱ���ñ�����¼��ʱ����ϵͳʱ��
}HMI_CHAN_ARGS;

#endif
