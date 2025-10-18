// From https://stackoverflow.com/a/38241481
export const getOperatingSystem = () => {
    var _a, _b;
    const userAgent = window.navigator.userAgent;
    const platform = 
    // @ts-expect-error
    ((_b = (_a = window.navigator) === null || _a === void 0 ? void 0 : _a.userAgentData) === null || _b === void 0 ? void 0 : _b.platform) || window.navigator.platform;
    const macosPlatforms = ["macOS", "Macintosh", "MacIntel", "MacPPC", "Mac68K"];
    const windowsPlatforms = ["Win32", "Win64", "Windows", "WinCE"];
    const iosPlatforms = ["iPhone", "iPad", "iPod"];
    if (macosPlatforms.indexOf(platform) !== -1) {
        return "Mac OS";
    }
    else if (iosPlatforms.indexOf(platform) !== -1) {
        return "iOS";
    }
    else if (windowsPlatforms.indexOf(platform) !== -1) {
        return "Windows";
    }
    else if (/Android/.test(userAgent)) {
        return "Android";
    }
    else if (/Linux/.test(platform)) {
        return "Linux";
    }
    return null;
};
