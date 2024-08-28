#if defined (CORE2)
#include "AXP.h"

void WriteBitOn(uint8_t Addr, uint8_t bit) {
    Write1Byte(Addr, (Read8bit(Addr)) | bit);
}

void WriteBitOff(uint8_t Addr, uint8_t bit) {
    Write1Byte(Addr, (Read8bit(Addr)) & ~bit);
}

void Write1Byte(uint8_t Addr, uint8_t Data) {
    Wire1.beginTransmission(0x34);
    Wire1.write(Addr);
    Wire1.write(Data);
    Wire1.endTransmission();
}

uint8_t Read8bit(uint8_t Addr) {
    Wire1.beginTransmission(0x34);
    Wire1.write(Addr);
    Wire1.endTransmission();
    Wire1.requestFrom(0x34, 1);
    return Wire1.read();
}

uint16_t Read12Bit(uint8_t Addr) {
    uint16_t Data = 0;
    uint8_t buf[2];
    ReadBuff(Addr, 2, buf);
    Data = ((buf[0] << 4) + buf[1]);  //
    return Data;
}

uint16_t Read13Bit(uint8_t Addr) {
    uint16_t Data = 0;
    uint8_t buf[2];
    ReadBuff(Addr, 2, buf);
    Data = ((buf[0] << 5) + buf[1]);  //
    return Data;
}

uint16_t Read16bit(uint8_t Addr) {
    uint16_t ReData = 0;
    Wire1.beginTransmission(0x34);
    Wire1.write(Addr);
    Wire1.endTransmission();
    Wire1.requestFrom(0x34, 2);
    for (int i = 0; i < 2; i++) {
        ReData <<= 8;
        ReData |= Wire1.read();
    }
    return ReData;
}

uint32_t Read24bit(uint8_t Addr) {
    uint32_t ReData = 0;
    Wire1.beginTransmission(0x34);
    Wire1.write(Addr);
    Wire1.endTransmission();
    Wire1.requestFrom(0x34, 3);
    for (int i = 0; i < 3; i++) {
        ReData <<= 8;
        ReData |= Wire1.read();
    }
    return ReData;
}

uint32_t Read32bit(uint8_t Addr) {
    uint32_t ReData = 0;
    Wire1.beginTransmission(0x34);
    Wire1.write(Addr);
    Wire1.endTransmission();
    Wire1.requestFrom(0x34, 4);
    for (int i = 0; i < 4; i++) {
        ReData <<= 8;
        ReData |= Wire1.read();
    }
    return ReData;
}

void ReadBuff(uint8_t Addr, uint8_t Size, uint8_t* Buff) {
    Wire1.beginTransmission(0x34);
    Wire1.write(Addr);
    Wire1.endTransmission();
    Wire1.requestFrom(0x34, (int)Size);
    for (int i = 0; i < Size; i++) {
        *(Buff + i) = Wire1.read();
    }
}

bool writeRegister8Array(const std::uint8_t* reg_data_array,
                         std::size_t length) {
    for (size_t i = 0; i < length; i += 2) {
        Write1Byte(reg_data_array[i], reg_data_array[i + 1]);
    }
    return true;
}

AXP::AXP() {
}

// Will be deprecated
void AXP::begin(mbus_mode_t mode) {
    begin();
}

void AXP::begin() {
    Wire1.begin(21, 22);
    Wire1.setClock(100000);
    uint8_t val = Read8bit(0x03);
    if (val == 0x03) {
        _pmic = pmic_axp192;
        axp192.begin();
    } else if (val == 0x4A) {
        _pmic = pmic_axp2101;
        axp2101.begin(&Wire1, AXP2101_ADDR, 21, 22);
        axp2101.set_bus_3v3(3300);
        ina3221.begin(&Wire1);
        axp2101.set_lcd_back_light_voltage(3000);
        axp2101.set_lcd_and_tf_voltage(3300);
        if (ina3221.getVoltage(INA3221_CH2) > 4.5f)
            axp2101.set_bus_5v(false);
        else {
            axp2101.set_bus_5v(true);
        }
        axp2101.set_sys_led(true);
    } else {
        _pmic = pmic_unknown;
    }
}

void AXP::ScreenBreath(int brightness) {
    if (_pmic == pmic_axp192) {
        axp192.ScreenBreath(brightness);
    }
    if (_pmic == pmic_axp2101) {
        if (brightness >= 100)
            brightness = 100;
        else if (brightness < 0)
            brightness = 0;
        int vol = map(brightness, 0, 100, 2400, 3300);
        axp2101.set_lcd_back_light_voltage(vol);
    }
}

bool AXP::GetBatState() {
    if (_pmic == pmic_axp192) {
        return axp192.GetBatState();
    }
    if (_pmic == pmic_axp2101) {
    }
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
void AXP::EnableCoulombcounter(void) {
    if (_pmic == pmic_axp192) {
        axp192.EnableCoulombcounter();
    }
}

void AXP::DisableCoulombcounter(void) {
    if (_pmic == pmic_axp192) {
        axp192.DisableCoulombcounter();
    }
}

void AXP::StopCoulombcounter(void) {
    if (_pmic == pmic_axp192) {
        axp192.StopCoulombcounter();
    }
}

void AXP::ClearCoulombcounter(void) {
    if (_pmic == pmic_axp192) {
        axp192.ClearCoulombcounter();
    }
}

uint32_t AXP::GetCoulombchargeData(void) {
    if (_pmic == pmic_axp192) {
        return axp192.GetCoulombchargeData();
    }
    return 0;
}

uint32_t AXP::GetCoulombdischargeData(void) {
    if (_pmic == pmic_axp192) {
        return axp192.GetCoulombdischargeData();
    }
    return 0;
}

float AXP::GetCoulombData(void) {
    if (_pmic == pmic_axp192) {
        return axp192.GetCoulombData();
    }
    return -1;
}

// Cut all power, except for LDO1 (RTC)
void AXP::PowerOff(void) {
    if (_pmic == pmic_axp192) {
        axp192.PowerOff();
    }

    if (_pmic == pmic_axp2101) {
        axp2101.power_off();
    }
}

void AXP::SetAdcState(bool state) {
    // Enable / Disable all ADCs

    if (_pmic == pmic_axp192) {
        axp192.SetAdcState(state);
    }
}

void AXP::PrepareToSleep(void) {
    if (_pmic == pmic_axp192) {
        axp192.PrepareToSleep();
    }

    if (_pmic == pmic_axp2101) {
        axp2101.set_sys_led(false);
    }
}

// Get current battery level
float AXP::GetBatteryLevel(void) {
    if (_pmic == pmic_axp192) {
        return axp192.GetBatteryLevel();
    }
    if (_pmic == pmic_axp2101) {
        const float batVoltage = ina3221.getVoltage(INA3221_CH1);
        const float batPercentage =
            (batVoltage < 3.248088) ? (0) : (batVoltage - 3.120712) * 100;
        return (batPercentage <= 100) ? batPercentage : 100;
    }
    return -1;
}

void AXP::RestoreFromLightSleep(void) {
    if (_pmic == pmic_axp192) {
        axp192.RestoreFromLightSleep();
    }
}

uint8_t AXP::GetWarningLeve(void) {
    if (_pmic == pmic_axp192) {
        return axp192.GetWarningLeve();
    }
    return -1;
}

// -- sleep
void AXP::DeepSleep(uint64_t time_in_us) {
    if (_pmic == pmic_axp192) {
        return axp192.DeepSleep(time_in_us);
    }
}

void AXP::LightSleep(uint64_t time_in_us) {
    if (_pmic == pmic_axp192) {
        return axp192.LightSleep(time_in_us);
    }
}

uint8_t AXP::GetWarningLevel(void) {
    if (_pmic == pmic_axp192) {
        return axp192.GetWarningLevel();
    }
    return -1;
}

float AXP::GetBatVoltage() {
    if (_pmic == pmic_axp192) {
        return axp192.GetBatVoltage();
    }
    if (_pmic == pmic_axp2101) {
        return ina3221.getVoltage(INA3221_CH1);
    }
    return -1;
}

float AXP::GetBatCurrent() {
    if (_pmic == pmic_axp192) {
        return axp192.GetBatCurrent();
    }
    if (_pmic == pmic_axp2101) {
        return ina3221.getCurrent(INA3221_CH1);
    }
    return -1;
}

float AXP::GetVinVoltage() {
    if (_pmic == pmic_axp192) {
        return axp192.GetVinVoltage();
    }
    if (_pmic == pmic_axp2101) {
        return ina3221.getVoltage(INA3221_CH3);
    }
    return -1;
}

float AXP::GetVinCurrent() {
    if (_pmic == pmic_axp192) {
        return axp192.GetVinCurrent();
    }
    if (_pmic == pmic_axp2101) {
        return ina3221.getCurrent(INA3221_CH3);
    }
    return -1;
}

float AXP::GetVBusVoltage() {
    if (_pmic == pmic_axp192) {
        return axp192.GetVBusVoltage();
    }
    if (_pmic == pmic_axp2101) {
        return ina3221.getVoltage(INA3221_CH2);
    }
    return -1;
}

float AXP::GetVBusCurrent() {
    if (_pmic == pmic_axp192) {
        return axp192.GetVBusCurrent();
    }
    if (_pmic == pmic_axp2101) {
        return ina3221.getCurrent(INA3221_CH2);
    }
    return -1;
}

float AXP::GetTempInAXP192() {
    if (_pmic == pmic_axp192) {
        return axp192.GetTempInAXP192();
    }
    return -1;
}

float AXP::GetBatPower() {
    if (_pmic == pmic_axp192) {
        return axp192.GetBatPower();
    }
    return -1;
}

float AXP::GetBatChargeCurrent() {
    if (_pmic == pmic_axp192) {
        return axp192.GetBatChargeCurrent();
    }
    return -1;
}
float AXP::GetAPSVoltage() {
    if (_pmic == pmic_axp192) {
        return axp192.GetAPSVoltage();
    }
    return -1;
}

float AXP::GetBatCoulombInput() {
    if (_pmic == pmic_axp192) {
        return axp192.GetBatCoulombInput();
    }
    return -1;
}

float AXP::GetBatCoulombOut() {
    if (_pmic == pmic_axp192) {
        return axp192.GetBatCoulombOut();
    }
    return -1;
}

void AXP::SetCoulombClear() {
    if (_pmic == pmic_axp192) {
        axp192.SetCoulombClear();
    }
}

void AXP::SetLDO2(bool State) {
    if (_pmic == pmic_axp192) {
        axp192.SetLDO2(State);
    }
}

void AXP::SetDCDC3(bool State) {
    if (_pmic == pmic_axp192) {
        axp192.SetDCDC3(State);
    }
}

uint8_t AXP::AXPInState() {
    if (_pmic == pmic_axp192) {
        return axp192.AXPInState();
    }
    return -1;
}
bool AXP::isACIN() {
    if (_pmic == pmic_axp192) {
        return axp192.isACIN();
    }
    if (_pmic == pmic_axp2101) {
        return (ina3221.getVoltage(INA3221_CH3) > 1);
    }

    return true;
}
bool AXP::isCharging() {
    if (_pmic == pmic_axp192) {
        return axp192.isCharging();
    }
    return true;
}
bool AXP::isVBUS() {
    if (_pmic == pmic_axp192) {
        return axp192.isVBUS();
    }
    if (_pmic == pmic_axp2101) {
        return (ina3221.getVoltage(INA3221_CH2) > 1);
    }
    return true;
}

uint8_t calcVoltageData(uint16_t value, uint16_t maxv, uint16_t minv,
                        uint16_t step) {
    uint8_t data = 0;
    if (value > maxv) value = maxv;
    if (value > minv) data = (value - minv) / step;
    return data;
}

void AXP::SetLDOVoltage(uint8_t number, uint16_t voltage) {
    if (_pmic == pmic_axp192) {
        axp192.SetLDOVoltage(number, voltage);
    }
}

/// @param number 0=DCDC1 / 1=DCDC2 / 2=DCDC3
void AXP::SetDCVoltage(uint8_t number, uint16_t voltage) {
    if (_pmic == pmic_axp192) {
        axp192.SetDCVoltage(number, voltage);
    }
    if (_pmic == pmic_axp2101) {
    }
}

void AXP::SetESPVoltage(uint16_t voltage) {
    if (_pmic == pmic_axp192) {
        axp192.SetESPVoltage(voltage);
    }
    if (_pmic == pmic_axp2101) {
        axp2101.set_bus_3v3(voltage);
    }
}

void AXP::SetLcdVoltage(uint16_t voltage) {
    if (_pmic == pmic_axp192) {
        axp192.SetLcdVoltage(voltage);
    }
    if (_pmic == pmic_axp2101) {
        axp2101.set_lcd_back_light_voltage(voltage);
    }
}

void AXP::SetLDOEnable(uint8_t number, bool state) {
    if (_pmic == pmic_axp192) {
        axp192.SetLDOEnable(number, state);
    }

    if (_pmic == pmic_axp2101) {
    }
}

void AXP::SetLCDRSet(bool state) {
    if (_pmic == pmic_axp192) {
        axp192.SetLCDRSet(state);
    }
    if (_pmic == pmic_axp2101) {
        axp2101.set_lcd_rst(state);
    }
}

// Select source for BUS_5V
// 0 : use internal boost
// 1 : powered externally
void AXP::SetBusPowerMode(uint8_t state) {
    if (_pmic == pmic_axp192) {
        axp192.SetBusPowerMode(state);
    }
    if (_pmic == pmic_axp2101) {
    }
}

void AXP::SetLed(uint8_t state) {
    if (_pmic == pmic_axp192) {
        axp192.SetLed(state);
    }
    if (_pmic == pmic_axp2101) {
        axp2101.set_sys_led(state);
    }
}

// set led state(GPIO high active,set 1 to enable amplifier)
void AXP::SetSpkEnable(uint8_t state) {
    if (_pmic == pmic_axp192) {
        axp192.SetSpkEnable(state);
    }
    if (_pmic == pmic_axp2101) {
        axp2101.set_spk(state);
    }
}

void AXP::SetCHGCurrent(uint8_t state) {
    if (_pmic == pmic_axp192) {
        axp192.SetCHGCurrent(state);
    }
    if (_pmic == pmic_axp2101) {
        axp2101.set_bat_charge(1);
    }
}

void AXP::SetPeripherialsPower(uint8_t state) {
    if (_pmic == pmic_axp192) {
        axp192.SetPeripherialsPower(state);
    }
    if (_pmic == pmic_axp2101) {
        axp2101.set_bus_5v(state);
    }
}

void AXP::SetVibration(uint8_t state) {
    if (_pmic == pmic_axp192) {
        axp192.SetLDOEnable(3, state);
    }
    if (_pmic == pmic_axp2101) {
        if (state) {
            axp2101.set_vib_motor_voltage(3000);
        } else {
            axp2101.set_vib_motor_voltage(0);
        }
    }
}
#endif