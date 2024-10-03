// G8RTOS_Scheduler.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for scheduler functions

#include "../G8RTOS_Scheduler.h"

/************************************Includes***************************************/

#include <stdint.h>
#include <stdbool.h>

#include "../G8RTOS_CriticalSection.h"

#include <inc/hw_memmap.h>
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

/********************************Private Variables**********************************/

// Thread Control Blocks - array to hold information for each thread
static tcb_t threadControlBlocks[MAX_THREADS];

// Thread Stacks - array of arrays for individual stacks of each thread
static uint32_t threadStacks[MAX_THREADS][STACKSIZE];

// Periodic Event Threads - array to hold pertinent information for each thread
static ptcb_t pthreadControlBlocks[MAX_PTHREADS];

// Current Number of Threads currently in the scheduler
static uint32_t NumberOfThreads;

// Current Number of Periodic Threads currently in the scheduler
static uint32_t NumberOfPThreads;

static uint32_t threadCounter = 0;

/*******************************Private Functions***********************************/

// Occurs every 1 ms.
static void InitSysTick(void)
{
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

/********************************Public Variables***********************************/

uint32_t SystemTime;

tcb_t *CurrentlyRunningThread;

/********************************Public Functions***********************************/

// SysTick_Handler
// Increments system time, sets PendSV flag to start scheduler.
// Return: void
void SysTick_Handler()
{
    SystemTime++;

    // Traverse the linked-list to find which threads should be awake.
    // Traverse the periodic linked list to run which functions need to be run.
}

// G8RTOS_Init
// Initializes the RTOS by initializing system time.
// Return: void
void G8RTOS_Init()
{
    uint32_t newVTORTable = 0x20000000;
    uint32_t *newTable = (uint32_t *)newVTORTable;
    uint32_t *oldTable = (uint32_t *)0;

    for (int i = 0; i < 155; i++)
    {
        newTable[i] = oldTable[i];
    }

    HWREG(NVIC_VTABLE) = newVTORTable;

    SystemTime = 0;
    NumberOfThreads = 0;
    NumberOfPThreads = 0;
}

// G8RTOS_Launch
// Launches the RTOS.
// Return: error codes, 0 if none
int32_t G8RTOS_Launch()
{
    // Initialize system tick
    InitSysTick();

    // Set currently running thread to the first control block
    CurrentlyRunningThread = &threadControlBlocks[0];

    // Set interrupt priorities
    // Pendsv
    IntPrioritySet(FAULT_PENDSV, 0xFF);

    // Systick
    IntPrioritySet(FAULT_SYSTICK, 0xFF);

    // Call G8RTOS_Start()
    G8RTOS_Start();

    return 0;
}

// G8RTOS_Scheduler
// Chooses next thread in the TCB. This time uses priority scheduling.
// Return: void
void G8RTOS_Scheduler()
{
    // Using priority, determine the most eligible thread to run that
    // is not blocked or asleep. Set current thread to this thread's TCB.
    tcb_t *pt = CurrentlyRunningThread->nextTCB; // Start from the next thread
    tcb_t *highestPriorityThread = NULL;         // Pointer to hold the highest priority thread
    uint8_t highestPriority = 255;               // Lowest possible priority (255)

    // Traverse the entire list of TCBs to find the highest priority thread that is ready to run
    do
    {
        // Check if the thread is eligible to run (alive, not blocked, and not asleep)
        if (pt->alive && pt->blocked == NULL && pt->asleep == false)
        {
            // If the thread has a higher priority (lower number) than the current highest, update
            if (pt->priority < highestPriority)
            {
                highestPriority = pt->priority;
                highestPriorityThread = pt;
            }
        }

        pt = pt->nextTCB; // Move to the next thread in the linked list
    } while (pt != CurrentlyRunningThread); // Stop when we loop back to the starting thread

    // Set the currently running thread to the highest priority eligible thread found
    if (highestPriorityThread != NULL)
    {
        CurrentlyRunningThread = highestPriorityThread;
    }
}

// G8RTOS_AddThread
// Adds a thread. This is now in a critical section to support dynamic threads.
// It also now should initialize priority and account for live or dead threads.
// Param void* "threadToAdd": pointer to thread function address
// Param uint8_t "priority": priority from 0, 255.
// Param char* "name": character array containing the thread name.
// Return: sched_ErrCode_t
sched_ErrCode_t G8RTOS_AddThread(void (*threadToAdd)(void), uint8_t priority, char *name)
{
    // TODO: Your code here, modified from lab 3

    // This should be in a critical section!
}

// G8RTOS_Add_APeriodicEvent
// Param void* "AthreadToAdd": pointer to thread function address
// Param int32_t "IRQn": Interrupt request number that references the vector table. [0..155].
// Return: sched_ErrCode_t
sched_ErrCode_t G8RTOS_Add_APeriodicEvent(void (*AthreadToAdd)(void), uint8_t priority, int32_t IRQn)
{
    // Disable interrupts
    // Check if IRQn is valid
    // Check if priority is valid
    // Set corresponding index in interrupt vector table to handler.
    // Set priority.
    // Enable the interrupt.
    // End the critical section.
}

// G8RTOS_Add_PeriodicEvent
// Adds periodic threads to G8RTOS Scheduler
// Function will initialize a periodic event struct to represent event.
// The struct will be added to a linked list of periodic events
// Param void* "PThreadToAdd": void-void function for P thread handler
// Param uint32_t "period": period of P thread to add
// Param uint32_t "execution": When to execute the periodic thread
// Return: sched_ErrCode_t
sched_ErrCode_t G8RTOS_Add_PeriodicEvent(void (*PThreadToAdd)(void), uint32_t period, uint32_t execution)
{
    // your code
    // Make sure that the number of PThreads is not greater than max PThreads.
    // Check if there is no PThread. Initialize and set the first PThread.
    // Subsequent PThreads should be added, inserted similarly to a doubly-linked linked list
    // last PTCB should point to first, last PTCB should point to last.
    // Set function
    // Set period
    // Set execute time
    // Increment number of PThreads

    return NO_ERROR;
}

// G8RTOS_KillThread
// Param uint32_t "threadID": ID of thread to kill
// Return: sched_ErrCode_t
sched_ErrCode_t G8RTOS_KillThread(threadID_t threadID)
{
    // Start critical section
    // Check if there is only one thread, return if so
    // Traverse linked list, find thread to kill
    // Update the next tcb and prev tcb pointers if found
    // mark as not alive, release the semaphore it is blocked on
    // Otherwise, thread does not exist.
}

// G8RTOS_KillSelf
// Kills currently running thread.
// Return: sched_ErrCode_t
sched_ErrCode_t G8RTOS_KillSelf()
{
    // your code

    // Check if there is only one thread
    // Else, mark this thread as not alive.
}

// sleep
// Puts current thread to sleep
// Param uint32_t "durationMS": how many systicks to sleep for
void sleep(uint32_t durationMS)
{
    // Update time to sleep to
    // Set thread as asleep
}

// G8RTOS_GetThreadID
// Gets current thread ID.
// Return: threadID_t
threadID_t G8RTOS_GetThreadID(void)
{
    return CurrentlyRunningThread->ThreadID; // Returns the thread ID
}

// G8RTOS_GetNumberOfThreads
// Gets number of threads.
// Return: uint32_t
uint32_t G8RTOS_GetNumberOfThreads(void)
{
    return NumberOfThreads; // Returns the number of threads
}
