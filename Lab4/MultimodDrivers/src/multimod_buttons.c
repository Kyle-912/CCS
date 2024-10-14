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
    // 1. Use the common I2C_Init function to initialize I2C0 (PB2 for SCL, PB3 for SDA)
    I2C_Init(I2C0_BASE);

    // 2. Set PCA9555 GPIO bank 1 to input mode for buttons
    I2CMasterSlaveAddrSet(I2C0_BASE, BUTTONS_PCA9555_GPIO_ADDR, false); // PCA9555 address (0x23), write mode
    I2CMasterDataPut(I2C0_BASE, 0x06);                                  // Address of configuration register for Bank 1
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);       // Send start command
    while (I2CMasterBusy(I2C0_BASE))
    {
    }

    // Write 0xFF to configure all pins in Bank 1 as inputs
    I2CMasterDataPut(I2C0_BASE, 0xFF);                             // All input configuration
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH); // Send finish command
    while (I2CMasterBusy(I2C0_BASE))
    {
    }

    // 3. Configure interrupt pin connected to PCA9555 INT pin (PE4)
    GPIOPinTypeGPIOInput(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);              // Set PE4 as input
    GPIOIntTypeSet(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN, GPIO_FALLING_EDGE); // Interrupt on falling edge
    GPIOIntEnable(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);                     // Enable GPIO interrupt on PE4
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

    // 1. Set the PCA9555 address and specify the input register for buttons (Bank 1)
    I2CMasterSlaveAddrSet(I2C0_BASE, BUTTONS_PCA9555_GPIO_ADDR, false); // Set to write mode (0x23 address)
    I2CMasterDataPut(I2C0_BASE, 0x01);                                  // Address of input register for GPIO Bank 1
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);            // Send the command
    while (I2CMasterBusy(I2C0_BASE))
    {
    }

    // 2. Switch to read mode to retrieve button states from PCA9555 Bank 1
    I2CMasterSlaveAddrSet(I2C0_BASE, BUTTONS_PCA9555_GPIO_ADDR, true); // Set to read mode (0x23 address)
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);        // Receive the button states
    while (I2CMasterBusy(I2C0_BASE))
    {
    }

    // 3. Get the button state from the input register
    buttonState = I2CMasterDataGet(I2C0_BASE);

    // Return the states of buttons SW1-SW4
    return buttonState & (SW1 | SW2 | SW3 | SW4);
}
