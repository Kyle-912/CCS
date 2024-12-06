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

#include "driverlib/timer.h"
#include "driverlib/adc.h"

#include "GFX_Library.h"

#define SIGNAL_STEPS (2)
#define MAX_PAGES 4

/*********************************Global Variables**********************************/

uint16_t dac_step = 0;
int16_t dac_signal[SIGNAL_STEPS] = {0x001, 0x000};
int16_t volume = 0xFFF;
int16_t tempo = 120;
uint8_t grid[MAX_PAGES][8][8] = {0};
uint8_t current_page = 0;
uint8_t highlight_x = 0, highlight_y = 0;
uint8_t playing = 0;
uint16_t cell_width = X_MAX / 8;
uint16_t cell_height = Y_MAX / 8;
uint16_t colors[8] = {ST7789_RED, ST7789_ORANGE, ST7789_YELLOW, ST7789_GREEN, ST7789_BLUE, ST7789_VIOLET, ST7789_PINK, ST7789_RED};

/********************************Public Functions***********************************/
void DisplayPageNumber()
{
    // SysCtlDelay(10000);
    display_drawChar(5, 260, 1, ST7789_WHITE, ST7789_WHITE, 1);
    // display_drawChar(7, 260, '/', ST7789_WHITE, ST7789_WHITE, 1);
    // display_drawChar(20, 260, MAX_PAGES, ST7789_WHITE, ST7789_WHITE, 1);

    // uint16_t x = 0; // Starting X position
    // uint16_t y = 0; // Starting Y position

    // for (uint8_t c = 0; c < 256; c++)
    // {
    //     // Draw the character at the current position
    //     display_drawChar(x, y, c, ST7789_WHITE, ST7789_BLACK, 1);

    //     // Move to the next position
    //     x += 6; // Move horizontally by character width
    //     if (x + 6 > X_MAX)
    //     {
    //         // If the character would exceed the screen width, move to the next row
    //         x = 0;
    //         y += 8;
    //     }

    //     // If the Y position exceeds the screen height, stop printing
    //     if (y + 8 > Y_MAX)
    //     {
    //         break;
    //     }
    // }
}

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

    DisplayPageNumber();
}

void PlayNoteAtRow(uint8_t row)
{
    uint16_t frequencies[8] = {130, 147, 165, 175, 196, 220, 247, 260};
    uint32_t period = SysCtlClockGet() / (frequencies[row] * 2);
    TimerDisable(TIMER1_BASE, TIMER_A);
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
    int prev_col = -1;

    while (1)
    {
        if (playing)
        {
            for (int page = 0; page < MAX_PAGES; page++) // Iterate through all pages
            {
                current_page = page;

                for (int col = 0; col < 8; col++)
                {
                    G8RTOS_WaitSemaphore(&sem_SPIA);

                    // Highlight the current playing column with red
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

                    uint8_t note_playing = 0;

                    for (int row = 0; row < 8; row++) // Check each note in the column
                    {
                        if (grid[current_page][col][row] == 1)
                        {
                            PlayNoteAtRow(row);
                            note_playing = 1;
                        }
                    }

                    if (!note_playing) // Silence if no notes are selected in the column
                    {
                        TimerDisable(TIMER1_BASE, TIMER_A);
                    }

                    sleep(60000 / (tempo * 2));
                }

                // Clear the final column highlight after playback of the current page
                if (prev_col != -1)
                {
                    G8RTOS_WaitSemaphore(&sem_SPIA);
                    ST7789_DrawLine((prev_col * cell_width) + 1, 0, (prev_col * cell_width) + 1, Y_MAX - 1, ST7789_WHITE);                     // Left vertical line
                    ST7789_DrawLine(((prev_col + 1) * cell_width - 1) + 1, 0, ((prev_col + 1) * cell_width - 1) + 1, Y_MAX - 1, ST7789_WHITE); // Right vertical line
                    G8RTOS_SignalSemaphore(&sem_SPIA);
                    prev_col = -1;
                }
            }

            // Reset to the first page after completing playback of all pages
            current_page = 0;
            G8RTOS_WaitSemaphore(&sem_SPIA);
            InitializeGridDisplay();
            G8RTOS_SignalSemaphore(&sem_SPIA);
        }
        else
        {
            TimerDisable(TIMER1_BASE, TIMER_A);

            // Clear any remaining highlights when playback stops
            if (prev_col != -1)
            {
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawLine((prev_col * cell_width) + 1, 0, (prev_col * cell_width) + 1, Y_MAX - 1, ST7789_WHITE);                     // Left vertical line
                ST7789_DrawLine(((prev_col + 1) * cell_width - 1) + 1, 0, ((prev_col + 1) * cell_width - 1) + 1, Y_MAX - 1, ST7789_WHITE); // Right vertical line
                G8RTOS_SignalSemaphore(&sem_SPIA);
                prev_col = -1;
            }

            sleep(10);
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
    uint8_t prev_page = current_page;
    uint8_t prev_grid[MAX_PAGES][8][8] = {0};

    while (1)
    {
        G8RTOS_WaitSemaphore(&sem_SPIA);

        // Check if the page has changed
        if (prev_page != current_page)
        {
            ST7789_Fill(ST7789_BLACK);
            InitializeGridDisplay();

            for (uint8_t col = 0; col < 8; col++)
            {
                for (uint8_t row = 0; row < 8; row++)
                {
                    if (grid[current_page][col][row] == 1)
                    {
                        uint16_t color = colors[row];
                        ST7789_DrawRectangle(col * cell_width + 1, row * cell_height + 1, cell_width - 1, cell_height - 1, color);
                    }
                }
            }
            DisplayPageNumber();

            prev_page = current_page;
        }

        // Update grid contents based on note placement
        for (uint8_t col = 0; col < 8; col++)
        {
            for (uint8_t row = 0; row < 8; row++)
            {
                if (grid[current_page][col][row] != prev_grid[current_page][col][row])
                {
                    uint16_t color = (grid[current_page][col][row] == 1) ? colors[row] : ST7789_BLACK;

                    ST7789_DrawRectangle(col * cell_width + 1, row * cell_height + 1, cell_width - 1, cell_height - 1, color);

                    prev_grid[current_page][col][row] = grid[current_page][col][row];
                }
            }
        }

        // Clear the previous yellow highlight by restoring white grid lines
        ST7789_DrawLine(prev_x * cell_width, prev_y * cell_height, (prev_x + 1) * cell_width, prev_y * cell_height, ST7789_WHITE);             // Top
        ST7789_DrawLine(prev_x * cell_width, prev_y * cell_height, prev_x * cell_width, (prev_y + 1) * cell_height, ST7789_WHITE);             // Left
        ST7789_DrawLine((prev_x + 1) * cell_width, prev_y * cell_height, (prev_x + 1) * cell_width, (prev_y + 1) * cell_height, ST7789_WHITE); // Right
        ST7789_DrawLine(prev_x * cell_width, (prev_y + 1) * cell_height, (prev_x + 1) * cell_width, (prev_y + 1) * cell_height, ST7789_WHITE); // Bottom

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
            if (highlight_x == 0)
            {
                // Move to the previous page
                current_page = (current_page > 0) ? current_page - 1 : MAX_PAGES - 1;
                highlight_x = 7;
            }
            else
            {
                highlight_x--;
            }
        }
        else if (buttons & SW4) // Right
        {
            if (highlight_x == 7)
            {
                // Move to the next page
                current_page = (current_page < MAX_PAGES - 1) ? current_page + 1 : 0;
                highlight_x = 0;
            }
            else
            {
                highlight_x++;
            }
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
        grid[current_page][highlight_x][highlight_y] ^= 1;

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
