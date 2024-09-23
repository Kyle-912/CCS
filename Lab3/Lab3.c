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

int32_t counter0 = 0;
int32_t counter1 = 0;
int32_t counter2 = 0;

/********************************Public Variables***********************************/

/********************************Public Functions***********************************/

// Complete the functions below as test threads.
void task0()
{
    while (1)
    {
        counter0++;
        SysCtlDelay(1000000);
    }
}

void task1()
{
    while (1)
    {
        counter1++;
        SysCtlDelay(2000000);
    }
}

void task2()
{
    while (1)
    {
        counter2++;
        SysCtlDelay(3000000);
    }
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
    G8RTOS_AddThread(&task0);
    G8RTOS_AddThread(&task1);
    G8RTOS_AddThread(&task2);

    G8RTOS_Launch();
    while (1)
    {
        UARTprintf("Counter0: %d, Counter1: %d, Counter2: %d\n", counter0, counter1, counter2);
    }
}

/************************************MAIN*******************************************/
