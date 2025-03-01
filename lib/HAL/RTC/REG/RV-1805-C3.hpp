#ifndef __RV_1805_C3__
#define __RV_1805_C3__

#define RV1805C3_ADDRESS		0x69

/* Register Map */

#define REG_TIME_HUNDREDTHS		0x00 // Hundredths
#define REG_TIME_SECONDS		0x01 // Seconds
#define REG_TIME_MINUTES		0x02 // Minutes
#define REG_TIME_HOURS			0x03 // Hours
#define REG_DATE				0x04 // Date
#define REG_MONTHS				0x05 // Months
#define REG_YEARS				0x06 // Years
#define REG_WEEKDAYS			0x07 // Weekdays
#define REG_ALARM_HUNDREDTHS	0x08 // Hundredths Alarm
#define REG_ALARM_SECONDS		0x09 // Seconds Alarm
#define REG_ALARM_MINUTES		0x0A // Minutes Alarm
#define REG_ALARM_HOURS			0x0B // Hours Alarm
#define REG_ALARM_DATE			0x0C // Date Alarm
#define REG_ALARM_MONTHS		0x0D // Months Alarm
#define REG_ALARM_WEEKDAYS		0x0E // Weekdays Alarm
#define REG_STATUS				0x0F // Status
#define REG_CONTROL1			0x10 // Control1
#define REG_CONTROL2			0x11 // Control2
#define REG_INTERRUPT_MASK		0x12 // Interrupt Mask
#define REG_SQUARE_WAVE			0x13 // Square Wave SQW
#define REG_CAL_XT				0x14 // Calibration XT
#define REG_CAL_RC_UPPER		0x15 // Calibration RC Upper
#define REG_CAL_RC_LOWER		0x16 // Calibration RC Lower
#define REG_SLEEP_CONTROL		0x17 // Sleep Control
#define REG_COUNTDOWN_CONTROL	0x18 // Countdown Timer Control
#define REG_COUNTDOWN_TIMER		0x19 // Countdown Timer
#define REG_TIMER_INIT_VALUE	0x1A // Timer Initial Value
#define REG_WATCHDOG_TIMER		0x1B // Watchdog Timer
#define REG_OSC_CONTROL			0x1C // Oscillator Control
#define REG_OSC_STATUS			0x1D // Oscillator Status Register
#define REG_CONFIG_KEY			0x1F // Configuration Key
#define REG_TRICKLE_CHARGE		0x20 // Trickle Charge
#define REG_BREF_CONTROL		0x21 // BREF Control
#define REG_CAP_RC_CONTROL		0x26 // Cap_RC Control
#define REG_IO_BATMODE			0x27 // IO Batmode Register
#define REG_ID0					0x28 // ID0 (R)
#define REG_ID1					0x29 // ID1 (R)
#define REG_ANALOG_STAT			0x2F // Analog Stat. (R)
#define REG_OUTPUT_CONTROL		0x30 // Output Control Register

#define PART_NUMBER_MSB			0x18
#define PART_NUMBER_LSB			0x05

#define CONFKEY_OSC_CONTROL		0xA1
#define CONFKEY_RESET			0x3C
#define CONFKEY_REGISTERS		0x9D

#define DATETIME_COMPONENTS		8

enum DateTimeComponent {
	DATETIME_HUNDREDTH = 0,
	DATETIME_SECOND = 1,
	DATETIME_MINUTE = 2,
	DATETIME_HOUR = 3,
	DATETIME_DAY_OF_MONTH = 4,
	DATETIME_MONTH = 5,
	DATETIME_YEAR = 6,
	DATETIME_DAY_OF_WEEK = 7
};

enum DayOfWeek {
	SUN = 0,
	MON = 1,
	TUE = 2,
	WED = 3,
	THU = 4,
	FRI = 5,
	SAT = 6
};

enum AlarmMode {
	ALARM_DISABLED = 0,
	ALARM_ONCE_PER_YEAR = 1,
	ALARM_ONCE_PER_MONTH = 2,
	ALARM_ONCE_PER_WEEK = 3,
	ALARM_ONCE_PER_DAY = 4,
	ALARM_ONCE_PER_HOUR = 5,
	ALARM_ONCE_PER_MINUTE = 6,
	ALARM_ONCE_PER_SECOND = 7,
	ALARM_ONCE_PER_TENTH = 8,
	ALARM_ONCE_PER_HUNDREDTH = 9
};

enum CountdownUnit {
	COUNTDOWN_SECONDS = 0b10,
	COUNTDOWN_MINUTES = 0b11
};

enum InterruptType {
	INTERRUPT_WATCHDOG_TIMER = 5,
	INTERRUPT_BATTERY_LOW = 4,
	INTERRUPT_TIMER = 3,
	INTERRUPT_ALARM = 2,
	INTERRUPT_EXTERNAL = 1
};

enum SleepWaitPeriod {
	SLEEP_IMMEDIATELY = 0b000,
	SLEEP_WAIT_8MS = 0b001,
	SLEEP_WAIT_16MS = 0b010,
	SLEEP_WAIT_24MS = 0b011,
	SLEEP_WAIT_32MS = 0b100,
	SLEEP_WAIT_40MS = 0b101,
	SLEEP_WAIT_48MS = 0b110,
	SLEEP_WAIT_56MS = 0b111
};

enum PowerSwitchFunction {
	PSWS_INVERSE_COMBINED_IRQ = 0b000,
	PSWS_SQW = 0b001,
	PSWS_INVERSE_AIRQ = 0b011,
	PSWS_TIRQ = 0b100,
	PSWS_INVERSE_TIRQ = 0b101,
	PSWS_SLEEP = 0b110,
	PSWS_STATIC = 0b111
};

#endif  /* __RV_1805_C3__ */
