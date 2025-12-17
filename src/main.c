#include <stdio.h>
#include <string.h>
#include <math.h>
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/clocks.h"
#include "hardware/vreg.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

#include "../inc/mpu6050_registers.h"

// --- Constants ---
#define I2C_PORT        i2c0
#define SDA_PIN         4
#define SCL_PIN         5
#define MPU6050_ADDR    0x68

// Registers
#define REG_PWR_MGMT_1  0x6B
#define REG_WHO_AM_I    0x75
#define REG_GYRO_XOUT_H 0x43

// Sensitivity Scale Factor
// For range +/- 250 degrees/sec (default), scale is 131.0
#define GYRO_SCALE      131.0

/*
void mpu6050_init(){
    uint8_t data[] = {REG_PWR_MGMT_1, 0x00};
    i2c_write_blocking(i2c_default, MPU6050_ADDR, data, 2, false); //wake chip
}
*/

void mpu6050_init() {
    // 1. Wake up (Write 0 to PWR_MGMT_1)
    uint8_t buf[] = {REG_PWR_MGMT_1, 0x00};
    i2c_write_blocking(I2C_PORT, MPU6050_ADDR, buf, 2, false);
    
    // 2. Set Gyro Range to +/- 250 dps (Write 0 to GYRO_CONFIG)
    // This is the most sensitive setting, perfect for hand gestures.
    uint8_t conf[] = {REG_GYRO_CONFIG, 0x00};
    i2c_write_blocking(I2C_PORT, MPU6050_ADDR, conf, 2, false);
}

// Reads two bytes (High + Low) and combines them into a signed 16-bit int
int16_t mpu6050_read_axis(uint8_t reg) {
    uint8_t buffer[2];
    
    // Point to the High Byte register
    i2c_write_blocking(I2C_PORT, MPU6050_ADDR, &reg, 1, true);
    
    // Read 2 bytes sequentially (High then Low)
    i2c_read_blocking(I2C_PORT, MPU6050_ADDR, buffer, 2, false);
    
    // Combine: (High << 8) | Low
    return (int16_t)((buffer[0] << 8) | buffer[1]);
}

int main() {
    stdio_init_all();
    
    // Init I2C at 400kHz (Standard Speed)
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    sleep_ms(2000); // Wait for Serial Monitor
    printf("MPU6050 Gyroscope Demo Starting...\n");

    mpu6050_init();

    while (1) {
        // Read Raw values (16-bit integers)
        // Registers are sequential: X_H, X_L, Y_H, Y_L, Z_H, Z_L
        int16_t raw_x = mpu6050_read_axis(REG_GYRO_XOUT_H);
        int16_t raw_y = mpu6050_read_axis(REG_GYRO_XOUT_H + 2);
        int16_t raw_z = mpu6050_read_axis(REG_GYRO_XOUT_H + 4);

        // Convert to Degrees Per Second (dps)
        float gyro_x = raw_x / GYRO_SCALE;
        float gyro_y = raw_y / GYRO_SCALE;
        float gyro_z = raw_z / GYRO_SCALE;

        // Print formatted output (clears screen slightly with \r)
        // Pitch (X), Roll (Y), Yaw (Z)
        printf("Rot X: %6.2f | Rot Y: %6.2f | Rot Z: %6.2f (deg/s)\r", gyro_x, gyro_y, gyro_z);
        fflush(stdout);
        
        sleep_ms(100); // 10Hz Update Rate
    }
    return 0;
}