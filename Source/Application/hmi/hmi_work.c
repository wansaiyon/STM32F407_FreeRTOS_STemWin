#include "hmi.h"

/*******************************************************************************
* ���洴�������б���
*******************************************************************************/
static const pWinCreate hmiWinCreateTbl[HMI_WORK_WIN_NUM] =
{
	hmiWorkNotCreate,
	hmiWorkWelcomeCreate,
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
