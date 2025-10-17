import { ESP32C3ROM } from './esp32c3-CfxJx9As.js';
import './rom-DGwTkJep.js';

class ESP32C2ROM extends ESP32C3ROM {
  constructor() {
    super(...arguments);
    this.CHIP_NAME = "ESP32-C2";
    this.IMAGE_CHIP_ID = 12;
    this.EFUSE_BASE = 0x60008800;
    this.MAC_EFUSE_REG = this.EFUSE_BASE + 0x040;
    this.UART_CLKDIV_REG = 0x60000014;
    this.UART_CLKDIV_MASK = 0xfffff;
    this.UART_DATE_REG_ADDR = 0x6000007c;
    this.XTAL_CLK_DIVIDER = 1;
    this.FLASH_WRITE_SIZE = 0x400;
    this.BOOTLOADER_FLASH_OFFSET = 0;
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
  }
  async getPkgVersion(loader) {
    const numWord = 1;
    const block1Addr = this.EFUSE_BASE + 0x040;
    const addr = block1Addr + 4 * numWord;
    const word3 = await loader.readReg(addr);
    const pkgVersion = word3 >> 22 & 0x07;
    return pkgVersion;
  }
  async getChipRevision(loader) {
    const block1Addr = this.EFUSE_BASE + 0x040;
    const numWord = 1;
    const pos = 20;
    const addr = block1Addr + 4 * numWord;
    const ret = ((await loader.readReg(addr)) & 0x03 << pos) >> pos;
    return ret;
  }
  async getChipDescription(loader) {
    let desc;
    const pkgVer = await this.getPkgVersion(loader);
    if (pkgVer === 0 || pkgVer === 1) {
      desc = "ESP32-C2";
    } else {
      desc = "unknown ESP32-C2";
    }
    const chip_rev = await this.getChipRevision(loader);
    desc += " (revision " + chip_rev + ")";
    return desc;
  }
  async getChipFeatures(loader) {
    return ["Wi-Fi", "BLE"];
  }
  async getCrystalFreq(loader) {
    const uartDiv = (await loader.readReg(this.UART_CLKDIV_REG)) & this.UART_CLKDIV_MASK;
    const etsXtal = loader.transport.baudrate * uartDiv / 1000000 / this.XTAL_CLK_DIVIDER;
    let normXtal;
    if (etsXtal > 33) {
      normXtal = 40;
    } else {
      normXtal = 26;
    }
    if (Math.abs(normXtal - etsXtal) > 1) {
      loader.info("WARNING: Unsupported crystal in use");
    }
    return normXtal;
  }
  async changeBaudRate(loader) {
    const rom_with_26M_XTAL = await this.getCrystalFreq(loader);
    if (rom_with_26M_XTAL === 26) {
      loader.changeBaud();
    }
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
}

export { ESP32C2ROM };
