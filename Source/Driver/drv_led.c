#define _DRIVER_

#include "driver.h"

// ����ָʾ��
void ledRunningIndicator(void)
{
	while(1)
	{
		ledRunning(LED_ON);
		vTaskDelay(50);
		ledRunning(LED_OFF);
		vTaskDelay(950);
	}
}

// ����ָʾ��
void ledErrorIndicator(void)
{
	while(1)
	{
		ledError(LED_ON);
		vTaskDelay(200);
		ledError(LED_OFF);
		vTaskDelay(200);
	}
}
