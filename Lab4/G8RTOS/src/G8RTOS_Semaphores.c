// G8RTOS_Semaphores.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for semaphore functions

#include "../G8RTOS_Semaphores.h"

/************************************Includes***************************************/

#include "../G8RTOS_CriticalSection.h"
#include "../G8RTOS_Scheduler.h"

#include "inc/hw_types.h"
#include "inc/hw_nvic.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"

extern tcb_t *CurrentlyRunningThread;

/******************************Data Type Definitions********************************/

/****************************Data Structure Definitions*****************************/

/***********************************Externs*****************************************/

/********************************Public Variables***********************************/

/********************************Public Functions***********************************/
// G8RTOS_InitSemaphore
// Initializes semaphore to a value.
// Param "s": Pointer to semaphore
// Param "value": Value to initialize semaphore to
// Return: void
void G8RTOS_InitSemaphore(semaphore_t *s, int32_t value)
{
    IBit_State = StartCriticalSection();
    *s = value;
    EndCriticalSection(IBit_State);
}

// G8RTOS_WaitSemaphore
// Waits on the semaphore to become available, decrements value by 1.
// If the current resource is not available, block the current thread
// Param "s": Pointer to semaphore
// Return: void
void G8RTOS_WaitSemaphore(semaphore_t *s)
{
    IBit_State = StartCriticalSection();

    if (--(*s) < 0) // Decrement the semaphore; if < 0, thread must block
    {
        CurrentlyRunningThread->blocked = s; // Set the blocked pointer
        EndCriticalSection(IBit_State);
        HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PEND_SV; // FIXME: Yield
        while (CurrentlyRunningThread->blocked != 0)
        {
        }
    }
    else
    {
        // EndCriticalSection(IBit_State);
    }
}

// G8RTOS_SignalSemaphore
// Signals that the semaphore has been released by incrementing the value by 1.
// Unblocks the first thread currently blocked on the semaphore.
// Param "s": Pointer to semaphore
// Return: void
void G8RTOS_SignalSemaphore(semaphore_t *s)
{
    // IBit_State = StartCriticalSection();

    (*s)++; // Increment semaphore

    if (*s <= 0) // Only unblock a thread if semaphore was negative
    {
        tcb_t *pt = CurrentlyRunningThread->nextTCB;

        while (pt->blocked != s)
        {
            pt = pt->nextTCB;
        }

        pt->blocked = 0;
    }

    // FIXME: Move unblocked thread to be next thread executed?

    EndCriticalSection(IBit_State);
    HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PEND_SV; // FIXME: Yield
}
