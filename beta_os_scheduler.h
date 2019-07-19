#ifndef BETA_OS_SCHEDULER_H
#define BETA_OS_SCHEDULER_H

#include "beta_os_defines.h"


osBetaThread_t tcb[6] = {{.id = 0}, {.id = 1}, {.id = 2}, {.id = 3}, {.id = 4}, {.id = 5}};
	
osBetaThread_t *runningTask = &(tcb[0]); //global running task to be updated by the scheduler.. should this be defined as the idle task??
osBetaThread_t *nextTask = &(tcb[0]);

// SCHEDULER

//FPP Scheduler
typedef struct {
	uint8_t bitVector;
	osBetaLinkedList_t readyList[NUM_PRIORITIES];
} osBetaFPPScheduler_t;

osBetaFPPScheduler_t scheduler = { .bitVector = 0x0, .readyList = { NULL, NULL, NULL, NULL, NULL } };

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
osBetaThread_t* removeThreadFromScheduler(osBetaThread_t* thread) {
	osBetaThread_t *cursor = scheduler.readyList[thread->priority].head;
	
	if(cursor == NULL)
		return NULL;
	
	osBetaThread_t* threadToRemove;
	
	if(cursor->next == NULL)
	{
		threadToRemove = cursor;
		
		cursor = NULL;
	}
	else
	{
		while(cursor->next != thread)
			cursor = cursor->next;
	
		threadToRemove = cursor->next;
		
		// remove thread
		cursor->next = cursor->next->next;
	}
	
	if(scheduler.readyList[thread->priority].head == NULL)
		clearBitVector(thread->priority);
	
	threadToRemove->next = NULL;
	
	return threadToRemove;
}

//GET HIGHEST PRIORITY
osBetaPriority getHighestPriorityThread(void) {
	//__asm("clz %0" : "=r" (highestPriority) : "r" (scheduler.bitVector));
	for(int priority = 4; priority >= 0; priority--)
	{
		uint8_t result = scheduler.bitVector & (1 << priority);
		if( result == (0x01 << priority))
			return (osBetaPriority)priority;
	}
}

//RUN SCHEDULER
osBetaThread_t* runScheduler(void) {
	osBetaPriority highestPriority = getHighestPriorityThread();
	
	if(runningTask->priority > highestPriority)
		return NULL;
	else {
		//pre-empt
		return removeThreadFromScheduler(scheduler.readyList[highestPriority].head);
	}
}

#endif