// G8RTOS_Threads.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for thread functions.

/************************************Includes***************************************/

#include "./threads.h"

#include "./MultimodDrivers/multimod.h"
#include <driverlib/uartstdio.h>

/************************************Includes***************************************/

/*************************************Defines***************************************/
// Assumes that the system clock is 16 MHz.
#define delay_0_1_s (1600000 / 3)
/*************************************Defines***************************************/

/********************************Public Functions***********************************/

// Thread0, reads accel_x data, adjusts BLUE led duty cycle.
void Thread0(void)
{
    while (1)
    {
        G8RTOS_WaitSemaphore(&sem_I2CA);

        int16_t accelX = BMI160_AccelXGetResult();

        float absAccelX = (float)(abs(accelX)) / 16384.0f; // Normalize based on ±1g
        if (absAccelX > 1.0f)
        {
            absAccelX = 1.0f; // Cap at 100%
        }

        LaunchpadLED_PWMSetDuty(BLUE, absAccelX); // Set duty cycle for BLUE

        G8RTOS_SignalSemaphore(&sem_I2CA);

        SysCtlDelay(delay_0_1_s);
    }
}

// Thread1, reads gyro_x data, adjusts RED led duty cycle.
void Thread1(void)
{
    while (1)
    {
        G8RTOS_WaitSemaphore(&sem_I2CA);

        int16_t gyroX = BMI160_GyroXGetResult();

        float absGyroX = (float)(abs(gyroX)) / 32768.0f; // Normalize based on max rate
        if (absGyroX > 1.0f)
        {
            absGyroX = 1.0f; // Cap at 100%
        }
        absGyroX *= 2.0f; // Make Brighter

        LaunchpadLED_PWMSetDuty(RED, absGyroX); // Set duty cycle for RED

        G8RTOS_SignalSemaphore(&sem_I2CA);

        SysCtlDelay(delay_0_1_s);
    }
}

// Thread2, reads optical sensor values, adjusts GREEN led duty cycle.
void Thread2(void)
{
    while (1)
    {
        G8RTOS_WaitSemaphore(&sem_I2CA);

        uint32_t lightIntensity = OPT3001_GetResult();

        float lightLux = (float)lightIntensity / 83865.6f; // Normalize to max lux
        if (lightLux > 1.0f)
        {
            lightLux = 1.0f; // Cap at 100%
        }

        LaunchpadLED_PWMSetDuty(GREEN, lightLux); // Set duty cycle for GREEN

        G8RTOS_SignalSemaphore(&sem_I2CA);

        SysCtlDelay(delay_0_1_s);
    }
}

// Thread3, reads and output button 1 status using polling
void Thread3(void)
{
    while (1)
    {
        G8RTOS_WaitSemaphore(&sem_UART);

        if (LaunchpadButtons_ReadSW1())
        {
            UARTprintf("Button 1 Pressed\n");
        }

        G8RTOS_SignalSemaphore(&sem_UART);

        SysCtlDelay(delay_0_1_s);
    }
}

// Thread4, reads and output button 2 status using polling
void Thread4(void)
{
    while (1)
    {
        G8RTOS_WaitSemaphore(&sem_UART);

        if (LaunchpadButtons_ReadSW2())
        {
            UARTprintf("Button 2 Pressed\n");
        }

        G8RTOS_SignalSemaphore(&sem_UART);

        SysCtlDelay(delay_0_1_s);
    }
}

/********************************Public Functions***********************************/
