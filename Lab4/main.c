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
#include <driverlib/uartstdio.h>

#include "./threads.h"

/************************************MAIN*******************************************/
void SpawnerThread(void);
void SelfTerminatingThread(void);
void PeriodicPrinter(void);
void IdleThread(void);
semaphore_t uartSemaphore;
int main(void)
{
    // Sets clock speed to 80 MHz. You'll need it!
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    G8RTOS_Init();
    multimod_init();

    // Add threads, semaphores, here
    G8RTOS_InitSemaphore(&uartSemaphore, 1);
    G8RTOS_AddThread(&SpawnerThread, 0, "Spawner");      // Spawns other self terminating threads
    G8RTOS_Add_PeriodicEvent(&PeriodicPrinter, 1000, 0); // Add periodic event that triggers every second (1000 ticks = 1 second)
    G8RTOS_AddThread(&IdleThread, 255, "IdleThread");    // Idle thread with the lowest priority

    G8RTOS_Launch();
    while (1)
    {
    }
}

/************************************MAIN*******************************************/

/************************************Test Threads***********************************/

/**
 * Thread: SpawnerThread
 * Description: Spawns self-terminating threads dynamically.
 */
void SpawnerThread(void)
{
    static uint8_t threadCounter = 0;
    while (1)
    {
        if (threadCounter < 5)
        {
            // Create a unique thread name for the new thread
            char threadName[16] = "TermThread ";
            threadName[11] = '0' + threadCounter; // Append thread number
            threadName[12] = '\0';

            // Dynamically add a self-terminating thread
            G8RTOS_AddThread(&SelfTerminatingThread, 5, threadName); // Medium priority
            threadCounter++;
        }

        sleep(5); // Sleep for 500ms before spawning another thread
    }
}

/**
 * Thread: SelfTerminatingThread
 * Description: Runs for a while and then terminates itself.
 */
void SelfTerminatingThread(void)
{
    while (1)
    {
        G8RTOS_WaitSemaphore(&uartSemaphore);
        UARTprintf("Self-terminating thread started!\n");
        G8RTOS_SignalSemaphore(&uartSemaphore);

        sleep(20); // Simulates some work by sleeping for 2 seconds

        G8RTOS_WaitSemaphore(&uartSemaphore);
        UARTprintf("Self-terminating thread ending.\n");
        G8RTOS_SignalSemaphore(&uartSemaphore);

        G8RTOS_KillSelf();
    }
}

/**
 * Thread: PeriodicPrinter
 * Description: Prints a message every second with an incrementing value.
 */
void PeriodicPrinter(void)
{
    static uint32_t counter = 0; // Value to print with each message

    G8RTOS_WaitSemaphore(&uartSemaphore);
    UARTprintf("Periodic event triggered, value: %d\n", counter);
    G8RTOS_SignalSemaphore(&uartSemaphore);

    counter++; // Increment the value for the next print
}

/**
 * Thread: IdleThread
 * Description: Idle thread with the lowest priority. Does nothing.
 */
void IdleThread(void)
{
    while (1)
    {
    }
}

/************************************Test Threads***********************************/
