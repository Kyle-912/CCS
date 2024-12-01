// G8RTOS_Threads.c
// Date Created: 2024-11-30
// Date Updated: 2024-12-01
// Defines for thread functions.

/************************************Includes***************************************/

#include "./threads.h"

#include "./MultimodDrivers/multimod.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "driverlib/timer.h"
#include "driverlib/adc.h"

#define MAX_NUM_SAMPLES (200)
#define SIGNAL_STEPS (2)

/*********************************Global Variables**********************************/

uint16_t dac_step = 0;
int16_t dac_signal[SIGNAL_STEPS] = {0x001, 0x000};
int16_t volume = 0xFFF;
int16_t tempo = 120;
uint8_t grid[8][8] = {0};
uint8_t highlight_x = 0, highlight_y = 0;
uint8_t playing = 0;
uint16_t cell_width = X_MAX / 8;
uint16_t cell_height = Y_MAX / 8;
uint16_t colors[8] = {ST7789_RED, ST7789_ORANGE, ST7789_YELLOW, ST7789_GREEN, ST7789_BLUE, ST7789_VIOLET, ST7789_PINK, ST7789_RED};

float phases[8] = {0};     // Phase accumulators for active frequencies
float phase_increments[8]; // Phase increments based on frequencies

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

    // Extra horizontal line one pixel in from the top edge
    ST7789_DrawLine(0, Y_MAX - 1, X_MAX, Y_MAX - 1, ST7789_WHITE);

    // Extra vertical line one pixel in from the right edge
    ST7789_DrawLine(X_MAX - 1, 0, X_MAX - 1, Y_MAX, ST7789_WHITE);
}

void PlayNoteAtRow(uint8_t row)
{
    uint16_t frequencies[8] = {130, 147, 165, 175, 196, 220, 247, 260};
    uint32_t period = SysCtlClockGet() / (frequencies[row] * 2);
    TimerDisable(TIMER1_BASE, TIMER_A);
    TimerLoadSet(TIMER1_BASE, TIMER_A, period - 1);
    TimerEnable(TIMER1_BASE, TIMER_A);
}

uint16_t GenerateCombinedWaveform(uint8_t column)
{
    float sample = 0.0f;
    uint16_t frequencies[8] = {130, 147, 165, 175, 196, 220, 247, 260};
    uint8_t active_notes = 0;

    G8RTOS_WaitSemaphore(&sem_Phases); // Protect shared `phases` and `phase_increments`

    // Combine active notes in the column
    for (int row = 0; row < 8; row++)
    {
        if (grid[column][row] == 1) // If note is active
        {
            phase_increments[row] = (float)frequencies[row] / DAC_SAMPLE_FREQUENCY_HZ;
            phases[row] += phase_increments[row];

            if (phases[row] >= 1.0f)
                phases[row] -= 1.0f;

            sample += sinf(2.0f * 3.14159265358979323846 * phases[row]); // Add sine wave
            active_notes++;
        }
    }

    G8RTOS_SignalSemaphore(&sem_Phases);

    if (active_notes == 0)
    {
        // Silence if no active notes
        return 0;
    }

    // Scale and shift to fit the DAC range
    sample = (sample / active_notes) * (volume / 2.0f) + (volume / 2.0f);
    if (sample < 0)
        sample = 0;
    if (sample > 0xFFF)
        sample = 0xFFF;
    return (uint16_t)sample;
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
    int prev_col = -1;

    while (1)
    {
        if (playing)
        {
            for (int col = 0; col < 8; col++)
            {
                G8RTOS_WaitSemaphore(&sem_SPIA);

                // Highlight the current column with red
                ST7789_DrawLine((col * cell_width) + 1, 0, (col * cell_width) + 1, Y_MAX - 1, ST7789_RED);                     // Left vertical line
                ST7789_DrawLine(((col + 1) * cell_width - 1) + 1, 0, ((col + 1) * cell_width - 1) + 1, Y_MAX - 1, ST7789_RED); // Right vertical line

                // Clear the previous column highlight
                if (prev_col != -1 && prev_col != col)
                {
                    ST7789_DrawLine((prev_col * cell_width) + 1, 0, (prev_col * cell_width) + 1, Y_MAX - 1, ST7789_WHITE);                     // Left vertical line
                    ST7789_DrawLine(((prev_col + 1) * cell_width - 1) + 1, 0, ((prev_col + 1) * cell_width - 1) + 1, Y_MAX - 1, ST7789_WHITE); // Right vertical line
                }

                G8RTOS_SignalSemaphore(&sem_SPIA);
                prev_col = col;

                // Generate combined waveform and write to DAC
                int num_samples = DAC_SAMPLE_FREQUENCY_HZ / tempo; // Samples for the duration of the column
                for (int i = 0; i < num_samples; i++)
                {
                    uint16_t sample = GenerateCombinedWaveform(col); // Generate the waveform
                    MutimodDAC_Write(DAC_OUT_REG, sample);           // Write to DAC
                    sleep(5);                                        // Sleep for 5 ms (200 Hz sampling rate)
                }
            }

            // Clear final column highlight
            if (prev_col != -1)
            {
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawLine((prev_col * cell_width) + 1, 0, (prev_col * cell_width) + 1, Y_MAX - 1, ST7789_WHITE);                     // Left vertical line
                ST7789_DrawLine(((prev_col + 1) * cell_width - 1) + 1, 0, ((prev_col + 1) * cell_width - 1) + 1, Y_MAX - 1, ST7789_WHITE); // Right vertical line
                G8RTOS_SignalSemaphore(&sem_SPIA);
                prev_col = -1;
            }
        }
        else
        {
            // Silence the DAC when not playing
            MutimodDAC_Write(DAC_OUT_REG, 0);

            // Clear any remaining highlights
            if (prev_col != -1)
            {
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawLine((prev_col * cell_width) + 1, 0, (prev_col * cell_width) + 1, Y_MAX - 1, ST7789_WHITE);                     // Left vertical line
                ST7789_DrawLine(((prev_col + 1) * cell_width - 1) + 1, 0, ((prev_col + 1) * cell_width - 1) + 1, Y_MAX - 1, ST7789_WHITE); // Right vertical line
                G8RTOS_SignalSemaphore(&sem_SPIA);
                prev_col = -1;
            }

            sleep(10); // Prevent tight looping
        }
    }
}

void Volume_Thread(void)
{
    // Initialize / declare any variables here
    int32_t joystick_data;

    while (1)
    {
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
    uint8_t prev_x = 0, prev_y = 0;
    uint8_t prev_grid[8][8] = {0};

    while (1)
    {
        G8RTOS_WaitSemaphore(&sem_SPIA);

        // Clear the previous yellow highlight by restoring white grid lines
        ST7789_DrawLine(prev_x * cell_width, prev_y * cell_height, (prev_x + 1) * cell_width, prev_y * cell_height, ST7789_WHITE);             // Top
        ST7789_DrawLine(prev_x * cell_width, prev_y * cell_height, prev_x * cell_width, (prev_y + 1) * cell_height, ST7789_WHITE);             // Left
        ST7789_DrawLine((prev_x + 1) * cell_width, prev_y * cell_height, (prev_x + 1) * cell_width, (prev_y + 1) * cell_height, ST7789_WHITE); // Right
        ST7789_DrawLine(prev_x * cell_width, (prev_y + 1) * cell_height, (prev_x + 1) * cell_width, (prev_y + 1) * cell_height, ST7789_WHITE); // Bottom

        // Update grid contents based on note placement
        for (uint8_t col = 0; col < 8; col++)
        {
            for (uint8_t row = 0; row < 8; row++)
            {
                if (grid[col][row] != prev_grid[col][row])
                {
                    uint16_t color = (grid[col][row] == 1) ? colors[row] : ST7789_BLACK;

                    ST7789_DrawRectangle(col * cell_width + 1, row * cell_height + 1, cell_width - 1, cell_height - 1, color);

                    prev_grid[col][row] = grid[col][row];
                }
            }
        }

        // Draw new yellow highlight for the currently selected rectangle
        ST7789_DrawLine(highlight_x * cell_width, highlight_y * cell_height, (highlight_x + 1) * cell_width, highlight_y * cell_height, ST7789_YELLOW);             // Top
        ST7789_DrawLine(highlight_x * cell_width, highlight_y * cell_height, highlight_x * cell_width, (highlight_y + 1) * cell_height, ST7789_YELLOW);             // Left
        ST7789_DrawLine((highlight_x + 1) * cell_width, highlight_y * cell_height, (highlight_x + 1) * cell_width, (highlight_y + 1) * cell_height, ST7789_YELLOW); // Right
        ST7789_DrawLine(highlight_x * cell_width, (highlight_y + 1) * cell_height, (highlight_x + 1) * cell_width, (highlight_y + 1) * cell_height, ST7789_YELLOW); // Bottom

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

        // Switch status on the Multimod board.
        if (JOYSTICK_GetPress())
        {
            playing = !playing;
        }

        // Clear the interrupt
        GPIOIntClear(GPIO_PORTD_BASE, JOYSTICK_INT_PIN);

        // Re-enable the interrupt so it can occur again.
        GPIOIntEnable(GPIO_PORTD_BASE, JOYSTICK_INT_PIN);
    }
}
void Navigation_Thread(void)
{
    uint8_t buttons;

    while (1)
    {
        // Wait for the semaphore
        G8RTOS_WaitSemaphore(&sem_PCA9555_Debounce);

        // Sleep to debounce
        sleep(10);

        // Read buttons
        buttons = -MultimodButtons_Get();

        // Move highlight box
        if (buttons & SW1) // Up
        {
            highlight_y = (highlight_y < 7) ? highlight_y + 1 : 0;
        }
        else if (buttons & SW2) // Down
        {
            highlight_y = (highlight_y > 0) ? highlight_y - 1 : 7;
        }
        else if (buttons & SW3) // Left
        {
            highlight_x = (highlight_x > 0) ? highlight_x - 1 : 7;
        }
        else if (buttons & SW4) // Right
        {
            highlight_x = (highlight_x < 7) ? highlight_x + 1 : 0;
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

        // Sleep to debounce
        sleep(100);

        // Toggle the note in the grid
        grid[highlight_x][highlight_y] ^= 1;

        // Clear the interrupt
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);

        // Re-enable the interrupt
        GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_0);
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
    GPIOIntDisable(GPIO_PORTF_BASE, GPIO_PIN_0);
    G8RTOS_SignalSemaphore(&sem_Tiva_Button);
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
