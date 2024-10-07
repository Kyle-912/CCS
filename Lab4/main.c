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
void BlockingThread(void);
void SignalingThread(void);
void SleepingThread(void);
semaphore_t testSemaphore;
int main(void)
{
    // Sets clock speed to 80 MHz. You'll need it!
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    G8RTOS_Init();
    multimod_init();

    // Add threads, semaphores, here
    G8RTOS_InitFIFO(0);
    G8RTOS_AddThread(&ProducerThread, 0, "Producer");       // Produces data into FIFO
    G8RTOS_AddThread(&ConsumerThread, 1, "Consumer");       // Consumes data from FIFO
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
 * Description: Writes incrementing values to FIFO at index 0 every 1 second.
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
        SysCtlDelay(SysCtlClockGet() / 3); // 1-second delay using SysCtlDelay
    }
}

/**
 * Thread: ConsumerThread
 * Description: Reads data from FIFO at index 0 every 500 ms.
 */
void ConsumerThread(void)
{
    while (1)
    {
        int32_t data = G8RTOS_ReadFIFO(0); // Read data from FIFO
        if (data != -1)                    // If FIFO is not empty, data was read successfully
        {
            // Use a breakpoint here to observe the data being consumed
        }
        SysCtlDelay(SysCtlClockGet() / 6); // 500 ms delay using SysCtlDelay
    }
}

/**
 * Thread: BlockingThread
 * Description: Waits on a semaphore until it is signaled.
 */
void BlockingThread(void)
{
    while (1)
    {
        G8RTOS_WaitSemaphore(&testSemaphore); // Block until semaphore is signaled
        // Set a breakpoint here to observe unblocking in the debugger
    }
}

/**
 * Thread: SignalingThread
 * Description: Signals the blocked semaphore every 2 seconds.
 */
void SignalingThread(void)
{
    while (1)
    {
        SysCtlDelay(SysCtlClockGet() / 1.5);    // 2-second delay using SysCtlDelay
        G8RTOS_SignalSemaphore(&testSemaphore); // Signal the semaphore to unblock BlockingThread
    }
}

/**
 * Thread: SleepingThread
 * Description: Sleeps for 2 seconds and then wakes up to demonstrate sleep behavior.
 */
void SleepingThread(void)
{
    while (1)
    {
        CurrentlyRunningThread->asleep = true;  // Use the built-in sleep mechanism
        SysCtlDelay(SysCtlClockGet() / 1.5);    // Simulate a 2-second sleep using SysCtlDelay
        CurrentlyRunningThread->asleep = false; // Wake up after sleep
    }
}

/************************************Test Threads***********************************/
