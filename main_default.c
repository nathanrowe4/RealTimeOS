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

void threadFunc1(void *args)
{
	acquire(&mutex, runningTask->id);

	uint32_t period = 500;
	uint32_t prev = -period;
	uint32_t *argument = args;
	uint32_t releasePeriod = 20000;
	uint32_t releasePrev = 0;
	
	while (true)
	{
		if((uint32_t)(msTicks - prev) >= period)
		{
			printf("threadFunc3: %d\n", 3);
			prev += period;
		}
		if((uint32_t)(msTicks - releasePrev) >= releasePeriod)
		{
			release(&mutex, runningTask->id);
			releasePrev += releasePeriod;
		}
	}
}

void threadFunc2(void *args)
{
	uint32_t creationPeriod = 10000;
	uint32_t creationPrev = 0;
	uint32_t *argument = args;
	uint32_t period = 500;
	uint32_t prev = -period;
	bool created = false;
	
	while (true)
	{
		if((uint32_t)(msTicks - prev) >= period)
		{
			printf("threadFunc2: %d\n", 2);
			prev += period;
		}
		
		if(!created && (uint32_t)(msTicks - creationPrev) >= creationPeriod)
		{
			osBetaThread_id thread1_id = osBetaCreateThread(&threadFunc1, NULL, Chad);
			creationPrev += creationPeriod;
			created = true;
		}
	}
}

void threadFunc3(void *args)
{
	acquire(&mutex, runningTask->id);
	uint32_t *argument = args;
	uint32_t period = 500;
	uint32_t prev = -period;
	uint32_t creationPeriod = 5000;
	uint32_t creationPrev = 0;
	uint32_t relPeriod = 15000;
	uint32_t relPrev = 0;
	bool created = false;
	bool released = false;
	
	while (true)
	{
		if((uint32_t)(msTicks - prev) >= period)
		{
			printf("threadFunc1: %d\n", 1);
			prev += period;
		}
		
		if(!created && (uint32_t)(msTicks - creationPrev) >= creationPeriod)
		{
			osBetaThread_id thread2_id = osBetaCreateThread(&threadFunc2, NULL, Alpha);
			created = true;
			creationPrev += creationPeriod;
		}
		if(!released && (uint32_t)(msTicks - relPrev) >= relPeriod)
		{
			release(&mutex, runningTask->id);
			released = true;
			relPrev += relPeriod;
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
	
	osBetaThread_id thread3_id = osBetaCreateThread(&threadFunc3, (void *)&c, Normal);
	
	osBetaStart();
	
	while(true) {
		if((uint32_t)(msTicks - prev) >= period) {
			printf("tick ");
			prev += period;
		}
	}
}
