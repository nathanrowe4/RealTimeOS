#ifndef BLOCKING_SEMAPHORE_H
#define BLOCKING_SEMAPHORE_H

#include "beta_os_defines.h"

//INIT
void init(osBetaSemaphore_t *s, uint32_t count) {
    *s = count;
}

//WAIT
void wait(osBetaSemaphore_t *s) {
    __disable_irq();
    
    (*s)--;
    
    if( *s >= 0) {
        //block task
    }
    
    __enable_irq();
}

//SIGNAL
void signal(osBetaSemaphore_t *s) {
    __disable_irq();
    
    (*s)++;
    
    if( *s <= 0 ) {
        //unblock task
    }
    
    __enable_irq();
}

#endif