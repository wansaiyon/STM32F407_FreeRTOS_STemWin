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
    // 初始化软件定时器
    beepTmrHandle = xTimerCreate("Timer Beep", 150/portTICK_PERIOD_MS, pdTRUE, 0, beepTimerCallback);

    // 任务循环
    while(1)
    {
        xTaskNotifyWait(0x00000000, 0xFFFFFFFF, 0, portMAX_DELAY);
        beepOnOff(BEEP_ON);
        vTaskDelay(50);
        beepOnOff(BEEP_OFF);
    }
}

// 蜂鸣器响一声
void beepOnce(void)
{
	beepTimerCallback(0);
}

// 蜂鸣器循环响
void beepCycle(u8 onoff)
{
    static u32 cnt = 0;

    if(beepTmrHandle)
    {
        if(onoff)
        {
            if(cnt == 0)
            {
                xTimerStart(beepTmrHandle, 0);  // 若当前定时器已开启，则不重复开启
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
                    xTimerStop(beepTmrHandle, 0);   // 定时器多次开启后，要在全部调用关闭函数后才关闭定时器
                }
            }
        }
    }
}
