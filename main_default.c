/*
 * Default main.c for rtos lab.
 * @author Andrew Morton, 2018
 */
#include <LPC17xx.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "beta_os.h"
#include "test_suite.h"

void SysTick_Handler(void) {
  msTicks++;
	if((uint32_t)(msTicks % 2000) == 0)
		triggerPendSV();
}

int main(void) {
	SysTick_Config(SystemCoreClock/1000);
	printf("\nStarting...\n\n");
	
	uint32_t period = 1000; // 1s
	uint32_t prev = -period;
	
	osBetaInitialize();
	
	//select which test to run (runTest1() or runTest2()). Details on each test can be found in test_suite.h
	runTest2();
	
	osBetaStart();
	
	while(true) {
		if((uint32_t)(msTicks - prev) >= period) {
			printf("tick ");
			prev += period;
		}
	}
}
