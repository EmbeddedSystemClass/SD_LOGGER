#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "hd44780_emcu.h"
#include <misc.h>

#include <stdio.h>

//Инклуды от FreeRTOS:

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "menu.h"
#include "dol.h"
#include "proto.h"
#include "frequency.h"

uint8_t led=0;
static void prvSetupHardware( void );

/*xQueueHandle xKeyQueue;//очередь клавиатуры
xSemaphoreHandle xKeySemaphore;
extern uint8_t FlashError;*/

extern uint32_t counter;

/*static void DisplayRenew( void *pvParameters );
static void Keyboard(void *pvParameters);*/





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


/*void DisplayRenew( void *pvParameters )
{

	unsigned char str[20]="";

    while(1)
    {
    	sprintf(&str,"%u",counter+TIM3->CNT);
    	if(FlashError)
    	{
    		sprintf(&str,"%s","FL_ERR");
    	}
    	LCD_CMD (1);
    	LCD_GOTO (1,8);
    	LCD_STRING (&str);

    	//USART_SendData (USART1, 'D');
        //while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
        {
        }



    	vTaskDelay(500);
    }
}

static void Keyboard(void *pvParameters)
{
	unsigned int key, last_key;
#define key_mask 0x1C01
    while(1)
    {

    	key=GPIO_ReadInputData(GPIOA)&key_mask;
    	if((key&key_mask)!=(last_key&key_mask))
    	{
    		if((key&key_mask)!=key_mask)
    		{
				xSemaphoreGive(xKeySemaphore);
				 if( xKeyQueue != 0 )
				 {
					 xQueueSend( xKeyQueue,  &key, ( portTickType ) 0 );
				 }
    			last_key=key;
    		}
    	}

	vTaskDelay(10);
    }
}*/


int main(void)
{
	SystemInit();

	prvSetupHardware();
    DOL_Init();
    Frequency_Init();
    Proto_Init();

/*    xTaskCreate(DisplayRenew,(signed char*)"LED2",256,
            NULL, tskIDLE_PRIORITY + 1, NULL);

    xTaskCreate(MenuHandler,(signed char*)"Menu",64,
              NULL, tskIDLE_PRIORITY + 1, NULL);

    xKeyQueue = xQueueCreate(3, sizeof(unsigned int));*/

/*    if(xKeyQueue!=NULL)
    {
    	//xTaskCreate(Keyboard,(signed char*)"Key",configMINIMAL_STACK_SIZE,NULL, tskIDLE_PRIORITY + 1, NULL);
    }

    vSemaphoreCreateBinary( xKeySemaphore );*/
    /* Start the scheduler. */
    vTaskStartScheduler();

    while(1);
}
