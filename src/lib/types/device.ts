export type FeatureRow = {
	CC1101: boolean;
	NRF24: boolean;
	FMRadio: boolean;
	PN532: boolean;
	Mic_SPM1423: boolean;
	BadUSB: boolean;
	RGB_Led: boolean;
	Speaker: string | boolean;
};

export type DeviceCompatibility = {
	device: string;
} & FeatureRow;
