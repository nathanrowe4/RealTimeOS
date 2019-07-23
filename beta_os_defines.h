#ifndef BETA_OS_DEFINES_H
#define BETA_OS_DEFINES_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <lpc17xx.h>

#include "context.h"

//1 kiB
#define THREAD_STACK_SIZE 0x400

#define MAIN_STACK_BASE_POINTER 0x0

#define DEFAULT_STACK_PSR 0x01000000

#define NUM_PRIORITIES 0x05

typedef void (*osBetaThreadFunc_t)(void *args); // defines pointer to function type that takes void* as a parameter and returns void

typedef uint8_t osBetaThread_id;

uint32_t msTicks = 0;
osBetaThread_id next_thread_id = 0;
uint32_t next_mutex_id = 0;
uint32_t next_semaphore_id = 0;

///betaOS Priority List
//change NUM_PRIORITIES definition if this changes
typedef enum {
	Beta = 0,
	Weeb,
	Normal,
	Alpha,
	Chad
} osBetaPriority;

//OS Status
typedef enum {
	osBetaOK = 0,
	osBetaError
	
} osBetaStatus_t;

//Thread Status
typedef enum {
	osThreadInactive = 0,
	osThreadReady,
	osThreadRunning,
	osThreadBlocked,
	osThreadTerminated,
	osThreadError
} osBetaThreadStatus_t;

///TCB
typedef struct osBetaThread_s {
	osBetaThread_id 					id;
	osBetaThreadStatus_t 			state;
	uint32_t 									stackPointer;
	uint32_t									baseStackPointer;
	osBetaPriority 						priority;
	osBetaPriority						basePriority;
	struct osBetaThread_s 		*next;
	struct osBetaThread_s 		*prev;
	uint32_t									delay;
	uint32_t									eventFlags;
	uint32_t									entryAddress;
	
} osBetaThread_t;

typedef struct {
	osBetaThread_t *head;
	osBetaPriority priority;
} osBetaLinkedList_t;

typedef struct {
		uint32_t id;
    int32_t count;
    osBetaThread_t *waitList;
} osBetaSemaphore_t;

typedef struct {
		uint32_t id;
    bool inUse;
		osBetaThread_id owner;
    osBetaThread_t *waitList;
	
} osBetaMutex_t;

void triggerPendSV(void) {
	SCB->ICSR = SCB->ICSR | (1<<28);
}

// returns id and increments global variable
uint32_t getNextId(uint32_t *global_id) {
    osBetaThread_id idToReturn = *global_id;
    (*global_id)++;
    return idToReturn;
}

osBetaThread_t tcb[6] = {{.id = 0}, {.id = 1}, {.id = 2}, {.id = 3}, {.id = 4}, {.id = 5}};
	
osBetaThread_t *runningTask = &(tcb[0]); //global running task to be updated by the scheduler.. should this be defined as the idle task??
osBetaThread_t *nextTask = &(tcb[0]);

osBetaThread_t* getThread(osBetaThread_id thread_id)
{
	return &(tcb[thread_id]);
}

#endif