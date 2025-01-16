#ifndef __BM8563__
#define __BM8563__

#define BM8563_I2C_ADDRESS     (0x51)
#define BM8563_CONTROL_STATUS1 (0x00)
#define BM8563_CONTROL_STATUS2 (0x01)
#define BM8563_SECONDS         (0x02)
#define BM8563_MINUTES         (0x03)
#define BM8563_HOURS           (0x04)
#define BM8563_DAY             (0x05)
#define BM8563_WEEKDAY         (0x06)
#define BM8563_MONTH           (0x07)
#define BM8563_YEAR            (0x08)
#define BM8563_MINUTE_ALARM    (0x09)
#define BM8563_HOUR_ALARM      (0x0a)
#define BM8563_DAY_ALARM       (0x0b)
#define BM8563_WEEKDAY_ALARM   (0x0c)
#define BM8563_CLKOUT_CONTROL  (0x0d)
#define BM8563_TIMER_CONTROL   (0x0e)
#define BM8563_TIMER           (0x0f)

#define BM8563_TESTC           (0b00001000)
#define BM8563_STOP            (0b00100000)
#define BM8563_TEST1           (0b10000000)

#define BM8563_TIE             (0b00000001)
#define BM8563_AIE             (0b00000010)
#define BM8563_TF              (0b00000100)
#define BM8563_AF              (0b00001000)
#define BM8563_TI_TP           (0b00010000)
#define BM8563_TIME_SIZE       (0x07)
#define BM8563_CENTURY_BIT     (0b10000000)

#define BM8563_ALARM_DISABLE   (0b10000000)
#define BM8563_ALARM_NONE      (0xff)
#define BM8563_ALARM_SIZE      (0x04)

#define BM8563_TIMER_ENABLE    (0b10000000)
#define BM8563_TIMER_4_096KHZ  (0b00000000)
#define BM8563_TIMER_64HZ      (0b00000001)
#define BM8563_TIMER_1HZ       (0b00000010)
#define BM8563_TIMER_1_60HZ    (0b00000011)

/* IOCTL commands */
#define BM8563_ALARM_SET             (0x0900)
#define BM8563_ALARM_READ            (0x0901)
#define BM8563_CONTROL_STATUS1_READ  (0x0000)
#define BM8563_CONTROL_STATUS1_WRITE (0x0001)
#define BM8563_CONTROL_STATUS2_READ  (0x0100)
#define BM8563_CONTROL_STATUS2_WRITE (0x0101)
#define BM8563_TIMER_CONTROL_READ    (0x0e00)
#define BM8563_TIMER_CONTROL_WRITE   (0x0e01)
#define BM8563_TIMER_READ            (0x0f00)
#define BM8563_TIMER_WRITE           (0x0f01)

/* Status codes. */
#define BM8563_ERROR_NOTTY     (-1)
#define BM8563_OK              (0x00)
#define BM8563_ERR_LOW_VOLTAGE (0x80)

#endif /* __BM8563__ */
