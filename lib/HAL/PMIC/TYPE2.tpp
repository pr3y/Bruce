#ifndef __TYPE2__
#define __TYPE2__

#include "PMIC.tpp"
#include "REG/TYPE2.hpp"
#include <HardwareSerial.h>
#include <Arduino.h>
#ifdef PMIC_BQ25896
#include "REG/BQ25896.hpp"
#elif defined(PMIC_SY6970)
#include "REG/SY6970.hpp"
#endif
namespace HAL
{
	namespace PMIC
	{
		template <class Driver>
		class Type2 : public Base<class Type2<Driver>, Driver>
		{
			friend class Base<Type2<Driver>, Driver>;

		public:
			Type2() : __protectedMask(0) {};
			~Type2() {};
			virtual bool isPowerGood() = 0;
			virtual uint16_t getSysPowerDownVoltage() = 0;
			virtual const char *getBusStatusString() = 0;
			virtual const char *getChargeStatusString() = 0;
			virtual uint8_t getNTCStatus() = 0;
			virtual const char *getNTCStatusString() = 0;
			virtual bool isVbusIn() = 0;
			
			bool inline setRegisterBit(uint8_t registers, uint8_t bit) { return this->setRegisterBit(registers, bit); };
			bool inline clrRegisterBit(uint8_t registers, uint8_t bit) { return this->clrRegisterBit(registers, bit); }
			bool inline getRegisterBit(uint8_t registers, uint8_t bit) { return this->getRegisterBit(registers, bit); }
			int readRegister(uint8_t reg) { return this->readRegister(reg); }
			int readRegister(uint8_t reg, uint8_t *buf, uint8_t length) { return this->readRegister(reg, *buf, length); }
			int writeRegister(uint8_t reg, uint8_t *buf, uint8_t length) { return this->writeRegister(reg, *buf, length); }
			int writeRegister(uint8_t reg, uint8_t val) { return this->writeRegister(reg, val); }
			bool begin(TwoWire &w, uint8_t addr, int sda, int scl) { return this->begin(&w, addr, sda, scl); }
			bool begin() { return this->begin(); }
			void end() { this->end(); }
			void deinit() { this->end(); }
			bool init() { return this->begin(); }

			bool isEnableCharge()
			{
				return this->getRegisterBit(POWERS_PPM_REG_03H, 4);
			}

			void disableCharge()
			{
				__user_disable_charge = true;
				this->clrRegisterBit(POWERS_PPM_REG_03H, 4);
			}

			void enableCharge()
			{
				__user_disable_charge = false;
				this->setRegisterBit(POWERS_PPM_REG_03H, 4);
			}

			bool isEnableOTG()
			{
				return this->getRegisterBit(POWERS_PPM_REG_03H, 5);
			}

			void disableOTG()
			{
				this->clrRegisterBit(POWERS_PPM_REG_03H, 5);
				/*
				 * After turning on the OTG function, the charging function will
				 * be automatically disabled. If the user does not disable the charging
				 * function, the charging function will be automatically enabled after
				 * turning off the OTG output.
				 * */
				if (!__user_disable_charge)
				{
					this->setRegisterBit(POWERS_PPM_REG_03H, 4);
				}
			}

			bool enableOTG()
			{
				if (isVbusIn())
					return false;
				return this->setRegisterBit(POWERS_PPM_REG_03H, 5);
			}

			void feedWatchdog()
			{
				this->setRegisterBit(POWERS_PPM_REG_03H, 6);
			}

			// Charging Termination Enable
			void enableChargingTermination()
			{
				this->setRegisterBit(POWERS_PPM_REG_07H, 7);
			}

			// Charging Termination Enable
			void disableChargingTermination()
			{
				this->clrRegisterBit(POWERS_PPM_REG_07H, 7);
			}

			// Charging Termination Enable
			bool isEnableChargingTermination()
			{
				return this->getRegisterBit(POWERS_PPM_REG_07H, 7);
			}

			// STAT Pin function
			void disableStatPin()
			{
				this->setRegisterBit(POWERS_PPM_REG_07H, 6);
			}

			void enableStatPin()
			{
				this->clrRegisterBit(POWERS_PPM_REG_07H, 6);
			}

			bool isEnableStatPin()
			{
				return this->getRegisterBit(POWERS_PPM_REG_07H, 6) == false;
			}

			// I2C Watchdog Timer Setting
			bool isEnableWatchdog()
			{
				int regVal = this->readRegister(POWERS_PPM_REG_07H);
				if (regVal == -1)
				{
					log_e("Config watch dog failed!");
					return false;
				}
				regVal >>= 4;
				return regVal & 0x03;
			}

			void disableWatchdog()
			{
				int regVal = this->readRegister(POWERS_PPM_REG_07H);
				regVal &= 0xCF;
				this->writeRegister(POWERS_PPM_REG_07H, regVal);
			}

			void enableWatchdog(enum Timeout val)
			{
				int regVal = this->readRegister(POWERS_PPM_REG_07H);
				regVal &= 0xCF;
				switch (val)
				{
				case TIMER_OUT_40SEC:
					this->writeRegister(POWERS_PPM_REG_07H, regVal | 0x10);
					break;
				case TIMER_OUT_80SEC:
					this->writeRegister(POWERS_PPM_REG_07H, regVal | 0x20);
					break;
				case TIMER_OUT_160SEC:
					this->writeRegister(POWERS_PPM_REG_07H, regVal | 0x30);
					break;
				default:
					break;
				}
			}

			void disableChargingSafetyTimer()
			{
				this->clrRegisterBit(POWERS_PPM_REG_07H, 3);
			}

			void enableChargingSafetyTimer()
			{
				this->setRegisterBit(POWERS_PPM_REG_07H, 3);
			}

			bool isEnableChargingSafetyTimer()
			{
				return this->getRegisterBit(POWERS_PPM_REG_07H, 3);
			}

			void setFastChargeTimer(FastChargeTimer timer)
			{
				int val;
				switch (timer)
				{
				case FAST_CHARGE_TIMER_5H:
				case FAST_CHARGE_TIMER_8H:
				case FAST_CHARGE_TIMER_12H:
				case FAST_CHARGE_TIMER_20H:
					val = this->readRegister(POWERS_PPM_REG_07H);
					if (val == -1)
						return;
					val &= 0xF1;
					val |= (timer << 1);
					this->writeRegister(POWERS_PPM_REG_07H, val);
					break;
				default:
					break;
				}
			}

			FastChargeTimer getFastChargeTimer()
			{
				int val = this->readRegister(POWERS_PPM_REG_07H);
				return static_cast<FastChargeTimer>((val & 0x0E) >> 1);
			}

			// Return  Battery Load status
			bool isEnableBatLoad()
			{
				return this->getRegisterBit(POWERS_PPM_REG_03H, 7);
			}

			// Battery Load (10mA) Disable
			void disableBatLoad()
			{
				this->clrRegisterBit(POWERS_PPM_REG_03H, 7);
			}

			// Battery Load (10mA) Enable
			void enableBatLoad()
			{
				this->setRegisterBit(POWERS_PPM_REG_03H, 7);
			}

			bool enableMeasure(MeasureMode mode)
			{
				int val = this->readRegister(POWERS_PPM_REG_02H);
				switch (mode)
				{
				case CONTINUOUS:
					val |= _BV(6);
					break;
				case ONE_SHORT:
					val &= (~_BV(6));
				default:
					break;
				}
				val |= _BV(7);
				return this->writeRegister(POWERS_PPM_REG_02H, val) != -1;
			}

			bool disableADCMeasure()
			{
				int val = this->readRegister(POWERS_PPM_REG_02H);
				if (val == -1)
				{
					return false;
				}
				val &= (~_BV(7));
				return this->writeRegister(POWERS_PPM_REG_02H, val) != 1;
			}

			bool setBoostFreq(BoostFreq freq)
			{
				switch (freq)
				{
				case BOOST_FREQ_500KHZ:
					return this->setRegisterBit(POWERS_PPM_REG_02H, 5);
				case BOOST_FREQ_1500KHZ:
					return this->clrRegisterBit(POWERS_PPM_REG_02H, 5);
				default:
					break;
				}
				return false;
			}

			BoostFreq getBoostFreq()
			{
				return this->getRegisterBit(POWERS_PPM_REG_02H, 5) ? BOOST_FREQ_500KHZ : BOOST_FREQ_1500KHZ;
			}

			// Enable Force Input Detection , Force PSEL detection
			void enableInputDetection()
			{
				this->setRegisterBit(POWERS_PPM_REG_02H, 1);
			}

			// Disable Force Input Detection ,  Not in PSEL detection (default)
			void disableInputDetection()
			{
				this->clrRegisterBit(POWERS_PPM_REG_02H, 1);
			}

			// Get Force DP/DM detection
			bool isEnableInputDetection()
			{
				return this->getRegisterBit(POWERS_PPM_REG_02H, 1);
			}

			// Enable PSEL detection when VBUS is plugged-in (default)
			void enableAutomaticInputDetection()
			{
				this->setRegisterBit(POWERS_PPM_REG_02H, 0);
			}

			// Disable PSEL detection when VBUS is plugged-in
			void disableAutomaticInputDetection()
			{
				this->clrRegisterBit(POWERS_PPM_REG_02H, 0);
			}

			// Get DPDM detection when BUS is plugged-in.
			bool isEnableAutomaticInputDetection()
			{
				return this->getRegisterBit(POWERS_PPM_REG_02H, 0);
			}

			void enterHizMode()
			{
				this->setRegisterBit(POWERS_PPM_REG_00H, 7);
			}

			void exitHizMode()
			{
				this->clrRegisterBit(POWERS_PPM_REG_00H, 7);
			}

			bool isHizMode()
			{
				return this->getRegisterBit(POWERS_PPM_REG_00H, 7);
			}

			// Enable ILIM Pin
			void enableCurrentLimitPin()
			{
				this->setRegisterBit(POWERS_PPM_REG_00H, 6);
			}

			void disableCurrentLimitPin()
			{
				this->clrRegisterBit(POWERS_PPM_REG_00H, 6);
			}

			bool isEnableCurrentLimitPin()
			{
				return this->getRegisterBit(POWERS_PPM_REG_00H, 6);
			}

			bool isOTG()
			{
				return getBusStatus() == BUS_STATE_OTG;
			}

			bool isCharging(void)
			{
				return chargeStatus() != CHARGE_STATE_NO_CHARGE;
			}

			bool isChargeDone()
			{
				return chargeStatus() != CHARGE_STATE_DONE;
			}

			BusStatus getBusStatus()
			{
				int val = this->readRegister(POWERS_PPM_REG_0BH);
				return (BusStatus)((val >> 5) & 0x07);
			}

			ChargeStatus chargeStatus()
			{
				int val = this->readRegister(POWERS_PPM_REG_0BH);
				if (val == -1)
					return CHARGE_STATE_UNKOWN;
				return static_cast<ChargeStatus>((val >> 3) & 0x03);
			}

			void shutdown()
			{
				disableBatterPowerPath();
			}

			// Close battery power path
			void disableBatterPowerPath()
			{
				this->setRegisterBit(POWERS_PPM_REG_09H, 5); // Force BATFET Off : BATFET_DIS
			}

			// Enable battery power path
			void enableBatterPowerPath()
			{
				this->clrRegisterBit(POWERS_PPM_REG_09H, 5); // Force BATFET Off : BATFET_DIS
			}

			void resetDefault()
			{
				this->setRegisterBit(POWERS_PPM_REG_14H, 7);
			}

			uint8_t getChipID()
			{
				int val = this->readRegister(POWERS_PPM_REG_14H);
				if (val == -1)
					return 0;
				return (val & 0x03);
			}

		protected:
			TwoWire *myWire = this->myWire;
			int mySDA = this->mySDA;
			int mySCL = this->mySCL;
			uint8_t myADDR = this->myADDR;
			PMICChipModel myModel = this->myModel;
			uint32_t __protectedMask;
			bool __user_disable_charge;
		};
	}
}
#endif /* __TYPE2__ */
