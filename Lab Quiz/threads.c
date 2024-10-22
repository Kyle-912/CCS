// G8RTOS_Threads.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for thread functions.

/************************************Includes***************************************/

#include "./threads.h"

#include "./MultimodDrivers/multimod.h"
#include "./MiscFunctions/Shapes/inc/cube.h"
#include "./MiscFunctions/LinAlg/inc/linalg.h"
#include "./MiscFunctions/LinAlg/inc/quaternions.h"
#include "./MiscFunctions/LinAlg/inc/vect3d.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*********************************Global Variables**********************************/
// toggle on joystick press
uint8_t joystick_toggle = 1;

/*********************************Global Variables**********************************/

/*************************************Threads***************************************/

void Idle_Thread(void)
{
    time_t t;
    srand((unsigned)time(&t));
    while (1)
    {
    }
}

void Snake(void)
{
    ST7789_DrawRectangle(120, 160, 10, 10, 0xFFFF);
}

void Apple(void)
{
}

void Board(void)
{
}

void Read_Buttons()
{
    // Initialize / declare any variables here
    uint8_t button_state;
    int8_t x, y, z;

    while (1)
    {
        // Wait for a signal to read the buttons on the Multimod board.
        G8RTOS_WaitSemaphore(&sem_PCA9555_Debounce);

        // Sleep to debounce
        sleep(10);

        // Read the buttons status on the Multimod board.
        button_state = MultimodButtons_Get();

        // Process the buttons and determine what actions need to be performed.
        if (button_state & SW1)
        {
            {
            }
        }

        if (button_state & SW2) // SW2 Pressed
        {
        }

        // Clear the interrupt
        GPIOIntClear(GPIO_PORTE_BASE, BUTTONS_INT_PIN);

        // Re-enable the interrupt so it can occur again.
        GPIOIntEnable(GPIO_PORTE_BASE, BUTTONS_INT_PIN);
    }
}

void Read_JoystickPress()
{
    // Initialize / declare any variables here

    while (1)
    {
        // Wait for a signal to read the joystick press
        G8RTOS_WaitSemaphore(&sem_Joystick_Debounce);

        // Sleep to debounce
        sleep(10);

        // Read the joystick switch status on the Multimod board.
        if (JOYSTICK_GetPress())
        {
            joystick_toggle = !joystick_toggle; // Toggle the joystick_toggle flag.
        }

        // Clear the interrupt
        GPIOIntClear(GPIO_PORTD_BASE, JOYSTICK_INT_PIN);

        // Re-enable the interrupt so it can occur again.
        GPIOIntEnable(GPIO_PORTD_BASE, JOYSTICK_INT_PIN);
    }
}

/********************************Periodic Threads***********************************/

void Get_Joystick(void)
{
    // Read the joystick
    uint16_t x_raw = JOYSTICK_GetX();
    uint16_t y_raw = JOYSTICK_GetY();

    int16_t x = (int16_t)(x_raw - 2048); // Center around 0
    int16_t y = (int16_t)(y_raw - 2048); // Center around 0

    // Send through FIFO.
    G8RTOS_WriteFIFO(JOYSTICK_FIFO, ((uint32_t)x << 16) | (uint32_t)y);
}

/*******************************Aperiodic Threads***********************************/

void GPIOE_Handler()
{
    // Disable interrupt
    GPIOIntDisable(GPIO_PORTE_BASE, BUTTONS_INT_PIN);

    // Signal relevant semaphore
    G8RTOS_SignalSemaphore(&sem_PCA9555_Debounce);
}

void GPIOD_Handler()
{
    // Disable interrupt
    GPIOIntDisable(GPIO_PORTD_BASE, JOYSTICK_INT_PIN);

    // Signal relevant semaphore
    G8RTOS_SignalSemaphore(&sem_Joystick_Debounce);
}
