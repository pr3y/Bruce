#ifndef __PMIC__
#define __PMIC__
#ifdef PMIC_AXP192
#include "DRIVER/AXP192.tpp"
#elif defined(PMIC_AXP202)
#include "DRIVER/AXP202.tpp"
#elif defined(PMIC_AXP2101)
#include "DRIVER/AXP2101.tpp"
#elif defined(PMIC_BQ25896)
#include "DRIVER/BQ25896.tpp"
#elif defined(PMIC_SY6970)
#include "DRIVER/SY6970.tpp"
#endif

#endif /* __PMIC__ */
