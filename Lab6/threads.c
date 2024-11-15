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

    // Continue reading if there is still data

    // Store current data value

    // Signal data ready

    // Clear the asserted interrupts
}

/*******************************Aperiodic Threads***********************************/
