import { R as ROM } from './rom-DGwTkJep.js';

class ESP32S2ROM extends ROM {
  constructor() {
    super(...arguments);
    this.CHIP_NAME = "ESP32-S2";
    this.IMAGE_CHIP_ID = 2;
    this.IROM_MAP_START = 0x40080000;
    this.IROM_MAP_END = 0x40b80000;
    this.DROM_MAP_START = 0x3f000000;
    this.DROM_MAP_END = 0x3f3f0000;
    this.CHIP_DETECT_MAGIC_VALUE = [0x000007c6];
    this.SPI_REG_BASE = 0x3f402000;
    this.SPI_USR_OFFS = 0x18;
    this.SPI_USR1_OFFS = 0x1c;
    this.SPI_USR2_OFFS = 0x20;
    this.SPI_MOSI_DLEN_OFFS = 0x24;
    this.SPI_MISO_DLEN_OFFS = 0x28;
    this.SPI_W0_OFFS = 0x58;
    this.SPI_ADDR_REG_MSB = false;
    this.MAC_EFUSE_REG = 0x3f41a044; // ESP32-S2 has special block for MAC efuses
    this.UART_CLKDIV_REG = 0x3f400014;
    this.SUPPORTS_ENCRYPTED_FLASH = true;
    this.FLASH_ENCRYPTED_WRITE_ALIGN = 16;
    // todo: use espefuse APIs to get this info
    this.EFUSE_BASE = 0x3f41a000;
    this.EFUSE_RD_REG_BASE = this.EFUSE_BASE + 0x030; // BLOCK0 read base address
    this.EFUSE_BLOCK1_ADDR = this.EFUSE_BASE + 0x044;
    this.EFUSE_BLOCK2_ADDR = this.EFUSE_BASE + 0x05c;
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
    this.EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT = 1 << 19;
    this.EFUSE_SPI_BOOT_CRYPT_CNT_REG = this.EFUSE_BASE + 0x034;
    this.EFUSE_SPI_BOOT_CRYPT_CNT_MASK = 0x7 << 18;
    this.EFUSE_SECURE_BOOT_EN_REG = this.EFUSE_BASE + 0x038;
    this.EFUSE_SECURE_BOOT_EN_MASK = 1 << 20;
    this.EFUSE_RD_REPEAT_DATA3_REG = this.EFUSE_BASE + 0x3c;
    this.EFUSE_RD_REPEAT_DATA3_REG_FLASH_TYPE_MASK = 1 << 9;
    this.PURPOSE_VAL_XTS_AES256_KEY_1 = 2;
    this.PURPOSE_VAL_XTS_AES256_KEY_2 = 3;
    this.PURPOSE_VAL_XTS_AES128_KEY = 4;
    this.UARTDEV_BUF_NO = 0x3ffffd14; // Variable in ROM .bss which indicates the port in use
    this.UARTDEV_BUF_NO_USB_OTG = 2; // Value of the above indicating that USB-OTG is in use
    this.USB_RAM_BLOCK = 0x800; // Max block size USB-OTG is used
    this.GPIO_STRAP_REG = 0x3f404038;
    this.GPIO_STRAP_SPI_BOOT_MASK = 1 << 3; // Not download mode
    this.GPIO_STRAP_VDDSPI_MASK = 1 << 4;
    this.RTC_CNTL_OPTION1_REG = 0x3f408128;
    this.RTC_CNTL_FORCE_DOWNLOAD_BOOT_MASK = 0x1; // Is download mode forced over USB?
    this.RTCCNTL_BASE_REG = 0x3f408000;
    this.RTC_CNTL_WDTCONFIG0_REG = this.RTCCNTL_BASE_REG + 0x0094;
    this.RTC_CNTL_WDTCONFIG1_REG = this.RTCCNTL_BASE_REG + 0x0098;
    this.RTC_CNTL_WDTWPROTECT_REG = this.RTCCNTL_BASE_REG + 0x00ac;
    this.RTC_CNTL_WDT_WKEY = 0x50d83aa1;
    this.MEMORY_MAP = [[0x00000000, 0x00010000, "PADDING"], [0x3f000000, 0x3ff80000, "DROM"], [0x3f500000, 0x3ff80000, "EXTRAM_DATA"], [0x3ff9e000, 0x3ffa0000, "RTC_DRAM"], [0x3ff9e000, 0x40000000, "BYTE_ACCESSIBLE"], [0x3ff9e000, 0x40072000, "MEM_INTERNAL"], [0x3ffb0000, 0x40000000, "DRAM"], [0x40000000, 0x4001a100, "IROM_MASK"], [0x40020000, 0x40070000, "IRAM"], [0x40070000, 0x40072000, "RTC_IRAM"], [0x40080000, 0x40800000, "IROM"], [0x50000000, 0x50002000, "RTC_DATA"]];
    this.EFUSE_VDD_SPI_REG = this.EFUSE_BASE + 0x34;
    this.VDD_SPI_XPD = 1 << 4;
    this.VDD_SPI_TIEH = 1 << 5;
    this.VDD_SPI_FORCE = 1 << 6;
    this.UF2_FAMILY_ID = 0xbfdd4eee;
    this.EFUSE_MAX_KEY = 5;
    this.KEY_PURPOSES = {
      0: "USER/EMPTY",
      1: "RESERVED",
      2: "XTS_AES_256_KEY_1",
      3: "XTS_AES_256_KEY_2",
      4: "XTS_AES_128_KEY",
      5: "HMAC_DOWN_ALL",
      6: "HMAC_DOWN_JTAG",
      7: "HMAC_DOWN_DIGITAL_SIGNATURE",
      8: "HMAC_UP",
      9: "SECURE_BOOT_DIGEST0",
      10: "SECURE_BOOT_DIGEST1",
      11: "SECURE_BOOT_DIGEST2"
    };
    this.UART_CLKDIV_MASK = 0xfffff;
    this.UART_DATE_REG_ADDR = 0x60000078;
    this.FLASH_WRITE_SIZE = 0x400;
    this.BOOTLOADER_FLASH_OFFSET = 0x1000;
    this.FLASH_SIZES = {
      "1MB": 0x00,
      "2MB": 0x10,
      "4MB": 0x20,
      "8MB": 0x30,
      "16MB": 0x40
    };
  }
  async getPkgVersion(loader) {
    const numWord = 4;
    const addr = this.EFUSE_BLOCK1_ADDR + 4 * numWord;
    const word = await loader.readReg(addr);
    const pkgVersion = word >> 0 & 0x0f;
    return pkgVersion;
  }
  async getMinorChipVersion(loader) {
    const hiNumWord = 3;
    const hi = (await loader.readReg(this.EFUSE_BLOCK1_ADDR + 4 * hiNumWord)) >> 20 & 0x01;
    const lowNumWord = 4;
    const low = (await loader.readReg(this.EFUSE_BLOCK1_ADDR + 4 * lowNumWord)) >> 4 & 0x07;
    return (hi << 3) + low;
  }
  async getMajorChipVersion(loader) {
    const numWord = 3;
    return (await loader.readReg(this.EFUSE_BLOCK1_ADDR + 4 * numWord)) >> 18 & 0x03;
  }
  async getFlashVersion(loader) {
    const numWord = 3;
    return (await loader.readReg(this.EFUSE_BLOCK1_ADDR + 4 * numWord)) >> 21 & 0x0f;
  }
  async getChipDescription(loader) {
    const chipDesc = {
      0: "ESP32-S2",
      1: "ESP32-S2FH2",
      2: "ESP32-S2FH4",
      102: "ESP32-S2FNR2",
      100: "ESP32-S2R2"
    };
    const chipIndex = (await this.getFlashCap(loader)) + (await this.getPsramCap(loader)) * 100;
    const majorRev = await this.getMajorChipVersion(loader);
    const minorRev = await this.getMinorChipVersion(loader);
    return `${chipDesc[chipIndex] || "unknown ESP32-S2"} (revision v${majorRev}.${minorRev})`;
  }
  async getFlashCap(loader) {
    return await this.getFlashVersion(loader);
  }
  async getPsramVersion(loader) {
    const numWord = 3;
    const addr = this.EFUSE_BLOCK1_ADDR + 4 * numWord;
    const registerValue = await loader.readReg(addr);
    const psramCap = registerValue >> 28 & 0x0f;
    return psramCap;
  }
  async getPsramCap(loader) {
    return await this.getPsramVersion(loader);
  }
  async getBlock2Version(loader) {
    const numWord = 4;
    const addr = this.EFUSE_BLOCK2_ADDR + 4 * numWord;
    const registerValue = await loader.readReg(addr);
    const block2Ver = registerValue >> 4 & 0x07;
    return block2Ver;
  }
  async getChipFeatures(loader) {
    const features = ["Wi-Fi"];
    const flashMap = {
      0: "No Embedded Flash",
      1: "Embedded Flash 2MB",
      2: "Embedded Flash 4MB"
    };
    const flashCap = await this.getFlashCap(loader);
    const flashDescription = flashMap[flashCap] || "Unknown Embedded Flash";
    features.push(flashDescription);
    const psramMap = {
      0: "No Embedded Flash",
      1: "Embedded PSRAM 2MB",
      2: "Embedded PSRAM 4MB"
    };
    const psramCap = await this.getPsramCap(loader);
    const psramDescription = psramMap[psramCap] || "Unknown Embedded PSRAM";
    features.push(psramDescription);
    const block2VersionMap = {
      0: "No calibration in BLK2 of efuse",
      1: "ADC and temperature sensor calibration in BLK2 of efuse V1",
      2: "ADC and temperature sensor calibration in BLK2 of efuse V2"
    };
    const block2Ver = await this.getBlock2Version(loader);
    const block2VersionDescription = block2VersionMap[block2Ver] || "Unknown Calibration in BLK2";
    features.push(block2VersionDescription);
    return features;
  }
  async getCrystalFreq(loader) {
    return 40;
  }
  _d2h(d) {
    const h = (+d).toString(16);
    return h.length === 1 ? "0" + h : h;
  }
  async readMac(loader) {
    let mac0 = await loader.readReg(this.MAC_EFUSE_REG);
    mac0 = mac0 >>> 0;
    let mac1 = await loader.readReg(this.MAC_EFUSE_REG + 4);
    mac1 = mac1 >>> 0 & 0x0000ffff;
    const mac = new Uint8Array(6);
    mac[0] = mac1 >> 8 & 0xff;
    mac[1] = mac1 & 0xff;
    mac[2] = mac0 >> 24 & 0xff;
    mac[3] = mac0 >> 16 & 0xff;
    mac[4] = mac0 >> 8 & 0xff;
    mac[5] = mac0 & 0xff;
    return this._d2h(mac[0]) + ":" + this._d2h(mac[1]) + ":" + this._d2h(mac[2]) + ":" + this._d2h(mac[3]) + ":" + this._d2h(mac[4]) + ":" + this._d2h(mac[5]);
  }
  getEraseSize(offset, size) {
    return size;
  }
  async usingUsbOtg(loader) {
    const uartNo = (await loader.readReg(this.UARTDEV_BUF_NO)) & 0xff;
    return uartNo === this.UARTDEV_BUF_NO_USB_OTG;
  }
  async postConnect(loader) {
    const usingUsbOtg = await this.usingUsbOtg(loader);
    loader.debug("In _post_connect using USB OTG ?" + usingUsbOtg);
    if (usingUsbOtg) {
      loader.ESP_RAM_BLOCK = this.USB_RAM_BLOCK;
    }
  }
}

export { ESP32S2ROM };
