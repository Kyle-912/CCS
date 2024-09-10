// multimod_uart.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for UART functions

/************************************Includes***************************************/

#include "../multimod_uart.h"

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

/************************************Includes***************************************/

/********************************Public Functions***********************************/

// UART_Init
// Initializes UART serial communication with PC
// Return: void
void UART_Init() {
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
    // This should have been done in lab 0, so it's just copy & paste.
    // Enable port A

    // Enable UART0 module


    // Configure UART0 pins on port A



    // Set UART clock source

    // Configure UART baud rate

}

/********************************Public Functions***********************************/

