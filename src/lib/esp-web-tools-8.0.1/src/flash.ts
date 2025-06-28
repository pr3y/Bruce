import { ESPLoader, Logger } from "esp-web-flasher";
import {
  Build,
  FlashError,
  FlashState,
  Manifest,
  FlashStateType,
} from "./const";
import { getChipFamilyName } from "./util/chip-family-name";
import { sleep } from "./util/sleep";

export const flash = async (
  onEvent: (state: FlashState) => void,
  port: SerialPort,
  logger: Logger,
  manifestPath: string,
  eraseFirst: boolean
) => {
  let manifest: Manifest;
  let build: Build | undefined;
  let chipFamily: ReturnType<typeof getChipFamilyName>;

  const fireStateEvent = (stateUpdate: FlashState) =>
    onEvent({
      ...stateUpdate,
      manifest,
      build,
      chipFamily,
    });

  const manifestURL = new URL(manifestPath, location.toString()).toString();
  const manifestProm = fetch(manifestURL).then(
    (resp): Promise<Manifest> => resp.json()
  );

  const esploader = new ESPLoader(port, logger);

  // For debugging
  (window as any).esploader = esploader;

  fireStateEvent({
    state: FlashStateType.INITIALIZING,
    message: "Initializing...",
    details: { done: false },
  });

  try {
    await esploader.initialize();
  } catch (err: any) {
    logger.error(err);
    if (esploader.connected) {
      fireStateEvent({
        state: FlashStateType.ERROR,
        message:
          "Failed to initialize. Try resetting your device or holding the BOOT button while clicking INSTALL.",
        details: { error: FlashError.FAILED_INITIALIZING, details: err },
      });
      await esploader.disconnect();
    }
    return;
  }

  chipFamily = getChipFamilyName(esploader);

  fireStateEvent({
    state: FlashStateType.INITIALIZING,
    message: `Initialized. Found ${chipFamily}`,
    details: { done: true },
  });
  fireStateEvent({
    state: FlashStateType.MANIFEST,
    message: "Fetching manifest...",
    details: { done: false },
  });

  try {
    manifest = await manifestProm;
  } catch (err: any) {
    fireStateEvent({
      state: FlashStateType.ERROR,
      message: `Unable to fetch manifest: ${err}`,
      details: { error: FlashError.FAILED_MANIFEST_FETCH, details: err },
    });
    await esploader.disconnect();
    return;
  }

  build = manifest.builds.find((b) => b.chipFamily === chipFamily);

  fireStateEvent({
    state: FlashStateType.MANIFEST,
    message: `Found manifest for ${manifest.name}`,
    details: { done: true },
  });

  if (!build) {
    fireStateEvent({
      state: FlashStateType.ERROR,
      message: `Your ${chipFamily} board is not supported.`,
      details: { error: FlashError.NOT_SUPPORTED, details: chipFamily },
    });
    await esploader.disconnect();
    return;
  }

  fireStateEvent({
    state: FlashStateType.PREPARING,
    message: "Preparing installation...",
    details: { done: false },
  });

  const filePromises = build.parts.map(async (part) => {
    const url = new URL(part.path, manifestURL).toString();
    const resp = await fetch(url);
    if (!resp.ok) {
      throw new Error(
        `Downlading firmware ${part.path} failed: ${resp.status}`
      );
    }
    return resp.arrayBuffer();
  });

  // Run the stub while we wait for files to download
  const espStub = await esploader.runStub();

  const files: ArrayBuffer[] = [];
  let totalSize = 0;

  for (const prom of filePromises) {
    try {
      const data = await prom;
      files.push(data);
      totalSize += data.byteLength;
    } catch (err: any) {
      fireStateEvent({
        state: FlashStateType.ERROR,
        message: err.message,
        details: {
          error: FlashError.FAILED_FIRMWARE_DOWNLOAD,
          details: err.message,
        },
      });
      await esploader.disconnect();
      return;
    }
  }

  fireStateEvent({
    state: FlashStateType.PREPARING,
    message: "Installation prepared",
    details: { done: true },
  });

  if (eraseFirst) {
    fireStateEvent({
      state: FlashStateType.ERASING,
      message: "Erasing device...",
      details: { done: false },
    });
    await espStub.eraseFlash();
    fireStateEvent({
      state: FlashStateType.ERASING,
      message: "Device erased",
      details: { done: true },
    });
  }

  let lastPct = 0;

  fireStateEvent({
    state: FlashStateType.WRITING,
    message: `Writing progress: ${lastPct}%`,
    details: {
      bytesTotal: totalSize,
      bytesWritten: 0,
      percentage: lastPct,
    },
  });

  let totalWritten = 0;

  for (const part of build.parts) {
    const file = files.shift()!;
    try {
      await espStub.flashData(
        file,
        (bytesWritten: number) => {
          const newPct = Math.floor(
            ((totalWritten + bytesWritten) / totalSize) * 100
          );
          if (newPct === lastPct) {
            return;
          }
          lastPct = newPct;
          fireStateEvent({
            state: FlashStateType.WRITING,
            message: `Writing progress: ${newPct}%`,
            details: {
              bytesTotal: totalSize,
              bytesWritten: totalWritten + bytesWritten,
              percentage: newPct,
            },
          });
        },
        part.offset,
        true
      );
    } catch (err: any) {
      fireStateEvent({
        state: FlashStateType.ERROR,
        message: err.message,
        details: { error: FlashError.WRITE_FAILED, details: err },
      });
      await esploader.disconnect();
      return;
    }
    totalWritten += file.byteLength;
  }

  fireStateEvent({
    state: FlashStateType.WRITING,
    message: "Writing complete",
    details: {
      bytesTotal: totalSize,
      bytesWritten: totalWritten,
      percentage: 100,
    },
  });

  await sleep(100);
  console.log("DISCONNECT");
  await esploader.disconnect();
  console.log("HARD RESET");
  await esploader.hardReset();

  fireStateEvent({
    state: FlashStateType.FINISHED,
    message: "All done!",
  });
};
