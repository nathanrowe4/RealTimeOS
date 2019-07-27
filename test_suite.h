#include "beta_os.h"

osBetaSemaphore_t sem;
osBetaMutex_t mutex;

//TEST 1 THREAD DEFINITIONS-----------------------------------------------------------------------------------------------------------
void test1Thread3(void *args)
{
	acquire(&mutex, runningTask->id);

	uint32_t period = 1000;
	uint32_t prev = -period;
	uint32_t releasePeriod = 20000;
	uint32_t releasePrev = 0;
	
	while (true)
	{
		if((uint32_t)(msTicks - prev) >= period)
		{
			printf("Thread 3 Running, Thread 1's priority: %d\n", tcb[1].priority);
			prev += period;
		}
		if((uint32_t)(msTicks - releasePrev) >= releasePeriod)
		{
			release(&mutex, runningTask->id);
			releasePrev += releasePeriod;
		}
	}
}

void test1Thread2(void *args)
{
	uint32_t creationPeriod = 10000;
	uint32_t creationPrev = 0;
	uint32_t period = 1000;
	uint32_t prev = -period;
	bool created = false;
	
	while (true)
	{
		if((uint32_t)(msTicks - prev) >= period)
		{
			printf("Thread 2 Running, Thread 1's priority: %d\n", tcb[1].priority);
			prev += period;
		}
		
		if(!created && (uint32_t)(msTicks - creationPrev) >= creationPeriod)
		{
			osBetaThread_id thread1_id = osBetaCreateThread(&test1Thread3, NULL, Chad);
			creationPrev += creationPeriod;
			created = true;
		}
	}
}

void test1Thread1(void *args)
{
	acquire(&mutex, runningTask->id);
	uint32_t period = 1000;
	uint32_t prev = -period;
	uint32_t creationPeriod = 5000;
	uint32_t creationPrev = 0;
	uint32_t relPeriod = 15000;
	uint32_t relPrev = 0;
	bool created = false;
	bool released = false;
	
	while (true)
	{
		if((uint32_t)(msTicks - prev) >= period)
		{
			printf("Thread 1 Running, Thread 1's priority: %d\n", tcb[1].priority);
			prev += period;
		}
		
		if(!created && (uint32_t)(msTicks - creationPrev) >= creationPeriod)
		{
			osBetaThread_id thread2_id = osBetaCreateThread(&test1Thread2, NULL, Alpha);
			created = true;
			creationPrev += creationPeriod;
		}
		if(!released && (uint32_t)(msTicks - relPrev) >= relPeriod)
		{
			release(&mutex, runningTask->id);
			released = true;
			relPrev += relPeriod;
		}
	}
}
//END TEST 1 THREAD DEFINITIONS-------------------------------------------------------------------------------------------------------

//TEST 2 THREAD DEFINITIONS-----------------------------------------------------------------------------------------------------------
void test2Thread1(void *args)
{
	wait(&sem, runningTask->id);
	acquire(&mutex, runningTask->id);

	uint32_t period = 1000;
	uint32_t prev = -period;
	uint32_t releasePeriod = 20000;
	uint32_t releasePrev = 0;
	bool released = false;
	uint32_t *argument = args;
	
	while (true)
	{
		if((uint32_t)(msTicks - prev) >= period)
		{
			printf("Thread 1 Running, Semaphore count: %d. Mutex owned by: %d\n", *argument, mutex.owner);
			prev += period;
		}
		if(!released && (uint32_t)(msTicks - releasePrev) >= releasePeriod)
		{
			release(&mutex, runningTask->id);
			released = true;
			releasePrev += releasePeriod;
		}
	}
}

void test2Thread2(void *args)
{
	wait(&sem, runningTask->id);

	uint32_t period = 1000;
	uint32_t prev = -period;
	uint32_t *argument = args;
	uint32_t releasePeriod = 10000;
	uint32_t releasePrev = 0;
	bool released = false;
	
	while (true)
	{
		if((uint32_t)(msTicks - prev) >= period)
		{
			printf("Thread 2 Running, Semaphore count: %d. Mutex owned by: %d\n", *argument, mutex.owner);
			prev += period;
		}
		if(!released && (uint32_t)(msTicks - releasePrev) >= releasePeriod)
		{
			signal(&sem, runningTask->id);
			released = true;
			releasePrev += releasePeriod;
			wait(&sem, runningTask->id);
		}
	}
}

void test2Thread3(void *args)
{
	wait(&sem, runningTask->id);
	release(&mutex, runningTask->id);
	uint32_t period = 1000;
	uint32_t prev = -period;
	uint32_t *argument = args;
	
	while (true)
	{
		if((uint32_t)(msTicks - prev) >= period)
		{
			printf("Thread 3 Running, Semaphore count: %d. Mutex owned by: %d\n", *argument, mutex.owner);
			prev += period;
		}
	}
}
//END TEST 2 THREAD DEFINITIONS--------------------------------------------------------------------------------------------------------

//TEST 2 THREAD DEFINITIONS------------------------------------------------------------------------------------------------------------
void test3Thread1(void *args)
{
	acquire(&mutex, runningTask->id);

	uint32_t period = 1000;
	uint32_t prev = -period;
	uint32_t releasePeriod = 20000;
	uint32_t releasePrev = 0;
	bool released = false;
	
	while (true)
	{
		if((uint32_t)(msTicks - prev) >= period)
		{
			printf("Thread 1 Running, Mutex owned by: %d\n", mutex.owner);
			prev += period;
		}
		if(!released && (uint32_t)(msTicks - releasePrev) >= releasePeriod)
		{
			release(&mutex, runningTask->id);
			released = true;
			releasePrev += releasePeriod;
		}
	}
}

void test3Thread3(void *args)
{
	acquire(&mutex, runningTask->id);

	uint32_t period = 1000;
	uint32_t prev = -period;
	
	while (true)
	{
		if((uint32_t)(msTicks - prev) >= period)
		{
			printf("Thread 1 Running, Mutex owned by: %d\n", mutex.owner);
			prev += period;
		}
	}
}

void test3Thread2(void *args)
{
	acquire(&mutex, runningTask->id);

	uint32_t period = 1000;
	uint32_t prev = -period;
	uint32_t cperiod = 5000;
	uint32_t cprev = 0;
	bool created = false;
	
	while (true)
	{
		if((uint32_t)(msTicks - prev) >= period)
		{
			printf("Thread 2 Running, Mutex owned by: %d\n", mutex.owner);
			prev += period;
		}
		if(!created && (uint32_t)(msTicks - cprev) >= cperiod)
		{
			osBetaThread_id thread2_id = osBetaCreateThread(&test3Thread3, &sem.count, Chad);
			created = true;
			cprev += cperiod;
		}
	}
}

/*
	Test 1 showcases the osBetaMutex_t datatype, and its built-in priority inheritance. The expected behaviour of Test 1 is as follows:
	Task 1 will be created with a Normal priority, and will run for 5 seconds before creating Task 2. During this 5 seconds, Task 1 will
	also acquire the mutex. Task 2 has a priority of Alpha, which will run until it creates Task 3 at 10 seconds, which has the highest priority (Chad).
	Task 3 will attempt to acquire the mutex, but cannot since it is still owned by Task 1. However, Task 1 has the lowest priority out of the three created
	tasks, and as such, under normal circumstances, Task 3 would be blocked forever. Priority Inheritance will fix this by temporarily updating 
	Task 1's priority to the same priority level as Task 3, Chad. Now, Task 1 will run until it releases the mutex at 15 seconds. At this point,
	Task 1's priority will be reset to Normal (its original priority) and Task 3 will unblock, acquire the mutex, and run.
	
	This test also showcases: FPP scheduling, Context Switching
*/
void runTest1(void)
{
	initMutex(&mutex);
	osBetaThread_id thread1_id = osBetaCreateThread(&test1Thread1, NULL, Normal);
}

/*
	Test 2 showcases blocking semaphores. The expected behaviour of Test 2 is as follows:
	Three threads will be created, all with Normal priority. Each will wait on the semaphore, which has a count of 2. Because of this, when the
	third thread attempts to wait on the semaphore, it will be blocked. Thread 1 will additionally acquire the mutex.
	Since all threads have the same priority, this means that the scheduler will run threads 1 and 2 in a round-robin fashion. 
	After 10 seconds, Thread 2 will signal the semaphore, and immediately attempt to wait on it again. However, the act of signalling the semaphore 
	will unblock Thread 3, which will wait on it immediately. This means that now Threads 1 and 3 have successfully waited on the semaphore, 
	and Thread 2 will be blocked. Threads 1 and 3 will now be scheduled in a round-robin fashion. As soon as Thread 3 is able to run, it will attempt
	to release the mutex. However, this will be unsuccessful, as the owner of the mutex is Thread 1, and as such, only Thread 1 will be able to release it.
	At 20 seconds, Thread 1 will release the mutex, and the output terminal will show that the owner of the mutex shows as Thread 255, 
	meaning that it is not currently owned by any thread.
	
	This test also showcases how the FPP scheduler runs tasks of the same priority in a round-robin way if they are the highest priority available tasks
*/
void runTest2(void)
{
	initSemaphore(&sem, 2);
	osBetaThread_id thread1_id = osBetaCreateThread(&test2Thread1, &sem.count, Normal);
	osBetaThread_id thread2_id = osBetaCreateThread(&test2Thread2, &sem.count, Normal);
	osBetaThread_id thread3_id = osBetaCreateThread(&test2Thread3, &sem.count, Normal);
}

void runTest3(void)
{
	initMutex(&mutex);
	
	osBetaThread_id thread1_id = osBetaCreateThread(&test3Thread1, &sem.count, Normal);
	osBetaThread_id thread2_id = osBetaCreateThread(&test3Thread2, &sem.count, Normal);
}
