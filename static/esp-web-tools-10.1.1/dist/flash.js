import { Transport, ESPLoader } from "esptool-js";
import { hardReset } from "./util/reset";
export const flash = async (onEvent, port, manifestPath, manifest, eraseFirst) => {
    let build;
    let chipFamily;
    const fireStateEvent = (stateUpdate) => onEvent({
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
    window.esploader = esploader;
    fireStateEvent({
        state: "initializing" /* FlashStateType.INITIALIZING */,
        message: "Initializing...",
        details: { done: false },
    });
    try {
        await esploader.main();
        await esploader.flashId();
    }
    catch (err) {
        console.error(err);
        fireStateEvent({
            state: "error" /* FlashStateType.ERROR */,
            message: "Failed to initialize. Try resetting your device or holding the BOOT button while clicking INSTALL.",
            details: { error: "failed_initialize" /* FlashError.FAILED_INITIALIZING */, details: err },
        });
        await hardReset(transport);
        await transport.disconnect();
        return;
    }
    chipFamily = esploader.chip.CHIP_NAME;
    fireStateEvent({
        state: "initializing" /* FlashStateType.INITIALIZING */,
        message: `Initialized. Found ${chipFamily}`,
        details: { done: true },
    });
    build = manifest.builds.find((b) => b.chipFamily === chipFamily);
    if (!build) {
        fireStateEvent({
            state: "error" /* FlashStateType.ERROR */,
            message: `Your ${chipFamily} board is not supported.`,
            details: { error: "not_supported" /* FlashError.NOT_SUPPORTED */, details: chipFamily },
        });
        await hardReset(transport);
        await transport.disconnect();
        return;
    }
    fireStateEvent({
        state: "preparing" /* FlashStateType.PREPARING */,
        message: "Preparing installation...",
        details: { done: false },
    });
    const manifestURL = new URL(manifestPath, location.toString()).toString();
    const filePromises = build.parts.map(async (part) => {
        const url = new URL(part.path, manifestURL).toString();
        const resp = await fetch(url);
        if (!resp.ok) {
            throw new Error(`Downlading firmware ${part.path} failed: ${resp.status}`);
        }
        const reader = new FileReader();
        const blob = await resp.blob();
        return new Promise((resolve) => {
            reader.addEventListener("load", () => resolve(reader.result));
            reader.readAsBinaryString(blob);
        });
    });
    const fileArray = [];
    let totalSize = 0;
    for (let part = 0; part < filePromises.length; part++) {
        try {
            const data = await filePromises[part];
            fileArray.push({ data, address: build.parts[part].offset });
            totalSize += data.length;
        }
        catch (err) {
            fireStateEvent({
                state: "error" /* FlashStateType.ERROR */,
                message: err.message,
                details: {
                    error: "failed_firmware_download" /* FlashError.FAILED_FIRMWARE_DOWNLOAD */,
                    details: err.message,
                },
            });
            await hardReset(transport);
            await transport.disconnect();
            return;
        }
    }
    fireStateEvent({
        state: "preparing" /* FlashStateType.PREPARING */,
        message: "Installation prepared",
        details: { done: true },
    });
    if (eraseFirst) {
        fireStateEvent({
            state: "erasing" /* FlashStateType.ERASING */,
            message: "Erasing device...",
            details: { done: false },
        });
        await esploader.eraseFlash();
        fireStateEvent({
            state: "erasing" /* FlashStateType.ERASING */,
            message: "Device erased",
            details: { done: true },
        });
    }
    fireStateEvent({
        state: "writing" /* FlashStateType.WRITING */,
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
            reportProgress: (fileIndex, written, total) => {
                const uncompressedWritten = (written / total) * fileArray[fileIndex].data.length;
                const newPct = Math.floor(((totalWritten + uncompressedWritten) / totalSize) * 100);
                // we're done with this file
                if (written === total) {
                    totalWritten += uncompressedWritten;
                    return;
                }
                fireStateEvent({
                    state: "writing" /* FlashStateType.WRITING */,
                    message: `Writing progress: ${newPct}%`,
                    details: {
                        bytesTotal: totalSize,
                        bytesWritten: totalWritten + written,
                        percentage: newPct,
                    },
                });
            },
        });
    }
    catch (err) {
        fireStateEvent({
            state: "error" /* FlashStateType.ERROR */,
            message: err.message,
            details: { error: "write_failed" /* FlashError.WRITE_FAILED */, details: err },
        });
        await hardReset(transport);
        await transport.disconnect();
        return;
    }
    fireStateEvent({
        state: "writing" /* FlashStateType.WRITING */,
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
        state: "finished" /* FlashStateType.FINISHED */,
        message: "All done!",
    });
};
