// Lab 0, uP2 Fall 2023
// Created: 2023-07-31
// Updated: 2023-07-31
// Lab 0 is intended to introduce you to the code composer studio IDE, how to flash programs,
// reading the tm4c123gh6pm datasheet, and how to initialize the UART console to read
// data from the microcontroller.

/************************************Includes***************************************/

#include <stdint.h>
#include <stdbool.h>

#include <inc/tm4c123gh6pm.h>
#include <inc/hw_memmap.h>
#include <inc/hw_gpio.h>

#include "driverlib/uartstdio.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"

/************************************Includes***************************************/

/*************************************Defines***************************************/
// 16 MHz / 10 = 0.1 s
#define delay_0_1_s 1600000
/*************************************Defines***************************************/
/************************************MAIN*******************************************/

int main(void)
{
    // Set system clock speed
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Enable relevant port for launchpad switches
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }

    // Configure SW1 input
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);

    // Enable port A
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))
    {
    }

    // Enable UART0 module
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
    {
    }

    // Configure UART0 pins on port A
    GPIOPinTypeUART(GPIO_PORTA_BASE, (GPIO_PIN_0 | GPIO_PIN_1));

    // Set UART clock source
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_SYSTEM);

    // Configure UART baud rate
    UARTStdioConfig(0, 115200, SysCtlClockGet());

    uint32_t counter = 0;

    while (1)
    {
        // Print counter value through UART, or display message if button pressed
        if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4))
        {
            UARTprintf("Counter value: %d\n", counter);
        }
        else
        {
            UARTprintf("Button pressed!\n");
        }

        // Delay by 0.1 ms
        counter++;
        SysCtlDelay(delay_0_1_s);
    }
}
/************************************MAIN*******************************************/
