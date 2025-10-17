import { Transport, ESPLoader } from "esptool-js";
import {
  Build,
  FlashError,
  FlashState,
  Manifest,
  FlashStateType,
} from "./const";
import { hardReset } from "./util/reset";

export const flash = async (
  onEvent: (state: FlashState) => void,
  port: SerialPort,
  manifestPath: string,
  manifest: Manifest,
  eraseFirst: boolean,
) => {
  let build: Build | undefined;
  let chipFamily: Build["chipFamily"];

  const fireStateEvent = (stateUpdate: FlashState) =>
    onEvent({
      ...stateUpdate,
      manifest,
      build,
      chipFamily,
    });

  const transport = new Transport(port);
  const esploader = new ESPLoader({
    transport,
    baudrate: 115200,
    romBaudrate: 115200,
    enableTracing: false,
  });

  // For debugging
  (window as any).esploader = esploader;

  fireStateEvent({
    state: FlashStateType.INITIALIZING,
    message: "Initializing...",
    details: { done: false },
  });

  try {
    await esploader.main();
    await esploader.flashId();
  } catch (err: any) {
    console.error(err);
    fireStateEvent({
      state: FlashStateType.ERROR,
      message:
        "Failed to initialize. Try resetting your device or holding the BOOT button while clicking INSTALL.",
      details: { error: FlashError.FAILED_INITIALIZING, details: err },
    });

    await hardReset(transport);
    await transport.disconnect();
    return;
  }

  chipFamily = esploader.chip.CHIP_NAME as any;

  fireStateEvent({
    state: FlashStateType.INITIALIZING,
    message: `Initialized. Found ${chipFamily}`,
    details: { done: true },
  });

  build = manifest.builds.find((b) => b.chipFamily === chipFamily);

  if (!build) {
    fireStateEvent({
      state: FlashStateType.ERROR,
      message: `Your ${chipFamily} board is not supported.`,
      details: { error: FlashError.NOT_SUPPORTED, details: chipFamily },
    });
    await hardReset(transport);
    await transport.disconnect();
    return;
  }

  fireStateEvent({
    state: FlashStateType.PREPARING,
    message: "Preparing installation...",
    details: { done: false },
  });

  const manifestURL = new URL(manifestPath, location.toString()).toString();
  const filePromises = build.parts.map(async (part) => {
    const url = new URL(part.path, manifestURL).toString();
    const resp = await fetch(url);
    if (!resp.ok) {
      throw new Error(
        `Downlading firmware ${part.path} failed: ${resp.status}`,
      );
    }

    const reader = new FileReader();
    const blob = await resp.blob();

    return new Promise<string>((resolve) => {
      reader.addEventListener("load", () => resolve(reader.result as string));
      reader.readAsBinaryString(blob);
    });
  });

  const fileArray: Array<{ data: string; address: number }> = [];
  let totalSize = 0;

  for (let part = 0; part < filePromises.length; part++) {
    try {
      const data = await filePromises[part];
      fileArray.push({ data, address: build.parts[part].offset });
      totalSize += data.length;
    } catch (err: any) {
      fireStateEvent({
        state: FlashStateType.ERROR,
        message: err.message,
        details: {
          error: FlashError.FAILED_FIRMWARE_DOWNLOAD,
          details: err.message,
        },
      });
      await hardReset(transport);
      await transport.disconnect();
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
    await esploader.eraseFlash();
    fireStateEvent({
      state: FlashStateType.ERASING,
      message: "Device erased",
      details: { done: true },
    });
  }

  fireStateEvent({
    state: FlashStateType.WRITING,
    message: `Writing progress: 0%`,
    details: {
      bytesTotal: totalSize,
      bytesWritten: 0,
      percentage: 0,
    },
  });

  let totalWritten = 0;

  try {
    await esploader.writeFlash({
      fileArray,
      flashSize: "keep",
      flashMode: "keep",
      flashFreq: "keep",
      eraseAll: false,
      compress: true,
      // report progress
      reportProgress: (fileIndex: number, written: number, total: number) => {
        const uncompressedWritten =
          (written / total) * fileArray[fileIndex].data.length;

        const newPct = Math.floor(
          ((totalWritten + uncompressedWritten) / totalSize) * 100,
        );

        // we're done with this file
        if (written === total) {
          totalWritten += uncompressedWritten;
          return;
        }

        fireStateEvent({
          state: FlashStateType.WRITING,
          message: `Writing progress: ${newPct}%`,
          details: {
            bytesTotal: totalSize,
            bytesWritten: totalWritten + written,
            percentage: newPct,
          },
        });
      },
    });
  } catch (err: any) {
    fireStateEvent({
      state: FlashStateType.ERROR,
      message: err.message,
      details: { error: FlashError.WRITE_FAILED, details: err },
    });
    await hardReset(transport);
    await transport.disconnect();
    return;
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

  await hardReset(transport);

  console.log("DISCONNECT");
  await transport.disconnect();

  fireStateEvent({
    state: FlashStateType.FINISHED,
    message: "All done!",
  });
};
