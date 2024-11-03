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
// FIXME:
int16_t Goertzel_ReadSample(int FIFO_index)
{
    // read sample from FIFO
    // return sample value
    return (int16_t)G8RTOS_ReadFIFO(FIFO_index);
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
// TODO:
void Volume_Thread(void)
{
    // define variables

    while (1)
    {
        // read joystick values

        // normalize the joystick values

        // update volume based on joystickY_norm

        // limit volume to 0-4095 (12 bit range)
    }
}
// FIXME:
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
        ST7789_DrawRectangle(20, Y_MAX - previous_f1, 40, previous_f1, ST7789_BLACK); // Clear area for FREQ1
        ST7789_DrawRectangle(60, Y_MAX - previous_f2, 40, previous_f2, ST7789_BLACK); // Clear area for FREQ2

        // draw new rectangle
        ST7789_DrawRectangle(20, Y_MAX - magnitude_f1, 40, magnitude_f1, ST7789_RED);  // Draw FREQ1 bar
        ST7789_DrawRectangle(60, Y_MAX - magnitude_f2, 40, magnitude_f2, ST7789_BLUE); // Draw FREQ2 bar

        G8RTOS_SignalSemaphore(&sem_SPIA);

        // update previous value
        previous_f1 = Y_MAX - magnitude_f1;
        previous_f2 = Y_MAX - magnitude_f2;
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
    uint16_t x_raw = JOYSTICK_GetX();
    uint16_t y_raw = JOYSTICK_GetY();

    int16_t x = (int16_t)(x_raw - 2048); // Center around 0
    int16_t y = (int16_t)(y_raw - 2048); // Center around 0

    // push joystick value to fifo
    G8RTOS_WriteFIFO(JOYSTICK_FIFO, ((uint32_t)x << 16) | (uint32_t)y);
}

/*******************************Aperiodic Threads***********************************/

void Mic_Handler()
{
    uint32_t micData[4] = {0};

    // Clear the ADC interrupt
    ADCIntClear(ADC0_BASE, 1);

    // Read ADC Value
    ADCSequenceDataGet(ADC0_BASE, 1, micData);

    // FIXME: write new sample to audio FIFOs
    G8RTOS_WriteFIFO(FREQ1_FIFO, micData[0]);
    G8RTOS_WriteFIFO(FREQ2_FIFO, micData[0]);
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

    // write the output value to the dac
    MutimodDAC_Write(DAC_OUT_REG, output);
}
