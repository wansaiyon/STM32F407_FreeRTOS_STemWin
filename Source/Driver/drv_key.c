#define _DRIVER_

#include "driver.h"

/*******************************************************************************
* 变量定义
*******************************************************************************/
static KEY touchKey = {0, 0};

/*
	任务例程
*/
void keyScanTask(void* pArgs)
{
	u32 preKey = 0; // 上次有效键值
    u32 preVal = 0; // 上次扫描值
	u32 key;

	while(1)
	{
		key = touchGt9147Scan();

		// 按键策略为: 检测到键值有更新时
		// 若值不为0，则直接更新；若值为0，则需要两次确认
		if(    ((key != 0) && (preVal != key))
            || ((key == 0) && (preVal == key) && (preKey != key)))
		{
			taskENTER_CRITICAL();
			touchKey.key = preKey^key;
			touchKey.pre = key;
			taskEXIT_CRITICAL();

            preKey = key;
		}

        preVal = key;

		vTaskDelay(20);
	}
}

/*******************************************************************************
* Function Name  : keyGetKey
*******************************************************************************/
KEY keyGetKey(void)
{
	KEY tmp;

	taskENTER_CRITICAL();
	tmp = touchKey;
	touchKey.key = 0;
	taskEXIT_CRITICAL();

	return tmp;
}
