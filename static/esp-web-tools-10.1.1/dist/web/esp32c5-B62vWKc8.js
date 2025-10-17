import { ESP32C6ROM } from './esp32c6-DsFTtj2a.js';
import './rom-DGwTkJep.js';

class ESP32C5ROM extends ESP32C6ROM {
  constructor() {
    super(...arguments);
    this.CHIP_NAME = "ESP32-C5";
    this.IMAGE_CHIP_ID = 23;
    this.EFUSE_BASE = 0x600b4800;
    this.EFUSE_BLOCK1_ADDR = this.EFUSE_BASE + 0x044;
    this.MAC_EFUSE_REG = this.EFUSE_BASE + 0x044;
    this.UART_CLKDIV_REG = 0x60000014;
    this.EFUSE_RD_REG_BASE = this.EFUSE_BASE + 0x030; // BLOCK0 read base address
    this.EFUSE_PURPOSE_KEY0_REG = this.EFUSE_BASE + 0x34;
    this.EFUSE_PURPOSE_KEY0_SHIFT = 24;
    this.EFUSE_PURPOSE_KEY1_REG = this.EFUSE_BASE + 0x34;
    this.EFUSE_PURPOSE_KEY1_SHIFT = 28;
    this.EFUSE_PURPOSE_KEY2_REG = this.EFUSE_BASE + 0x38;
    this.EFUSE_PURPOSE_KEY2_SHIFT = 0;
    this.EFUSE_PURPOSE_KEY3_REG = this.EFUSE_BASE + 0x38;
    this.EFUSE_PURPOSE_KEY3_SHIFT = 4;
    this.EFUSE_PURPOSE_KEY4_REG = this.EFUSE_BASE + 0x38;
    this.EFUSE_PURPOSE_KEY4_SHIFT = 8;
    this.EFUSE_PURPOSE_KEY5_REG = this.EFUSE_BASE + 0x38;
    this.EFUSE_PURPOSE_KEY5_SHIFT = 12;
    this.EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_REG = this.EFUSE_RD_REG_BASE;
    this.EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT = 1 << 20;
    this.EFUSE_SPI_BOOT_CRYPT_CNT_REG = this.EFUSE_BASE + 0x034;
    this.EFUSE_SPI_BOOT_CRYPT_CNT_MASK = 0x7 << 18;
    this.EFUSE_SECURE_BOOT_EN_REG = this.EFUSE_BASE + 0x038;
    this.EFUSE_SECURE_BOOT_EN_MASK = 1 << 20;
    this.IROM_MAP_START = 0x42000000;
    this.IROM_MAP_END = 0x42800000;
    this.DROM_MAP_START = 0x42800000;
    this.DROM_MAP_END = 0x43000000;
    this.PCR_SYSCLK_CONF_REG = 0x60096110;
    this.PCR_SYSCLK_XTAL_FREQ_V = 0x7f << 24;
    this.PCR_SYSCLK_XTAL_FREQ_S = 24;
    this.XTAL_CLK_DIVIDER = 1;
    this.UARTDEV_BUF_NO = 0x4085f51c; // Variable in ROM .bss which indicates the port in use
    // Magic value for ESP32C5
    this.CHIP_DETECT_MAGIC_VALUE = [0x1101406f];
    this.FLASH_FREQUENCY = {
      "80m": 0xf,
      "40m": 0x0,
      "20m": 0x2
    };
    this.MEMORY_MAP = [[0x00000000, 0x00010000, "PADDING"], [0x42800000, 0x43000000, "DROM"], [0x40800000, 0x40860000, "DRAM"], [0x40800000, 0x40860000, "BYTE_ACCESSIBLE"], [0x4003a000, 0x40040000, "DROM_MASK"], [0x40000000, 0x4003a000, "IROM_MASK"], [0x42000000, 0x42800000, "IROM"], [0x40800000, 0x40860000, "IRAM"], [0x50000000, 0x50004000, "RTC_IRAM"], [0x50000000, 0x50004000, "RTC_DRAM"], [0x600fe000, 0x60100000, "MEM_INTERNAL2"]];
    this.UF2_FAMILY_ID = 0xf71c0343;
    this.EFUSE_MAX_KEY = 5;
    this.KEY_PURPOSES = {
      0: "USER/EMPTY",
      1: "ECDSA_KEY",
      2: "XTS_AES_256_KEY_1",
      3: "XTS_AES_256_KEY_2",
      4: "XTS_AES_128_KEY",
      5: "HMAC_DOWN_ALL",
      6: "HMAC_DOWN_JTAG",
      7: "HMAC_DOWN_DIGITAL_SIGNATURE",
      8: "HMAC_UP",
      9: "SECURE_BOOT_DIGEST0",
      10: "SECURE_BOOT_DIGEST1",
      11: "SECURE_BOOT_DIGEST2",
      12: "KM_INIT_KEY"
    };
  }
  async getPkgVersion(loader) {
    const numWord = 2;
    return (await loader.readReg(this.EFUSE_BLOCK1_ADDR + 4 * numWord)) >> 26 & 0x07;
  }
  async getMinorChipVersion(loader) {
    const numWord = 2;
    return (await loader.readReg(this.EFUSE_BLOCK1_ADDR + 4 * numWord)) >> 0 & 0x0f;
  }
  async getMajorChipVersion(loader) {
    const numWord = 2;
    return (await loader.readReg(this.EFUSE_BLOCK1_ADDR + 4 * numWord)) >> 4 & 0x03;
  }
  async getChipDescription(loader) {
    const pkgVer = await this.getPkgVersion(loader);
    let desc;
    if (pkgVer === 0) {
      desc = "ESP32-C5";
    } else {
      desc = "unknown ESP32-C5";
    }
    const majorRev = await this.getMajorChipVersion(loader);
    const minorRev = await this.getMinorChipVersion(loader);
    return `${desc} (revision v${majorRev}.${minorRev})`;
  }
  async getCrystalFreq(loader) {
    // The crystal detection algorithm of ESP32/ESP8266
    // works for ESP32-C5 as well.
    const uartDiv = (await loader.readReg(this.UART_CLKDIV_REG)) & this.UART_CLKDIV_MASK;
    const etsXtal = loader.transport.baudrate * uartDiv / 1000000 / this.XTAL_CLK_DIVIDER;
    let normXtal;
    if (etsXtal > 45) {
      normXtal = 48;
    } else if (etsXtal > 33) {
      normXtal = 40;
    } else {
      normXtal = 26;
    }
    if (Math.abs(normXtal - etsXtal) > 1) {
      loader.info("WARNING: Unsupported crystal in use");
    }
    return normXtal;
  }
  async getCrystalFreqRomExpect(loader) {
    return ((await loader.readReg(this.PCR_SYSCLK_CONF_REG)) & this.PCR_SYSCLK_XTAL_FREQ_V) >> this.PCR_SYSCLK_XTAL_FREQ_S;
  }
}

export { ESP32C5ROM };
