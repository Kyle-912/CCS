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
#define delay_0_500_ms 800000
/*************************************Defines***************************************/

// If using the SDK, there are two supported programming models: Direct Register Access
// and the software driver model.

// You can ctrl+click on functions and it will take you to where the functions are declared.
// This will help you learn how to navigate the SDK functions within the header files.

// Alternatively you can use the SDK manual.

/************************************MAIN*******************************************/
// Flash & make sure the LEDs are blinking.
// Read the launchpad schematic to make sure which port and pins are for which LEDs!
// Check the manual to see how to configure the IO pins.
int main(void)
{
    // Set clock speed
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Enable clock to port F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }

    // Set pins [2..0] to output, set as digital
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);

    int8_t count = 0;

    while (1)
    {
//        if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == 0)
//        {
//            if (count == 0)
//            {
//                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
//                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
//                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 255); // Red
//                count = 1;
//            }
//            else if (count == 1)
//            {
//                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
//                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
//                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 255); // Green
//                count = 2;
//            }
//            else
//            {
//                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
//                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
//                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 255); // Blue
//                count = 0;
//            }
//        }
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 255);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 255);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 255);
        SysCtlDelay(delay_0_1_s);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
        SysCtlDelay(delay_0_1_s);
        SysCtlDelay(delay_0_500_ms);
    }
}
/************************************MAIN*******************************************/
