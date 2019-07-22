/*
 * Default main.c for rtos lab.
 * @author Andrew Morton, 2018
 */
#include <LPC17xx.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "beta_os.h"

uint32_t msTicks = 0;
osBetaSemaphore_t sem = { .count = 0 };

void SysTick_Handler(void) {
  msTicks++;
	if((uint32_t)(msTicks % 2000) == 0)
		triggerPendSV();
}

void threadFunc3(void *args)
{
	uint32_t count = 0;
	uint32_t *argument = args;
	uint32_t period = 15000;
	uint32_t prev = 0;
	
	while (true)
	{
		if(count % 30000 == 0)
			printf("threadFunc3: %d\n", sem.count);
		
		if((uint32_t)(msTicks - prev) >= period)
		{
			wait(&sem);
			prev += period;
		}
		count++;
	}
}

void threadFunc1(void *args)
{
	wait(&sem);
	uint32_t period = 10000;
	uint32_t prev = 0;
	uint32_t count = 0;
	uint32_t *argument = args;
	bool threadCreated = false;
	
	while (true)
	{
		if(count % 30000 == 0)
			printf("threadFunc1: %d\n", sem.count);
		if((uint32_t)(msTicks - prev) >= period && !threadCreated)
		{
			osBetaThread_id thread3_id = osBetaCreateThread(&threadFunc3, NULL, Chad);
			threadCreated = true;
			prev += period;
		}
		
		count++;
	}
}

void threadFunc2(void *args)
{
	//wait(&sem);
	uint32_t period = 5000;
	uint32_t prev = 0;
	uint32_t count = 0;
	uint32_t *argument = args;
	while (true)
	{
		if(count % 30000 == 0)
			printf("threadFunc2: %d\n", sem.count);
		if((uint32_t)(msTicks - prev) >= period)
		{
			signal(&sem);
			prev += period;
		}
		count++;
	}
	
}

int main(void) {
	SysTick_Config(SystemCoreClock/1000);
	printf("\nStarting...\n\n");
	
	uint32_t period = 1000; // 1s
	uint32_t prev = -period;
	
	osBetaInitialize();

	uint32_t a = 1;
	uint32_t b = 2;
	uint32_t c = 3;
	
	osBetaThread_id thread1_id = osBetaCreateThread(&threadFunc1, (void *)&a, Alpha);
	osBetaThread_id thread2_id = osBetaCreateThread(&threadFunc2, (void *)&b, Normal);
	//osBetaThread_id thread3_id = osBetaCreateThread(&threadFunc3, (void *)&c, Alpha);
	
	while(true) {
		if((uint32_t)(msTicks - prev) >= period) {
			printf("tick ");
			prev += period;
		}
	}
}
