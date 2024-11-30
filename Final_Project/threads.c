// G8RTOS_Threads.c
// Date Created: 2024-11-30
// Date Updated: 2024-11-30
// Defines for thread functions.

/************************************Includes***************************************/

#include "./threads.h"

#include "./MultimodDrivers/multimod.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "driverlib/timer.h"
#include "driverlib/adc.h"

#define MAX_NUM_SAMPLES (200)
#define SIGNAL_STEPS (2)

/*********************************Global Variables**********************************/

uint16_t dac_step = 0;
int16_t dac_signal[SIGNAL_STEPS] = {0x001, 0x000};
int16_t volume = 0xFFF;

uint8_t grid[8][8] = {0};                 // 8x8 grid for note placement
uint8_t highlight_x = 0, highlight_y = 0; // Highlighted box position
int16_t tempo = 120;                      // Initial tempo (BPM)
uint8_t playing = 0;                      // Playback state (0 = stopped, 1 = playing)
uint8_t playback_column = 0;
uint16_t cell_width = X_MAX / 8;
uint16_t cell_height = Y_MAX / 8;

/********************************Public Functions***********************************/

void InitializeGridDisplay()
{
    for (int y = 0; y <= 8; y++)
    {
        ST7789_DrawLine(0, y * cell_height, X_MAX, y * cell_height, ST7789_WHITE); // Horizontal lines
    }
    for (int x = 0; x <= 8; x++)
    {
        ST7789_DrawLine(x * cell_width, 0, x * cell_width, Y_MAX, ST7789_WHITE); // Vertical lines
    }
}

uint16_t GetRainbowColor(uint8_t row)
{
    uint16_t colors[8] = {ST7789_RED, ST7789_ORANGE, ST7789_YELLOW, ST7789_GREEN, ST7789_BLUE, ST7789_VIOLET, ST7789_PINK, ST7789_RED};
    return colors[row];
}

void PlayNoteAtRow(uint8_t row)
{
    uint16_t frequencies[8] = {130, 147, 165, 175, 196, 220, 247, 260};
    uint16_t period = SysCtlClockGet() / (frequencies[row] * 2);
    TimerLoadSet(TIMER1_BASE, TIMER_A, period - 1);
    TimerEnable(TIMER1_BASE, TIMER_A);
}

/*************************************Threads***************************************/

void Idle_Thread(void)
{
    while (1)
    {
    }
}

void Speaker_Thread(void)
{
    while (1)
    {
        if (playing)
        {
            for (int col = 0; col < 8; col++)
            {
                playback_column = col; // Update playback column
                for (int row = 0; row < 8; row++)
                {
                    if (grid[row][col] == 1)
                    {
                        PlayNoteAtRow(row);
                    }
                }
                sleep(60000 / (tempo * 8)); // Tempo-based delay
            }
        }
        else
        {
            sleep(10); // Sleep briefly when not playing
        }
    }
}

void Volume_Thread(void)
{
    // Initialize / declare any variables here
    int32_t joystick_data;

    while (1)
    {
        // Read joystick values
        joystick_data = G8RTOS_ReadFIFO(JOYSTICK_FIFO);
        int16_t x = (joystick_data >> 16) & 0xFFFF;
        int16_t y = joystick_data & 0xFFFF;

        // Adjust volume
        if (y > 50)
        {
            volume += 250;
        }
        else if (y < -50)
        {
            volume -= 250;
        }

        // Adjust tempo
        if (x > 50)
        {
            tempo = (tempo < 240) ? tempo + 1 : 240;
        }
        else if (x < -50)
        {
            tempo = (tempo > 40) ? tempo - 1 : 40;
        }

        // Limit volume to 0-4095 (12 bit range)
        if (volume < 0)
        {
            volume = 0;
        }
        if (volume > 4095)
        {
            volume = 4095;
        }

        if (tempo < 40)
        {
            tempo = 40;
        }
        if (tempo > 240)
        {
            tempo = 240;
        }

        sleep(10);
    }
}

void Display_Thread(void)
{
    // Initialize / declare any variables here
    static uint8_t prev_x = 0, prev_y = 0;

    while (1)
    {
        G8RTOS_WaitSemaphore(&sem_SPIA);

        // Reset previous highlight to white
        ST7789_DrawLine(prev_x * cell_width, prev_y * cell_height, (prev_x + 1) * cell_width, prev_y * cell_height, ST7789_WHITE);             // Top
        ST7789_DrawLine(prev_x * cell_width, prev_y * cell_height, prev_x * cell_width, (prev_y + 1) * cell_height, ST7789_WHITE);             // Left
        ST7789_DrawLine((prev_x + 1) * cell_width, prev_y * cell_height, (prev_x + 1) * cell_width, (prev_y + 1) * cell_height, ST7789_WHITE); // Right
        ST7789_DrawLine(prev_x * cell_width, (prev_y + 1) * cell_height, (prev_x + 1) * cell_width, (prev_y + 1) * cell_height, ST7789_WHITE); // Bottom

        // Set new highlight to yellow
        ST7789_DrawLine(highlight_x * cell_width, highlight_y * cell_height, (highlight_x + 1) * cell_width, highlight_y * cell_height, ST7789_YELLOW);             // Top
        ST7789_DrawLine(highlight_x * cell_width, highlight_y * cell_height, highlight_x * cell_width, (highlight_y + 1) * cell_height, ST7789_YELLOW);             // Left
        ST7789_DrawLine((highlight_x + 1) * cell_width, highlight_y * cell_height, (highlight_x + 1) * cell_width, (highlight_y + 1) * cell_height, ST7789_YELLOW); // Right
        ST7789_DrawLine(highlight_x * cell_width, (highlight_y + 1) * cell_height, (highlight_x + 1) * cell_width, (highlight_y + 1) * cell_height, ST7789_YELLOW); // Bottom

        // Update previous highlight position
        prev_x = highlight_x;
        prev_y = highlight_y;

        G8RTOS_SignalSemaphore(&sem_SPIA);
        sleep(10);
    }
}

void JoystickPress_Thread()
{
    while (1)
    {
        // Wait for a signal to read the joystick press
        G8RTOS_WaitSemaphore(&sem_Joystick_Debounce);

        // Sleep to debounce
        sleep(10);

        // Read the joystick switch status on the Multimod board.
        if (JOYSTICK_GetPress())
        {
            playing = !playing; // Toggle the playing flag.
        }

        // Clear the interrupt
        GPIOIntClear(GPIO_PORTD_BASE, JOYSTICK_INT_PIN);

        // Re-enable the interrupt so it can occur again.
        GPIOIntEnable(GPIO_PORTD_BASE, JOYSTICK_INT_PIN);
    }
}
void Navigation_Thread(void)
{
    while (1)
    {
        // Wait for the semaphore
        G8RTOS_WaitSemaphore(&sem_PCA9555_Debounce);

        // Read buttons
        uint8_t buttons = -MultimodButtons_Get();

        // Move highlight box
        if (buttons & SW1)
        {
            highlight_y = (highlight_y > 0) ? highlight_y - 1 : 7; // Up
        }
        if (buttons & SW2)
        {
            highlight_y = (highlight_y < 7) ? highlight_y + 1 : 0; // Down
        }
        if (buttons & SW3)
        {
            highlight_x = (highlight_x > 0) ? highlight_x - 1 : 7; // Left
        }
        if (buttons & SW4)
        {
            highlight_x = (highlight_x < 7) ? highlight_x + 1 : 0; // Right
        }

        // Clear button interrupt
        GPIOIntClear(GPIO_PORTE_BASE, BUTTONS_INT_PIN);

        // Re-enable the interrupt so it can occur again.
        GPIOIntEnable(GPIO_PORTE_BASE, BUTTONS_INT_PIN);
    }
}

void NotePlacement_Thread(void)
{
    while (1)
    {
        // Wait for the semaphore
        G8RTOS_WaitSemaphore(&sem_Tiva_Button);

        // Toggle the note in the grid
        grid[highlight_y][highlight_x] ^= 1;
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

void Button_Handler()
{
    // Disable interrupt and signal semaphore
    GPIOIntDisable(GPIO_PORTE_BASE, BUTTONS_INT_PIN);
    G8RTOS_SignalSemaphore(&sem_PCA9555_Debounce);
}

void Joystick_Button_Handler()
{
    // Disable interrupt and signal semaphore
    GPIOIntDisable(GPIO_PORTD_BASE, JOYSTICK_INT_PIN);
    G8RTOS_SignalSemaphore(&sem_Joystick_Debounce);
}

void TivaButton_Handler(void)
{
    // Disable further interrupts to debounce
    GPIOIntDisable(GPIO_PORTF_BASE, GPIO_PIN_4);
    G8RTOS_SignalSemaphore(&sem_Tiva_Button);
    // Clear the interrupt
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);

    // Re-enable the interrupt
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_4);
}

void DAC_Timer_Handler()
{
    // Clear the timer interrupt
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    // Read next output sample
    uint32_t output = (volume) * (dac_signal[dac_step++ % SIGNAL_STEPS]);

    // Write the output value to the dac
    MutimodDAC_Write(DAC_OUT_REG, output);
}
