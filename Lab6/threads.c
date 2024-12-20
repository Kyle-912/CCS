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
#define UART_BUFFER_SIZE 128
volatile char UART4_DataBuffer[UART_BUFFER_SIZE];
volatile uint32_t UART4_BufferHead = 0;
volatile uint32_t UART4_BufferTail = 0;

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
    uint16_t x, y, width, height, color;

    while (1)
    {
        // Wait for data
        G8RTOS_WaitSemaphore(&sem_UART4_Data);

        // Read in data
        while (((UART4_BufferHead + UART_BUFFER_SIZE) - UART4_BufferTail) % UART_BUFFER_SIZE >= 10)
        {
            // Extract rectangle data from the buffer
            x = (UART4_DataBuffer[UART4_BufferTail] << 8) | UART4_DataBuffer[UART4_BufferTail + 1];
            UART4_BufferTail = (UART4_BufferTail + 2) % UART_BUFFER_SIZE;

            y = (UART4_DataBuffer[UART4_BufferTail] << 8) | UART4_DataBuffer[UART4_BufferTail + 1];
            UART4_BufferTail = (UART4_BufferTail + 2) % UART_BUFFER_SIZE;

            width = (UART4_DataBuffer[UART4_BufferTail] << 8) | UART4_DataBuffer[UART4_BufferTail + 1];
            UART4_BufferTail = (UART4_BufferTail + 2) % UART_BUFFER_SIZE;

            height = (UART4_DataBuffer[UART4_BufferTail] << 8) | UART4_DataBuffer[UART4_BufferTail + 1];
            UART4_BufferTail = (UART4_BufferTail + 2) % UART_BUFFER_SIZE;

            color = (UART4_DataBuffer[UART4_BufferTail] << 8) | UART4_DataBuffer[UART4_BufferTail + 1];
            UART4_BufferTail = (UART4_BufferTail + 2) % UART_BUFFER_SIZE;

            // Draw rectangle
            G8RTOS_WaitSemaphore(&sem_SPIA);
            ST7789_DrawRectangle(x, y, width, height, color);
            G8RTOS_SignalSemaphore(&sem_SPIA);
        }
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

        // Add data to the buffer and handle buffer overflow (circular buffer)
        UART4_DataBuffer[UART4_BufferHead] = data;
        UART4_BufferHead = (UART4_BufferHead + 1) % UART_BUFFER_SIZE;

        // Handle buffer overflow by advancing the tail if needed
        if (UART4_BufferHead == UART4_BufferTail)
        {
            UART4_BufferTail = (UART4_BufferTail + 1) % UART_BUFFER_SIZE;
        }
    }

    // Signal data ready
    G8RTOS_SignalSemaphore(&sem_UART4_Data);

    // Clear the asserted interrupts
    UARTIntClear(UART4_BASE, ui32Status);
}

/*******************************Aperiodic Threads***********************************/
