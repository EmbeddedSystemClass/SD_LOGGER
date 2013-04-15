#ifndef DOL_H
#define DOL_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include <misc.h>

//������� �� FreeRTOS:

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

void DOL_Init(void);//������������� ������� ����
void DOL_Process( void *pvParameters );//������� ���������� �������� ����-��������������


#endif
