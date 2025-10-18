import { ESP32C6ROM } from './esp32c6-DsFTtj2a.js';
import './rom-DGwTkJep.js';

class ESP32C61ROM extends ESP32C6ROM {
  constructor() {
    super(...arguments);
    this.CHIP_NAME = "ESP32-C61";
    this.IMAGE_CHIP_ID = 20;
    this.CHIP_DETECT_MAGIC_VALUE = [0x33f0206f, 0x2421606f];
    this.UART_DATE_REG_ADDR = 0x60000000 + 0x7c;
    this.EFUSE_BASE = 0x600b4800;
    this.EFUSE_BLOCK1_ADDR = this.EFUSE_BASE + 0x044;
    this.MAC_EFUSE_REG = this.EFUSE_BASE + 0x044;
    this.EFUSE_RD_REG_BASE = this.EFUSE_BASE + 0x030; // BLOCK0 read base address
    this.EFUSE_PURPOSE_KEY0_REG = this.EFUSE_BASE + 0x34;
    this.EFUSE_PURPOSE_KEY0_SHIFT = 0;
    this.EFUSE_PURPOSE_KEY1_REG = this.EFUSE_BASE + 0x34;
    this.EFUSE_PURPOSE_KEY1_SHIFT = 4;
    this.EFUSE_PURPOSE_KEY2_REG = this.EFUSE_BASE + 0x34;
    this.EFUSE_PURPOSE_KEY2_SHIFT = 8;
    this.EFUSE_PURPOSE_KEY3_REG = this.EFUSE_BASE + 0x34;
    this.EFUSE_PURPOSE_KEY3_SHIFT = 12;
    this.EFUSE_PURPOSE_KEY4_REG = this.EFUSE_BASE + 0x34;
    this.EFUSE_PURPOSE_KEY4_SHIFT = 16;
    this.EFUSE_PURPOSE_KEY5_REG = this.EFUSE_BASE + 0x34;
    this.EFUSE_PURPOSE_KEY5_SHIFT = 20;
    this.EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT_REG = this.EFUSE_RD_REG_BASE;
    this.EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT = 1 << 20;
    this.EFUSE_SPI_BOOT_CRYPT_CNT_REG = this.EFUSE_BASE + 0x030;
    this.EFUSE_SPI_BOOT_CRYPT_CNT_MASK = 0x7 << 23;
    this.EFUSE_SECURE_BOOT_EN_REG = this.EFUSE_BASE + 0x034;
    this.EFUSE_SECURE_BOOT_EN_MASK = 1 << 26;
    this.FLASH_FREQUENCY = {
      "80m": 0xf,
      "40m": 0x0,
      "20m": 0x2
    };
    this.MEMORY_MAP = [[0x00000000, 0x00010000, "PADDING"], [0x41800000, 0x42000000, "DROM"], [0x40800000, 0x40860000, "DRAM"], [0x40800000, 0x40860000, "BYTE_ACCESSIBLE"], [0x4004ac00, 0x40050000, "DROM_MASK"], [0x40000000, 0x4004ac00, "IROM_MASK"], [0x41000000, 0x41800000, "IROM"], [0x40800000, 0x40860000, "IRAM"], [0x50000000, 0x50004000, "RTC_IRAM"], [0x50000000, 0x50004000, "RTC_DRAM"], [0x600fe000, 0x60100000, "MEM_INTERNAL2"]];
    this.UF2_FAMILY_ID = 0x77d850c4;
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
      12: "KM_INIT_KEY",
      13: "XTS_AES_256_KEY_1_PSRAM",
      14: "XTS_AES_256_KEY_2_PSRAM",
      15: "XTS_AES_128_KEY_PSRAM"
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
      desc = "ESP32-C61";
    } else {
      desc = "unknown ESP32-C61";
    }
    const majorRev = await this.getMajorChipVersion(loader);
    const minorRev = await this.getMinorChipVersion(loader);
    return `${desc} (revision v${majorRev}.${minorRev})`;
  }
  async getChipFeatures(loader) {
    return ["WiFi 6", "BT 5"];
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
}

export { ESP32C61ROM };
