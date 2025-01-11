#ifndef __PMIC__
#define __PMIC__

#ifdef PMIC_AXP192
#include "DRIVER/AXP192.tpp"
typedef class HAL::PMIC::AXP192 PMIC;
#elif defined(PMIC_AXP202)
#include "DRIVER/AXP202.tpp"
typedef class HAL::PMIC::AXP202 PMIC;
#elif defined(PMIC_AXP2101)
#include "DRIVER/AXP2101.tpp"
typedef class HAL::PMIC::AXP2101 PMIC;
#elif defined(PMIC_BQ25896)
#include "DRIVER/BQ25896.tpp"
typedef class HAL::PMIC::BQ25896 PMIC;
#elif defined(PMIC_SY6970)
#include "DRIVER/SY6970.tpp"
typedef class HAL::PMIC::SY6970 PMIC;
#endif

#endif  /* __PMIC__ */
