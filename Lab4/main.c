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
int main(void)
{
    // Sets clock speed to 80 MHz. You'll need it!
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    G8RTOS_Init();
    multimod_init();

    // Add threads, semaphores, FIFOs here

    G8RTOS_AddThread(&IdleThread, 255, "IdleThread");    // Idle thread with the lowest priority

    G8RTOS_InitSemaphore(&sem_I2CA, 1);              // Initialize I2C semaphore
    G8RTOS_InitSemaphore(&sem_SPIA, 1);              // Initialize SPI semaphore
    G8RTOS_InitSemaphore(&sem_PCA9555_Debounce, 1);  // PCA9555 button debounce semaphore
    G8RTOS_InitSemaphore(&sem_Joystick_Debounce, 1); // Joystick press debounce semaphore
    G8RTOS_InitSemaphore(&sem_KillCube, 1);

    G8RTOS_Launch();
    while (1)
    {
    }
}

/************************************MAIN*******************************************/

/************************************Test Threads***********************************/



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
