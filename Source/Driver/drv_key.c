#define _DRIVER_

#include "driver.h"

/*******************************************************************************
* ��������
*******************************************************************************/
static KEY touchKey = {0, 0};

/*
	��������
*/
void keyScanTask(void* pArgs)
{
	u32 preKey = 0; // �ϴ���Ч��ֵ
    u32 preVal = 0; // �ϴ�ɨ��ֵ
	u32 key;

	while(1)
	{
		key = touchGt9147Scan();

		// ��������Ϊ: ��⵽��ֵ�и���ʱ
		// ��ֵ��Ϊ0����ֱ�Ӹ��£���ֵΪ0������Ҫ����ȷ��
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
