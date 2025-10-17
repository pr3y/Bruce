export const downloadManifest = async (manifestPath) => {
    const manifestURL = new URL(manifestPath, location.toString()).toString();
    const resp = await fetch(manifestURL);
    const manifest = await resp.json();
    if ("new_install_skip_erase" in manifest) {
        console.warn('Manifest option "new_install_skip_erase" is deprecated. Use "new_install_prompt_erase" instead.');
        if (manifest.new_install_skip_erase) {
            manifest.new_install_prompt_erase = true;
        }
    }
    return manifest;
};
