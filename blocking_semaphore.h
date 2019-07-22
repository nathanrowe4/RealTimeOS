#ifndef BLOCKING_SEMAPHORE_H
#define BLOCKING_SEMAPHORE_H

#include "beta_os_defines.h"

//INIT
void init(osBetaSemaphore_t *s, uint32_t count) {
    
    *s = count;
    
}

//BLOCK
void blockTask(osBetaSemaphore_t *s) {
    
    osBetaThread_t *cursor = s->waitList;
    
    if(cursor == NULL)
        s->waitList = runningTask;
    else {
        while(cursor->next != NULL)
            cursor = cursor->next;
        
        cursor->next = runningTask;
    }
    
}

//UNBLOCK
void unblockTask(osBetaSemaphore_t *s) {
    
    if(s->waitList == NULL)
        return;
    else {
        osBetaThread_t *thread = s->waitList;
        s->waitList = thread->next;
        addThreadToScheduler(thread);
    }
    
}

//WAIT
void wait(osBetaSemaphore_t *s) {
    __disable_irq();
    
    (*s)--;
    
    if( *s >= 0) {
        blockTask(s);
    }
    
    __enable_irq();
}

//SIGNAL
void signal(osBetaSemaphore_t *s) {
    __disable_irq();
    
    (*s)++;
    
    if( *s <= 0 ) {
        unblockTask(s);
    }
    
    __enable_irq();
}

#endif