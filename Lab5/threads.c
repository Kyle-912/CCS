// G8RTOS_Threads.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for thread functions.

/************************************Includes***************************************/

#include "./threads.h"

#include "./MultimodDrivers/multimod.h"
#include "./MiscFunctions/Signals/inc/goertzel.h"

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
int16_t current_volume = 0xFFF;
uint8_t current_buttons = 0;

/*********************************Global Variables**********************************/

/********************************Public Functions***********************************/

int16_t Goertzel_ReadSample(int FIFO_index)
{
    // read sample from FIFO and return sample value
    return G8RTOS_ReadFIFO(FIFO_index);
}

/*************************************Threads***************************************/

void Idle_Thread(void)
{
    while (1)
    {
    }
}

void Mic_Thread(void)
{
    // Input sample rate.
    const double sample_rate_hz = MIC_SAMPLE_RATE_HZ;

    // Frequency of DFT bins to calculate.
    const double detect_hz[2] = {1000.0, 2000.0};

    // Number of samples for detection
    const int N = MAX_NUM_SAMPLES;

    while (1)
    {
        // use goertzel function to calculate magnitudes for FREQ_1 and FREQ_2
        // NOTE: make sure you have implemented the FIFO read function in Goertzel_ReadSample
        float magnitude_f1 = (float)(goertzel(detect_hz[0], sample_rate_hz, N, Goertzel_ReadSample, FREQ1_FIFO));
        float magnitude_f2 = (float)(goertzel(detect_hz[1], sample_rate_hz, N, Goertzel_ReadSample, FREQ2_FIFO));

        // calculate magnitudes of FREQ_1 and FREQ_2
        magnitude_f1 = fabs(2.0 * Y_MAX * (magnitude_f1 / N));
        magnitude_f2 = fabs(2.0 * Y_MAX * (magnitude_f2 / N));

        // pack magnitude into 32-bit integer
        uint32_t packed_result = (int16_t)(magnitude_f1) << 16 | (int16_t)(magnitude_f2);

        // push newly magnitude ratio to display FIFO
        if (packed_result)
        {
            G8RTOS_WriteFIFO(DISPLAY_FIFO, packed_result);
        }
    }
}

void Speaker_Thread(void)
{
    uint8_t buttons = 0;

    while (1)
    {
        // wait for button semaphore
        G8RTOS_WaitSemaphore(&sem_PCA9555_Debounce);

        // debounce buttons
        sleep(15);

        // Get buttons
        buttons = current_buttons;

        // clear button interrupt
        GPIOIntClear(GPIO_PORTE_BASE, BUTTONS_INT_PIN);

        // TODO: check which buttons are pressed

        // TODO: set DAC output rate to 1000Hz
        //                              2000Hz
        //                              etc.
    }
}

void Volume_Thread(void)
{
    // define variables
    int16_t y;
    float norm_x, norm_y;
    int16_t volume;

    while (1)
    {
        // read joystick values
        y = (int16_t)G8RTOS_ReadFIFO(JOYSTICK_FIFO);

        y = -y;

        // If joystick axis within deadzone, set to 0
        if (abs(y) < 50)
        {
            y = 0;
        }

        // normalize the joystick values
        norm_y = (y != 0) ? (float)y / 2048.0f : 0.0f;

        // update volume based on joystickY_norm TODO: test
        volume = (int16_t)(2047 * (1 + norm_y));

        // limit volume to 0-4095 (12 bit range) TODO: test
        if (current_volume < 0)
        {
            current_volume = 0;
        }
        if (current_volume > 4095)
        {
            current_volume = 4095;
        }
    }
}

void Display_Thread(void)
{
    // Initialize / declare any variables here
    uint32_t packed_result;
    int16_t magnitude_f1, magnitude_f2;
    int previous_f1 = 0, previous_f2 = 0;

    while (1)
    {
        // read display FIFO for updated magnitude ratio
        packed_result = G8RTOS_ReadFIFO(DISPLAY_FIFO);

        // unpack result values
        magnitude_f1 = (int16_t)(packed_result >> 16);
        magnitude_f2 = (int16_t)(packed_result & 0xFFFF);

        // draw the magnitudes on the display
        G8RTOS_WaitSemaphore(&sem_SPIA);

        // limit the magnitude values to the display range
        if (magnitude_f1 > Y_MAX)
        {
            magnitude_f1 = Y_MAX;
        }
        if (magnitude_f2 > Y_MAX)
        {
            magnitude_f2 = Y_MAX;
        }

        // clear previous rectangle
        ST7789_DrawRectangle((X_MAX / 2) - 30, 0, 30, previous_f1, ST7789_BLACK);
        ST7789_DrawRectangle((X_MAX / 2), 0, 30, previous_f2, ST7789_BLACK);

        // draw new rectangle
        ST7789_DrawRectangle((X_MAX / 2) - 30, 0, 30, magnitude_f1, ST7789_RED);
        ST7789_DrawRectangle((X_MAX / 2), 0, 30, magnitude_f2, ST7789_BLUE);

        G8RTOS_SignalSemaphore(&sem_SPIA);

        // update previous value
        previous_f1 = magnitude_f1;
        previous_f2 = magnitude_f2;
    }
}

void Read_Buttons(void)
{
    // Initialize / declare any variables here
    uint8_t buttons;

    while (1)
    {

        // wait for button semaphore
        G8RTOS_WaitSemaphore(&sem_PCA9555_Debounce);

        // Get buttons
        buttons = MultimodButtons_Get();

        // debounce buttons
        sleep(15);

        // clear button interrupt
        GPIOIntClear(GPIO_PORTE_BASE, BUTTONS_INT_PIN);

        // update current_buttons value
        current_buttons = buttons;
    }
}

/********************************Periodic Threads***********************************/

void Update_Volume(void)
{
    // read joystick values
    uint16_t y_raw = JOYSTICK_GetY();

    int16_t y = (int16_t)(y_raw - 2048); // Center around 0

    // push joystick value to fifo
    G8RTOS_WriteFIFO(JOYSTICK_FIFO, y);
}

/*******************************Aperiodic Threads***********************************/

void Mic_Handler()
{
    uint32_t micData[4] = {0};

    // Clear the ADC interrupt
    ADCIntClear(ADC0_BASE, 1);

    // Read ADC Value
    ADCSequenceDataGet(ADC0_BASE, 1, micData);

    // write new sample to audio FIFOs
    G8RTOS_WriteFIFO(FREQ1_FIFO, micData[0]);
    G8RTOS_WriteFIFO(FREQ2_FIFO, micData[0]);
    G8RTOS_WriteFIFO(OUTPUT_FIFO, micData[0]);
}

void Button_Handler()
{
    // disable interrupt and signal semaphore
    GPIOIntDisable(GPIO_PORTE_BASE, BUTTONS_INT_PIN);
    G8RTOS_SignalSemaphore(&sem_PCA9555_Debounce);
}

void DAC_Timer_Handler()
{
    // clear the timer interrupt
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    // read next output sample
    uint32_t output = (current_volume) * (dac_signal[dac_step++ % SIGNAL_STEPS]);

    // TODO: BONUS: stream microphone input to DAC output via FIFO
    int16_t dac_data = G8RTOS_ReadFIFO(OUTPUT_FIFO);

    // write the output value to the dac
    MutimodDAC_Write(DAC_OUT_REG, output);
}
