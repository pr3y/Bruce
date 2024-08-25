#if defined (CORE)
#ifndef _SH200Q_H_
#define _SH200Q_H_

#include <Arduino.h>
#include <Wire.h>

#include "MahonyAHRS.h"

#define SH200I_ADDRESS     0x6C  // 7bit i2c address
#define SH200I_WHOAMI      0x30
#define SH200I_ACC_CONFIG  0x0E
#define SH200I_GYRO_CONFIG 0x0F
#define SH200I_GYRO_DLPF   0x11
#define SH200I_FIFO_CONFIG 0x12
#define SH200I_ACC_RANGE   0x16
#define SH200I_GYRO_RANGE  0x2B
#define SH200I_OUTPUT_ACC  0x00
#define SH200I_OUTPUT_GYRO 0x06
#define SH200I_OUTPUT_TEMP 0x0C
#define SH200I_REG_SET1    0xBA
#define SH200I_REG_SET2    0xCA  // ADC reset
#define SH200I_ADC_RESET   0xC2  // drive reset
#define SH200I_SOFT_RESET  0x7F
#define SH200I_RESET       0x75

//#define G (9.8)
#define RtA     57.324841
#define AtR     0.0174533
#define Gyro_Gr 0.0010653

class SH200Q {
   public:
    enum Ascale { AFS_4G = 0, AFS_8G, AFS_16G };

    enum Gscale {
        GFS_2000DPS = 0,
        GFS_1000DPS,
        GFS_500DPS,
        GFS_250DPS,
        GFS_125DPS,
    };

   public:
    SH200Q();

    int Init(void);

    void getAccelAdc(int16_t* ax, int16_t* ay, int16_t* az);
    void getGyroAdc(int16_t* gx, int16_t* gy, int16_t* gz);
    void getTempAdc(int16_t* t);

    void getAccelData(float* ax, float* ay, float* az);
    void getGyroData(float* gx, float* gy, float* gz);
    void getTempData(float* t);
    void getAhrsData(float* pitch, float* roll, float* yaw);

    void setGyroFsr(Gscale scale);
    void setAccelFsr(Ascale scale);

   public:
    float aRes, gRes;
    uint8_t imuId;
    Gscale Gyscale;
    Ascale Acscale;

   protected:
    void I2C_Read_NBytes(uint8_t driver_Addr, uint8_t start_Addr,
                         uint8_t number_Bytes, uint8_t* read_Buffer);
    void I2C_Write_NBytes(uint8_t driver_Addr, uint8_t start_Addr,
                          uint8_t number_Bytes, uint8_t* write_Buffer);
    void sh200i_ADCReset(void);
    void sh200i_Reset(void);
    void updateGres();
    void updateAres();
};
#endif
#endif