// multimod_BMI160.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for BMI160 functions

/************************************Includes***************************************/

#include "../multimod_BMI160.h"

#include <stdint.h>
#include "../multimod_i2c.h"

/************************************Includes***************************************/

/********************************Public Functions***********************************/

// BMI160_Init
// Initializes the BMI160. Currently enables the accelerometer
// in full-power mode.
// Return: void
void BMI160_Init()
{
    I2C_Init(I2C1_BASE);

    // Power on accelerometer
    BMI160_WriteRegister(BMI160_CMD_ADDR, 0x11);

    // Power on gyroscope
    BMI160_WriteRegister(BMI160_CMD_ADDR, 0x15);

    /*
    // FIXME: From here down
    // Step 1: Enable I2C passthrough mode for the BMI160
    // Write 0x02 to IF_CONF (0x6B) to enable the magnetometer interface
    BMI160_WriteRegister(0x6B, 0x02); // Enable magnetometer mode

    // Step 2: Enable manual mode to access the BMM150 registers through BMI160
    // Write 0x80 to MAG_IF[0] (0x4B) to enable manual mode (mag_manual_en = 1)
    BMI160_WriteRegister(0x4B, 0x80); // Enable manual access to BMM150

    // Step 3: Power up the BMM150 (BMM150 power control register is at 0x4B on the BMM150 side)
    // First, write the BMM150 register address (0x4B) to MAG_IF[3] (0x4E) and then the data (0x01) to MAG_IF[4] (0x4F)
    BMI160_WriteRegister(0x4E, 0x4B); // Set the BMM150 register address to 0x4B (power control)
    BMI160_WriteRegister(0x4F, 0x01); // Set the BMM150 to normal mode

    // Step 4: Set the BMM150 to continuous measurement mode (BMM150 operating mode register is 0x4C)
    // Write the BMM150 register address (0x4C) to MAG_IF[3] (0x4E) and then the data (0x00) to MAG_IF[4] (0x4F)
    BMI160_WriteRegister(0x4E, 0x4C); // Set the BMM150 register address to 0x4C (operation mode)
    BMI160_WriteRegister(0x4F, 0x00); // Set the BMM150 to continuous measurement mode

    // Step 5: Disable manual mode to allow automatic read loop
    // Write 0x00 to MAG_IF[0] (0x4B) to disable manual access and start automatic data read loop
    BMI160_WriteRegister(0x4B, 0x00); // Disable manual access to allow auto read
    */

    return;
}

// BMI160_Init
// Writes to a register address in the BMI160.
// Param uint8_t "addr": Register address
// Param uint8_t "data": data to write
// Return: void
void BMI160_WriteRegister(uint8_t addr, uint8_t data)
{
    uint8_t arr[2] = {addr, data};
    I2C_WriteMultiple(I2C1_BASE, BMI160_ADDR, arr, 2);
    return;
}

// BMI160_ReadRegister
// Reads from a register address in the BMI160.
// Param uint8_t "addr": Register address
// Return: void
uint8_t BMI160_ReadRegister(uint8_t addr)
{
    I2C_WriteSingle(I2C1_BASE, BMI160_ADDR, addr);
    return I2C_ReadSingle(I2C1_BASE, BMI160_ADDR);
}

// BMI160_MultiReadRegister
// Uses the BMI160 auto-increment function to read from multiple registers.
// Param uint8_t "addr": beginning register address
// Param uint8_t* "data": pointer to an array to store data in
// Param uint8_t "num_bytes": number of bytes to read
// Return: void
void BMI160_MultiReadRegister(uint8_t addr, uint8_t *data, uint8_t num_bytes)
{
    I2C_WriteSingle(I2C1_BASE, BMI160_ADDR, addr);
    I2C_ReadMultiple(I2C1_BASE, BMI160_ADDR, data, num_bytes);
    return;
}

// BMI160_AccelXGetResult
// Gets the 16-bit x-axis acceleration result.
// Return: uint16_t
int16_t BMI160_AccelXGetResult()
{
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_ACC))
    {
    }
    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + ACCELX_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_AccelYGetResult
// Gets the 16-bit y-axis acceleration result.
// Return: uint16_t
int16_t BMI160_AccelYGetResult()
{
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_ACC))
    {
    }

    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + ACCELY_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_AccelZGetResult
// Gets the 16-bit z-axis acceleration result.
// Return: uint16_t
int16_t BMI160_AccelZGetResult()
{
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_GYR))
    {
    }

    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + ACCELZ_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_GyroXGetResult
// Gets the 16-bit x-axis gyroscope result.
// Return: uint16_t
int16_t BMI160_GyroXGetResult()
{
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_GYR))
    {
    }

    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + GYROX_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_GyroYGetResult
// Gets the 16-bit y-axis gyroscope result.
// Return: uint16_t
int16_t BMI160_GyroYGetResult()
{
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_GYR))
    {
    }

    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + GYROY_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_GyroZGetResult
// Gets the 16-bit z-axis gyroscope result.
// Return: uint16_t
int16_t BMI160_GyroZGetResult()
{
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_GYR))
    {
    }

    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + GYROZ_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_MagXGetResult
// Gets the 16-bit x-axis magnetometer result.
// Return: uint16_t
int16_t BMI160_MagXGetResult()
{
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_MAG))
    {
    }

    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + MAGX_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_MagYGetResult
// Gets the 16-bit y-axis magnetometer result.
// Return: uint16_t
int16_t BMI160_MagYGetResult()
{
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_MAG))
    {
    }

    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + MAGY_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_MagZGetResult
// Gets the 16-bit z-axis magnetometer result.
// Return: uint16_t
int16_t BMI160_MagZGetResult()
{
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_MAG))
    {
    }

    uint8_t bytes[2];

    BMI160_MultiReadRegister(BMI160_DATA_O + MAGZ_O, bytes, 2);

    return (bytes[1] << 8 | bytes[0]);
}

// BMI160_AccelXYZGetResult
// Stores the 16-bit XYZ accelerometer results in an array.
// Param uint16_t* "data": pointer to an array of 16-bit data.
// Return: void
void BMI160_AccelXYZGetResult(uint16_t *data)
{
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_ACC))
    {
    }

    uint8_t bytes[6];

    BMI160_MultiReadRegister(BMI160_DATA_O + ACCELX_O, bytes, 6);

    *(data++) = (bytes[1] << 8 | bytes[0]);
    *(data++) = (bytes[3] << 8 | bytes[2]);
    *(data++) = (bytes[5] << 8 | bytes[4]);

    return;
}

// BMI160_GyroXYZGetResult
// Stores the 16-bit XYZ gyroscope results in an array.
// Param uint16_t* "data": pointer to an array of 16-bit data.
// Return: void
void BMI160_GyroXYZGetResult(uint16_t *data)
{
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_GYR))
    {
    }

    uint8_t bytes[6];

    BMI160_MultiReadRegister(BMI160_DATA_O + GYROX_O, bytes, 6);

    *(data++) = (bytes[1] << 8 | bytes[0]);
    *(data++) = (bytes[3] << 8 | bytes[2]);
    *(data++) = (bytes[5] << 8 | bytes[4]);

    return;
}

// BMI160_MagXYZGetResult
// Stores the 16-bit XYZ magnetometer results in an array.
// Param uint16_t* "data": pointer to an array of 16-bit data.
// Return: void
void BMI160_MagXYZGetResult(uint16_t *data)
{
    while (!(BMI160_GetDataStatus() & BMI160_STATUS_DRDY_MAG))
    {
    }

    uint8_t bytes[6];

    BMI160_MultiReadRegister(BMI160_DATA_O + MAGX_O, bytes, 6);

    *(data++) = (bytes[1] << 8 | bytes[0]);
    *(data++) = (bytes[3] << 8 | bytes[2]);
    *(data++) = (bytes[5] << 8 | bytes[4]);

    return;
}

// BMI160_GetDataStatus
// Gets the status register to determine if data is ready to read.
// Return: uint8_t
uint8_t BMI160_GetDataStatus()
{
    return BMI160_ReadRegister(BMI160_STATUS_ADDR);
}

/********************************Public Functions***********************************/
