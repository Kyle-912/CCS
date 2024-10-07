// G8RTOS_IPC.h
// Date Created: 2023-07-26
// Date Updated: 2023-07-26
// Interprocess communication code for G8RTOS

#ifndef G8RTOS_IPC_H_
#define G8RTOS_IPC_H_

/************************************Includes***************************************/

#include <stdint.h>

#include "./G8RTOS_Semaphores.h"

/************************************Includes***************************************/

/*************************************Defines***************************************/

#define FIFO_SIZE 16
#define MAX_NUMBER_OF_FIFOS 4

/*************************************Defines***************************************/

/******************************Data Type Definitions********************************/
/******************************Data Type Definitions********************************/

/****************************Data Structure Definitions*****************************/

typedef struct G8RTOS_FIFO_t
{
    int32_t buffer[FIFO_SIZE]; // Buffer array for FIFO data
    int32_t *head;             // Pointer to the head of FIFO
    int32_t *tail;             // Pointer to the tail of FIFO
    int32_t lostData;          // Tracks lost data count
    int32_t currentSize;       // Current size of data in FIFO
    semaphore_t mutex;         // Mutex for FIFO synchronization
} G8RTOS_FIFO_t;

/****************************Data Structure Definitions*****************************/

/********************************Public Variables***********************************/
/********************************Public Variables***********************************/

/********************************Public Functions***********************************/

int32_t G8RTOS_InitFIFO(uint32_t FIFO_index);
int32_t G8RTOS_ReadFIFO(uint32_t FIFO_index);
int32_t G8RTOS_WriteFIFO(uint32_t FIFO_index, uint32_t data);

/********************************Public Functions***********************************/

#endif /* G8RTOS_IPC_H_ */
