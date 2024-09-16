#include "multimod_BME280.h"
#include "multimod_i2c.h" // Assuming you have an I2C library for communication

#include <stdint.h>
#include <stdbool.h>

// Calibration data variables
uint16_t dig_T1;
int16_t dig_T2, dig_T3;

void BME280_Init(void)
{
    // Initialize I2C module (assuming you are using I2C1)
    I2C_Init(I2C1_BASE); // Replace with the correct I2C base used for BME280

    // Soft reset the sensor
    BME280_WriteRegister(BME280_REG_RESET, 0xB6);

    // Read calibration data from the sensor
    BME280_ReadCalibrationData();

    // Configure sensor settings for temperature measurement only
    BME280_WriteRegister(BME280_REG_CTRL_MEAS, 0x20); // Temperature oversampling x1, forced mode
}

void BME280_ReadCalibrationData(void)
{
    uint8_t calib[6]; // Calibration data for temperature

    // Read the calibration data from registers 0x88 to 0x8D
    BME280_ReadRegister(BME280_CALIB_DATA_START, calib, 6);

    // Parse temperature calibration data
    dig_T1 = (calib[1] << 8) | calib[0];
    dig_T2 = (calib[3] << 8) | calib[2];
    dig_T3 = (calib[5] << 8) | calib[4];
}

void BME280_ReadTemperature(void)
{
    uint8_t data[3];

    // Burst read from register 0xFA to 0xFC (temperature data)
    BME280_ReadRegister(BME280_REG_TEMP_MSB, data, 3);

    int32_t adc_T = ((int32_t)(data[0]) << 12) | ((int32_t)(data[1]) << 4) | ((int32_t)(data[2] >> 4));

    // Apply compensation formula to get calibrated temperature value
    float temperature = BME280_CompensateTemperature(adc_T);

    // Output the result
    UARTprintf("Temperature: %.2f °C\n", temperature);
}

void BME280_WriteRegister(uint8_t addr, uint8_t data)
{
    uint8_t buffer[2] = {addr, data};
    I2C_WriteMultiple(I2C1_BASE, BME280_I2C_ADDR, buffer, 2);
}

void BME280_ReadRegister(uint8_t addr, uint8_t *data, uint8_t len)
{
    I2C_ReadMultiple(I2C1_BASE, addr, data, len);
}

// Compensation formula based on BME280 datasheet
float BME280_CompensateTemperature(int32_t adc_T)
{
    int32_t var1, var2;
    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
    int32_t t_fine = var1 + var2;
    float T = (t_fine * 5 + 128) >> 8;
    return T / 100.0;
}