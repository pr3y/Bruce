#if defined (CORE)
#include "SH200Q.h"

#include <Arduino.h>
#include <math.h>

SH200Q::SH200Q() {
}

void SH200Q::I2C_Read_NBytes(uint8_t driver_Addr, uint8_t start_Addr,
                             uint8_t number_Bytes, uint8_t* read_Buffer) {
    Wire.beginTransmission(driver_Addr);
    Wire.write(start_Addr);
    Wire.endTransmission();
    uint8_t i = 0;
    Wire.requestFrom(driver_Addr, number_Bytes);
    // byte buf = Wire1.read();
    //*read_Buffer = buf;
    //! Put read results in the Rx buffer
    while (Wire.available()) {
        read_Buffer[i++] = Wire1.read();
    }
}

void SH200Q::I2C_Write_NBytes(uint8_t driver_Addr, uint8_t start_Addr,
                              uint8_t number_Bytes, uint8_t* write_Buffer) {
    Wire.beginTransmission(driver_Addr);
    Wire.write(start_Addr);
    Wire.write(*write_Buffer);
    Wire.endTransmission();
    // Serial.printf("I2C Write OP, ADDR: 0x%02x, ADS: 0x%02x, NumBytes: %u,
    // Data: 0x%02x\n\r", driver_Addr, start_Addr, number_Bytes, *write_Buffer);
}

void SH200Q::sh200i_ADCReset(void) {
    unsigned char tempdata[1];
    // set 0xC2 bit2 1-->0
    I2C_Read_NBytes(SH200I_ADDRESS, SH200I_ADC_RESET, 1, tempdata);

    tempdata[0] = tempdata[0] | 0x04;
    I2C_Write_NBytes(SH200I_ADDRESS, SH200I_ADC_RESET, 1, tempdata);
    delay(1);

    tempdata[0] = tempdata[0] & 0xFB;
    // tempdata[0] = 0x0A; //C8
    I2C_Write_NBytes(SH200I_ADDRESS, SH200I_ADC_RESET, 1, tempdata);
}

void SH200Q::sh200i_Reset(void) {
    unsigned char tempdata[1];

    I2C_Read_NBytes(SH200I_ADDRESS, SH200I_RESET, 1, tempdata);

    // SH200I_RESET
    tempdata[0] = tempdata[0] | 0x80;
    I2C_Write_NBytes(SH200I_ADDRESS, SH200I_RESET, 1, tempdata);

    delay(1);

    tempdata[0] = tempdata[0] & 0x7F;
    I2C_Write_NBytes(SH200I_ADDRESS, SH200I_RESET, 1, tempdata);
}

//初始化
int SH200Q::Init(void) {
    unsigned char tempdata[1];
    Gyscale = GFS_2000DPS;
    Acscale = AFS_8G;

    I2C_Read_NBytes(SH200I_ADDRESS, SH200I_WHOAMI, 1, tempdata);
    if (tempdata[0] != 0x18) {
        return -1;
    }
    imuId = tempdata[0];

    sh200i_ADCReset();

    I2C_Read_NBytes(SH200I_ADDRESS, 0xD8, 1, tempdata);

    tempdata[0] = tempdata[0] | 0x80;
    I2C_Write_NBytes(SH200I_ADDRESS, 0xD8, 1, tempdata);

    delay(1);

    tempdata[0] = tempdata[0] & 0x7F;
    I2C_Write_NBytes(SH200I_ADDRESS, 0xD8, 1, tempdata);

    tempdata[0] = 0x61;
    I2C_Write_NBytes(SH200I_ADDRESS, 0x78, 1, tempdata);

    delay(1);

    tempdata[0] = 0x00;
    I2C_Write_NBytes(SH200I_ADDRESS, 0x78, 1, tempdata);

    // set acc odr 256hz
    tempdata[0] = 0x91;  // 0x81 1024hz   //0x89 512hz    //0x91  256hz
    I2C_Write_NBytes(SH200I_ADDRESS, SH200I_ACC_CONFIG, 1, tempdata);

    // set gyro odr 500hz
    tempdata[0] = 0x13;  // 0x11 1000hz    //0x13  500hz   //0x15  256hz
    I2C_Write_NBytes(SH200I_ADDRESS, SH200I_GYRO_CONFIG, 1, tempdata);

    // set gyro dlpf 50hz
    tempdata[0] =
        0x03;  // 0x00 250hz   //0x01 200hz   0x02 100hz  0x03 50hz  0x04 25hz
    I2C_Write_NBytes(SH200I_ADDRESS, SH200I_GYRO_DLPF, 1, tempdata);

    // set no buffer mode
    tempdata[0] = 0x00;
    I2C_Write_NBytes(SH200I_ADDRESS, SH200I_FIFO_CONFIG, 1, tempdata);

    // set acc range +-8G
    tempdata[0] = 0x01;
    I2C_Write_NBytes(SH200I_ADDRESS, SH200I_ACC_RANGE, 1, tempdata);

    // set gyro range +-2000¶È/s
    tempdata[0] = 0x00;
    I2C_Write_NBytes(SH200I_ADDRESS, SH200I_GYRO_RANGE, 1, tempdata);

    tempdata[0] = 0xC0;
    I2C_Write_NBytes(SH200I_ADDRESS, SH200I_REG_SET1, 1, tempdata);

    I2C_Read_NBytes(SH200I_ADDRESS, SH200I_REG_SET2, 1, tempdata);

    // ADC Reset
    tempdata[0] = tempdata[0] | 0x10;
    I2C_Write_NBytes(SH200I_ADDRESS, SH200I_REG_SET2, 1, tempdata);

    delay(1);

    tempdata[0] = tempdata[0] & 0xEF;
    I2C_Write_NBytes(SH200I_ADDRESS, SH200I_REG_SET2, 1, tempdata);

    delay(10);

    setGyroFsr(Gyscale);
    setAccelFsr(Acscale);
    return 0;
}

// Possible gyro scales (and their register bit settings) are:
// 250 DPS (00), 500 DPS (01), 1000 DPS (10), and 2000 DPS  (11).
// Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit
// value:
void SH200Q::updateGres() {
    switch (Gyscale) {
        case GFS_125DPS:
            gRes = 125.0 / 32768.0;
            break;
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
void SH200Q::updateAres() {
    switch (Acscale) {
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

void SH200Q::setGyroFsr(Gscale scale) {
    uint8_t regData;
    I2C_Read_NBytes(SH200I_ADDRESS, SH200I_GYRO_RANGE, 1, &regData);
    regData = (regData & 0xf8) | (scale & 0x07);
    I2C_Write_NBytes(SH200I_ADDRESS, SH200I_GYRO_RANGE, 1, &regData);
    delay(10);
    Gyscale = scale;
    updateGres();
}

void SH200Q::setAccelFsr(Ascale scale) {
    uint8_t regData;
    I2C_Read_NBytes(SH200I_ADDRESS, SH200I_ACC_RANGE, 1, &regData);
    regData = (regData & 0xf8) | (scale & 0x07);
    I2C_Write_NBytes(SH200I_ADDRESS, SH200I_ACC_RANGE, 1, &regData);
    delay(10);
    Acscale = scale;
    updateAres();
}

void SH200Q::getAccelAdc(int16_t* ax, int16_t* ay, int16_t* az) {
    uint8_t buf[6];
    I2C_Read_NBytes(SH200I_ADDRESS, SH200I_OUTPUT_ACC, 6, buf);

    *ax = (int16_t)((buf[1] << 8) | buf[0]);
    *ay = (int16_t)((buf[3] << 8) | buf[2]);
    *az = (int16_t)((buf[5] << 8) | buf[4]);
}

void SH200Q::getAccelData(float* ax, float* ay, float* az) {
    uint8_t buf[6];
    I2C_Read_NBytes(SH200I_ADDRESS, SH200I_OUTPUT_ACC, 6, buf);

    *ax = (int16_t)((buf[1] << 8) | buf[0]) * aRes;
    *ay = (int16_t)((buf[3] << 8) | buf[2]) * aRes;
    *az = (int16_t)((buf[5] << 8) | buf[4]) * aRes;
}

void SH200Q::getGyroAdc(int16_t* gx, int16_t* gy, int16_t* gz) {
    uint8_t buf[6];
    I2C_Read_NBytes(SH200I_ADDRESS, SH200I_OUTPUT_GYRO, 6, buf);

    *gx = (int16_t)((buf[1] << 8) | buf[0]);
    *gy = (int16_t)((buf[3] << 8) | buf[2]);
    *gz = (int16_t)((buf[5] << 8) | buf[4]);
}

void SH200Q::getGyroData(float* gx, float* gy, float* gz) {
    uint8_t buf[6];
    I2C_Read_NBytes(SH200I_ADDRESS, SH200I_OUTPUT_GYRO, 6, buf);

    *gx = (int16_t)((buf[1] << 8) | buf[0]) * gRes;
    *gy = (int16_t)((buf[3] << 8) | buf[2]) * gRes;
    *gz = (int16_t)((buf[5] << 8) | buf[4]) * gRes;
}

void SH200Q::getTempAdc(int16_t* t) {
    uint8_t buf[2];
    I2C_Read_NBytes(SH200I_ADDRESS, SH200I_OUTPUT_TEMP, 2, buf);
    *t = (int16_t)((buf[1] << 8) | buf[0]);
}

void SH200Q::getTempData(float* t) {
    uint8_t buf[2];
    I2C_Read_NBytes(SH200I_ADDRESS, SH200I_OUTPUT_TEMP, 2, buf);
    *t = (int16_t)((buf[1] << 8) | buf[0]) / 333.87 + 21.0;
}

//!俯仰，航向，横滚: pitch，yaw，roll，指三维空间中飞行器的旋转状态。
void SH200Q::getAhrsData(float* pitch, float* roll, float* yaw) {
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
#endif