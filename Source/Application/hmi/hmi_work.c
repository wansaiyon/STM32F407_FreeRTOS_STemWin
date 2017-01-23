#include "hmi.h"

/*******************************************************************************
* ���洴�������б���
*******************************************************************************/
static const pWinCreate hmiWinCreateTbl[HMI_WORK_WIN_NUM] =
{
	hmiWorkNotCreate,
	hmiWorkWelcomeCreate,
	hmiWorkMenuCreate,
	hmiWorkStatusCreate,
};

/*******************************************************************************
* ��������
*******************************************************************************/
// �ñ�����ʾ�Ƿ���һ��ͨ�������û�����ģʽ
// ������ƣ�ͬһʱ�����ֻ����һ��ͨ������ά��ģʽ
// ���Ե���ͨ�������û�����ģʽʱ������ͨ�����������ά��ģʽ
static u32 hmiAnyChanInWork = 0;
#define hmiChanInWorkInc()  {hmiAnyChanInWork++;}
#define hmiChanInWorkDec()  {if(hmiAnyChanInWork)hmiAnyChanInWork--;}

/*******************************************************************************
* hmiIsAnyChanInWork
*******************************************************************************/
u8 hmiIsAnyChanInWork(void)
{
	return hmiAnyChanInWork? 1 : 0;
}

/*******************************************************************************
* �û�����ģʽ����
*******************************************************************************/
void hmiWorkCreate(void)
{
	u32 i;
	HMI_CHAN_ARGS* pArgs;

	// ��ȡ����TCB�����Ĳ���
	pArgs = (HMI_CHAN_ARGS*)pvTaskGetThreadLocalStoragePointer(0, 0);

	// ����ģʽͨ������һ
	hmiChanInWorkInc();

	// ��������
    for(i=0; i<HMI_WORK_WIN_NUM; i++)
    {
		if(!WM_IsWindow(pArgs->hWinList[i] = hmiWinCreateTbl[i](pArgs->hWin)))
		{
			ledErrorIndicator();
		}
		WM_HideWindow(pArgs->hWinList[i]);
	}

	// ��ʾ��һ������
	if(hmiIsAnyChanInMaintain())
	{
		WM_ShowWindow(pArgs->hWinList[HMI_WORK_WIN_CAN_NOT_INTO_WORK]);
	}
	else
	{
		WM_CreateTimer(pArgs->hWinList[HMI_WORK_WIN_WELCOME], pArgs->chan, 2000, 0);	// ������ʱ������ʱ��ID��Ϊͨ����
		pArgs->offFlg = HMI_SW_OFF_FLG_NACK;											// ���ο��ؼ�
		WM_ShowWindow(pArgs->hWinList[HMI_WORK_WIN_WELCOME]);							// ��ʾ����
	}
}

/*******************************************************************************
* �û�����ģʽ�˳�
*******************************************************************************/
void hmiWorkExit(void)
{
	u32 i;
	HMI_CHAN_ARGS* pArgs;

	// ��ȡ����TCB�����Ĳ���
	pArgs = (HMI_CHAN_ARGS*)pvTaskGetThreadLocalStoragePointer(0, 0);

	// ɾ������
    for(i=0; i<HMI_WORK_WIN_NUM; i++)
    {
		if(!WM_IsWindow(pArgs->hWinList[i]))
		{
			ledErrorIndicator();
		}
		WM_DeleteWindow(pArgs->hWinList[i]);
	}

	// ����ģʽͨ������һ
	hmiChanInWorkDec();
}
