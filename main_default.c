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
osBetaMutex_t mutex;

void SysTick_Handler(void) {
  msTicks++;
	if((uint32_t)(msTicks % 2000) == 0)
		triggerPendSV();
}

void threadFunc3(void *args)
{
	release(&mutex, runningTask->id);
	uint32_t *argument = args;
	uint32_t period = 500;
	uint32_t prev = -period;
	
	while (true)
	{
		if((uint32_t)(msTicks - prev) >= period)
		{
			printf("threadFunc3: %d\n", 3);
			prev += period;
		}
	}
}

void threadFunc1(void *args)
{
	acquire(&mutex, runningTask->id);
	uint32_t releasePeriod = 5000;
	uint32_t period = 500;
	uint32_t prev = -period;
	uint32_t releasePrev = 0;
	uint32_t *argument = args;
	bool mutexReleased = false;
	
	while (true)
	{
		if((uint32_t)(msTicks - prev) >= period)
		{
			printf("threadFunc1: %d\n", 1);
			prev += period;
		}
		if(!mutexReleased && (uint32_t)(msTicks - releasePrev) >= releasePeriod)
		{
			release(&mutex, runningTask->id);
			releasePrev += releasePeriod;
			mutexReleased = true;
		}
	}
}

void threadFunc2(void *args)
{
	acquire(&mutex, runningTask->id);
	uint32_t period = 500;
	uint32_t prev = -period;
	uint32_t *argument = args;
	while (true)
	{
		if((uint32_t)(msTicks - prev) >= period)
		{
			printf("threadFunc2: %d\n", 2);
			prev += period;
		}
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
	
	initMutex(&mutex);
	
	osBetaThread_id thread1_id = osBetaCreateThread(&threadFunc1, (void *)&a, Chad);
	osBetaThread_id thread2_id = osBetaCreateThread(&threadFunc2, (void *)&b, Chad);
	osBetaThread_id thread3_id = osBetaCreateThread(&threadFunc3, (void *)&c, Chad);
	
	osBetaStart();
	
	while(true) {
		if((uint32_t)(msTicks - prev) >= period) {
			printf("tick ");
			prev += period;
		}
	}
}
