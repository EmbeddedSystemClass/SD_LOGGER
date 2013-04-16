#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"

#include <misc.h>

#include <stdio.h>

//Инклуды от FreeRTOS:

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "dol.h"
#include "proto.h"
#include "frequency.h"

#include "rtc.h"
#include "diskio.h"

static void prvSetupHardware( void );
void ReadRtc( void *pvParameters );
void ReadSD( void *pvParameters );

RTC_t rtc;
BYTE buf[512];
extern DWORD Timer1, Timer2;	/* 100Hz decrement timers */

void prvSetupHardware()
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);

    //---------------------
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);



	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//светодиоды
	  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8|GPIO_Pin_9 ;

	  GPIO_Init(GPIOC, &GPIO_InitStructure);
}
//---------------------------------------------------------------
void ReadRtc( void *pvParameters )
{
	while(1)
	{
		vTaskDelay(500);
		rtc_gettime(&rtc);
	}
}
//-------------------------------------------------------------
void ReadSD( void *pvParameters )
{
	static uint8_t disk_stat=0, read_stat=0;

	disk_stat=disk_initialize (0);
	while(1)
	{
		vTaskDelay(100);
		read_stat=disk_read(0,&buf,0,1);
		vTaskDelay(100);
	}
}
//-------------------------------------------------------------
void vApplicationTickHook( void )//отбиваем тики
{
	static uint8_t count=0;
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	count++;

	if(count==10)
	{
		count=0;

		if(Timer1!=0)
		{
			Timer1--;
		}

		if(Timer2!=0)
		{
			Timer2--;
		}
	}

}
//-------------------------------------------------------------
int main(void)
{

	SystemInit();

	prvSetupHardware();

	rtc_init();



    DOL_Init();
    Frequency_Init();
    Proto_Init();
	 xTaskCreate(ReadRtc,(signed char*)"READ RTC",64,
	            NULL, tskIDLE_PRIORITY + 1, NULL);

	 xTaskCreate(ReadSD,(signed char*)"READ SD",64,
	            NULL, tskIDLE_PRIORITY + 1, NULL);
	 /* Start the scheduler. */
    vTaskStartScheduler();



    while(1);
}
