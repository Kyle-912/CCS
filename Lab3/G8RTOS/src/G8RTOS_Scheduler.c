// G8RTOS_Scheduler.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for scheduler functions

#include "../G8RTOS_Scheduler.h"

/************************************Includes***************************************/

#include <stdint.h>

#include "../G8RTOS_CriticalSection.h"

#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

/************************************Includes***************************************/

/********************************Private Variables**********************************/

// Thread Control Blocks - array to hold information for each thread
static tcb_t threadControlBlocks[MAX_THREADS];

// Thread Stacks - array of arrays for individual stacks of each thread
static uint32_t threadStacks[MAX_THREADS][STACKSIZE];

// Current Number of Threads currently in the scheduler
static uint32_t NumberOfThreads;

/********************************Private Variables**********************************/

/*******************************Private Functions***********************************/

// Occurs every 1 ms.
static void InitSysTick(void) // TODO: Test this
{
    // hint: use SysCtlClockGet() to get the clock speed without having to hardcode it!
    // Set systick period to overflow every 1 ms.
    SysTickPeriodSet(SysCtlClockGet() / 1000);

    // Set systick interrupt handler
    SysTickIntRegister(SysTick_Handler);

    // Set pendsv handler
    IntRegister(FAULT_PENDSV, PendSV_Handler);

    // Enable systick interrupt
    SysTickIntEnable();

    // Enable systick
    SysTickEnable();
}

/*******************************Private Functions***********************************/

/********************************Public Variables***********************************/

uint32_t SystemTime;

tcb_t *CurrentlyRunningThread;

/********************************Public Variables***********************************/

/********************************Public Functions***********************************/

// G8RTOS_Init
// Initializes the RTOS by initializing system time.
// Return: void
void G8RTOS_Init() // TODO:
{
    // Initialize system time to zero
    SystemTime = 0;

    // Set the number of threads to zero
    NumberOfThreads = 0;
}

// G8RTOS_Launch
// Launches the RTOS.
// Return: error codes, 0 if none
int32_t G8RTOS_Launch()
{
    // Initialize system tick

    // Set currently running thread to the first control block
    // Set interrupt priorities
    // Pendsv
    // Systick
    // Call G8RTOS_Start()

    return 0;
}

// G8RTOS_Scheduler
// Chooses next thread in the TCB. Round-robin scheduling.
// Return: void
void G8RTOS_Scheduler()
{
}

// G8RTOS_AddThread
// - Adds threads to G8RTOS Scheduler
// - Checks if there are still available threads to insert to scheduler
// - Initializes the thread control block for the provided thread
// - Initializes the stack for the provided thread to hold a "fake context"
// - Sets stack thread control block stack pointer to top of thread stack
// - Sets up the next and previous thread control block pointers in a round robin fashion
// Param void* "threadToAdd": pointer to thread function address
// Return: scheduler error code
sched_ErrCode_t G8RTOS_AddThread(void (*threadToAdd)(void))
{
    // If number of threads is greater than the maximum number of threads
    // return
    // else
    // if no threads
    // else
    /*
    Append the new thread to the end of the linked list
    * 1. Number of threads will refer to the newest thread to be added since the current index would be NumberOfThreads-1
    * 2. Set the next thread for the new thread to be the first in the list, so that round-robin will be maintained
    * 3. Set the current thread's nextTCB to be the new thread
    * 4. Set the first thread's previous thread to be the new thread, so that it goes in the right spot in the list
    * 5. Point the previousTCB of the new thread to the current thread so that it moves in the correct order
    */
    if (NumberOfThreads >= MAX_THREADS)
        return;

    // Get the next available thread control block
    tcb_t *newTCB = &threadControlBlocks[NumberOfThreads];

    // Initialize the stack for the new thread
    newTCB->stackPointer = &threadStacks[NumberOfThreads][STACKSIZE - 16];

    // Set up the thread context with initial register values
    newTCB->stackPointer[15] = (int32_t)threadToAdd; // PC = thread function address
    newTCB->stackPointer[14] = 0x01000000;           // xPSR with Thumb bit set

    // Add the TCB to the round-robin list
    if (NumberOfThreads == 0)
    {
        // First thread, point to itself
        newTCB->nextTCB = newTCB;
        newTCB->prevTCB = newTCB;
    }
    else
    {
        // Add to the end of the list
        tcb_t *lastTCB = CurrentlyRunningThread->prevTCB;
        lastTCB->nextTCB = newTCB;
        newTCB->prevTCB = lastTCB;
        newTCB->nextTCB = CurrentlyRunningThread;
        CurrentlyRunningThread->prevTCB = newTCB;
    }

    NumberOfThreads++;

    return 0;
}

// SysTick_Handler
// Increments system time, sets PendSV flag to start scheduler.
// Return: void
void SysTick_Handler()
{
}

/********************************Public Functions***********************************/
