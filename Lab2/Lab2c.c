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

    // Configure timers as half-width, periodic 16-bit or (32-bit if using 64-bit timers) timers for a total of 4 timers

    // Set prescalers

    // Load initial timer values
    // Sysclock / prescaler * desired seconds = timer period

    // Enable timer interrupts

    // Enable timers
}

void Int_Init(void)
{
    IntMasterDisable();

    // Enable timer interrupt, set interrupt priorities

    // Point to relevant timer handler function

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

    Int_Init();
    Timer_Init();
    UART_Init();

    LED_Init();
    BMI160_Init();
    OPT3001_Init();

    int16_t x_accel_value = 0;
    uint16_t opt_value = 0;

    while (1)
    {
        // Write code to read the x-axis accelerometer value,
        // opt3001 sensor, toggle the red led, and print
        // values out to the console when needed.
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
