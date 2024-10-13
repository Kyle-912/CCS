// G8RTOS_IPC.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for FIFO functions for interprocess communication

#include "../G8RTOS_IPC.h"

/************************************Includes***************************************/

#include "../G8RTOS_Semaphores.h"

/******************************Data Type Definitions********************************/

/****************************Data Structure Definitions*****************************/

/***********************************Externs*****************************************/

/********************************Private Variables***********************************/

static G8RTOS_FIFO_t FIFOs[MAX_NUMBER_OF_FIFOS];

/********************************Public Functions***********************************/

// G8RTOS_InitFIFO
// Initializes FIFO, points head & tai to relevant buffer
// memory addresses. Returns - 1 if FIFO full, 0 if no error
// Param uint32_t "FIFO_index": Index of FIFO block
// Return: int32_t
int32_t G8RTOS_InitFIFO(uint32_t FIFO_index)
{
    // Check if FIFO index is out of bounds
    if (FIFO_index >= MAX_NUMBER_OF_FIFOS)
    {
        return -1;
    }

    // Init head, tail pointers
    FIFOs[FIFO_index].head = FIFOs[FIFO_index].buffer;
    FIFOs[FIFO_index].tail = FIFOs[FIFO_index].buffer;

    // Init the mutex, current size
    G8RTOS_InitSemaphore(&(FIFOs[FIFO_index].mutex), 1);
    G8RTOS_InitSemaphore(&(FIFOs[FIFO_index].read), 0);
    G8RTOS_InitSemaphore(&(FIFOs[FIFO_index].write), FIFO_SIZE);
}

// G8RTOS_ReadFIFO
// Reads data from head pointer of FIFO.
// Param uint32_t "FIFO_index": Index of FIFO block
// Return: int32_t
int32_t G8RTOS_ReadFIFO(uint32_t FIFO_index)
{
    if (FIFO_index >= MAX_NUMBER_OF_FIFOS)
    {
        return -1; // Check if index is valid
    }

    G8RTOS_WaitSemaphore(&(FIFOs[FIFO_index].read));
    G8RTOS_WaitSemaphore(&(FIFOs[FIFO_index].mutex));

    // Read data from head and increment head pointer
    int32_t data = *(FIFOs[FIFO_index].head);
    FIFOs[FIFO_index].head++;

    // Wrap head pointer if it reaches the end
    if (FIFOs[FIFO_index].head == FIFOs[FIFO_index].buffer + FIFO_SIZE)
    {
        FIFOs[FIFO_index].head = FIFOs[FIFO_index].buffer;
    }

    G8RTOS_SignalSemaphore(&(FIFOs[FIFO_index].mutex));
    G8RTOS_SignalSemaphore(&(FIFOs[FIFO_index].write));
    
    return data;
}

// G8RTOS_WriteFIFO
// Writes data to tail of buffer.
// 0 if no error, -1 if out of bounds, -2 if full
// Param uint32_t "FIFO_index": Index of FIFO block
// Param uint32_t "data": data to be written
// Return: int32_t
int32_t G8RTOS_WriteFIFO(uint32_t FIFO_index, uint32_t data)
{
    if (FIFO_index >= MAX_NUMBER_OF_FIFOS)
    {
        return -1; // Check if index is valid
    }

    G8RTOS_WaitSemaphore(&(FIFOs[FIFO_index].write));
    G8RTOS_WaitSemaphore(&(FIFOs[FIFO_index].mutex));

    // Write data to tail and increment tail pointer
    *(FIFOs[FIFO_index].tail) = data;
    FIFOs[FIFO_index].tail++;

    // Wrap tail pointer if it reaches the end
    if (FIFOs[FIFO_index].tail == FIFOs[FIFO_index].buffer + FIFO_SIZE)
    {
        FIFOs[FIFO_index].tail = FIFOs[FIFO_index].buffer;
    }

    G8RTOS_SignalSemaphore(&(FIFOs[FIFO_index].mutex));
    G8RTOS_SignalSemaphore(&(FIFOs[FIFO_index].read));

    return 0;
}
