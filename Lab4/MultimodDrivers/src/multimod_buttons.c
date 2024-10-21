// multimod_buttons.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for button functions

/************************************Includes***************************************/

#include "../multimod_buttons.h"

#include "../multimod_i2c.h"

#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>

#include <inc/tm4c123gh6pm.h>
#include <inc/hw_i2c.h>

/********************************Public Functions***********************************/

// Buttons_Init
// Initializes buttons on the multimod by configuring the I2C module and
// relevant interrupt pin.
// Return: void
void MultimodButtons_Init()
{
    // 1. Initialize I2C0 (PB2 for SCL, PB3 for SDA)
    I2C_Init(I2C0_BASE);

    // 2. Configure PCA9555 Ports 0 and 1 as inputs
    // Include the 0x06 register address in the I2C_WriteMultiple call
    uint8_t configData[3] = {0x06, 0xFF}; // 0x06 is the config register, set all pins as inputs

    // Write the configuration data using I2C_WriteMultiple
    I2C_WriteMultiple(I2C0_BASE, PCA9555_BUTTONS_ADDR, configData, 2);

    // 3. Configure the interrupt pin connected to PCA9555 INT
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); // Enable clock to Port E
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE))
    {
    }

    GPIOPinTypeGPIOInput(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);              // Set as input
    GPIOIntTypeSet(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN, GPIO_FALLING_EDGE); // Falling edge trigger
    GPIOIntEnable(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);                     // Enable interrupt
    IntEnable(INT_GPIOE);                                                      // Enable Port E interrupt in NVIC

    // 4. Enable global interrupts
    IntMasterEnable();
}

// MultimodButtons_Get
// Gets the input to GPIO bank 1, [0..7].
// Return: uint8_t
uint8_t MultimodButtons_Get()
{
    uint8_t buttonState = 0;

    // 1. Write to select Input Register 0 (Port 0) using I2C_WriteSingle
    uint8_t inputRegister = 0x00; // Command byte for Input Register 0
    I2C_WriteSingle(I2C0_BASE, PCA9555_BUTTONS_ADDR, inputRegister);

    // 2. Read the input register using I2C_ReadSingle
    buttonState = I2C_ReadSingle(I2C0_BASE, PCA9555_BUTTONS_ADDR);

    // 3. Mask only the relevant bits for SW1-SW4 (bits 1-4)
    return buttonState & (SW1 | SW2 | SW3 | SW4);
}
