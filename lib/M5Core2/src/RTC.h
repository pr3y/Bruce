#if defined (CORE2)
#ifndef __RTC_H__
#define __RTC_H__

#include <Wire.h>

typedef struct {
    int8_t Hours;
    int8_t Minutes;
    int8_t Seconds;
} RTC_TimeTypeDef;

typedef struct {
    int8_t WeekDay;
    int8_t Month;
    int8_t Date;
    int16_t Year;
} RTC_DateTypeDef;

class RTC {
   public:
    RTC();

    void begin(void);
    void GetBm8563Time(void);

    int SetTime(RTC_TimeTypeDef* RTC_TimeStruct);
    int SetDate(RTC_DateTypeDef* RTC_DateStruct);

    void GetTime(RTC_TimeTypeDef* RTC_TimeStruct);
    void GetDate(RTC_DateTypeDef* RTC_DateStruct);

    int SetAlarmIRQ(int afterSeconds);
    int SetAlarmIRQ(const RTC_TimeTypeDef& RTC_TimeStruct);
    int SetAlarmIRQ(const RTC_DateTypeDef& RTC_DateStruct,
                    const RTC_TimeTypeDef& RTC_TimeStruct);

    void clearIRQ();
    void disableIRQ();
  
    bool getVoltLow(void);

   public:
    uint8_t Second;
    uint8_t Minute;
    uint8_t Hour;
    uint8_t Week;
    uint8_t Day;
    uint8_t Month;
    uint8_t Year;
    uint8_t DateString[9];
    uint8_t TimeString[9];

    uint8_t asc[14];

   private:
    void Bcd2asc(void);
    void DataMask();
    void Str2Time(void);
    void WriteReg(uint8_t reg, uint8_t data);
    uint8_t ReadReg(uint8_t reg);
    uint8_t Bcd2ToByte(uint8_t Value);
    uint8_t ByteToBcd2(uint8_t Value);

   private:
    /*定义数组用来存储读取的时间数据 */
    uint8_t trdata[7];
    /*定义数组用来存储转换的 asc 码时间数据*/
    // uint8_t asc[14];
};

#endif
#endif