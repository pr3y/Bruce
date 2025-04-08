#include "rf_utils.h"
#include "core/settings.h"

// CRC-64-ECMA constants
const uint64_t CRC64_ECMA_POLY = 0x42F0E1EBA9EA3693; // Polynomial for CRC-64-ECMA
const uint64_t CRC64_ECMA_INIT = 0xFFFFFFFFFFFFFFFF; // Initial value

const int range_limits[4][2] = {
    {0,  23}, // 300-348 MHz
    {24, 47}, // 387-464 MHz
    {48, 56}, // 779-928 MHz
    {0,  56}  // All ranges
};
const char *subghz_frequency_ranges[] = {"300-348 MHz", "387-464 MHz", "779-928 MHz", "All ranges"};
const float subghz_frequency_list[] = {
    /* 300 - 348 MHz Frequency Range */
    300.000f,
    302.757f,
    303.875f,
    303.900f,
    304.250f,
    307.000f,
    307.500f,
    307.800f,
    309.000f,
    310.000f,
    312.000f,
    312.100f,
    312.200f,
    313.000f,
    313.850f,
    314.000f,
    314.350f,
    314.980f,
    315.000f,
    318.000f,
    330.000f,
    345.000f,
    348.000f,
    350.000f,

    /* 387 - 464 MHz Frequency Range */
    387.000f,
    390.000f,
    418.000f,
    430.000f,
    430.500f,
    431.000f,
    431.500f,
    433.075f,
    433.220f,
    433.420f,
    433.657f,
    433.889f,
    433.920f,
    434.075f,
    434.177f,
    434.190f,
    434.390f,
    434.420f,
    434.620f,
    434.775f,
    438.900f,
    440.175f,
    464.000f,
    467.750f,

    /* 779 - 928 MHz Frequency Range */
    779.000f,
    868.350f,
    868.400f,
    868.800f,
    868.950f,
    906.400f,
    915.000f,
    925.000f,
    928.000f
};

RfCodes recent_rfcodes[16];       // TODO: save/load in EEPROM
int recent_rfcodes_last_used = 0; // TODO: save/load in EEPROM

bool initRfModule(String mode, float frequency) {

    if (bruceConfig.CC1101_bus.mosi == (gpio_num_t)TFT_MOSI &&
        bruceConfig.CC1101_bus.mosi != GPIO_NUM_NC) { // (T_EMBED), CORE2 and others
#if TFT_MOSI > 0
        initCC1101once(&tft.getSPIinstance());
#else
        yield();
#endif
    } else if (bruceConfig.CC1101_bus.mosi ==
               bruceConfig.SDCARD_bus.mosi) { // (CARDPUTER) and (ESP32S3DEVKITC1) and devices that share
                                              // CC1101 pin with only SDCard
        initCC1101once(&sdcardSPI);
    } else if (bruceConfig.NRF24_bus.mosi == bruceConfig.CC1101_bus.mosi &&
               bruceConfig.CC1101_bus.mosi !=
                   bruceConfig.SDCARD_bus.mosi) { // This board uses the same Bus for NRF and CC1101, but with
                                                  // different CS pins, different from Stick_Cs down below..
        CC_NRF_SPI.begin(
            bruceConfig.CC1101_bus.sck, bruceConfig.CC1101_bus.miso, bruceConfig.CC1101_bus.mosi
        );
        initCC1101once(&CC_NRF_SPI);
    } else {
        // (STICK_C_PLUS) || (STICK_C_PLUS2) and others that doesn´t share SPI with other devices (need to
        // change it when Bruce board comes to shore)
        ELECHOUSE_cc1101.setBeginEndLogic(true
        ); // make sure to use BeginEndLogic for StickCs in the shared pins (not bus) config
        initCC1101once(NULL);
    }

    // use default frequency if no one is passed
    if (!frequency) frequency = bruceConfig.rfFreq;

    if (bruceConfig.rfModule == CC1101_SPI_MODULE) { // CC1101 in use

        ELECHOUSE_cc1101.Init();
        if (ELECHOUSE_cc1101.getCC1101()) { // Check the CC1101 Spi connection.
            Serial.println("cc1101 Connection OK");
        } else {
            displayError("CC1101 not found");
            Serial.println("cc1101 Connection Error");
            return false;
        }

        // make sure it is in idle state when changing frequency and other parameters
        // "If any frequency programming register is altered when the frequency synthesizer is running, the
        // synthesizer may give an undesired response. Hence, the frequency programming should only be updated
        // when the radio is in the IDLE state." https://github.com/LSatan/SmartRC-CC1101-Driver-Lib/issues/65
        // ELECHOUSE_cc1101.setSidle();
        // Serial.println("cc1101 setSidle();");

        if (!((frequency >= 280 && frequency <= 350) || (frequency >= 387 && frequency <= 468) ||
              (frequency >= 779 && frequency <= 928))) {
            Serial.println("Invalid Frequency, setting default");
            frequency = 433.92;
            displayWarning("Wrong freq, set to 433.92", true);
        }
        // else
        // ELECHOUSE_cc1101.setRxBW(812.50);  // reset to default
        ELECHOUSE_cc1101.setRxBW(256);      // narrow band for better accuracy
        ELECHOUSE_cc1101.setClb(1, 13, 15); // Calibration Offset
        ELECHOUSE_cc1101.setClb(2, 16, 19); // Calibration Offset
        ELECHOUSE_cc1101.setModulation(2
        ); // set modulation mode. 0 = 2-FSK, 1 = GFSK, 2 = ASK/OOK, 3 = 4-FSK, 4 = MSK.
        ELECHOUSE_cc1101.setDRate(50
        ); // Set the Data Rate in kBaud. Value from 0.02 to 1621.83. Default is 99.97 kBaud!
        ELECHOUSE_cc1101.setPktFormat(3
        ); // Format of RX and TX data. 0 = Normal mode, use FIFOs for RX and TX.
           // 1 = Synchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins.
           // 2 = Random TX mode; sends random data using PN9 generator. Used for test. Works as normal mode,
           // setting 0 (00), in RX. 3 = Asynchronous serial mode, Data in on GDO0 and data out on either of
           // the GDOx pins.
        setMHZ(frequency);
        Serial.println("cc1101 setMHZ(frequency);");

        /* MEMO: cannot change other params after this is executed */
        if (mode == "tx") {
            pinMode(bruceConfig.CC1101_bus.io0, OUTPUT);
            ELECHOUSE_cc1101.setPA(12); // set TxPower. The following settings are possible depending
            Serial.println("cc1101 setPA();");
            ELECHOUSE_cc1101.SetTx();
            Serial.println("cc1101 SetTx();");
        } else if (mode == "rx") {
            pinMode(bruceConfig.CC1101_bus.io0, INPUT);
            ELECHOUSE_cc1101.SetRx();
            Serial.println("cc1101 SetRx();");
        }
        // else if mode is unspecified wont start TX/RX mode here -> done by the caller

    } else {
        // single-pinned module
        if (frequency != bruceConfig.rfFreq) {
            Serial.println("unsupported frequency");
            return false;
        }

        if (mode == "tx") {
            gsetRfTxPin(false);
            pinMode(bruceConfig.rfTx, OUTPUT);
            digitalWrite(bruceConfig.rfTx, LOW);

        } else if (mode == "rx") {
            // Rx Mode
            gsetRfRxPin(false);
            pinMode(bruceConfig.rfRx, INPUT);
        }
    }
    // no error
    return true;
}

void deinitRfModule() {
    if (bruceConfig.rfModule == CC1101_SPI_MODULE) {
        if (bruceConfig.CC1101_bus.mosi == TFT_MOSI ||
            bruceConfig.CC1101_bus.mosi == bruceConfig.SDCARD_bus.mosi) { // (T_EMBED), CORE2 and others
            ELECHOUSE_cc1101.setSidle();
        } else { // (STICK_C_PLUS) || (STICK_C_PLUS2) and others that doesn´t share SPI with other devices
                 // (need to change it when Bruce board comes to shore)
#if TFT_MOSI > 0
            ELECHOUSE_cc1101.getSPIinstance()->end();
#endif
        }
    } else digitalWrite(bruceConfig.rfTx, LED_OFF);
}

void initCC1101once(SPIClass *SSPI) {
    // the init (); command may only be executed once in the entire program sequence. Otherwise problems can
    // arise.  https://github.com/LSatan/SmartRC-CC1101-Driver-Lib/issues/65

    // derived from
    // https://github.com/LSatan/SmartRC-CC1101-Driver-Lib/blob/master/examples/Rc-Switch%20examples%20cc1101/ReceiveDemo_Advanced_cc1101/ReceiveDemo_Advanced_cc1101.ino
    if (SSPI != NULL) ELECHOUSE_cc1101.setSPIinstance(SSPI); // New, to use the SPI instance we want.
    else ELECHOUSE_cc1101.setSPIinstance(nullptr);
    ELECHOUSE_cc1101.setSpiPin(
        bruceConfig.CC1101_bus.sck,
        bruceConfig.CC1101_bus.miso,
        bruceConfig.CC1101_bus.mosi,
        bruceConfig.CC1101_bus.cs
    );
    if (bruceConfig.CC1101_bus.io2 != GPIO_NUM_NC)
        ELECHOUSE_cc1101.setGDO(
            bruceConfig.CC1101_bus.io0, bruceConfig.CC1101_bus.io2
        ); // Set Gdo0 (tx) and Gdo2 (rx) for serial transmission function.
    else ELECHOUSE_cc1101.setGDO0(bruceConfig.CC1101_bus.io0); // use Gdo0 for both Tx and Rx

    return;
}

void deinitRMT() {
    // Deinit RMT channels in use by RF
    ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_driver_uninstall((rmt_channel_t)RMT_RX_CHANNEL));
}

void initRMT() {
    deinitRMT();

    rmt_config_t rxconfig;
    rxconfig.rmt_mode = RMT_MODE_RX;
    rxconfig.channel = RMT_RX_CHANNEL;
    rxconfig.gpio_num = gpio_num_t(bruceConfig.rfRx);

    if (bruceConfig.rfModule == CC1101_SPI_MODULE) rxconfig.gpio_num = gpio_num_t(bruceConfig.CC1101_bus.io0);

    rxconfig.clk_div = RMT_CLK_DIV; // RMT_DEFAULT_CLK_DIV=32
    rxconfig.mem_block_num = 1;
    rxconfig.flags = 0;
    rxconfig.rx_config.idle_threshold = 3 * RMT_1MS_TICKS,
    rxconfig.rx_config.filter_ticks_thresh = 200 * RMT_1US_TICKS;
    rxconfig.rx_config.filter_en = true;

    ESP_ERROR_CHECK(rmt_config(&rxconfig));
    ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_driver_install(rxconfig.channel, 2048, 0));
}

void setMHZ(float frequency) {
    if (frequency > 928 || frequency < 280) {
        frequency = 433.92;
        Serial.println("Frequency out of band");
    }
#if defined(T_EMBED_1101)
    static uint8_t antenna = 200; // 0=(<300), 1=(350-468), 2=(>778), 200=start to settle at the fisrt time
    // SW1:1  SW0:0 --- 315MHz
    // SW1:0  SW0:1 --- 868/915MHz
    // SW1:1  SW0:1 --- 434MHz
    if (frequency <= 350 && antenna != 0) {
        digitalWrite(CC1101_SW1_PIN, HIGH);
        digitalWrite(CC1101_SW0_PIN, LOW);
        antenna = 0;
        delay(10); // time to settle the antenna signal
    } else if (frequency > 350 && frequency < 468 && antenna != 1) {
        digitalWrite(CC1101_SW1_PIN, HIGH);
        digitalWrite(CC1101_SW0_PIN, HIGH);
        antenna = 1;
        delay(10); // time to settle the antenna signal
    } else if (frequency > 778 && antenna != 2) {
        digitalWrite(CC1101_SW1_PIN, LOW);
        digitalWrite(CC1101_SW0_PIN, HIGH);
        antenna = 2;
        delay(10); // time to settle the antenna signal
    }
#endif

    ELECHOUSE_cc1101.setMHZ(frequency);
}

int find_pulse_index(const std::vector<int> &indexed_durations, int duration) {
    int abs_duration = abs(duration);
    int closest_index = -1;
    int closest_diff = 999999; // Large number to find minimum difference

    for (size_t i = 0; i < indexed_durations.size(); i++) {
        int diff = abs(indexed_durations[i] - abs_duration);
        if (diff <= 50) { // ±50µs tolerance
            return i;     // Found a close match, return its index
        }
        if (diff < closest_diff) {
            closest_diff = diff;
            closest_index = i; // Store closest match
        }
    }

    // If there's space for a new duration, return -1 to signal adding it
    if (indexed_durations.size() < 4) { return -1; }

    return closest_index; // Otherwise, return the closest match
}

// Function to compute CRC-64-ECMA
uint64_t crc64_ecma(const std::vector<int> &data) {
    uint64_t crc = CRC64_ECMA_INIT;

    for (int value : data) {
        crc ^= (uint64_t)value << 56; // Use the value as the high byte
        for (int i = 0; i < 8; i++) {
            if (crc & 0x8000000000000000) {
                crc = (crc << 1) ^ CRC64_ECMA_POLY;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

void addToRecentCodes(struct RfCodes rfcode) {
    // copy rfcode -> recent_rfcodes[recent_rfcodes_last_used]
    recent_rfcodes[recent_rfcodes_last_used] = rfcode;
    recent_rfcodes_last_used += 1;
    if (recent_rfcodes_last_used == 16) recent_rfcodes_last_used = 0; // cycle
}

struct RfCodes selectRecentRfMenu() {
    options = {};
    bool exit = false;
    struct RfCodes selected_code;

    for (int i = 0; i < 16; i++) {
        if (recent_rfcodes[i].filepath == "") continue; // not inited

        options.emplace_back(recent_rfcodes[i].filepath.c_str(), [i, &selected_code]() {
            selected_code = recent_rfcodes[i];
        });
    }
    options.emplace_back("Main Menu", [&]() { exit = true; });

    loopOptions(options);
    options.clear();

    return selected_code;
}
