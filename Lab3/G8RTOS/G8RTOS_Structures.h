// G8RTOS_Structures.h
// Date Created: 2023-07-26
// Date Updated: 2023-07-26
// Thread block definitions

#ifndef G8RTOS_STRUCTURES_H_
#define G8RTOS_STRUCTURES_H_

/************************************Includes***************************************/

#include <stdbool.h>
#include <stdint.h>

#include "G8RTOS_Structures.h"
#include "G8RTOS_Semaphores.h"

/************************************Includes***************************************/

/*************************************Defines***************************************/

#define MAX_NAME_LENGTH 16

/*************************************Defines***************************************/

/******************************Data Type Definitions********************************/

// Thread ID
typedef int32_t threadID_t;

/******************************Data Type Definitions********************************/

/****************************Data Structure Definitions*****************************/

/*
 *  Thread Control Block:
 *      - Every thread has a Thread Control Block
 *      - The Thread Control Block holds information about the Thread Such as
 *        the Stack Pointer, Priority Level, and Blocked Status
 *      - For Lab 3 the TCB will only hold the Stack Pointer, next TCB and the
 *        previous TCB (for Round Robin Scheduling)
 *  Create thread control block structure here
 *      - pay close attention to the order of variables!
 * */
typedef struct tcb_t
{
    // Add three pointers: stackPointer, nextTCB, and previousTCB
    uint32_t *stackPointer;
    struct tcb_t *nextTCB;
    struct tcb_t *prevTCB;
} tcb_t;

/****************************Data Structure Definitions*****************************/

/********************************Public Variables***********************************/
/********************************Public Variables***********************************/

/********************************Public Functions***********************************/
/********************************Public Functions***********************************/

/*******************************Private Variables***********************************/
/*******************************Private Variables***********************************/

/*******************************Private Functions***********************************/
/*******************************Private Functions***********************************/

#endif /* G8RTOS_STRUCTURES_H_ */
