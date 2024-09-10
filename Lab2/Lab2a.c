// Lab 1, uP2 Fall 2023
// Created: 2023-07-31
// Updated: 2023-08-01
// Lab 1 is intended to introduce you to I2C, the sensor backpack, how to interface with them,
// and output the values through UART.

// a. Create I2C functions, interface and initialize sensors
//      - introduction to i2c, different datasheets

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

// Write i2c functions, check to see if you can get the chip IDs.
int main(void) {
    UART_Init();

    uint8_t BMI160_chipid = BMI160_ReadRegister(BMI160_CHIPID_ADDR);

    uint8_t OPT3001_chipid = BMI160_ReadRegister(BMI160_CHIPID_ADDR);

    // Output chip IDs

    while(1);
}

/************************************MAIN*******************************************/
// Start - Write address