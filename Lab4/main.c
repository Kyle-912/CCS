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

semaphore_t testSemaphore;
int main(void)
{
    // Sets clock speed to 80 MHz. You'll need it!
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    G8RTOS_Init();
    multimod_init();

    // Add threads, semaphores, here
    G8RTOS_InitFIFO(0);
    G8RTOS_InitSemaphore(&testSemaphore, 1);
    G8RTOS_AddThread(&ProducerThread, 0, "Producer"); // Writes to FIFO
    G8RTOS_AddThread(&ConsumerThread, 1, "Consumer"); // Reads from FIFO
    G8RTOS_AddThread(&BlockedSemaphoreThread, 2, "BlockedThread");

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
        G8RTOS_Sleep(500); // Sleep for 500 ms
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
            // Debug/Log: Read data from FIFO (can set a breakpoint here)
        }
        G8RTOS_Sleep(200); // Sleep for 200 ms before next read
    }
}

/**
 * Thread: BlockedSemaphoreThread
 * Description: Tries to access a binary semaphore, blocks if unavailable.
 */
void BlockedSemaphoreThread(void)
{
    while (1)
    {
        G8RTOS_WaitSemaphore(&testSemaphore); // Will block if semaphore value is 0
        // Do some work while holding the semaphore (e.g., simulate resource access)
        G8RTOS_Sleep(300);                      // Sleep for 300 ms to simulate work
        G8RTOS_SignalSemaphore(&testSemaphore); // Release semaphore
        G8RTOS_Sleep(500);                      // Sleep for a while before trying to acquire again
    }
}

/************************************Test Threads***********************************/