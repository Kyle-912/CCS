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

    ST7789_WriteCommand(ST7789_SWRESET_ADDR);  // Software reset
    delay_ms(150);  // Wait for reset to complete
    ST7789_WriteCommand(ST7789_SLPOUT_ADDR);   // Sleep out
    delay_ms(500);  // Wait for sleep out to complete
    ST7789_WriteCommand(ST7789_DISPON_ADDR);   // Display on

    ST7789_SetWindow(0, 0, X_MAX, Y_MAX); // Full-screen window

    // Write a single color (Red) across the screen
    for (int i = 0; i < X_MAX * Y_MAX; i++)
    {
        ST7789_WriteData(0xF8); // Red (High byte)
        ST7789_WriteData(0x00); // Red (Low byte)
    }
}

/************************************MAIN*******************************************/
int main(void)
{
    // Sets clock speed to 80 MHz. You'll need it!
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    G8RTOS_Init();
    multimod_init();

    // Test_Display();

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
