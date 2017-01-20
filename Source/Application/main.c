#define _MAIN_

#include "includes.h"

/* �������������� */
extern TaskHandle_t beepTaskHandle;
void beepTask(void* pArgs);
void keyScanTask(void* pArgs);

/* �������������� */
void handshakeTask(void* pArgs);
void panelTask(void* pArgs);
void selftestTask(void* pArgs);

/* ������������ */
void CreateTasks(void)
{
//				����			����		��ջ��С	����	���ȼ�	���ָ��
	xTaskCreate(panelTask,		"Panel",		512,	0,		1,			0               );
    xTaskCreate(selftestTask,	"Selftest", 	64,		0,		7,			0               );
	xTaskCreate(keyScanTask,	"Key Scan",		64,		0,		8,			0               );
    xTaskCreate(beepTask,       "Beep",         64,     0,      9,          &beepTaskHandle );
	xTaskCreate(handshakeTask,	"Handshake",	64,		0,		10,			0               );
}

/* ������ */
int main(void)
{
	/* Driver ��ʼ�� */
	drvInit();

	/* �����ʼ�� */
	CreateTasks();

	/* Start the FreeRTOS scheduler */
	vTaskStartScheduler();
}
