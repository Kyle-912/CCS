// Lab 4, uP2 Fall 2023
// Created: 2023-07-31
// Updated: 2023-08-01
// Lab 4 is intended to introduce you to more advanced RTOS concepts. In this, you will
// - implement blocking, yielding, sleeping
// - Thread priorities, aperiodic & periodic threads
// - IPC using FIFOs
// - Dynamic thread creation & deletion

/************************************Includes***************************************/

#include "G8RTOS/G8RTOS.h"
#include "./MultimodDrivers/multimod.h"

#include "./threads.h"

/************************************MAIN*******************************************/
void ProducerThread(void);
void ConsumerThread(void);
void BlockedSemaphoreThread(void);
semaphore_t testSemaphore;
int main(void)
{
    // Sets clock speed to 80 MHz. You'll need it!
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    G8RTOS_Init();
    multimod_init();

    // Add threads, semaphores, here
    G8RTOS_InitFIFO(0);
    G8RTOS_AddThread(&ProducerThread, 0, "Producer");       // Writes to FIFO
    G8RTOS_AddThread(&ConsumerThread, 1, "Consumer");       // Reads from FIFO
    G8RTOS_AddThread(&BlockingThread, 2, "BlockingThread"); // Blocks on a semaphore
    G8RTOS_AddThread(&SignalingThread, 3, "Signaler");      // Signals the blocked thread
    G8RTOS_AddThread(&SleepingThread, 4, "SleepingThread"); // Shows sleep and yield behavior

    G8RTOS_Launch();
    while (1)
    {
    }
}

/************************************MAIN*******************************************/

/************************************Test Threads***********************************/

/**
 * Thread: ProducerThread
 * Description: Writes incrementing values to FIFO at index 0.
 */
void ProducerThread(void)
{
    uint32_t data = 0;
    while (1)
    {
        if (G8RTOS_WriteFIFO(0, data) == 0) // Write data to FIFO
        {
            data++; // Increment data if write is successful
        }
        G8RTOS_Yield(); // Yield control to allow other threads to run
    }
}

/**
 * Thread: ConsumerThread
 * Description: Reads data from FIFO at index 0.
 */
void ConsumerThread(void)
{
    while (1)
    {
        int32_t data = G8RTOS_ReadFIFO(0); // Read data from FIFO
        if (data != -1)                    // If FIFO is not empty
        {
            // Debug: Can set a breakpoint here to view consumed data
        }
        G8RTOS_Yield(); // Yield control after reading from FIFO
    }
}

/**
 * Thread: BlockingThread
 * Description: Waits on a semaphore until signaled.
 */
void BlockingThread(void)
{
    while (1)
    {
        G8RTOS_WaitSemaphore(&testSemaphore); // Block until semaphore is signaled
        // Perform some work or set a breakpoint here to observe unblocking
        G8RTOS_Yield(); // Yield after unblocking
    }
}

/**
 * Thread: SignalingThread
 * Description: Signals the blocked semaphore every few seconds.
 */
void SignalingThread(void)
{
    while (1)
    {
        for (volatile int i = 0; i < 1000000; i++) // Basic delay loop
        {
        }
        G8RTOS_SignalSemaphore(&testSemaphore); // Signal the semaphore to unblock
        G8RTOS_Yield();                         // Yield control
    }
}

/**
 * Thread: SleepingThread
 * Description: Sleeps for 1 second and then wakes up to demonstrate sleep behavior.
 */
void SleepingThread(void)
{
    while (1)
    {
        G8RTOS_Sleep(1000); // Sleep for 1000 milliseconds
        // Thread wakes up after sleep, observe in debugger
        G8RTOS_Yield(); // Yield control
    }
}

/************************************Test Threads***********************************/