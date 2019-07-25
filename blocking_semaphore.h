#ifndef BLOCKING_SEMAPHORE_H
#define BLOCKING_SEMAPHORE_H

#include "beta_os_defines.h"

//INIT
void initSemaphore(osBetaSemaphore_t *s, int32_t count ) {
    
		s->id = getNextId(&next_semaphore_id);
    s->count = count;
		s->waitList = NULL;
    
}

//BLOCK
void blockTask(osBetaSemaphore_t *s) {
	
		printf("BLOCKING TASK %d!\n", runningTask->id);
    
    osBetaThread_t *cursor = s->waitList;
    
    if(cursor == NULL)
        s->waitList = runningTask;
		
    else {
        while(cursor->next != NULL)
            cursor = cursor->next;
			
        cursor->next = runningTask;
		}

		runningTask->state = osThreadBlocked;
		
		triggerPendSV();
    
    
}

//UNBLOCK
void unblockTask(osBetaSemaphore_t *s) {  
	
    if(s->waitList == NULL)
        return;
    else {
				printf("UNBLOCKING TASK %d!\n", s->waitList->id);
        osBetaThread_t *thread = s->waitList;
			
				thread->state = osThreadReady;
        s->waitList = thread->next;
			
        addThreadToScheduler(thread);
				triggerPendSV();
    }
    
}

//WAIT
void wait(osBetaSemaphore_t *s, osBetaThread_id thread_id) {
    __disable_irq();
    
		(s->count)--;
		printf("Thread %d tried to wait on Semaphore %d\n", thread_id, s->id);
	
    if( s->count < 0 ) {
        blockTask(s);
    }
		else
			printf("Thread %d successfully waited on Semaphore %d\n", thread_id, s->id);
    
    __enable_irq();
}

//SIGNAL
void signal(osBetaSemaphore_t *s, osBetaThread_id thread_id) {
    __disable_irq();
    
    (s->count)++;
    printf("Thread %d signalled Semaphore %d\n", thread_id, s->id);
	
    if( s->count - 1 < 0 ) {
        unblockTask(s);
    }
    
    __enable_irq();
}

#endif
