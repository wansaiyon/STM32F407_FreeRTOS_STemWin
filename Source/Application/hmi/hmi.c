#include "hmi.h"

/*******************************************************************************
* HMI任务消息队列
*******************************************************************************/
static QueueHandle_t hmiQueueTbl[CHAN_NUM] = {0};

/*******************************************************************************
* HMI任务消息投递函数
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
* HMI任务消息获取函数
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
* HMI任务
* 传入参数为各通道参数结构体，含义请参看结构体定义
*******************************************************************************/
void hmiTask(void* pArgs)
{
	u8 loop_on = 1;	// 循环变量，为 1 时继续循环
	HMI_CHAN_ARGS chanArgs = {0};
	WM_MESSAGE msg;

	// 保存任务参数
	// 指针参数在后续运行中将被改变，故需要将参数进行保存
	chanArgs.chan = ((HMI_CHAN_ARGS*)pArgs)->chan;
	chanArgs.mode = ((HMI_CHAN_ARGS*)pArgs)->mode;
	chanArgs.hWin = ((HMI_CHAN_ARGS*)pArgs)->hWin;

	// 通知界面任务参数已保存完毕，可以继续往后执行
	panelTaskStoreArgsCopyDone();

	// 将参数关联到任务TCB,将在任务的后续函数中读取该参数并使用
	vTaskSetThreadLocalStoragePointer(0, 0, &chanArgs);

	// 创建任务消息队列
	if((hmiQueueTbl[chanArgs.chan] = xQueueCreate(5, sizeof(WM_MESSAGE))) == 0)
	{
		ledErrorIndicator();
	}

	// 读取通道参数并进行参数检查 todo

	// 创建所有界面
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

	// 任务循环
	while(loop_on)
	{
		if(hmiGetMsg(chanArgs.chan, &msg, 0) == pdPASS)
		{
			if((msg.MsgId == HMI_MSG_KEY) && (HMI_KEY_VALUE(msg.Data.v) == HMI_KEY_SW))	// 关机按键处理
			{
				// 有按键动作的情况下，要么关机开始，要么关机终止，程序均应该继续执行
				// 所以此处不需对返回值进行处理
				hmiSwitchOff(msg.Data.v);
			}
			else if(!WM_IsWindow(chanArgs.hWinOff))		// 若当前不处于关机过程中，则其他消息发送到当前工作界面
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
			// 关机界面处理
			switch(hmiSwitchOff(HMI_SW_OFF_ONGOING))
			{
			case HMI_SW_OFF_RET_DONE:	// 关机成功，退出循环
				loop_on = 0;
				break;
			case HMI_SW_OFF_RET_ERROR:	// 出错，发送消息给报警界面 todo
				break;
			default:
				break;
			}
			
			// 若未获取到消息，则应当延时，否则将阻塞更低优先级任务
			vTaskDelay(100);
		}
	}

	// 删除所有创建的界面
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

	// 保存通道参数 todo

	// 关闭通道外设 todo

	// 发送关机消息到界面
	msg.MsgId = HMI_MSG_SW_OFF;
	msg.Data.v = chanArgs.chan;
	panelWinStoreMsg(&msg);
	
	// 删除任务消息队列
	vQueueDelete(hmiQueueTbl[chanArgs.chan]);

	// 删除任务自身，该语句后不可执行其他操作，
	// 否则因为任务已删除，其他动作将不被执行
	vTaskDelete(0);
}
