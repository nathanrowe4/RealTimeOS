#ifndef BETA_OS_SCHEDULER_H
#define BETA_OS_SCHEDULER_H

#include "beta_os_defines.h"

// SCHEDULER

//FPP Scheduler
typedef struct {
	uint8_t bitVector;
	osBetaLinkedList_t readyList[NUM_PRIORITIES];
} osBetaFPPScheduler_t;

osBetaFPPScheduler_t scheduler = { .bitVector = 0x0, .readyList = { {.head=NULL}, {.head=NULL}, {.head=NULL}, {.head=NULL}, {.head=NULL} } };

//SET BIT VECTOR
void setBitVector(osBetaPriority priority) {
	scheduler.bitVector = scheduler.bitVector | (0x01 << priority);
}

//CLEAR BIT VECTOR
void clearBitVector(osBetaPriority priority) {
	scheduler.bitVector = scheduler.bitVector & ~(0x01 << priority);
}

//ADD THREAD TO SCHEDULER
void addThreadToScheduler(osBetaThread_t *newThread) {

	newThread->next = NULL;
	
	newThread->state = osThreadReady;
	
	//put node in position
	osBetaPriority priority = newThread->priority;
	osBetaThread_t *cursor = scheduler.readyList[priority].head;
	
	if(cursor == NULL) {
		// case 1: list is empty
		scheduler.readyList[priority].head = newThread;
		setBitVector(priority);
	}
	else {
		// case 2: list is non-empty
		while(cursor->next != NULL) {
			cursor = cursor->next;
		}
		
		cursor->next = newThread;
	}
}

//REMOVE THREAD FROM SCHEDULER
osBetaThread_t* removeThreadFromScheduler(osBetaPriority priority) {

  osBetaThread_t *threadToRemove = scheduler.readyList[priority].head;
	
	if(threadToRemove == NULL)
		return NULL;
	
	scheduler.readyList[priority].head = threadToRemove->next;
	
	if(scheduler.readyList[priority].head == NULL)
		clearBitVector(priority);
	
	threadToRemove->next = NULL;
	
	return threadToRemove;
}

//GET HIGHEST PRIORITY THREAD WIHTOUT REMOVING IT FROM THE READY LIST OF SCHEDULER
// currently O(n) runtime... if time, implement in O(1) using embedded assembly instruction
osBetaPriority getHighestPriority(void) {
	
    //__asm("clz %0" : "=r" (highestPriority) : "r" (scheduler.bitVector));
	for(int priority = 4; priority >= 0; priority--)
	{
		uint8_t result = scheduler.bitVector & (1 << priority);
		if( result == (0x01 << priority))
			return (osBetaPriority)priority;
	}
    // no tasks in ready list
    return (osBetaPriority)0x0;
}

//RUN SCHEDULER
osBetaThread_t* runScheduler(void) {
  osBetaPriority highestPriority = getHighestPriority();
	
	if(runningTask == NULL || runningTask->state == osThreadBlocked || runningTask->priority <= highestPriority)
		return removeThreadFromScheduler(highestPriority);
	else
		return NULL;
}

#endif
