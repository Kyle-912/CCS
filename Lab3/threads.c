// G8RTOS_Threads.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for thread functions.

/************************************Includes***************************************/

#include "./threads.h"

#include "./MultimodDrivers/multimod.h"

/************************************Includes***************************************/

/*************************************Defines***************************************/
// Assumes that the system clock is 16 MHz.
#define delay_0_1_s (1600000 / 3)
/*************************************Defines***************************************/

/********************************Public Functions***********************************/

// Thread0, reads accel_x data, adjusts BLUE led duty cycle.
void Thread0(void)
{
    // sem_I2CA
}

// Thread1, reads gyro_x data, adjusts RED led duty cycle.
void Thread1(void)
{
    // sem_I2CA
}

// Thread2, reads optical sensor values, adjusts GREEN led duty cycle.
void Thread2(void)
{
    // sem_I2CA
    G8RTOS_WaitSemaphore(&sem_UART);
    // Critical section: access UART
    G8RTOS_SignalSemaphore(&sem_UART);
}

// Thread3, reads and output button 1 status using polling
void Thread3(void)
{
    G8RTOS_WaitSemaphore(&sem_UART);
    // Critical section: access UART
    G8RTOS_SignalSemaphore(&sem_UART);
}

// Thread4, reads and output button 2 status using polling
void Thread4(void)
{
    G8RTOS_WaitSemaphore(&sem_UART);
    // Critical section: access UART
    G8RTOS_SignalSemaphore(&sem_UART);
}

/********************************Public Functions***********************************/
