import { R as ROM } from './rom-DGwTkJep.js';

class ESP32S3ROM extends ROM {
  constructor() {
    super(...arguments);
    this.CHIP_NAME = "ESP32-S3";
    this.IMAGE_CHIP_ID = 9;
    this.EFUSE_BASE = 0x60007000;
    this.MAC_EFUSE_REG = this.EFUSE_BASE + 0x044;
    this.EFUSE_BLOCK1_ADDR = this.EFUSE_BASE + 0x44;
    this.EFUSE_BLOCK2_ADDR = this.EFUSE_BASE + 0x5c;
    this.UART_CLKDIV_REG = 0x60000014;
    this.UART_CLKDIV_MASK = 0xfffff;
    this.UART_DATE_REG_ADDR = 0x60000080;
    this.FLASH_WRITE_SIZE = 0x400;
    this.BOOTLOADER_FLASH_OFFSET = 0x0;
    this.FLASH_SIZES = {
      "1MB": 0x00,
      "2MB": 0x10,
      "4MB": 0x20,
      "8MB": 0x30,
      "16MB": 0x40
    };
    this.SPI_REG_BASE = 0x60002000;
    this.SPI_USR_OFFS = 0x18;
    this.SPI_USR1_OFFS = 0x1c;
    this.SPI_USR2_OFFS = 0x20;
    this.SPI_MOSI_DLEN_OFFS = 0x24;
    this.SPI_MISO_DLEN_OFFS = 0x28;
    this.SPI_W0_OFFS = 0x58;
    this.USB_RAM_BLOCK = 0x800;
    this.UARTDEV_BUF_NO_USB = 3;
    this.UARTDEV_BUF_NO = 0x3fcef14c;
  }
  async getChipDescription(loader) {
    const majorRev = await this.getMajorChipVersion(loader);
    const minorRev = await this.getMinorChipVersion(loader);
    const pkgVersion = await this.getPkgVersion(loader);
    const chipName = {
      0: "ESP32-S3 (QFN56)",
      1: "ESP32-S3-PICO-1 (LGA56)"
    };
    return `${chipName[pkgVersion] || "unknown ESP32-S3"} (revision v${majorRev}.${minorRev})`;
  }
  async getPkgVersion(loader) {
    const numWord = 3;
    return (await loader.readReg(this.EFUSE_BLOCK1_ADDR + 4 * numWord)) >> 21 & 0x07;
  }
  async getRawMinorChipVersion(loader) {
    const hiNumWord = 5;
    const hi = (await loader.readReg(this.EFUSE_BLOCK1_ADDR + 4 * hiNumWord)) >> 23 & 0x01;
    const lowNumWord = 3;
    const low = (await loader.readReg(this.EFUSE_BLOCK1_ADDR + 4 * lowNumWord)) >> 18 & 0x07;
    return (hi << 3) + low;
  }
  async getMinorChipVersion(loader) {
    const minorRaw = await this.getRawMinorChipVersion(loader);
    if (await this.isEco0(loader, minorRaw)) {
      return 0;
    }
    return this.getRawMinorChipVersion(loader);
  }
  async getRawMajorChipVersion(loader) {
    const numWord = 5;
    return (await loader.readReg(this.EFUSE_BLOCK1_ADDR + 4 * numWord)) >> 24 & 0x03;
  }
  async getMajorChipVersion(loader) {
    const minorRaw = await this.getRawMinorChipVersion(loader);
    if (await this.isEco0(loader, minorRaw)) {
      return 0;
    }
    return this.getRawMajorChipVersion(loader);
  }
  async getBlkVersionMajor(loader) {
    const numWord = 4;
    return (await loader.readReg(this.EFUSE_BLOCK2_ADDR + 4 * numWord)) >> 0 & 0x03;
  }
  async getBlkVersionMinor(loader) {
    const numWord = 3;
    return (await loader.readReg(this.EFUSE_BLOCK1_ADDR + 4 * numWord)) >> 24 & 0x07;
  }
  async isEco0(loader, minorRaw) {
    // Workaround: The major version field was allocated to other purposes
    // when block version is v1.1.
    // Luckily only chip v0.0 have this kind of block version and efuse usage.
    return (minorRaw & 0x7) === 0 && (await this.getBlkVersionMajor(loader)) === 1 && (await this.getBlkVersionMinor(loader)) === 1;
  }
  async getFlashCap(loader) {
    const numWord = 3;
    const block1Addr = this.EFUSE_BASE + 0x044;
    const addr = block1Addr + 4 * numWord;
    const registerValue = await loader.readReg(addr);
    const flashCap = registerValue >> 27 & 0x07;
    return flashCap;
  }
  async getFlashVendor(loader) {
    const numWord = 4;
    const block1Addr = this.EFUSE_BASE + 0x044;
    const addr = block1Addr + 4 * numWord;
    const registerValue = await loader.readReg(addr);
    const vendorId = registerValue >> 0 & 0x07;
    const vendorMap = {
      1: "XMC",
      2: "GD",
      3: "FM",
      4: "TT",
      5: "BY"
    };
    return vendorMap[vendorId] || "";
  }
  async getPsramCap(loader) {
    const numWord = 4;
    const block1Addr = this.EFUSE_BASE + 0x044;
    const addr = block1Addr + 4 * numWord;
    const registerValue = await loader.readReg(addr);
    const psramCap = registerValue >> 3 & 0x03;
    return psramCap;
  }
  async getPsramVendor(loader) {
    const numWord = 4;
    const block1Addr = this.EFUSE_BASE + 0x044;
    const addr = block1Addr + 4 * numWord;
    const registerValue = await loader.readReg(addr);
    const vendorId = registerValue >> 7 & 0x03;
    const vendorMap = {
      1: "AP_3v3",
      2: "AP_1v8"
    };
    return vendorMap[vendorId] || "";
  }
  async getChipFeatures(loader) {
    const features = ["Wi-Fi", "BLE"];
    const flashMap = {
      0: null,
      1: "Embedded Flash 8MB",
      2: "Embedded Flash 4MB"
    };
    const flashCap = await this.getFlashCap(loader);
    const flashVendor = await this.getFlashVendor(loader);
    const flash = flashMap[flashCap];
    const flashDescription = flash !== undefined ? flash : "Unknown Embedded Flash";
    if (flash !== null) {
      features.push(`${flashDescription} (${flashVendor})`);
    }
    const psramMap = {
      0: null,
      1: "Embedded PSRAM 8MB",
      2: "Embedded PSRAM 2MB"
    };
    const psramCap = await this.getPsramCap(loader);
    const psramVendor = await this.getPsramVendor(loader);
    const psram = psramMap[psramCap];
    const psramDescription = psram !== undefined ? psram : "Unknown Embedded PSRAM";
    if (psram !== null) {
      features.push(`${psramDescription} (${psramVendor})`);
    }
    return features;
  }
  async getCrystalFreq(loader) {
    return 40;
  }
  _d2h(d) {
    const h = (+d).toString(16);
    return h.length === 1 ? "0" + h : h;
  }
  async postConnect(loader) {
    const bufNo = (await loader.readReg(this.UARTDEV_BUF_NO)) & 0xff;
    loader.debug("In _post_connect " + bufNo);
    if (bufNo == this.UARTDEV_BUF_NO_USB) {
      loader.ESP_RAM_BLOCK = this.USB_RAM_BLOCK;
    }
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
}

export { ESP32S3ROM };
