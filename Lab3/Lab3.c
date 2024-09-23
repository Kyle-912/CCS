// Lab 3, uP2 Fall 2023
// Created: 2023-07-31
// Updated: 2023-08-01
// Lab 3 is intended to introduce you to RTOS concepts. In this, you will
// - configure the systick function
// - write asm functions for context switching
// - write semaphore functions
// - write scheduler functions to add threads / run scheduling algorithms
// - write critical section assembly functions

/************************************Includes***************************************/

#include "G8RTOS/G8RTOS.h"
#include "./MultimodDrivers/multimod.h"

#include "./threads.h"

/************************************Includes***************************************/

/*************************************Defines***************************************/
/*************************************Defines***************************************/

/********************************Public Variables***********************************/
/********************************Public Variables***********************************/

/********************************Public Functions***********************************/

// Complete the functions below as test threads.
void task0()
{
}

void task1()
{
}

void task2()
{
}

/********************************Public Functions***********************************/

/************************************MAIN*******************************************/

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    G8RTOS_Init();

    // Initializes the necessary peripherals.
    Multimod_Init();

    // Add threads, initialize semaphores here!

    G8RTOS_Launch();
    while (1)
    {
    }
}

/************************************MAIN*******************************************/