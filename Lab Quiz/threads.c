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
uint8_t numApples = 0;
uint8_t snakeLength = 2;
uint16_t applePos[2] = {0, 0};
uint16_t headPos[2] = {0, 0};
uint8_t direction = 0;
bool gameOver = false;

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

    int32_t joystick_data;
    float norm_x, norm_y;

    while (1)
    {
        // Get result from joystick
        joystick_data = G8RTOS_ReadFIFO(JOYSTICK_FIFO);
        int16_t x = (joystick_data >> 16) & 0xFFFF;
        int16_t y = joystick_data & 0xFFFF;

        x = -x;
        y = -y;

        // If joystick axis within deadzone, set to 0. Otherwise normalize it.
        if (abs(x) < 50)
        {
            x = 0;
        }
        if (abs(y) < 50)
        {
            y = 0;
        }

        norm_x = (x != 0) ? (float)x / 2048.0f : 0.0f;
        norm_y = (y != 0) ? (float)y / 2048.0f : 0.0f;

        if (norm_x > norm_y || -norm_x > -norm_y)
        {
            if (norm_x > 1)
            {
                direction = 0;
            }
            else
            {
                direction = 1;
            }
        }
        else
        {
            if (norm_y > 1)
            {
                direction = 2;
            }
            else
            {
                direction = 3;
            }
        }
    }
}

void Apple(void)
{
    uint16_t x = 0;
    uint16_t y = 0;

    while (1)
    {
        if (numApples == 0)
        {
            x = (rand() % X_MAX);
            y = (rand() % Y_MAX);

            applePos[0] = x;
            applePos[1] = y;

            G8RTOS_WaitSemaphore(&sem_SPIA);
            ST7789_DrawRectangle(x, y, 10, 10, ST7789_RED);
            G8RTOS_SignalSemaphore(&sem_SPIA);

            numApples++;

            // sleep(100);
        }

        if (headPos[0] == applePos[0] && headPos[1] == applePos[1])
        {
            G8RTOS_WaitSemaphore(&sem_SPIA);
            ST7789_DrawRectangle(applePos[0], applePos[1], 10, 10, ST7789_BLACK);
            G8RTOS_SignalSemaphore(&sem_SPIA);

            snakeLength++;
        }
    }
}

void Board(void)
{
    G8RTOS_WaitSemaphore(&sem_SPIA);
    ST7789_DrawRectangle(120, 160, 10, 10, 0xFFFF);
    G8RTOS_SignalSemaphore(&sem_SPIA);

    while (1)
    {
        if (gameOver == true)
        {
            for (uint16_t y = 0; y < Y_MAX; y++)
            {
                for (uint16_t x = 0; x < X_MAX; x++)
                {
                    ST7789_DrawPixel(x, y, ST7789_RED);
                }
            }

            G8RTOS_WaitSemaphore(&sem_I2CA);

            UARTprintf("Game Over!");

            IBit_State = StartCriticalSection();

            while (1)
            {
            }
        }
    }
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
