// Lab 6, uP2 Fall 2023
// Created: 2023-07-31
// Updated: 2023-08-01
// Lab 6 is intended to serve as an introduction to the BeagleBone Black and wireless
// communication concepts.

/************************************Includes***************************************/

#include "G8RTOS/G8RTOS.h"
#include "./MultimodDrivers/multimod.h"

#include "./threads.h"

/************************************MAIN*******************************************/

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    multimod_init();
    G8RTOS_Init();

    // Initialize Semaphores
    G8RTOS_InitSemaphore(&sem_UART4_Data, 0);
    G8RTOS_InitSemaphore(&sem_SPIA, 1);

    // Add background threads
    G8RTOS_AddThread(&Idle_Thread, 255, "IdleThread");
    G8RTOS_AddThread(&DrawBox_Thread, 0, "DrawBoxThread");

    // Add periodic threads

    // Add aperiodic threads
    G8RTOS_Add_APeriodicEvent(UART4_Handler, 0, INT_UART4);

    G8RTOS_Launch();
    while (1)
    {
    }
}

/************************************MAIN*******************************************/
