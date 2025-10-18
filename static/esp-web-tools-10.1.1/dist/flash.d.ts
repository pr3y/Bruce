import { FlashState, Manifest } from "./const";
export declare const flash: (onEvent: (state: FlashState) => void, port: SerialPort, manifestPath: string, manifest: Manifest, eraseFirst: boolean) => Promise<void>;
