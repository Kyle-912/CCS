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

    // you might want a delay here (~10 ms) to make sure the display has powered up

    // initialize the G8RTOS framework

    // Add semaphores, threads, FIFOs here

    // add periodic and aperiodic events here (check multimod_mic.h and multimod_buttons.h for defines)


    while (1);
}

/************************************MAIN*******************************************/
