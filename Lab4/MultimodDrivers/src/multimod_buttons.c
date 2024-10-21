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

    // 2. Configure PCA9555 Ports 0 and 1 as inputs (using 4-byte sequence)
    I2CMasterSlaveAddrSet(I2C0_BASE, PCA9555_BUTTONS_ADDR, false); // Address 0x23 for configuration

    // Start burst transmission to write 4 bytes
    I2CMasterDataPut(I2C0_BASE, 0x06);                            // Command byte: Configuration Register 0x06
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START); // Start condition
    while (I2CMasterBusy(I2C0_BASE))
    {
    }

    I2CMasterDataPut(I2C0_BASE, 0xFF);                           // Data byte 1: Set all Port 0 pins as inputs
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT); // Continue condition
    while (I2CMasterBusy(I2C0_BASE))
    {
    }

    I2CMasterDataPut(I2C0_BASE, 0xFF);                             // Data byte 2: Set all Port 1 pins as inputs
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH); // Finish condition
    while (I2CMasterBusy(I2C0_BASE))
    {
    }

    // 3. Configure the interrupt pin connected to PCA9555 INT
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); // Enable clock to Port E
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE))
    {
    }

    // Use BUTTONS_INT_PIN define instead of GPIO_PIN_4
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

    // 1. Set the PCA9555 address to select Input Register 0 (Port 0)
    I2CMasterSlaveAddrSet(I2C0_BASE, PCA9555_BUTTONS_ADDR, false); // Address 0x21 for reading input
    I2CMasterDataPut(I2C0_BASE, 0x00);                             // Command byte: Select Input Register 0
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);       // Send command
    while (I2CMasterBusy(I2C0_BASE))
    {
    }

    // 2. Switch to read mode to retrieve button states
    I2CMasterSlaveAddrSet(I2C0_BASE, PCA9555_BUTTONS_ADDR, true); // Set to read mode
    // 3. Read data from Input Register 0 (Port 0)
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START); // Start receiving
    while (I2CMasterBusy(I2C0_BASE))
        ;                                      // Wait until ready
    buttonState = I2CMasterDataGet(I2C0_BASE); // Store data from Port 0

    // 4. Read data from Input Register 1 (Port 1)
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH); // Finish receiving
    while (I2CMasterBusy(I2C0_BASE))
        ;                                      // Wait until ready
    buttonState = I2CMasterDataGet(I2C0_BASE); // Store data from Port 1

    // 4. Mask only the relevant bits for SW1-SW4 (bits 1-4)
    return buttonState & (SW1 | SW2 | SW3 | SW4);
}
