#if defined (CORE2)
#include "AXP192.h"
#include "AXP.h"

AXP192::AXP192() {
}

void AXP192::begin() {
    // Wire1.begin(21, 22);
    // Wire1.setClock(400000);

    // AXP192 30H
    Write1Byte(0x30, (Read8bit(0x30) & 0x04) | 0X02);
    Serial.printf("axp: vbus limit off\n");

    // AXP192 GPIO1:OD OUTPUT
    Write1Byte(0x92, Read8bit(0x92) & 0xf8);
    Serial.printf("axp: gpio1 init\n");

    // AXP192 GPIO2:OD OUTPUT
    Write1Byte(0x93, Read8bit(0x93) & 0xf8);
    Serial.printf("axp: gpio2 init\n");

    // AXP192 RTC CHG
    Write1Byte(0x35, (Read8bit(0x35) & 0x1c) | 0xa2);
    Serial.printf("axp: rtc battery charging enabled\n");

    SetESPVoltage(3350);
    Serial.printf("axp: esp32 power voltage was set to 3.35v\n");

    SetLcdVoltage(2800);
    Serial.printf("axp: lcd backlight voltage was set to 2.80v\n");

    SetLDOVoltage(2, 3300);  // Periph power voltage preset (LCD_logic, SD card)
    Serial.printf("axp: lcd logic and sdcard voltage preset to 3.3v\n");

    SetLDOVoltage(3, 2000);  // Vibrator power voltage preset
    Serial.printf("axp: vibrator voltage preset to 2v\n");

    SetLDOEnable(2, true);
    SetDCDC3(true);  // LCD backlight
    SetLed(true);

    SetCHGCurrent(kCHG_100mA);
    // SetAxpPriphPower(1);
    // Serial.printf("axp: lcd_logic and sdcard power enabled\n\n");

    // pinMode(39, INPUT_PULLUP);

    // AXP192 GPIO4
    Write1Byte(0X95, (Read8bit(0x95) & 0x72) | 0X84);

    Write1Byte(0X36, 0X4C);

    Write1Byte(0x82, 0xff);

    SetLCDRSet(0);
    delay(100);
    SetLCDRSet(1);
    delay(100);
    // I2C_WriteByteDataAt(0X15,0XFE,0XFF);

    // axp: check v-bus status
    if (Read8bit(0x00) & 0x08) {
        Write1Byte(0x30, Read8bit(0x30) | 0x80);
        // if v-bus can use, disable M-Bus 5V output to input
        SetBusPowerMode(kMBusModeInput);
    } else {
        // if not, enable M-Bus 5V output
        SetBusPowerMode(kMBusModeOutput);
    }
}

void AXP192::ScreenBreath(int brightness) {
    if (brightness >= 100)
        brightness = 100;
    else if (brightness < 0)
        brightness = 0;
    int vol = map(brightness, 0, 100, 2400, 3300);
    SetLcdVoltage((uint16_t)vol);
}

bool AXP192::GetBatState() {
    if (Read8bit(0x01) | 0x20)
        return true;
    else
        return false;
}
//---------coulombcounter_from_here---------
// enable: void EnableCoulombcounter(void);
// disable: void DisableCOulombcounter(void);
// stop: void StopCoulombcounter(void);
// clear: void ClearCoulombcounter(void);
// get charge data: uint32_t GetCoulombchargeData(void);
// get discharge data: uint32_t GetCoulombdischargeData(void);
// get coulomb val affter calculation: float GetCoulombData(void);
//------------------------------------------
void AXP192::EnableCoulombcounter(void) {
    Write1Byte(0xB8, 0x80);
}

void AXP192::DisableCoulombcounter(void) {
    Write1Byte(0xB8, 0x00);
}

void AXP192::StopCoulombcounter(void) {
    Write1Byte(0xB8, 0xC0);
}

void AXP192::ClearCoulombcounter(void) {
    Write1Byte(0xB8, 0xA0);
}

uint32_t AXP192::GetCoulombchargeData(void) {
    return Read32bit(0xB0);
}

uint32_t AXP192::GetCoulombdischargeData(void) {
    return Read32bit(0xB4);
}

float AXP192::GetCoulombData(void) {
    uint32_t coin  = 0;
    uint32_t coout = 0;

    coin  = GetCoulombchargeData();
    coout = GetCoulombdischargeData();

    // c = 65536 * current_LSB * (coin - coout) / 3600 / ADC rate
    // Adc rate can be read from 84H ,change this variable if you change the ADC
    // reate
    float ccc = 65536 * 0.5 * (int32_t)(coin - coout) / 3600.0 / 25.0;
    return ccc;
}

// Cut all power, except for LDO1 (RTC)
void AXP192::PowerOff(void) {
    Write1Byte(0x32, Read8bit(0x32) | 0b10000000);
}

void AXP192::SetAdcState(bool state) {
    // Enable / Disable all ADCs
    Write1Byte(0x82, state ? 0xff : 0x00);
}

void AXP192::PrepareToSleep(void) {
    // Disable ADCs
    SetAdcState(false);

    // Turn LED off
    SetLed(false);

    // Turn LCD backlight off
    SetDCDC3(false);
}

// Get current battery level
float AXP192::GetBatteryLevel(void) {
    const float batVoltage = GetBatVoltage();
    const float batPercentage =
        (batVoltage < 3.248088) ? (0) : (batVoltage - 3.120712) * 100;
    return (batPercentage <= 100) ? batPercentage : 100;
}

void AXP192::RestoreFromLightSleep(void) {
    // Turn LCD backlight on
    SetDCDC3(true);

    // Turn LED on
    SetLed(true);

    // Enable ADCs
    SetAdcState(true);
}

uint8_t AXP192::GetWarningLeve(void) {
    Wire1.beginTransmission(AXP192_ADDR);
    Wire1.write(0x47);
    Wire1.endTransmission();
    Wire1.requestFrom(AXP192_ADDR, 1);
    uint8_t buf = Wire1.read();
    return (buf & 0x01);
}

// -- sleep
void AXP192::DeepSleep(uint64_t time_in_us) {
    PrepareToSleep();

    if (time_in_us > 0) {
        esp_sleep_enable_timer_wakeup(time_in_us);
    } else {
        esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
    }
    (time_in_us == 0) ? esp_deep_sleep_start() : esp_deep_sleep(time_in_us);

    // Never reached - after deep sleep ESP32 restarts
}

void AXP192::LightSleep(uint64_t time_in_us) {
    PrepareToSleep();

    if (time_in_us > 0) {
        esp_sleep_enable_timer_wakeup(time_in_us);
    } else {
        esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
    }
    esp_light_sleep_start();

    RestoreFromLightSleep();
}

uint8_t AXP192::GetWarningLevel(void) {
    return Read8bit(0x47) & 0x01;
}

float AXP192::GetBatVoltage() {
    float ADCLSB    = 1.1 / 1000.0;
    uint16_t ReData = Read12Bit(0x78);
    return ReData * ADCLSB;
}

float AXP192::GetBatCurrent() {
    float ADCLSB        = 0.5;
    uint16_t CurrentIn  = Read13Bit(0x7A);
    uint16_t CurrentOut = Read13Bit(0x7C);
    return (CurrentIn - CurrentOut) * ADCLSB;
}

float AXP192::GetVinVoltage() {
    float ADCLSB    = 1.7 / 1000.0;
    uint16_t ReData = Read12Bit(0x56);
    return ReData * ADCLSB;
}

float AXP192::GetVinCurrent() {
    float ADCLSB    = 0.625;
    uint16_t ReData = Read12Bit(0x58);
    return ReData * ADCLSB;
}

float AXP192::GetVBusVoltage() {
    float ADCLSB    = 1.7 / 1000.0;
    uint16_t ReData = Read12Bit(0x5A);
    return ReData * ADCLSB;
}

float AXP192::GetVBusCurrent() {
    float ADCLSB    = 0.375;
    uint16_t ReData = Read12Bit(0x5C);
    return ReData * ADCLSB;
}

float AXP192::GetTempInAXP192() {
    float ADCLSB             = 0.1;
    const float OFFSET_DEG_C = -144.7;
    uint16_t ReData          = Read12Bit(0x5E);
    return OFFSET_DEG_C + ReData * ADCLSB;
}

float AXP192::GetBatPower() {
    float VoltageLSB = 1.1;
    float CurrentLCS = 0.5;
    uint32_t ReData  = Read24bit(0x70);
    return VoltageLSB * CurrentLCS * ReData / 1000.0;
}

float AXP192::GetBatChargeCurrent() {
    float ADCLSB    = 0.5;
    uint16_t ReData = Read12Bit(0x7A);
    return ReData * ADCLSB;
}
float AXP192::GetAPSVoltage() {
    float ADCLSB    = 1.4 / 1000.0;
    uint16_t ReData = Read12Bit(0x7E);
    return ReData * ADCLSB;
}

float AXP192::GetBatCoulombInput() {
    uint32_t ReData = Read32bit(0xB0);
    return ReData * 65536 * 0.5 / 3600 / 25.0;
}

float AXP192::GetBatCoulombOut() {
    uint32_t ReData = Read32bit(0xB4);
    return ReData * 65536 * 0.5 / 3600 / 25.0;
}

void AXP192::SetCoulombClear() {
    Write1Byte(0xB8, 0x20);
}

void AXP192::SetLDO2(bool State) {
    uint8_t buf = Read8bit(0x12);
    if (State == true)
        buf = (1 << 2) | buf;
    else
        buf = ~(1 << 2) & buf;
    Write1Byte(0x12, buf);
}

void AXP192::SetDCDC3(bool State) {
    uint8_t buf = Read8bit(0x12);
    if (State == true)
        buf = (1 << 1) | buf;
    else
        buf = ~(1 << 1) & buf;
    Write1Byte(0x12, buf);
}

uint8_t AXP192::AXPInState() {
    return Read8bit(0x00);
}
bool AXP192::isACIN() {
    return (Read8bit(0x00) & 0x80) ? true : false;
}
bool AXP192::isCharging() {
    return (Read8bit(0x00) & 0x04) ? true : false;
}
bool AXP192::isVBUS() {
    return (Read8bit(0x00) & 0x20) ? true : false;
}

void AXP192::SetLDOVoltage(uint8_t number, uint16_t voltage) {
    uint8_t vdata = calcVoltageData(voltage, 3300, 1800, 100) & 0x0F;
    switch (number) {
        // uint8_t reg, data;
        case 2:
            Write1Byte(0x28, (Read8bit(0x28) & 0x0F) | (vdata << 4));
            break;
        case 3:
            Write1Byte(0x28, (Read8bit(0x28) & 0xF0) | vdata);
            break;
    }
}

/// @param number 0=DCDC1 / 1=DCDC2 / 2=DCDC3
void AXP192::SetDCVoltage(uint8_t number, uint16_t voltage) {
    uint8_t addr;
    uint8_t vdata;
    if (number > 2) return;
    switch (number) {
        case 0:
            addr  = 0x26;
            vdata = calcVoltageData(voltage, 3500, 700, 25) & 0x7F;
            break;
        case 1:
            addr  = 0x25;
            vdata = calcVoltageData(voltage, 2275, 700, 25) & 0x3F;
            break;
        case 2:
            addr  = 0x27;
            vdata = calcVoltageData(voltage, 3500, 700, 25) & 0x7F;
            break;
    }
    // Serial.printf("result:%hhu\n", (Read8bit(addr) & 0X80) | (voltage &
    // 0X7F)); Serial.printf("result:%d\n", (Read8bit(addr) & 0X80) | (voltage &
    // 0X7F)); Serial.printf("result:%x\n", (Read8bit(addr) & 0X80) | (voltage &
    // 0X7F));
    Write1Byte(addr, (Read8bit(addr) & 0x80) | vdata);
}

void AXP192::SetESPVoltage(uint16_t voltage) {
    if (voltage >= 3000 && voltage <= 3400) {
        SetDCVoltage(0, voltage);
    }
}

void AXP192::SetLcdVoltage(uint16_t voltage) {
    if (voltage >= 2500 && voltage <= 3300) {
        SetDCVoltage(2, voltage);
    }
}

void AXP192::SetLDOEnable(uint8_t number, bool state) {
    uint8_t mark = 0x01;
    if ((number < 2) || (number > 3)) return;

    mark <<= number;
    if (state) {
        Write1Byte(0x12, (Read8bit(0x12) | mark));
    } else {
        Write1Byte(0x12, (Read8bit(0x12) & (~mark)));
    }
}

void AXP192::SetLCDRSet(bool state) {
    uint8_t reg_addr = 0x96;
    uint8_t gpio_bit = 0x02;
    uint8_t data;
    data = Read8bit(reg_addr);

    if (state) {
        data |= gpio_bit;
    } else {
        data &= ~gpio_bit;
    }

    Write1Byte(reg_addr, data);
}

// Select source for BUS_5V
// 0 : use internal boost
// 1 : powered externally
void AXP192::SetBusPowerMode(uint8_t state) {
    uint8_t data;
    if (state == 0) {
        // Set GPIO to 3.3V (LDO OUTPUT mode)
        data = Read8bit(0x91);
        Write1Byte(0x91, (data & 0x0F) | 0xF0);
        // Set GPIO0 to LDO OUTPUT, pullup N_VBUSEN to disable VBUS supply from
        // BUS_5V
        data = Read8bit(0x90);
        Write1Byte(0x90, (data & 0xF8) | 0x02);
        // Set EXTEN to enable 5v boost
        data = Read8bit(0x10);
        Write1Byte(0x10, data | 0x04);
    } else {
        // Set EXTEN to disable 5v boost
        data = Read8bit(0x10);
        Write1Byte(0x10, data & ~0x04);
        // Set GPIO0 to float, using enternal pulldown resistor to enable VBUS
        // supply from BUS_5V
        data = Read8bit(0x90);
        Write1Byte(0x90, (data & 0xF8) | 0x07);
    }
}

void AXP192::SetLed(uint8_t state) {
    uint8_t reg_addr = 0x94;
    uint8_t data;
    data = Read8bit(reg_addr);

    if (state) {
        data = data & 0XFD;
    } else {
        data |= 0X02;
    }

    Write1Byte(reg_addr, data);
}

// set led state(GPIO high active,set 1 to enable amplifier)
void AXP192::SetSpkEnable(uint8_t state) {
    uint8_t reg_addr = 0x94;
    uint8_t gpio_bit = 0x04;
    uint8_t data;
    data = Read8bit(reg_addr);

    if (state) {
        data |= gpio_bit;
    } else {
        data &= ~gpio_bit;
    }

    Write1Byte(reg_addr, data);
}

void AXP192::SetCHGCurrent(uint8_t state) {
    uint8_t data = Read8bit(0x33);
    data &= 0xf0;
    data = data | (state & 0x0f);
    Write1Byte(0x33, data);
}

void AXP192::SetPeripherialsPower(uint8_t state) {
    if (!state)
        Write1Byte(0x10, Read8bit(0x10) & 0XFB);
    else if (state)
        Write1Byte(0x10, Read8bit(0x10) | 0X04);
    // uint8_t data;
    // Set EXTEN to enable 5v boost
}
#endif