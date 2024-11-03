// Lab 5, uP2 Fall 2024
// Created: 2023-07-31
// Updated: 2024-08-01
// Lab 5 is intended to introduce you to basic signal processing. In this, you will
// - continuously sample audio data from a microphone
// - process the audio data stream using the Goertzel algorithm
// - output audio data to headphones via a DAC
// - provide user feedback via the display

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

    // initialize the G8RTOS framework
    G8RTOS_Init();
    multimod_init();

    // Add semaphores, threads, FIFOs here
    G8RTOS_AddThread(&Idle_Thread, 255, "IdleThread");
    G8RTOS_AddThread(&Mic_Thread, 0, "MicThread");
    G8RTOS_AddThread(&Display_Thread, 1, "DisplayThread");

    // add periodic and aperiodic events here (check multimod_mic.h and multimod_buttons.h for defines)

    G8RTOS_Launch();
    while (1)
    {
    }
}

/************************************MAIN*******************************************/
