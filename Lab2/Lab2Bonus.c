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
/********************************Public Variables***********************************/

/********************************Public Functions***********************************/
// Timer 0A interrupt handler
void TIMER0A_Handler(void)
{
    // Clear the timer interrupt flag
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Add your code here to handle the interrupt
}

// Timer 0B interrupt handler
void TIMER0B_Handler(void)
{
    // Clear the timer interrupt flag
    TimerIntClear(TIMER0_BASE, TIMER_TIMB_TIMEOUT);

    // Add your code here to handle the interrupt
}

// Timer 1A interrupt handler
void TIMER1A_Handler(void)
{
    // Clear the timer interrupt flag
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    // Add your code here to handle the interrupt
}

// Timer 1B interrupt handler
void TIMER1B_Handler(void)
{
    // Clear the timer interrupt flag
    TimerIntClear(TIMER1_BASE, TIMER_TIMB_TIMEOUT);

    // Add your code here to handle the interrupt
}
/********************************Public Functions***********************************/

/************************************MAIN*******************************************/
int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    UART_Init();
    BME280_Init();

    int32_t temp_value;

    while (1)
    {
        temp_value = BME280_ReadTemperature();

        // Print the temperature data over UART
        UARTprintf("Temperature: %d.%02d °C\n", temp_value / 100, temp_value % 100);
    }
}
/************************************MAIN*******************************************/
