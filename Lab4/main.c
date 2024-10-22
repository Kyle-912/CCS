// Lab 4, uP2 Fall 2023
// Created: 2023-07-31
// Updated: 2023-08-01
// Lab 4 is intended to introduce you to more advanced RTOS concepts. In this, you will
// - implement blocking, yielding, sleeping
// - Thread priorities, aperiodic & periodic threads
// - IPC using FIFOs
// - Dynamic thread creation & deletion

/************************************Includes***************************************/

#include "G8RTOS/G8RTOS.h"
#include "./MultimodDrivers/multimod.h"
#include <driverlib/uartstdio.h>

#include "./threads.h"

void Test_Display(void)
{
    // Loop through all rows (Y-axis)
    for (uint16_t y = 0; y < Y_MAX; y++)
    {
        // Loop through all columns (X-axis)
        for (uint16_t x = 0; x < X_MAX; x++)
        {
            // Draw a red pixel at the current position
            ST7789_DrawPixel(x, y, 0xF800); // Red color in RGB565 format
        }
    }
}

/************************************MAIN*******************************************/
int main(void)
{
    // Sets clock speed to 80 MHz. You'll need it!
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    G8RTOS_Init();
    multimod_init();

    Test_Display();

    // Add threads, semaphores, FIFOs here
    G8RTOS_AddThread(&Idle_Thread, 255, "IdleThread");
    G8RTOS_AddThread(&Cube_Thread, 1, "CubeThread");
    G8RTOS_AddThread(&CamMove_Thread, 1, "CamMoveThread");
    G8RTOS_AddThread(&Read_Buttons, 1, "ReadButtons");
    G8RTOS_AddThread(&Read_JoystickPress, 1, "JoystickPress");

    G8RTOS_Add_PeriodicEvent(&Print_WorldCoords, 100, 0);
    G8RTOS_Add_PeriodicEvent(&Get_Joystick, 100, 50);

    G8RTOS_Add_APeriodicEvent(GPIOE_Handler, 1, INT_GPIOE);
    G8RTOS_Add_APeriodicEvent(GPIOD_Handler, 1, INT_GPIOD);

    G8RTOS_InitSemaphore(&sem_I2CA, 1);
    G8RTOS_InitSemaphore(&sem_SPIA, 1);
    G8RTOS_InitSemaphore(&sem_PCA9555_Debounce, 0);
    G8RTOS_InitSemaphore(&sem_Joystick_Debounce, 0);
    G8RTOS_InitSemaphore(&sem_KillCube, 1);

    G8RTOS_InitFIFO(JOYSTICK_FIFO);
    G8RTOS_InitFIFO(SPAWNCOOR_FIFO);

    G8RTOS_Launch();
    while (1)
    {
    }
}

/************************************MAIN*******************************************/
