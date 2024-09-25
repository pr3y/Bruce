void newPacketSD(uint32_t ts_sec, uint32_t ts_usec, uint32_t len, uint8_t* buf);

bool openFile(FS &Fs);

void openFile2(FS &Fs);

void sniffer_setup();

void sniffer(void *buf, wifi_promiscuous_pkt_type_t type);