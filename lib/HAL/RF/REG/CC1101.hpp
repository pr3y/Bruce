#ifndef __CC1101__
#define __CC1101__
#include <cstdint>

// CC1101 CONFIG REGSITER
#define CC1101_IOCFG2       0x00        // GDO2 output pin configuration
#define CC1101_IOCFG1       0x01        // GDO1 output pin configuration
#define CC1101_IOCFG0       0x02        // GDO0 output pin configuration
#define CC1101_FIFOTHR      0x03        // RX FIFO and TX FIFO thresholds
#define CC1101_SYNC1        0x04        // Sync word, high INT8U
#define CC1101_SYNC0        0x05        // Sync word, low INT8U
#define CC1101_PKTLEN       0x06        // Packet length
#define CC1101_PKTCTRL1     0x07        // Packet automation control
#define CC1101_PKTCTRL0     0x08        // Packet automation control
#define CC1101_ADDR         0x09        // Device address
#define CC1101_CHANNR       0x0A        // Channel number
#define CC1101_FSCTRL1      0x0B        // Frequency synthesizer control
#define CC1101_FSCTRL0      0x0C        // Frequency synthesizer control
#define CC1101_FREQ2        0x0D        // Frequency control word, high INT8U
#define CC1101_FREQ1        0x0E        // Frequency control word, middle INT8U
#define CC1101_FREQ0        0x0F        // Frequency control word, low INT8U
#define CC1101_MDMCFG4      0x10        // Modem configuration
#define CC1101_MDMCFG3      0x11        // Modem configuration
#define CC1101_MDMCFG2      0x12        // Modem configuration
#define CC1101_MDMCFG1      0x13        // Modem configuration
#define CC1101_MDMCFG0      0x14        // Modem configuration
#define CC1101_DEVIATN      0x15        // Modem deviation setting
#define CC1101_MCSM2        0x16        // Main Radio Control State Machine configuration
#define CC1101_MCSM1        0x17        // Main Radio Control State Machine configuration
#define CC1101_MCSM0        0x18        // Main Radio Control State Machine configuration
#define CC1101_FOCCFG       0x19        // Frequency Offset Compensation configuration
#define CC1101_BSCFG        0x1A        // Bit Synchronization configuration
#define CC1101_AGCCTRL2     0x1B        // AGC control
#define CC1101_AGCCTRL1     0x1C        // AGC control
#define CC1101_AGCCTRL0     0x1D        // AGC control
#define CC1101_WOREVT1      0x1E        // High INT8U Event 0 timeout
#define CC1101_WOREVT0      0x1F        // Low INT8U Event 0 timeout
#define CC1101_WORCTRL      0x20        // Wake On Radio control
#define CC1101_FREND1       0x21        // Front end RX configuration
#define CC1101_FREND0       0x22        // Front end TX configuration
#define CC1101_FSCAL3       0x23        // Frequency synthesizer calibration
#define CC1101_FSCAL2       0x24        // Frequency synthesizer calibration
#define CC1101_FSCAL1       0x25        // Frequency synthesizer calibration
#define CC1101_FSCAL0       0x26        // Frequency synthesizer calibration
#define CC1101_RCCTRL1      0x27        // RC oscillator configuration
#define CC1101_RCCTRL0      0x28        // RC oscillator configuration
#define CC1101_FSTEST       0x29        // Frequency synthesizer calibration control
#define CC1101_PTEST        0x2A        // Production test
#define CC1101_AGCTEST      0x2B        // AGC test
#define CC1101_TEST2        0x2C        // Various test settings
#define CC1101_TEST1        0x2D        // Various test settings
#define CC1101_TEST0        0x2E        // Various test settings

enum class CC1101_REG : uint8_t {
    IOCFG2 = 0x00,         // GDO2 output pin configuration
    IOCFG1 = 0x01,         // GDO1 output pin configuration
    IOCFG0 = 0x02,         // GDO0 output pin configuration
    FIFOTHR = 0x03,        // RX FIFO and TX FIFO thresholds
    SYNC1 = 0x04,          // Sync word, high INT8U
    SYNC0 = 0x05,          // Sync word, low INT8U
    PKTLEN = 0x06,         // Packet length
    PKTCTRL1 = 0x07,       // Packet automation control
    PKTCTRL0 = 0x08,       // Packet automation control
    ADDR = 0x09,           // Device address
    CHANNR = 0x0A,         // Channel number
    FSCTRL1 = 0x0B,        // Frequency synthesizer control
    FSCTRL0 = 0x0C,        // Frequency synthesizer control
    FREQ2 = 0x0D,          // Frequency control word, high INT8U
    FREQ1 = 0x0E,          // Frequency control word, middle INT8U
    FREQ0 = 0x0F,          // Frequency control word, low INT8U
    MDMCFG4 = 0x10,        // Modem configuration
    MDMCFG3 = 0x11,        // Modem configuration
    MDMCFG2 = 0x12,        // Modem configuration
    MDMCFG1 = 0x13,        // Modem configuration
    MDMCFG0 = 0x14,        // Modem configuration
    DEVIATN = 0x15,        // Modem deviation setting
    MCSM2 = 0x16,          // Main Radio Control State Machine configuration
    MCSM1 = 0x17,          // Main Radio Control State Machine configuration
    MCSM0 = 0x18,          // Main Radio Control State Machine configuration
    FOCCFG = 0x19,         // Frequency Offset Compensation configuration
    BSCFG = 0x1A,          // Bit Synchronization configuration
    AGCCTRL2 = 0x1B,       // AGC control
    AGCCTRL1 = 0x1C,       // AGC control
    AGCCTRL0 = 0x1D,       // AGC control
    WOREVT1 = 0x1E,        // High INT8U Event 0 timeout
    WOREVT0 = 0x1F,        // Low INT8U Event 0 timeout
    WORCTRL = 0x20,        // Wake On Radio control
    FREND1 = 0x21,         // Front end RX configuration
    FREND0 = 0x22,         // Front end TX configuration
    FSCAL3 = 0x23,         // Frequency synthesizer calibration
    FSCAL2 = 0x24,         // Frequency synthesizer calibration
    FSCAL1 = 0x25,         // Frequency synthesizer calibration
    FSCAL0 = 0x26,         // Frequency synthesizer calibration
    RCCTRL1 = 0x27,        // RC oscillator configuration
    RCCTRL0 = 0x28,        // RC oscillator configuration
    FSTEST = 0x29,         // Frequency synthesizer calibration control
    PTEST = 0x2A,          // Production test
    AGCTEST = 0x2B,        // AGC test
    TEST2 = 0x2C,          // Various test settings
    TEST1 = 0x2D,          // Various test settings
    TEST0 = 0x2E           // Various test settings
};

//CC1101 Strobe commands
#define CC1101_SRES         0x30        // Reset chip.
#define CC1101_SFSTXON      0x31        // Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1).
                                        // If in RX/TX: Go to a wait state where only the synthesizer is
                                        // running (for quick RX / TX turnaround).
#define CC1101_SXOFF        0x32        // Turn off crystal oscillator.
#define CC1101_SCAL         0x33        // Calibrate frequency synthesizer and turn it off
                                        // (enables quick start).
#define CC1101_SRX          0x34        // Enable RX. Perform calibration first if coming from IDLE and
                                        // MCSM0.FS_AUTOCAL=1.
#define CC1101_STX          0x35        // In IDLE state: Enable TX. Perform calibration first if
                                        // MCSM0.FS_AUTOCAL=1. If in RX state and CCA is enabled:
                                        // Only go to TX if channel is clear.
#define CC1101_SIDLE        0x36        // Exit RX / TX, turn off frequency synthesizer and exit
                                        // Wake-On-Radio mode if applicable.
#define CC1101_SAFC         0x37        // Perform AFC adjustment of the frequency synthesizer
#define CC1101_SWOR         0x38        // Start automatic RX polling sequence (Wake-on-Radio)
#define CC1101_SPWD         0x39        // Enter power down mode when CSn goes high.
#define CC1101_SFRX         0x3A        // Flush the RX FIFO buffer.
#define CC1101_SFTX         0x3B        // Flush the TX FIFO buffer.
#define CC1101_SWORRST      0x3C        // Reset real time clock.
#define CC1101_SNOP         0x3D        // No operation. May be used to pad strobe commands to two
                                        // INT8Us for simpler software.
//CC1101 STATUS REGSITER
#define CC1101_PARTNUM      0x30
#define CC1101_VERSION      0x31
#define CC1101_FREQEST      0x32
#define CC1101_LQI          0x33
#define CC1101_RSSI         0x34
#define CC1101_MARCSTATE    0x35
#define CC1101_WORTIME1     0x36
#define CC1101_WORTIME0     0x37
#define CC1101_PKTSTATUS    0x38
#define CC1101_VCO_VC_DAC   0x39
#define CC1101_TXBYTES      0x3A
#define CC1101_RXBYTES      0x3B

//CC1101 PATABLE,TXFIFO,RXFIFO
#define CC1101_PATABLE      0x3E
#define CC1101_TXFIFO       0x3F
#define CC1101_RXFIFO       0x3F

#endif  /* __CC1101__ */
