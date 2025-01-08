#ifndef __AXP216__
#define __AXP216__
#include <Arduino.h>

#define AXP216_SLAVE_ADDRESS                            (0x34)

#define AXP216_CHIP_ID                          (0x41)

//CONTROL REGISTER
#define AXP216_STATUS                           (0x00)
#define AXP216_MODE_CHGSTATUS                   (0x01)
#define AXP216_DATA_BUFFER1                     (0x04)
#define AXP216_DATA_BUFFER2                     (0x05)
#define AXP216_DATA_BUFFER3                     (0x06)
#define AXP216_DATA_BUFFER4                     (0x07)
#define AXP216_DATA_BUFFER5                     (0x08)
#define AXP216_DATA_BUFFER6                     (0x09)
#define AXP216_DATA_BUFFER7                     (0x0A)
#define AXP216_DATA_BUFFER8                     (0x0B)
#define AXP216_DATA_BUFFER9                     (0x0C)
#define AXP216_DATA_BUFFERA                     (0x0D)
#define AXP216_DATA_BUFFERB                     (0x0E)
#define AXP216_DATA_BUFFERC                     (0x0F)

#define AXP216_IC_TYPE                          (0x03)

#define AXP216_DC12345_ALDO12_CTL               (0x10)
#define AXP216_LDO123_CTL                       (0x12)
#define AXP216_OUTPUT_CTL                       (0x13)
#define AXP216_ELDO1_VCTL                       (0x19)
#define AXP216_ELDO2_VCTL                       (0x1A)
#define AXP216_DCDC1_VCTL                       (0x21)
#define AXP216_DCDC2_VCTL                       (0x22)
#define AXP216_DCDC3_VCTL                       (0x23)

#define AXP216_DCDC4_VCTL                       (0x24)
#define AXP216_DCDC5_VCTL                       (0x25)
#define AXP216_DCDC23_VOLX                      (0x27)
#define AXP216_ALDO1_VCTL                       (0x28)
#define AXP216_ALDO2_VCTL                       (0x29)
#define AXP216_ALDO3_VCTL                       (0x2A)
#define AXP216_VBUS_IPSOUT                      (0x30)
#define AXP216_VOFF_SET                         (0x31)
#define AXP216_OFF_CTL                          (0x32)
#define AXP216_CHARGE1                          (0x33)
#define AXP216_CHARGE2                          (0x34)
#define AXP216_CHARGE3                          (0x35)
#define AXP216_PEK_SET                          (0x36)
#define AXP216_OFFLEVEL                         (0x37)
#define AXP216_VLTF_CHGSET                      (0x38)
#define AXP216_VHTF_CHGSET                      (0x39)
#define AXP216_DCDC_FREQ                        (0x3B)
#define AXP216_TLTF_DISCHGSET                   (0x3C)
#define AXP216_THTF_DISCHGSET                   (0x3D)
#define AXP216_DCDC_MODESET                     (0x80)
#define AXP216_ADC_CTL                          (0x82)
#define AXP216_ADC_SPEED                        (0x84)
#define AXP216_TS_ADC                           (0x85)
#define AXP216_TIMER_CTL                        (0x8A)
#define AXP216_HOTOVER_CTL                      (0x8F)

//GPIO REGISTER
#define AXP216_GPIO1_CTL                        (0x92)
#define AXP216_GPIO1_LDO_CTL                    (0x93)
#define AXP216_GPIO01_STATUS                    (0x94)
#define AXP216_GPIO1_PULLDOWN_CTL               (0x97)

//XPOWERS INTERRUPT REGISTER
#define AXP216_INTEN1                           (0x40)
#define AXP216_INTEN2                           (0x41)
#define AXP216_INTEN3                           (0x42)
#define AXP216_INTEN4                           (0x43)
#define AXP216_INTEN5                           (0x44)

//XPOWERS INTERRUPT STATUS REGISTER
#define AXP216_INTSTS1                          (0x48)
#define AXP216_INTSTS2                          (0x49)
#define AXP216_INTSTS3                          (0x4A)
#define AXP216_INTSTS4                          (0x4B)
#define AXP216_INTSTS5                          (0x4C)

//XPOWERS ADC DATA REGISTER
#define AXP216_INTERNAL_TEMP_H8                 (0x56)
#define AXP216_INTERNAL_TEMP_L4                 (0x57)
#define AXP216_TS_IN_H8                         (0x58)
#define AXP216_TS_IN_L4                         (0x59)
#define AXP216_BAT_VOLTAGE_H8                   (0x78)
#define AXP216_BAT_VOLTAGE_L4                   (0x79)
#define AXP216_BAT_CHGCURCUR_H8                 (0x7A)
#define AXP216_BAT_CHGCURCUR_L4                 (0x7B)
#define AXP216_BAT_DISCHGCUR_H8                 (0x7C)
#define AXP216_BAT_DISCHGCUR_L4                 (0x7D)
#define AXP216_FUEL_GAUGE_CTRL                  (0xB8)
#define AXP216_FUEL_GAUGE_REULST                (0xB9)
#define AXP216_BAT_CAPACITY_0                   (0xE0)
#define AXP216_BAT_CAPACITY_1                   (0xE1)
#define AXP216_BAT_LOW_WARNING_CTRL             (0xE6)


#endif  /* __AXP216__ */
