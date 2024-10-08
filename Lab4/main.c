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
void ProducerThread(void);
void ConsumerThread(void);
void UARTWriter(void);
void SleepingThread(void);
void IdleThread(void);
semaphore_t uartSemaphore;
int main(void)
{
    // Sets clock speed to 80 MHz. You'll need it!
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    G8RTOS_Init();
    multimod_init();

    // Add threads, semaphores, here
    G8RTOS_InitFIFO(0);
    G8RTOS_InitSemaphore(&uartSemaphore, 1);
    G8RTOS_AddThread(&ProducerThread, 0, "Producer");       // Produces data into FIFO
    G8RTOS_AddThread(&ConsumerThread, 1, "Consumer");       // Consumes data from FIFO and writes using UART
    G8RTOS_AddThread(&UARTWriter, 2, "UARTWriter");         // Writes using UART
    G8RTOS_AddThread(&SleepingThread, 4, "SleepingThread"); // Shows sleep and yield behavior
    G8RTOS_AddThread(&IdleThread, 255, "IdleThread");       // Idle thread with the lowest priority

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
        sleep(5);
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
        G8RTOS_WaitSemaphore(&uartSemaphore);

        int32_t data = G8RTOS_ReadFIFO(0); // Read data from FIFO
        if (data != -1)                    // If FIFO is not empty, data was read successfully
        {
            UARTprintf("Data from FIFO: %d\n", data);
        }

        G8RTOS_SignalSemaphore(&uartSemaphore);
        sleep(5);
    }
}

void UARTWriter(void)
{
    while (1)
    {
        G8RTOS_WaitSemaphore(&uartSemaphore);
        UARTprintf("UART semaphore test\n");    // Critical section (access to UART)
        G8RTOS_SignalSemaphore(&uartSemaphore);
        sleep(5);
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
        sleep(10);
    }
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
