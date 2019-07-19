#ifndef BETA_OS_H
#define BETA_OS_H

#include "beta_os_scheduler.h"

osBetaThread_id next_thread_id = 0;
bool threadCreated = false;
bool osInitialized = false;

//INITIALIZE
void osBetaInitialize(void)
{
	__disable_irq();
	printf("INITIALIZING OS BETA RTOS\n");
	
  // Main stsck base address stored at index 0 of vector table
  uint32_t *mainStackBaseAddress = 0x0;
	
  uint32_t stackStart = *mainStackBaseAddress - THREAD_STACK_SIZE*7;
    
  // Initialize 6 TCBs
  for(int i = 0; i < 6; i++){
      tcb[i].stackPointer = stackStart+THREAD_STACK_SIZE*i;
      tcb[i].baseStackPointer = stackStart+THREAD_STACK_SIZE*i;
			tcb[i].priority = Normal;
  }
    
  // Copy main stack into process stack of main thread
  uint32_t mainStackSize = *mainStackBaseAddress - __get_MSP();

	memcpy((void*)(tcb[0].stackPointer - mainStackSize), (void*)__get_MSP(), mainStackSize);
  
	// Reset MSP to base address of main stack
  __set_MSP((uint32_t)*mainStackBaseAddress);
    
    
  // Set SPSEL bit to switch from MSP to PSP
	CONTROL_Type control;
	control.w = __get_CONTROL();
	control.b.SPSEL = 1;
	__set_CONTROL(control.w);
	
	// Set PSP to top of main thread process stack
  __set_PSP((uint32_t)tcb[0].stackPointer - mainStackSize);
		
	runningTask = &tcb[next_thread_id];
	tcb[next_thread_id].state = osThreadRunning;
	next_thread_id++;
	
	addThreadToScheduler(runningTask);
	
	osInitialized = true;
	printf("OS BETA RTOS INITIALIZATION COMPLETE\n\n");
	__enable_irq();
}

//CREATE THREAD
osBetaThread_id osBetaCreateThread(osBetaThreadFunc_t function, void *args, osBetaPriority priority) {
	__disable_irq();
	
	osBetaThread_t *thread = &tcb[next_thread_id];
	
	printf("CREATING THREAD %d...\n", (uint32_t)thread->id);
	
	next_thread_id++;
	thread->state = osThreadReady;
	thread->priority = priority;
	
	for( uint8_t i = 0; i < 16; i++ )
	{
		thread->stackPointer -= 4;
		
		if( i == 0 )
			(*(uint32_t*)thread->stackPointer) = (uint32_t)DEFAULT_STACK_PSR;
		
		else if( i == 1 )
			(*(uint32_t*)thread->stackPointer) = (uint32_t)function;
		
		else if( i == 7 )
			(*(uint32_t*)thread->stackPointer) = (uint32_t)args;
		
		else
			(*(uint32_t*)thread->stackPointer) = (uint32_t)0;
	}
	
	addThreadToScheduler( thread );
	
	printf("THREAD %d CREATED!\n\n", (uint32_t)thread->id);
	threadCreated = true;
	__enable_irq();
	return thread->id;
}

//CONTEXT SWITCH
void PendSV_Handler(void)
{
	__disable_irq();
	
	//Do FPP Scheduling
	if(osInitialized)
	{
		nextTask = runScheduler();
		addThreadToScheduler(runningTask);
	}
	
	if( nextTask->id != runningTask->id && osInitialized && threadCreated )
	{
		printf("CONTEXT SWITCH REQUIRED...\n");
		runningTask->stackPointer = storeContext();
		printf("STORED CONTEXT FROM TASK %d!\n", runningTask->id);

		__set_PSP(nextTask->stackPointer);
		
		nextTask->state = osThreadRunning;
		runningTask->state = osThreadReady; 
		runningTask = nextTask;
		
		restoreContext(runningTask->stackPointer);
		printf("RESTORED CONTEXT FROM TASK %d!\n\n", runningTask->id);
	}
	
	__enable_irq();
}

#endif