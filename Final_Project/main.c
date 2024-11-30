// Final Lab, uP2 Fall 2024
// Created: 2024-11-30
// Updated: 2024-11-30

/************************************Includes***************************************/

#include "G8RTOS/G8RTOS.h"
#include "./MultimodDrivers/multimod.h"

#include "./threads.h"
#include "driverlib/interrupt.h"

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

/************************************Includes***************************************/

/*************************************Defines***************************************/
/*************************************Defines***************************************/

/********************************Public Variables***********************************/
/********************************Public Variables***********************************/

/************************************MAIN*******************************************/

int main(void)
{
    // Sets clock speed to 80 MHz. You'll need it!
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Initialize the G8RTOS framework
    G8RTOS_Init();
    multimod_init();

    Test_Display();

    InitializeGridDisplay();

    // Add semaphores, threads, FIFOs
    G8RTOS_InitSemaphore(&sem_SPIA, 1);
    G8RTOS_InitSemaphore(&sem_PCA9555_Debounce, 0);
    G8RTOS_InitSemaphore(&sem_Joystick_Debounce, 0);
    G8RTOS_InitSemaphore(&sem_Tiva_Button, 0);

    G8RTOS_AddThread(&Idle_Thread, 255, "IdleThread");
    G8RTOS_AddThread(&Speaker_Thread, 1, "Speaker");
    G8RTOS_AddThread(&Volume_Thread, 0, "Volume");
    G8RTOS_AddThread(&Display_Thread, 1, "Display");
    G8RTOS_AddThread(&JoystickPress_Thread, 1, "JoystickPress");
    G8RTOS_AddThread(&Navigation_Thread, 1, "Navigation");
    // G8RTOS_AddThread(&NotePlacement_Thread, 1, "NotePlacement");

    G8RTOS_InitFIFO(JOYSTICK_FIFO);

    // Add periodic and aperiodic events
    // G8RTOS_Add_PeriodicEvent(&Get_Joystick, 1000, 0);

    G8RTOS_Add_APeriodicEvent(Button_Handler, 0, BUTTON_INTERRUPT);
    G8RTOS_Add_APeriodicEvent(Joystick_Button_Handler, 0, JOYSTICK_INTERRUPT);
    G8RTOS_Add_APeriodicEvent(TivaButton_Handler, 0, INT_GPIOF);
    G8RTOS_Add_APeriodicEvent(DAC_Timer_Handler, 0, DAC_INTERRUPT);

    G8RTOS_Launch();
    while (1)
    {
    }
}

/************************************MAIN*******************************************/
