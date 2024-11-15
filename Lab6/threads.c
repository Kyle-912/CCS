// G8RTOS_Threads.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for thread functions.

/************************************Includes***************************************/

#include "./threads.h"

#include "./MultimodDrivers/multimod.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/************************************Includes***************************************/

/*************************************Defines***************************************/

/*************************************Defines***************************************/

/*********************************Global Variables**********************************/

// Global data structure for keeping track of data

/*********************************Global Variables**********************************/

/*************************************Threads***************************************/

void Idle_Thread(void)
{
    while (1)
    {
    }
}

void DrawBox_Thread(void)
{
    SysCtlDelay(1);

    // Declare variables

    while (1)
    {
        // Wait for data
        G8RTOS_WaitSemaphore(sem_UART4_Data);

        // Read in data

        // Draw rectangle
    }
}

/********************************Periodic Threads***********************************/

/********************************Periodic Threads***********************************/

/*******************************Aperiodic Threads***********************************/

void UART4_Handler()
{
    // Prepare to read data

    // Get interrupt status
    uint32_t ui32Status = UARTIntStatus(UART4_BASE, true);

    // Continue reading if there is still data
    while (UARTCharsAvail(UART4_BASE))
    {
        // Store current data value
        char data = UARTCharGetNonBlocking(UART4_BASE);
    }

    // Signal data ready
    G8RTOS_SignalSemaphore(sem_UART4_Data);

    // Clear the asserted interrupts
    UARTIntClear(UART4_BASE, ui32Status);
}

/*******************************Aperiodic Threads***********************************/
