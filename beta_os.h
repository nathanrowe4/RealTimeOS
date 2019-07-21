#ifndef BETA_OS_H
#define BETA_OS_H

#include "beta_os_scheduler.h"

osBetaThread_id next_thread_id = 0;
bool threadCreated = false;
bool osInitialized = false;

// returns id and increments global variable
osBetaThread_id getNextId(void) {
    osBetaThread_id idToReturn = next_thread_id;
    next_thread_id++;
    return idToReturn;
}

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
	
    //set running task to main thread
	runningTask = &tcb[getNextId()];
	runningTask->state = osThreadRunning;
	
	osInitialized = true;
	printf("OS BETA RTOS INITIALIZATION COMPLETE\n\n");
	__enable_irq();
}

//CREATE THREAD
osBetaThread_id osBetaCreateThread(osBetaThreadFunc_t function, void *args, osBetaPriority priority) {
	__disable_irq();
	
    // access the tcb of the thread to be created and increment global id tracker
	osBetaThread_t *thread = &tcb[getNextId()];
	
	printf("CREATING THREAD %d...\n", (uint32_t)thread->id);
	
    // change thread properties to reflect newly created thread
	thread->state = osThreadReady;
	thread->priority = priority;
	
	for( uint8_t i = 0; i < 16; i++ )
	{
		thread->stackPointer -= 4;
        
        switch(i)
        {
            // PSR
            case 0:
                (*(uint32_t*)thread->stackPointer) = (uint32_t)DEFAULT_STACK_PSR;
                break;
            // function pointer
            case 1:
                (*(uint32_t*)thread->stackPointer) = (uint32_t)function;
                break;
            // arguments to function pointer
            case 7:
                (*(uint32_t*)thread->stackPointer) = (uint32_t)args;
                break;
            // no specific value... just need to be defined
            default:
                (*(uint32_t*)thread->stackPointer) = (uint32_t)0;
        }
	}
	
    // put the task into the scheduler
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
        // returns next task to run... NULL if no context switch is required
		nextTask = runScheduler();
	}
    
    // no context switch required
    if( nextTask == NULL )
    {
        __enable_irq();
        return;
    }
	
    // context switch required
	if( nextTask->id != runningTask->id && osInitialized && threadCreated )
	{
		printf("CONTEXT SWITCH REQUIRED...\n");
        
        // store context of old running task
		runningTask->stackPointer = storeContext();
		printf("STORED CONTEXT FROM TASK %d!\n", runningTask->id);

        // set PSP to sp of new running task
		__set_PSP(nextTask->stackPointer);
		
        // change thread states
		nextTask->state = osThreadRunning;
		runningTask->state = osThreadReady;
        
        // add old running task to ready list of scheduler
        addThreadToScheduler(runningTask);
        
        // update runningTask
		runningTask = nextTask;
		
        // restore context of new running task
		restoreContext(runningTask->stackPointer);
		printf("RESTORED CONTEXT FROM TASK %d!\n\n", runningTask->id);
	}
	
	__enable_irq();
}

#endif