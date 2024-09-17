// Lab 1, uP2 Fall 2023
// Created: 2023-07-31
// Updated: 2023-08-01
// Lab 1 is intended to introduce you to I2C, the sensor backpack, how to interface with them,
// and output the values through UART.
// It is recommended to use 16-bit timers 0a, 0b, 1a, 1b as your timers and output the values through UART.

// c. sample sensor values at a specific interval using timer modules
//      - use timers to sample from opt3001 and accelerometer (or others) at specific rates

/************************************Includes***************************************/

#include <stdint.h>
#include <stdbool.h>

#include <inc/tm4c123gh6pm.h>
#include <inc/hw_memmap.h>
#include <inc/hw_gpio.h>

#include <driverlib/uartstdio.h>
#include <driverlib/gpio.h>
#include <driverlib/uart.h>
#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>
#include <driverlib/interrupt.h>
#include <driverlib/timer.h>

#include "MultimodDrivers/multimod.h"

/************************************Includes***************************************/

/*************************************Defines***************************************/
/*************************************Defines***************************************/

/********************************Public Variables***********************************/

uint8_t read_imu_flag = 0;
uint8_t read_opt_flag = 0;
uint8_t toggle_led_flag = 0;
uint8_t print_uart_flag = 0;

/********************************Public Variables***********************************/

/********************************Public Functions***********************************/

void TIMER0A_Handler(void);
void TIMER0B_Handler(void);
void TIMER1A_Handler(void);
void TIMER1B_Handler(void);

void Timer_Init()
{
    // Initialize timers
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); // For UART output and LED toggle
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))
    {
    }
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1); // For BMI160 and OPT3001 sampling
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1))
    {
    }

    // Disable timers
    TimerDisable(TIMER0_BASE, TIMER_A); // Timer 0A for UART output
    TimerDisable(TIMER0_BASE, TIMER_B); // Timer 0B for LED toggle
    TimerDisable(TIMER1_BASE, TIMER_A); // Timer 1A for BMI160 sampling
    TimerDisable(TIMER1_BASE, TIMER_B); // Timer 1B for OPT3001 sampling

    // Configure timers as half-width, periodic 16-bit or (32-bit if using 64-bit timers) timers for a total of 4 timers
    TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC | TIMER_CFG_B_PERIODIC);
    TimerConfigure(TIMER1_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC | TIMER_CFG_B_PERIODIC);

    // Set prescalers
    TimerPrescaleSet(TIMER0_BASE, TIMER_A, 50); // Prescaler for Timer 0A (UART output)
    TimerPrescaleSet(TIMER0_BASE, TIMER_B, 50); // Prescaler for Timer 0B (LED toggle)
    TimerPrescaleSet(TIMER1_BASE, TIMER_A, 50); // Prescaler for Timer 1A (BMI160 sampling)
    TimerPrescaleSet(TIMER1_BASE, TIMER_B, 50); // Prescaler for Timer 1B (OPT3001 sampling)

    // Load initial timer values
    // Sysclock / prescaler * desired seconds = timer period
    TimerLoadSet(TIMER0_BASE, TIMER_A, (SysCtlClockGet() / 50 * 0.5)); // 500 ms for UART output
    TimerLoadSet(TIMER0_BASE, TIMER_B, (16000000 / 50 * 0.1)); // 100 ms for LED toggle
    TimerLoadSet(TIMER1_BASE, TIMER_A, (16000000 / 50 * 0.1)); // 100 ms for BMI160 sampling
    TimerLoadSet(TIMER1_BASE, TIMER_B, (16000000 / 50 * 0.15)); // 150 ms for OPT3001 sampling

    // Enable timer interrupts
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT | TIMER_TIMB_TIMEOUT);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT | TIMER_TIMB_TIMEOUT);

    // Enable timers
    TimerEnable(TIMER0_BASE, TIMER_A);
    TimerEnable(TIMER0_BASE, TIMER_B);
    TimerEnable(TIMER1_BASE, TIMER_A);
    TimerEnable(TIMER1_BASE, TIMER_B);
}

void Int_Init(void)
{
    IntMasterDisable();

    // Enable timer interrupt, set interrupt priorities
    IntEnable(INT_TIMER0A); // UART output interrupt
    IntEnable(INT_TIMER0B); // LED toggle interrupt
    IntEnable(INT_TIMER1A); // BMI160 sampling interrupt
    IntEnable(INT_TIMER1B); // OPT3001 sampling interrupt

    IntPrioritySet(INT_TIMER0A, 0x00);
    IntPrioritySet(INT_TIMER0B, 0x20);
    IntPrioritySet(INT_TIMER1A, 0x40);
    IntPrioritySet(INT_TIMER1B, 0x60);

    // Point to relevant timer handler function
    IntRegister(INT_TIMER0A, TIMER0A_Handler);
    IntRegister(INT_TIMER0B, TIMER0B_Handler);
    IntRegister(INT_TIMER1A, TIMER1A_Handler);
    IntRegister(INT_TIMER1B, TIMER1B_Handler);

    IntMasterEnable();
}

void LED_Init(void)
{
    // Enable clock to port F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Set pins [2..0] to output, set as digital
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1, 0x00);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1);
}

/********************************Public Functions***********************************/

/************************************MAIN*******************************************/
// Use timers to enforce specific schedules for each event.
int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    UART_Init();
    LED_Init();
    BMI160_Init();
    OPT3001_Init();

    Int_Init();
    Timer_Init();

    int16_t x_accel_value = 0;
    uint16_t opt_value = 0;

    while (1)
    {
        // Write code to read the x-axis accelerometer value,
        // opt3001 sensor, toggle the red led, and print
        // values out to the console when needed.

        if (print_uart_flag)
        {
            print_uart_flag = 0; // Clear flag after handling
            UARTprintf("Print every 500 ms\n\n\n");
        }

        if (read_imu_flag)
        {
            read_imu_flag = 0;                        // Clear flag after handling
            x_accel_value = BMI160_AccelXGetResult(); // Read x-axis acceleration
            UARTprintf("Accelerometer X: %d\n", x_accel_value);
        }

        if (read_opt_flag)
        {
            read_opt_flag = 0;               // Clear flag after handling
            opt_value = OPT3001_GetResult(); // Read optical sensor data
            UARTprintf("Optical: %d\n", opt_value);
        }

        if (toggle_led_flag)
        {
            toggle_led_flag = 0; // Clear flag after handling

            uint8_t current_val = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1); // Read the current state of Port F
            // Toggle the Red LED (PF1) while ensuring Blue (PF2) and Green (PF3) are off
            if (current_val & GPIO_PIN_1)
            {
                // If Red LED is on, turn it off
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x00);
            }
            else
            {
                // If Red LED is off, turn it on
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_1);
            }
        }
    }
}
/************************************MAIN*******************************************/

/********************************Public Functions***********************************/

/*******************************Interrupt Handlers**********************************/

// Timer handlers are provided to you.
void TIMER0A_Handler(void)
{
    read_imu_flag = 1;
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    return;
}

void TIMER0B_Handler(void)
{
    read_opt_flag = 1;
    TimerIntClear(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
    return;
}

void TIMER1A_Handler(void)
{
    toggle_led_flag = 1;
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    return;
}

void TIMER1B_Handler(void)
{
    print_uart_flag = 1;
    TimerIntClear(TIMER1_BASE, TIMER_TIMB_TIMEOUT);
    return;
}
/*******************************Interrupt Handlers**********************************/
