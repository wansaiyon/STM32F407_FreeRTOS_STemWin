#define _MAIN_

#include "includes.h"

/* 驱动任务函数声明 */
extern TaskHandle_t beepTaskHandle;
void beepTask(void* pArgs);
void keyScanTask(void* pArgs);

/* 功能任务函数声明 */
void handshakeTask(void* pArgs);
void panelTask(void* pArgs);
void selftestTask(void* pArgs);

/* 创建所有任务 */
void CreateTasks(void)
{
//				任务			名称		堆栈大小	参数	优先级	句柄指针
	xTaskCreate(panelTask,		"Panel",		512,	0,		1,			0               );
    xTaskCreate(selftestTask,	"Selftest", 	64,		0,		7,			0               );
	xTaskCreate(keyScanTask,	"Key Scan",		64,		0,		8,			0               );
    xTaskCreate(beepTask,       "Beep",         64,     0,      9,          &beepTaskHandle );
	xTaskCreate(handshakeTask,	"Handshake",	64,		0,		10,			0               );
}

/* 主函数 */
int main(void)
{
	/* Driver 初始化 */
	drvInit();

	/* 任务初始化 */
	CreateTasks();

	/* Start the FreeRTOS scheduler */
	vTaskStartScheduler();
}
