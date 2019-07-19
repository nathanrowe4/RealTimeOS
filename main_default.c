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

void SysTick_Handler(void) {
    msTicks++;
		SCB->ICSR = SCB->ICSR | (1<<28);
		
}

void threadFunc1(void *args)
{
	uint32_t count = 0;
	uint32_t *argument = args;
	while (true)
	{
		if(count % 30000 == 0)
			printf("threadFunc1: %d\n", *argument);
		
		count++;
	}
}

void threadFunc2(void *args)
{
	uint32_t count = 0;
	uint32_t *argument = args;
	while (true)
	{
		if(count % 30000 == 0)
			printf("threadFunc2: %d\n", *argument);
		
		count++;
	}
}

void godFunction(void)
{
	return;
}

int main(void) {
	SysTick_Config(SystemCoreClock/1000);
	printf("\nStarting...\n\n");
	
	uint32_t period = 1000; // 1s
	uint32_t prev = -period;
	osBetaInitialize();

	uint32_t a = 8;
	uint32_t b = 10;
	osBetaThread_id thread1_id = osBetaCreateThread(&threadFunc1, (void *)&a, Alpha);
	osBetaThread_id thread2_id = osBetaCreateThread(&threadFunc2, (void *)&b, Alpha);
	
	while(true) {
		if((uint32_t)(msTicks - prev) >= period) {
			printf("tick ");
			prev += period;
		}
	}
}
