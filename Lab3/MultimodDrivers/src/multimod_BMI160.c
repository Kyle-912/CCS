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

    // FIXME: From here down

    // Step 1: Configure the BMI160 auxiliary interface for I2C passthrough
    // Set the auxiliary interface to use I2C mode
    BMI160_WriteRegister(BMI160_IFCONF_ADDR, 0x40); // Enable I2C for external devices

    // Step 2: Configure the BMM150's I2C address (shifted left by 1)
    BMI160_WriteRegister(BMI160_MAGIF_O, 0x0 << 1); // BMM150 address = 0x20

    // Step 3: Power up the BMM150 magnetometer
    // Write to BMM150's power control register (0x4B)
    BMI160_WriteRegister(BMI160_MAGCONF_ADDR, 0x01); // Normal mode

    // Step 4: Verify communication by reading the BMM150's chip ID
    uint8_t chipID = BMI160_ReadRegister(BMI160_DATA_O + MAGX_O); // Read BMM150 Chip ID register (0x40)

    uint8_t chipID = BMI160_ReadRegister(0x40);

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
