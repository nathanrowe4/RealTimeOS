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
    int32_t count;
    osBetaThread_t *waitList;
} osBetaSemaphore_t;

typedef struct {
    bool inUse;
		osBetaThread_id owner;
    osBetaThread_t *waitList;
	
} osBetaMutex_t;

void triggerPendSV(void) {
	SCB->ICSR = SCB->ICSR | (1<<28);
}

#endif