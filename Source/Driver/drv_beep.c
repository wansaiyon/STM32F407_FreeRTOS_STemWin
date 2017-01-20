#define _DRIVER_

#include "driver.h"

TaskHandle_t beepTaskHandle = 0;
static TimerHandle_t beepTmrHandle = 0;

static void beepTimerCallback(TimerHandle_t hTmr)
{
    if(beepTaskHandle)
    {
        xTaskNotify(beepTaskHandle, 0, eSetValueWithOverwrite);
    }
}

void beepTask(void* pArgs)
{
    // ��ʼ�������ʱ��
    beepTmrHandle = xTimerCreate("Timer Beep", 150/portTICK_PERIOD_MS, pdTRUE, 0, beepTimerCallback);

    // ����ѭ��
    while(1)
    {
        xTaskNotifyWait(0x00000000, 0xFFFFFFFF, 0, portMAX_DELAY);
        beepOnOff(BEEP_ON);
        vTaskDelay(50);
        beepOnOff(BEEP_OFF);
    }
}

// ��������һ��
void beepOnce(void)
{
	beepTimerCallback(0);
}

// ������ѭ����
void beepCycle(u8 onoff)
{
    static u32 cnt = 0;

    if(beepTmrHandle)
    {
        if(onoff)
        {
            if(cnt == 0)
            {
                xTimerStart(beepTmrHandle, 0);  // ����ǰ��ʱ���ѿ��������ظ�����
            }
            cnt++;
        }
        else
        {
            if(cnt != 0)
            {
                cnt--;
                if(cnt == 0)
                {
                    xTimerStop(beepTmrHandle, 0);   // ��ʱ����ο�����Ҫ��ȫ�����ùرպ�����Źرն�ʱ��
                }
            }
        }
    }
}
