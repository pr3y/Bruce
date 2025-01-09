#ifndef __PMIC__
#define __PMIC__

/**
 * @brief chip type
 */
enum PMICChipModel
{
    AXP173,
    AXP192,
    AXP202,
    AXP216,
    AXP2101,
    BQ25896,
    SY6970,
    UNDEFINED,
};

#ifdef PMIC_AXP192
#include "DRIVER/AXP192.tpp"
typedef class AXP192 PMIC;
#elif defined(PMIC_AXP202)
#include "DRIVER/AXP202.tpp"
typedef class AXP202 PMIC;
#elif defined(PMIC_AXP2101)
#include "DRIVER/AXP2101.tpp"
typedef class AXP2101 PMIC;
#elif defined(PMIC_BQ25896)
#include "DRIVER/BQ25896.tpp"
typedef HAL::PMIC::BQ25896 PMIC;
#elif defined(PMIC_SY6970)
#include "DRIVER/SY6970.tpp"
typedef class SY6970 PMIC;
#endif

#endif  /* __PMIC__ */
