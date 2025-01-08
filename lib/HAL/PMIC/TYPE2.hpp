#ifndef __TYPE2__
#define __TYPE2__

#include "PMIC.tpp"
#include <HardwareSerial.h>

#include "REG/TYPE2.hpp"
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
		class Type2 : public Base<Type2<Driver>, Driver>
		{
			friend class Base<Type2<Driver>, Driver>;

		public:
			Type2() : __protectedMask(0) {};
			virtual bool isPowerGood() = 0;
			virtual uint16_t getSysPowerDownVoltage() = 0;
			virtual const char *getBusStatusString() = 0;
			virtual const char *getChargeStatusString() = 0;
			virtual uint8_t getNTCStatus() = 0;
			virtual const char *getNTCStatusString() = 0;
			bool init();
			void deinit();
			bool isEnableCharge();
			void disableCharge();
			void enableCharge();
			bool isEnableOTG();
			void disableOTG();
			bool enableOTG();
			void feedWatchdog();
			void enableChargingTermination();
			void disableChargingTermination();
			bool isEnableChargingTermination();
			void disableStatPin();
			void enableStatPin();
			bool isEnableStatPin();
			bool isEnableWatchdog();
			void disableWatchdog();
			void enableWatchdog(enum Timeout val);
			void disableChargingSafetyTimer();
			void enableChargingSafetyTimer();
			bool isEnableChargingSafetyTimer();
			void setFastChargeTimer(FastChargeTimer timer);
			FastChargeTimer getFastChargeTimer();
			bool isEnableBatLoad();
			void disableBatLoad();
			void enableBatLoad();
			bool enableMeasure(MeasureMode mode = CONTINUOUS);
			bool disableADCMeasure();
			bool setBoostFreq(BoostFreq freq);
			BoostFreq getBoostFreq();
			void enableInputDetection();
			void disableInputDetection();
			bool isEnableInputDetection();
			void enableAutomaticInputDetection();
			void disableAutomaticInputDetection();
			bool isEnableAutomaticInputDetection();
			void enterHizMode();
			void exitHizMode();
			bool isHizMode();
			void enableCurrentLimitPin();
			void disableCurrentLimitPin();
			bool isEnableCurrentLimitPin();
			bool isOTG();
			bool isCharging(void);
			bool isChargeDone();
			BusStatus getBusStatus();
			ChargeStatus chargeStatus();
			void shutdown();
			void disableBatterPowerPath();
			void enableBatterPowerPath();
			void resetDefault();
			uint8_t getChipID();

		protected:
			void setChipModel(PMICChipModel m) { myModel = m; }
			uint32_t __protectedMask;
			bool __user_disable_charge;
		};
	}
}
#endif /* __TYPE2__ */
