/// <reference types="w3c-web-serial" />
import { Logger } from "esp-web-flasher";
import { FlashState } from "./const";
export declare const flash: (onEvent: (state: FlashState) => void, port: SerialPort, logger: Logger, manifestPath: string, eraseFirst: boolean) => Promise<void>;
