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

int main(void) {
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    multimod_init();
    G8RTOS_Init();

    // Initialize Semaphores

    // Add background threads

    // Add periodic threads

    // Add aperiodic threads

    
    G8RTOS_Launch();
    while(1);
}

/************************************MAIN*******************************************/
