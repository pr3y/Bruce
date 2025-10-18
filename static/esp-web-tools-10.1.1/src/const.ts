export interface Logger {
  log(msg: string, ...args: any[]): void;
  error(msg: string, ...args: any[]): void;
  debug(msg: string, ...args: any[]): void;
}

export interface Build {
  chipFamily:
    | "ESP32"
    | "ESP32-C2"
    | "ESP32-C3"
    | "ESP32-C6"
    | "ESP32-H2"
    | "ESP32-S2"
    | "ESP32-S3"
    | "ESP8266";
  parts: {
    path: string;
    offset: number;
  }[];
}

export interface Manifest {
  name: string;
  version: string;
  home_assistant_domain?: string;
  funding_url?: string;
  /** @deprecated use `new_install_prompt_erase` instead */
  new_install_skip_erase?: boolean;
  new_install_prompt_erase?: boolean;
  /* Time to wait to detect Improv Wi-Fi. Set to 0 to disable. */
  new_install_improv_wait_time?: number;
  builds: Build[];
}

export interface BaseFlashState {
  state: FlashStateType;
  message: string;
  manifest?: Manifest;
  build?: Build;
  chipFamily?: Build["chipFamily"] | "Unknown Chip";
}

export interface InitializingState extends BaseFlashState {
  state: FlashStateType.INITIALIZING;
  details: { done: boolean };
}

export interface PreparingState extends BaseFlashState {
  state: FlashStateType.PREPARING;
  details: { done: boolean };
}

export interface ErasingState extends BaseFlashState {
  state: FlashStateType.ERASING;
  details: { done: boolean };
}

export interface WritingState extends BaseFlashState {
  state: FlashStateType.WRITING;
  details: { bytesTotal: number; bytesWritten: number; percentage: number };
}

export interface FinishedState extends BaseFlashState {
  state: FlashStateType.FINISHED;
}

export interface ErrorState extends BaseFlashState {
  state: FlashStateType.ERROR;
  details: { error: FlashError; details: string | Error };
}

export type FlashState =
  | InitializingState
  | PreparingState
  | ErasingState
  | WritingState
  | FinishedState
  | ErrorState;

export const enum FlashStateType {
  INITIALIZING = "initializing",
  PREPARING = "preparing",
  ERASING = "erasing",
  WRITING = "writing",
  FINISHED = "finished",
  ERROR = "error",
}

export const enum FlashError {
  FAILED_INITIALIZING = "failed_initialize",
  FAILED_MANIFEST_FETCH = "fetch_manifest_failed",
  NOT_SUPPORTED = "not_supported",
  FAILED_FIRMWARE_DOWNLOAD = "failed_firmware_download",
  WRITE_FAILED = "write_failed",
}

declare global {
  interface HTMLElementEventMap {
    "state-changed": CustomEvent<FlashState>;
  }
}
