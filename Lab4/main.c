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
    // Top 1/6 of the screen: Red (ST7789_RED = 0x001F)
    for (uint16_t y = 0; y < Y_MAX / 6; y++)
    {
        for (uint16_t x = 0; x < X_MAX; x++)
        {
            ST7789_DrawPixel(x, y, ST7789_RED); // Red (0x001F in BGR565)
        }
    }

    // Second 1/6 of the screen: Green (ST7789_GREEN = 0x07E0)
    for (uint16_t y = Y_MAX / 6; y < 2 * Y_MAX / 6; y++)
    {
        for (uint16_t x = 0; x < X_MAX; x++)
        {
            ST7789_DrawPixel(x, y, ST7789_GREEN); // Green (0x07E0 in BGR565)
        }
    }

    // Third 1/6 of the screen: Blue (ST7789_BLUE = 0xF800)
    for (uint16_t y = 2 * Y_MAX / 6; y < 3 * Y_MAX / 6; y++)
    {
        for (uint16_t x = 0; x < X_MAX; x++)
        {
            ST7789_DrawPixel(x, y, ST7789_BLUE); // Blue (0xF800 in BGR565)
        }
    }

    // Fourth 1/6 of the screen: Yellow (Red + Green = 0x07FF)
    for (uint16_t y = 3 * Y_MAX / 6; y < 4 * Y_MAX / 6; y++)
    {
        for (uint16_t x = 0; x < X_MAX; x++)
        {
            ST7789_DrawPixel(x, y, 0x07FF); // Yellow (Red + Green)
        }
    }

    // Fifth 1/6 of the screen: Cyan (Green + Blue = 0xFFE0)
    for (uint16_t y = 4 * Y_MAX / 6; y < 5 * Y_MAX / 6; y++)
    {
        for (uint16_t x = 0; x < X_MAX; x++)
        {
            ST7789_DrawPixel(x, y, 0xFFE0); // Cyan (Green + Blue)
        }
    }

    // Sixth 1/6 of the screen: Magenta (Red + Blue = 0xF81F)
    for (uint16_t y = 5 * Y_MAX / 6; y < Y_MAX; y++)
    {
        for (uint16_t x = 0; x < X_MAX; x++)
        {
            ST7789_DrawPixel(x, y, 0xF81F); // Magenta (Red + Blue)
        }
    }
}

/************************************MAIN*******************************************/
int main(void)
{
    // Sets clock speed to 0x07E0 MHz. You'll need it!
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    G8RTOS_Init();
    multimod_init();

    //  Test_Display();

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
