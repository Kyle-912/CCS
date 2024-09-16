// Lab 1, uP2 Fall 2023
// Created: 2023-07-31
// Updated: 2023-08-01
// Lab 1 is intended to introduce you to I2C, the sensor backpack, how to interface with them,
// and output the values through UART.

// b. Read values from sensors, UART these values

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
/********************************Public Functions***********************************/

/************************************MAIN*******************************************/

// Read accelerometer x sensor values, output them through UART.
int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    UART_Init();
    BME280_Init();

    uint16_t temp_value = 0;

    while (1)
    {
        BME280_Data temp_value = BME280_ReadTemperature();

        UARTprintf("Temperature: %.2f °C\n", temp_value.temperature);
    }
}

/************************************MAIN*******************************************/
