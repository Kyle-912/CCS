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

    tcb_t *pt = CurrentlyRunningThread;
    ptcb_t *Ppt = &pthreadControlBlocks[0];

    // Traverse the linked-list to find which threads should be awake.
    do
    {
        if (pt->asleep)
        {
            if (pt->sleepCount == SystemTime)
            {
                pt->asleep = false;
            }
        }

        pt = pt->nextTCB;
    } while (pt != CurrentlyRunningThread);

    // TODO: Traverse the periodic linked list to run which functions need to be run.

    HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PEND_SV;
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
    IntPrioritySet(FAULT_PENDSV, 0xFF);  // Pendsv
    IntPrioritySet(FAULT_SYSTICK, 0xFF); // Systick

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

    tcb_t *highestPriorityThread = 0; // Pointer to hold the highest priority thread
    uint16_t highestPriority = 256;   // Worse than lowest possible priority (255)

    // Traverse the entire list of TCBs to find the highest priority thread that is ready to run
    do
    {
        // Check if the thread is eligible to run
        if (pt->alive && !pt->blocked && !pt->asleep)
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

    // If no eligible thread is found, fallback to the currently running thread
    if (highestPriorityThread == 0)
    {
        highestPriorityThread = CurrentlyRunningThread; // Shouldn't execute unless there is no idle thread
    }

    // Set the currently running thread to the highest priority eligible thread found
    CurrentlyRunningThread = highestPriorityThread;
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
    IBit_State = StartCriticalSection();

    if (NumberOfThreads >= MAX_THREADS)
    {
        EndCriticalSection(IBit_State);
        return THREAD_LIMIT_REACHED;
    }

    // Get the next available thread control block
    tcb_t *newTCB = &threadControlBlocks[NumberOfThreads];

    // Initialize the stack for the new thread
    newTCB->stackPointer = &threadStacks[NumberOfThreads][STACKSIZE - 16];

    // Set up the thread context with initial register values
    newTCB->stackPointer[15] = THUMBBIT;              // xPSR
    newTCB->stackPointer[14] = (uint32_t)threadToAdd; // PC
    newTCB->stackPointer[13] = 0x00000000;            // LR
    newTCB->stackPointer[12] = 0x12121212;            // R12
    newTCB->stackPointer[11] = 0x03030303;            // R3
    newTCB->stackPointer[10] = 0x02020202;            // R2
    newTCB->stackPointer[9] = 0x01010101;             // R1
    newTCB->stackPointer[8] = 0x00000000;             // R0
    newTCB->stackPointer[7] = 0x11111111;             // R11
    newTCB->stackPointer[6] = 0x10101010;             // R10
    newTCB->stackPointer[5] = 0x09090909;             // R9
    newTCB->stackPointer[4] = 0x08080808;             // R8
    newTCB->stackPointer[3] = 0x07070707;             // R7
    newTCB->stackPointer[2] = 0x06060606;             // R6
    newTCB->stackPointer[1] = 0x05050505;             // R5
    newTCB->stackPointer[0] = 0x04040404;             // R4

    // Initialize thread properties
    newTCB->priority = priority;
    newTCB->alive = true;
    newTCB->asleep = false;
    newTCB->blocked = 0;
    newTCB->sleepCount = 0;
    newTCB->threadID = threadCounter++;

    // Copy the thread name
    int i = 0;
    for (; i < MAX_NAME_LENGTH - 1 && name[i] != '\0'; i++)
    {
        newTCB->threadName[i] = name[i];
    }
    // Fill the rest of threadName with 0
    for (; i < MAX_NAME_LENGTH; i++)
    {
        newTCB->threadName[i] = 0;
    }

    // Add the TCB to the round-robin linked list
    if (NumberOfThreads == 0)
    {
        // First thread, point to itself
        newTCB->nextTCB = newTCB;
        newTCB->prevTCB = newTCB;
    }
    else
    {
        // Add to the end of the list
        tcb_t *lastTCB = &threadControlBlocks[NumberOfThreads - 1];
        tcb_t *firstTCB = &threadControlBlocks[0];

        lastTCB->nextTCB = newTCB;  // Point last thread's next to new thread
        newTCB->prevTCB = lastTCB;  // New thread's prev points to the last thread
        newTCB->nextTCB = firstTCB; // New thread's next points to the first thread
        firstTCB->prevTCB = newTCB; // First thread's prev points to the new thread
    }

    NumberOfThreads++;

    EndCriticalSection(IBit_State);

    return NO_ERROR;
}

// G8RTOS_Add_APeriodicEvent
// Param void* "AthreadToAdd": pointer to thread function address
// Param int32_t "IRQn": Interrupt request number that references the vector table. [0..155].
// Return: sched_ErrCode_t
sched_ErrCode_t G8RTOS_Add_APeriodicEvent(void (*AthreadToAdd)(void), uint8_t priority, int32_t IRQn)
{
    // Disable interrupts
    IBit_State = StartCriticalSection();

    // Check if IRQn is valid
    if (IRQn < 0 || IRQn >= 155)
    {
        EndCriticalSection(IBit_State);
        return IRQn_INVALID; // Invalid IRQ number
    }

    // Check if priority is valid
    if (priority > 6)
    {
        EndCriticalSection(IBit_State);
        return HWI_PRIORITY_INVALID;
    }
    // Set corresponding index in interrupt vector table to handler.
    uint32_t newVTORTable = 0x20000000;
    uint32_t *newTable = (uint32_t *)newVTORTable;
    uint32_t *oldTable = (uint32_t *)0;
    for (int i = 0; i < 155; i++)
    {
        newTable[i] = oldTable[i];
    }
    newTable[IRQn + 16] = (uint32_t)AthreadToAdd;
    HWREG(NVIC_VTABLE) = newVTORTable;

    // Set priority.
    IntPrioritySet(IRQn, priority);

    // Enable the interrupt.
    NVIC_EnableIRQ(IRQn);

    // End the critical section.
    EndCriticalSection(IBit_State);
    return NO_ERROR;
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
    IBit_State = StartCriticalSection();

    // Check if there is only one thread, return if so
    if (NumberOfThreads == 1)
    {
        EndCriticalSection(IBit_State);
        return CANNOT_KILL_LAST_THREAD;
    }

    // Traverse linked list, find thread to kill
    tcb_t *pt = CurrentlyRunningThread;
    do
    {
        if (pt->threadID == threadID)
        {
            // Update the next tcb and prev tcb pointers if found
            pt->prevTCB->nextTCB = pt->nextTCB;
            pt->nextTCB->prevTCB = pt->prevTCB;

            // Mark thread as not alive, release the semaphore it is blocked on
            pt->alive = false;
            if (pt->blocked != 0)
            {
                G8RTOS_SignalSemaphore(pt->blocked); // Release the semaphore the thread is blocked on
                pt->blocked = 0;                     // Clear the blocked semaphore
            }

            NumberOfThreads--;

            EndCriticalSection(IBit_State);
            return NO_ERROR;
        }

        pt = pt->nextTCB;
    } while (pt != CurrentlyRunningThread);

    // Otherwise, thread does not exist.
    EndCriticalSection(IBit_State);
    return THREAD_DOES_NOT_EXIST;
}

// G8RTOS_KillSelf
// Kills currently running thread.
// Return: sched_ErrCode_t
sched_ErrCode_t G8RTOS_KillSelf()
{
    IBit_State = StartCriticalSection();

    // Check if there is only one thread
    if (NumberOfThreads == 1)
    {
        EndCriticalSection(IBit_State);
        return CANNOT_KILL_LAST_THREAD;
    }
    else // Else, mark this thread as not alive.
    {
        CurrentlyRunningThread->alive = false;

        CurrentlyRunningThread->prevTCB->nextTCB = CurrentlyRunningThread->nextTCB;
        CurrentlyRunningThread->nextTCB->prevTCB = CurrentlyRunningThread->prevTCB;

        NumberOfThreads--;
    }

    EndCriticalSection(IBit_State);

    

    return NO_ERROR;
}

// sleep
// Puts current thread to sleep
// Param uint32_t "durationMS": how many systicks to sleep for
void sleep(uint32_t durationMS)
{
    // Update time to sleep to
    CurrentlyRunningThread->sleepCount = durationMS + SystemTime;
    // Set thread as asleep
    CurrentlyRunningThread->asleep = true;
    HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PEND_SV; // Yield
}

// G8RTOS_GetThreadID
// Gets current thread ID.
// Return: threadID_t
threadID_t G8RTOS_GetThreadID(void)
{
    return CurrentlyRunningThread->threadID; // Returns the thread ID
}

// G8RTOS_GetNumberOfThreads
// Gets number of threads.
// Return: uint32_t
uint32_t G8RTOS_GetNumberOfThreads(void)
{
    return NumberOfThreads; // Returns the number of threads
}
