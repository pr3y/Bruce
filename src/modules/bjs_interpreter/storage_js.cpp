#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#include "storage_js.h"

#include "core/sd_functions.h"

#include "helpers_js.h"

duk_ret_t putPropStorageFunctions(duk_context *ctx, duk_idx_t obj_idx, uint8_t magic) {
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "readdir", native_storageReaddir, 2, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "read", native_storageRead, 2, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "write", native_storageWrite, 4, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "rename", native_storageRename, 2, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "remove", native_storageRemove, 1, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "mkdir", native_storageMkdir, 1, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "rmdir", native_storageRmdir, 1, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "spaceLittleFS", native_storageSpaceLittleFS, 0, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "spaceSDCard", native_storageSpaceSDCard, 0, magic);
    return 0;
}

duk_ret_t registerStorage(duk_context *ctx) {
    bduk_register_c_lightfunc(ctx, "storageReaddir", native_storageReaddir, 2);
    bduk_register_c_lightfunc(ctx, "storageRead", native_storageRead, 2);
    bduk_register_c_lightfunc(ctx, "storageWrite", native_storageWrite, 4);
    bduk_register_c_lightfunc(ctx, "storageRename", native_storageRename, 2);
    bduk_register_c_lightfunc(ctx, "storageRemove", native_storageRemove, 1);
    bduk_register_c_lightfunc(ctx, "storageSpaceLittleFS", native_storageSpaceLittleFS, 0);
    bduk_register_c_lightfunc(ctx, "storageSpaceSDCard", native_storageSpaceSDCard, 0);
    return 0;
}

duk_ret_t native_storageReaddir(duk_context *ctx) {
    // usage: storageReaddir(path: string | Path, options?: { withFileTypes?:
    // false }): string[]
    // usage: storageReaddir(path: string | Path, options: {
    // withFileTypes: true }): { name: string, size: number, isDirectory: boolean
    // }[]

    // Extract path
    FileParamsJS fileParams = js_get_path_from_params(ctx, true);
    if (!fileParams.exist) {
        return duk_error(
            ctx, DUK_ERR_ERROR, "%s: Directory does not exist: %s", "storageReaddir", fileParams.path.c_str()
        );
    }

    // Extract options object (optional)
    duk_get_prop_string(ctx, 1, "withFileTypes");
    bool withFileTypes = duk_get_boolean_default(ctx, -1, false);
    duk_pop(ctx);

    // Open directory
    File root = (fileParams.fs)->open(fileParams.path);
    if (!root || !root.isDirectory()) {
        return duk_error(
            ctx, DUK_ERR_ERROR, "%s: Not a directory: %s", "storageReaddir", fileParams.path.c_str()
        );
    }

    // Create result array
    duk_idx_t arr_idx = duk_push_array(ctx);
    int index = 0;

    while (true) {
        bool isDir;
        String fullPath = root.getNextFileName(&isDir);
        String nameOnly = fullPath.substring(fullPath.lastIndexOf("/") + 1);
        if (fullPath == "") { break; }
        // Serial.printf("Path: %s (isDir: %d)\n", fullPath.c_str(), isDir);

        if (withFileTypes) {
            // Return objects with name, size, and isDirectory
            duk_idx_t obj_idx = duk_push_object(ctx);
            duk_push_string(ctx, nameOnly.c_str());
            duk_put_prop_string(ctx, obj_idx, "name");

            if (isDir) {
                duk_push_int(ctx, 0);
            } else {
                // Serial.printf("Opening file for size check: %s\n", fullPath.c_str());
                File file = (fileParams.fs)->open(fullPath);
                // Serial.printf("File size: %llu bytes\n", file.size());
                duk_push_int(ctx, file.size());
                file.close();
            }
            duk_put_prop_string(ctx, obj_idx, "size");

            duk_push_boolean(ctx, isDir);
            duk_put_prop_string(ctx, obj_idx, "isDirectory");

            duk_put_prop_index(ctx, arr_idx, index++);
        } else {
            // Return an array of filenames
            duk_push_string(ctx, nameOnly.c_str());
            duk_put_prop_index(ctx, arr_idx, index++);
        }
    }
    root.close();

    return 1; // Return array
}

duk_ret_t native_storageRead(duk_context *ctx) {
    // usage: storageRead(path: string | Path, binary: boolean): string |
    // Uint8Array returns: file contents as a string. Empty string on any error.
    bool binary = duk_get_boolean_default(ctx, 1, false);
    size_t fileSize = 0;
    char *fileContent = NULL;
    FileParamsJS fileParams = js_get_path_from_params(ctx, true);
    if (!fileParams.exist) {
        return duk_error(
            ctx, DUK_ERR_ERROR, "%s: File: %s does not exist", "storageRead", fileParams.path.c_str()
        );
    }
    if (!fileParams.path.startsWith("/")) fileParams.path = "/" + fileParams.path; // add "/" if missing

    // TODO: Change to use duk_push_fixed_buffer
    fileContent = readBigFile(*fileParams.fs, fileParams.path, binary, &fileSize);

    if (fileContent == NULL) {
        return duk_error(
            ctx, DUK_ERR_ERROR, "%s: Could not read file: %s", "storageRead", fileParams.path.c_str()
        );
    }

    if (binary && fileSize != 0) {
        void *buf = duk_push_fixed_buffer(ctx, fileSize);
        memcpy(buf, fileContent, fileSize);
        // Convert buffer to Uint8Array
        duk_push_buffer_object(ctx, -1, 0, fileSize, DUK_BUFOBJ_UINT8ARRAY);
    } else {
        duk_push_string(ctx, fileContent);
    }
    free(fileContent);
    return 1;
}

duk_ret_t native_storageWrite(duk_context *ctx) {
    // usage: storageWrite(path: string | Path, data: string | Uint8Array, mode:
    // "write" | "append", position: number | string): boolean The write function
    // writes a string to a file, returning true if successful. Overwrites
    // existing file. The first parameter is the path of the file or object {fs:
    // string, path: string}. The second parameter is the contents to write

    duk_size_t dataSize;
    void *data = duk_to_buffer(ctx, 1, &dataSize);

    FileParamsJS fileParams = js_get_path_from_params(ctx, true);
    if (!fileParams.path.startsWith("/")) fileParams.path = "/" + fileParams.path; // add "/" if missing

    const char *mode = FILE_APPEND; // default append
    const char *modeString = duk_get_string_default(ctx, 2, "a");
    if (modeString[0] == 'w') mode = FILE_WRITE;

    File file = (fileParams.fs)->open(fileParams.path, mode, true);
    if (!file) {
        duk_push_boolean(ctx, false);
        return 1;
    }

    // Check if position is provided
    if (duk_is_number(ctx, 3)) {
        // Get position as number
        int64_t pos = duk_get_int(ctx, 3);
        if (pos < 0) {
            // Negative index: seek from end
            file.seek(file.size() + pos, SeekSet);
        } else {
            file.seek(pos, SeekSet);
        }
    } else if (duk_is_string(ctx, 3)) {
        // Get position as string
        size_t fileSize = 0;
        char *fileContent = readBigFile(*fileParams.fs, fileParams.path, false, &fileSize);

        if (fileContent == NULL) {
            return duk_error(
                ctx, DUK_ERR_ERROR, "%s: Could not read file: %s", "storageWrite", fileParams.path.c_str()
            );
        }

        char *foundPos = strstr(fileContent, duk_get_string(ctx, 3));
        free(fileContent); // Free fileContent after usage

        if (foundPos) {
            file.seek(foundPos - fileContent, SeekSet);
        } else {
            file.seek(0, SeekEnd); // Append if string is not found
        }
    }

    // Write data
    file.write((const uint8_t *)data, dataSize);
    file.close();

    duk_push_boolean(ctx, true);

    return 1;
}

duk_ret_t native_storageRename(duk_context *ctx) {
    // usage: storageRename(oldPath: string | Path, newPath: string): boolean
    FileParamsJS oldFileParams = js_get_path_from_params(ctx, true);
    String newPath = duk_get_string_default(ctx, 1, "");

    if (!oldFileParams.exist) {
        return duk_error(
            ctx, DUK_ERR_ERROR, "%s: File: %s does not exist", "storageRename", oldFileParams.path.c_str()
        );
    }

    if (!oldFileParams.path.startsWith("/")) oldFileParams.path = "/" + oldFileParams.path;
    if (!newPath.startsWith("/")) newPath = "/" + newPath;

    bool success = (oldFileParams.fs)->rename(oldFileParams.path, newPath);
    duk_push_boolean(ctx, success);
    return 1;
}

duk_ret_t native_storageRemove(duk_context *ctx) {
    // usage: storageRemove(path: string | Path): boolean
    FileParamsJS fileParams = js_get_path_from_params(ctx, true);
    if (!fileParams.exist) {
        return duk_error(
            ctx, DUK_ERR_ERROR, "%s: File: %s does not exist", "storageRemove", fileParams.path.c_str()
        );
    }

    if (!fileParams.path.startsWith("/")) { fileParams.path = "/" + fileParams.path; }

    bool success = (fileParams.fs)->remove(fileParams.path);
    duk_push_boolean(ctx, success);
    return 1;
}

duk_ret_t native_storageMkdir(duk_context *ctx) {
    // usage: storageMkdir(path: string | Path): boolean
    FileParamsJS fileParams = js_get_path_from_params(ctx, true);

    if (!fileParams.path.startsWith("/")) fileParams.path = "/" + fileParams.path;

    String tempPath;
    bool success = true;

    // Create each part of the path
    // for (size_t i = 1; i < fileParams.path.length(); i++) {
    //   if (fileParams.path[i] == '/') {
    //     tempPath = fileParams.path.substring(0, i);
    //     if (!(fileParams.fs)->exists(tempPath)) {
    //       success = (fileParams.fs)->mkdir(tempPath);
    //       if (!success) break;
    //     }
    //   }
    // }

    // Create full directory if it does not exist
    if (success && !(fileParams.fs)->exists(fileParams.path)) {
        success = (fileParams.fs)->mkdir(fileParams.path);
    }

    duk_push_boolean(ctx, success);
    return 1;
}

duk_ret_t native_storageRmdir(duk_context *ctx) {
    // usage: storageRmdir(path: string | Path): boolean
    FileParamsJS fileParams = js_get_path_from_params(ctx, true);

    if (!fileParams.path.startsWith("/")) fileParams.path = "/" + fileParams.path;

    // Ensure the directory exists before attempting to remove it
    if (!(fileParams.fs)->exists(fileParams.path)) {
        duk_push_boolean(ctx, false);
        return 1;
    }

    bool success = (fileParams.fs)->rmdir(fileParams.path);
    duk_push_boolean(ctx, success);
    return 1;
}

duk_ret_t native_storageSpaceLittleFS(duk_context *ctx) {
    uint32_t totalKiloBytes = (uint32_t)(LittleFS.totalBytes() / 1024);
    uint32_t usedKiloBytes = (uint32_t)(LittleFS.usedBytes() / 1024);

    duk_idx_t obj_idx = duk_push_object(ctx);
    bduk_put_prop(ctx, obj_idx, "total", duk_push_uint, totalKiloBytes);
    bduk_put_prop(ctx, obj_idx, "used", duk_push_uint, usedKiloBytes);
    bduk_put_prop(ctx, obj_idx, "free", duk_push_uint, (totalKiloBytes - usedKiloBytes));

    return 1;
}

duk_ret_t native_storageSpaceSDCard(duk_context *ctx) {
    uint32_t totalKiloBytes = (uint32_t)(SD.totalBytes() / 1024);
    uint32_t usedKiloBytes = (uint32_t)(SD.usedBytes() / 1024);

    duk_idx_t obj_idx = duk_push_object(ctx);
    bduk_put_prop(ctx, obj_idx, "total", duk_push_uint, totalKiloBytes);
    bduk_put_prop(ctx, obj_idx, "used", duk_push_uint, usedKiloBytes);
    bduk_put_prop(ctx, obj_idx, "free", duk_push_uint, (totalKiloBytes - usedKiloBytes));

    return 1;
}
#endif
