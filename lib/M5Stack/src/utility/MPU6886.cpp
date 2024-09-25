#if defined (CORE)
#include "MPU6886.h"

#include <Arduino.h>
#include <math.h>

MPU6886::MPU6886() {
}

void MPU6886::I2C_Read_NBytes(uint8_t driver_Addr, uint8_t start_Addr,
                              uint8_t number_Bytes, uint8_t* read_Buffer) {
    Wire.beginTransmission(driver_Addr);
    Wire.write(start_Addr);
    Wire.endTransmission(false);
    uint8_t i = 0;
    Wire.requestFrom(driver_Addr, number_Bytes);

    //! Put read results in the Rx buffer
    while (Wire.available()) {
        read_Buffer[i++] = Wire.read();
    }
}

void MPU6886::I2C_Write_NBytes(uint8_t driver_Addr, uint8_t start_Addr,
                               uint8_t number_Bytes, uint8_t* write_Buffer) {
    Wire.beginTransmission(driver_Addr);
    Wire.write(start_Addr);
    Wire.write(*write_Buffer);
    Wire.endTransmission();
}

int MPU6886::Init(void) {
    unsigned char tempdata[1];
    unsigned char regdata;

    Gyscale = GFS_2000DPS;
    Acscale = AFS_8G;

    Wire.begin(21, 22);

    I2C_Read_NBytes(MPU6886_ADDRESS, MPU6886_WHOAMI, 1, tempdata);
    imuId = tempdata[0];
    delay(1);

    regdata = 0x00;
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_PWR_MGMT_1, 1, &regdata);
    delay(10);

    regdata = (0x01 << 7);
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_PWR_MGMT_1, 1, &regdata);
    delay(10);

    regdata = (0x01 << 0);
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_PWR_MGMT_1, 1, &regdata);
    delay(10);

    // +- 8g
    regdata = 0x10;
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_ACCEL_CONFIG, 1, &regdata);
    delay(1);

    // +- 2000 dps
    regdata = 0x18;
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_GYRO_CONFIG, 1, &regdata);
    delay(1);

    // 1khz output
    regdata = 0x01;
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_CONFIG, 1, &regdata);
    delay(1);

    // 2 div, FIFO 500hz out
    regdata = 0x01;
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_SMPLRT_DIV, 1, &regdata);
    delay(1);

    regdata = 0x00;
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_INT_ENABLE, 1, &regdata);
    delay(1);

    regdata = 0x00;
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_ACCEL_CONFIG2, 1, &regdata);
    delay(1);

    regdata = 0x00;
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_USER_CTRL, 1, &regdata);
    delay(1);

    regdata = 0x00;
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_FIFO_EN, 1, &regdata);
    delay(1);

    regdata = 0x22;
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_INT_PIN_CFG, 1, &regdata);
    delay(1);

    regdata = 0x01;
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_INT_ENABLE, 1, &regdata);

    delay(10);

    setGyroFsr(Gyscale);
    setAccelFsr(Acscale);
    return 0;
}

void MPU6886::getAccelAdc(int16_t* ax, int16_t* ay, int16_t* az) {
    uint8_t buf[6];
    I2C_Read_NBytes(MPU6886_ADDRESS, MPU6886_ACCEL_XOUT_H, 6, buf);

    *ax = ((int16_t)buf[0] << 8) | buf[1];
    *ay = ((int16_t)buf[2] << 8) | buf[3];
    *az = ((int16_t)buf[4] << 8) | buf[5];
}

void MPU6886::getGyroAdc(int16_t* gx, int16_t* gy, int16_t* gz) {
    uint8_t buf[6];
    I2C_Read_NBytes(MPU6886_ADDRESS, MPU6886_GYRO_XOUT_H, 6, buf);

    *gx = ((uint16_t)buf[0] << 8) | buf[1];
    *gy = ((uint16_t)buf[2] << 8) | buf[3];
    *gz = ((uint16_t)buf[4] << 8) | buf[5];
}

void MPU6886::getTempAdc(int16_t* t) {
    uint8_t buf[14];
    I2C_Read_NBytes(MPU6886_ADDRESS, MPU6886_TEMP_OUT_H, 14, buf);

    *t = ((uint16_t)buf[6] << 8) | buf[7];
}

//!俯仰，航向，横滚: pitch，yaw，roll，指三维空间中飞行器的旋转状态。
void MPU6886::getAhrsData(float* pitch, float* roll, float* yaw) {
    float accX = 0;
    float accY = 0;
    float accZ = 0;

    float gyroX = 0;
    float gyroY = 0;
    float gyroZ = 0;

    getGyroData(&gyroX, &gyroY, &gyroZ);
    getAccelData(&accX, &accY, &accZ);
    MahonyAHRSupdateIMU(gyroX * DEG_TO_RAD, gyroY * DEG_TO_RAD,
                        gyroZ * DEG_TO_RAD, accX, accY, accZ, pitch, roll, yaw);
}

// Possible gyro scales (and their register bit settings)
void MPU6886::updateGres() {
    switch (Gyscale) {
        case GFS_250DPS:
            gRes = 250.0 / 32768.0;
            break;
        case GFS_500DPS:
            gRes = 500.0 / 32768.0;
            break;
        case GFS_1000DPS:
            gRes = 1000.0 / 32768.0;
            break;
        case GFS_2000DPS:
            gRes = 2000.0 / 32768.0;
            break;
    }
}

// Possible accelerometer scales (and their register bit settings) are:
// 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11).
// Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit
// value:
void MPU6886::updateAres() {
    switch (Acscale) {
        case AFS_2G:
            aRes = 2.0 / 32768.0;
            break;
        case AFS_4G:
            aRes = 4.0 / 32768.0;
            break;
        case AFS_8G:
            aRes = 8.0 / 32768.0;
            break;
        case AFS_16G:
            aRes = 16.0 / 32768.0;
            break;
    }
}

void MPU6886::setGyroFsr(Gscale scale) {
    unsigned char regdata;
    regdata = (scale << 3);
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_GYRO_CONFIG, 1, &regdata);
    delay(10);
    Gyscale = scale;
    updateGres();
}

void MPU6886::setAccelFsr(Ascale scale) {
    unsigned char regdata;
    regdata = (scale << 3);
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_ACCEL_CONFIG, 1, &regdata);
    delay(10);
    Acscale = scale;
    updateAres();
}

void MPU6886::getAccelData(float* ax, float* ay, float* az) {
    int16_t accX = 0;
    int16_t accY = 0;
    int16_t accZ = 0;
    getAccelAdc(&accX, &accY, &accZ);

    *ax = (float)accX * aRes;
    *ay = (float)accY * aRes;
    *az = (float)accZ * aRes;
}

void MPU6886::getGyroData(float* gx, float* gy, float* gz) {
    int16_t gyroX = 0;
    int16_t gyroY = 0;
    int16_t gyroZ = 0;
    getGyroAdc(&gyroX, &gyroY, &gyroZ);

    *gx = (float)gyroX * gRes;
    *gy = (float)gyroY * gRes;
    *gz = (float)gyroZ * gRes;
}

void MPU6886::getTempData(float* t) {
    int16_t temp = 0;
    getTempAdc(&temp);

    *t = (float)temp / 326.8 + 25.0;
}

void MPU6886::setGyroOffset(uint16_t x, uint16_t y, uint16_t z) {
    uint8_t buf_out[6];
    buf_out[0] = x >> 8;
    buf_out[1] = x & 0xff;
    buf_out[2] = y >> 8;
    buf_out[3] = y & 0xff;
    buf_out[4] = z >> 8;
    buf_out[5] = z & 0xff;
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_GYRO_OFFSET, 6, buf_out);
}

void MPU6886::setFIFOEnable(bool enableflag) {
    uint8_t regdata = 0;
    regdata         = enableflag ? 0x18 : 0x00;
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_FIFO_ENABLE, 1, &regdata);
    regdata = enableflag ? 0x40 : 0x00;
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_USER_CTRL, 1, &regdata);
}

uint8_t MPU6886::ReadFIFO() {
    uint8_t ReData = 0;
    I2C_Read_NBytes(MPU6886_ADDRESS, MPU6886_FIFO_R_W, 1, &ReData);
    return ReData;
}

void MPU6886::ReadFIFOBuff(uint8_t* DataBuff, uint16_t Length) {
    uint8_t number = Length / 210;
    for (uint8_t i = 0; i < number; i++) {
        I2C_Read_NBytes(MPU6886_ADDRESS, MPU6886_FIFO_R_W, 210,
                        &DataBuff[i * 210]);
    }

    I2C_Read_NBytes(MPU6886_ADDRESS, MPU6886_FIFO_R_W, Length % 210,
                    &DataBuff[number * 210]);
}

void MPU6886::RestFIFO() {
    uint8_t buf_out;
    I2C_Read_NBytes(MPU6886_ADDRESS, MPU6886_USER_CTRL, 1, &buf_out);
    buf_out |= 0x04;
    I2C_Write_NBytes(MPU6886_ADDRESS, MPU6886_USER_CTRL, 1, &buf_out);
}

uint16_t MPU6886::ReadFIFOCount() {
    uint8_t Buff[2];
    uint16_t ReData = 0;
    I2C_Read_NBytes(MPU6886_ADDRESS, MPU6886_FIFO_COUNT, 2, Buff);
    ReData = (Buff[0] << 8) | Buff[1];
    return ReData;
}
#endif