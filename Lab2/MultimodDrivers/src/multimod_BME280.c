#include "../multimod_BME280.h"
#include "../multimod_i2c.h"
#include "../multimod_uart.h" // Include your UART library for UARTprintf()

// Calibration data variables
static uint16_t dig_T1;
static int16_t dig_T2, dig_T3;

// Function to initialize the BME280 sensor
void BME280_Init(void)
{
    // Initialize I2C module (assuming I2C1 is used)
    I2C_Init(I2C1_BASE);

    // Soft reset the BME280 sensor by writing 0xB6 to the RESET register (0xE0)
    I2C_WriteSingle(I2C1_BASE, BME280_I2C_ADDR, BME280_REG_RESET); // Set register address
    I2C_WriteSingle(I2C1_BASE, BME280_I2C_ADDR, 0xB6);             // Reset command

    // Wait for the reset to complete (2 ms delay)
    SysCtlDelay(SysCtlClockGet() / (1000 * 3)); // Approximate delay for 2 ms

    // Read temperature calibration data from the sensor (registers 0x88 to 0x8D)
    uint8_t calib[6];
    I2C_WriteSingle(I2C1_BASE, BME280_I2C_ADDR, 0x88); // Set starting register address

    for (uint8_t i = 0; i < 6; i++)
    {
        calib[i] = I2C_ReadSingle(I2C1_BASE, BME280_I2C_ADDR); // Sequentially read calibration data
    }

    // Parse temperature calibration data
    dig_T1 = (calib[1] << 8) | calib[0];
    dig_T2 = (calib[3] << 8) | calib[2];
    dig_T3 = (calib[5] << 8) | calib[4];

    // Debug prints for calibration data (optional)
    UARTprintf("dig_T1: %u\n", dig_T1);
    UARTprintf("dig_T2: %d\n", dig_T2);
    UARTprintf("dig_T3: %d\n", dig_T3);

    // Set the sensor to forced mode, temperature oversampling x1
    I2C_WriteSingle(I2C1_BASE, BME280_I2C_ADDR, BME280_REG_CTRL_MEAS); // Set register address
    I2C_WriteSingle(I2C1_BASE, BME280_I2C_ADDR, 0x25);                 // 0x20 for temperature oversampling x1, 0x05 for forced mode (0b00000101)
}

// Function to read the temperature from BME280 and return it as an int32_t in units of 0.01°C
int32_t BME280_ReadTemperature(void)
{
    uint8_t temp_raw[3];

    // Set the starting register address to read temperature data (registers 0xFA, 0xFB, 0xFC)
    I2C_WriteSingle(I2C1_BASE, BME280_I2C_ADDR, BME280_REG_TEMP_MSB); // Set starting register address

    // Read the raw temperature data from the sensor
    temp_raw[0] = I2C_ReadSingle(I2C1_BASE, BME280_I2C_ADDR); // Read MSB
    temp_raw[1] = I2C_ReadSingle(I2C1_BASE, BME280_I2C_ADDR); // Read LSB
    temp_raw[2] = I2C_ReadSingle(I2C1_BASE, BME280_I2C_ADDR); // Read XLSB

    // Convert the raw temperature data to a 20-bit integer
    int32_t adc_T = ((int32_t)(temp_raw[0]) << 12) | ((int32_t)(temp_raw[1]) << 4) | ((int32_t)(temp_raw[2] >> 4));

    // Compensate temperature using calibration data
    int32_t var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    int32_t var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
    int32_t t_fine = var1 + var2;
    int32_t temperature = (t_fine * 5 + 128) >> 8; // Temperature in 0.01°C

    return temperature; // Returns temperature in 0.01°C
}
