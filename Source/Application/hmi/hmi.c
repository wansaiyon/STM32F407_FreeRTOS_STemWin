#include "hmi.h"

/*******************************************************************************
* HMI������Ϣ����
*******************************************************************************/
static QueueHandle_t hmiQueueTbl[CHAN_NUM] = {0};

/*******************************************************************************
* HMI������ϢͶ�ݺ���
*******************************************************************************/
BaseType_t hmiStoreMsg(u8 chan, WM_MESSAGE* pMsg, u32 delay)
{
    if((chan >= CHAN_NUM) || (pMsg == 0))
    {
        return pdFAIL;
    }
    if(hmiQueueTbl[chan] == 0)
    {
        return pdFAIL;
    }
    return xQueueSend(hmiQueueTbl[chan], pMsg, delay);
}

/*******************************************************************************
* HMI������Ϣ��ȡ����
*******************************************************************************/
BaseType_t hmiGetMsg(u8 chan, WM_MESSAGE* pMsg, u32 delay)
{
    if((chan >= CHAN_NUM) || (pMsg == 0))
    {
        return pdFAIL;
    }
    if(hmiQueueTbl[chan] == 0)
    {
        return pdFAIL;
    }
    return xQueueReceive(hmiQueueTbl[chan], pMsg, delay);
}

/*******************************************************************************
* HMI����
* �������Ϊ��ͨ�������ṹ�壬������ο��ṹ�嶨��
*******************************************************************************/
void hmiTask(void* pArgs)
{
	u8 loop_on = 1;	// ѭ��������Ϊ 1 ʱ����ѭ��
	HMI_CHAN_ARGS chanArgs = {0};
	WM_MESSAGE msg;

	// �����������
	// ָ������ں��������н����ı䣬����Ҫ���������б���
	chanArgs.chan = ((HMI_CHAN_ARGS*)pArgs)->chan;
	chanArgs.mode = ((HMI_CHAN_ARGS*)pArgs)->mode;
	chanArgs.hWin = ((HMI_CHAN_ARGS*)pArgs)->hWin;

	// ֪ͨ������������ѱ�����ϣ����Լ�������ִ��
	panelTaskStoreArgsCopyDone();

	// ����������������TCB,��������ĺ��������ж�ȡ�ò�����ʹ��
	vTaskSetThreadLocalStoragePointer(0, 0, &chanArgs);

	// ����������Ϣ����
	if((hmiQueueTbl[chanArgs.chan] = xQueueCreate(5, sizeof(WM_MESSAGE))) == 0)
	{
		ledErrorIndicator();
	}

	// ��ȡͨ�����������в������ todo

	// �������н���
	switch(chanArgs.mode)
	{
	case CHAN_MODE_WORK:
		hmiWorkCreate();
		break;
	case CHAN_MODE_MAINTAIN:
		hmiMaintainCreate();
		break;
	default:
		break;
	}

	// ����ѭ��
	while(loop_on)
	{
		if(hmiGetMsg(chanArgs.chan, &msg, 0) == pdPASS)
		{
			if((msg.MsgId == HMI_MSG_KEY) && (HMI_KEY_VALUE(msg.Data.v) == HMI_KEY_SW))	// �ػ���������
			{
				// �а�������������£�Ҫô�ػ���ʼ��Ҫô�ػ���ֹ�������Ӧ�ü���ִ��
				// ���Դ˴�����Է���ֵ���д���
				hmiSwitchOff(msg.Data.v);
			}
			else if(!WM_IsWindow(chanArgs.hWinOff))		// ����ǰ�����ڹػ������У���������Ϣ���͵���ǰ��������
			{
				if(WM_IsWindow(chanArgs.hWinAlarm))
				{
					WM_SendMessage(chanArgs.hWinAlarm, &msg);
				}
				if(WM_IsWindow(chanArgs.hWinClient))
				{
					WM_SendMessage(chanArgs.hWinClient, &msg);
				}
			}
		}
		else
		{
			// �ػ����洦��
			switch(hmiSwitchOff(HMI_SW_OFF_ONGOING))
			{
			case HMI_SW_OFF_RET_DONE:	// �ػ��ɹ����˳�ѭ��
				loop_on = 0;
				break;
			case HMI_SW_OFF_RET_ERROR:	// ����������Ϣ���������� todo
				break;
			default:
				break;
			}
			
			// ��δ��ȡ����Ϣ����Ӧ����ʱ�����������������ȼ�����
			vTaskDelay(100);
		}
	}

	// ɾ�����д����Ľ���
	switch(chanArgs.mode)
	{
	case CHAN_MODE_WORK:
		hmiWorkExit();
		break;
	case CHAN_MODE_MAINTAIN:
		hmiMaintainExit();
		break;
	default:
		break;
	}

	// ����ͨ������ todo

	// �ر�ͨ������ todo

	// ���͹ػ���Ϣ������
	msg.MsgId = HMI_MSG_SW_OFF;
	msg.Data.v = chanArgs.chan;
	panelWinStoreMsg(&msg);
	
	// ɾ��������Ϣ����
	vQueueDelete(hmiQueueTbl[chanArgs.chan]);

	// ɾ���������������󲻿�ִ������������
	// ������Ϊ������ɾ������������������ִ��
	vTaskDelete(0);
}
