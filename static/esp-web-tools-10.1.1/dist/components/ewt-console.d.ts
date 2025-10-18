import { Logger } from "../const";
export declare class EwtConsole extends HTMLElement {
    port: SerialPort;
    logger: Logger;
    allowInput: boolean;
    private _console?;
    private _cancelConnection?;
    logs(): string;
    connectedCallback(): void;
    private _connect;
    private _sendCommand;
    disconnect(): Promise<void>;
    reset(): Promise<void>;
}
declare global {
    interface HTMLElementTagNameMap {
        "ewt-console": EwtConsole;
    }
}
