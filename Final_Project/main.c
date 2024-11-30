// Final Lab, uP2 Fall 2024
// Created: 2024-11-30
// Updated: 2024-11-30

/************************************Includes***************************************/

#include "G8RTOS/G8RTOS.h"
#include "./MultimodDrivers/multimod.h"

#include "./threads.h"
#include "driverlib/interrupt.h"

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

    // you might want a delay here (~10 ms) to make sure the display has powered up
    SysCtlDelay(10000);

    // Initialize the G8RTOS framework
    G8RTOS_Init();
    multimod_init();

    // Initialize grid
    InitializeGridDisplay();

    // Add semaphores, threads, FIFOs
    G8RTOS_InitSemaphore(&sem_SPIA, 1);
    G8RTOS_InitSemaphore(&sem_PCA9555_Debounce, 0);
    G8RTOS_InitSemaphore(&sem_Joystick_Debounce, 0);
    G8RTOS_InitSemaphore(&sem_Tiva_Button, 0);

    G8RTOS_AddThread(&Idle_Thread, 255, "IdleThread");
    G8RTOS_AddThread(&Speaker_Thread, 0, "Speaker");
    G8RTOS_AddThread(&Volume_Thread, 0, "Volume");
    G8RTOS_AddThread(&Display_Thread, 0, "Display");
    G8RTOS_AddThread(&JoystickPress_Thread, 0, "JoystickPress");
    G8RTOS_AddThread(&Navigation_Thread, 0, "Navigation");
    G8RTOS_AddThread(&NotePlacement_Thread, 0, "NotePlacement");

    G8RTOS_InitFIFO(JOYSTICK_FIFO);

    // Add periodic and aperiodic events
    G8RTOS_Add_PeriodicEvent(&Get_Joystick, DAC_SAMPLE_FREQUENCY_HZ, 0);

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
