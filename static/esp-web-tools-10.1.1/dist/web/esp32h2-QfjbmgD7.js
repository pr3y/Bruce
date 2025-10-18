import { R as ROM } from './rom-DGwTkJep.js';

class ESP32H2ROM extends ROM {
  constructor() {
    super(...arguments);
    this.CHIP_NAME = "ESP32-H2";
    this.IMAGE_CHIP_ID = 16;
    this.EFUSE_BASE = 0x60008800;
    this.MAC_EFUSE_REG = this.EFUSE_BASE + 0x044;
    this.UART_CLKDIV_REG = 0x3ff40014;
    this.UART_CLKDIV_MASK = 0xfffff;
    this.UART_DATE_REG_ADDR = 0x6000007c;
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
    return this.CHIP_NAME;
  }
  async getChipFeatures(loader) {
    return ["BLE", "IEEE802.15.4"];
  }
  async getCrystalFreq(loader) {
    // ESP32H2 XTAL is fixed to 32MHz
    return 32;
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

export { ESP32H2ROM };
