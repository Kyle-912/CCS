#include "multimod_BME280.h"
#include "multimod_i2c.h" // Include your actual I2C library

// Calibration data variables
static uint16_t dig_T1;
static int16_t dig_T2, dig_T3;

// Function to initialize the BME280 sensor
void BME280_Init(void)
{
    // Initialize I2C module (assuming I2C1 is used)
    I2C_Init(I2C1_BASE); // Replace with your I2C initialization function

    // Soft reset the BME280 sensor
    I2C_WriteSingle(I2C1_BASE, BME280_I2C_ADDR, BME280_REG_RESET); // Correct number of arguments

    // Read temperature calibration data from the sensor
    uint8_t calib[6];
    for (uint8_t i = 0; i < 6; i++)
    {
        calib[i] = I2C_ReadSingle(I2C1_BASE, BME280_I2C_ADDR); // Correct number of arguments
    }

    // Parse temperature calibration data
    dig_T1 = (calib[1] << 8) | calib[0];
    dig_T2 = (calib[3] << 8) | calib[2];
    dig_T3 = (calib[5] << 8) | calib[4];

    // Set the sensor to forced mode, temperature oversampling x1
    I2C_WriteSingle(I2C1_BASE, BME280_I2C_ADDR, BME280_REG_CTRL_MEAS); // Correct number of arguments
}

// Function to read the temperature from BME280 and return it as an int32_t in units of 0.01°C
int32_t BME280_ReadTemperature(void)
{
    uint8_t temp_raw[3];

    // Read the raw temperature data from the sensor
    temp_raw[0] = I2C_ReadSingle(I2C1_BASE, BME280_I2C_ADDR); // Correct number of arguments
    temp_raw[1] = I2C_ReadSingle(I2C1_BASE, BME280_I2C_ADDR); // Correct number of arguments
    temp_raw[2] = I2C_ReadSingle(I2C1_BASE, BME280_I2C_ADDR); // Correct number of arguments

    // Convert the raw temperature data to a 20-bit integer
    int32_t adc_T = ((int32_t)(temp_raw[0]) << 12) | ((int32_t)(temp_raw[1]) << 4) | ((int32_t)(temp_raw[2] >> 4));

    // Compensate temperature using calibration data
    int32_t var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    int32_t var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
    int32_t t_fine = var1 + var2;
    int32_t temperature = (t_fine * 5 + 128) >> 8; // Temperature in 0.01°C

    return temperature; // Returns temperature in 0.01°C
}
