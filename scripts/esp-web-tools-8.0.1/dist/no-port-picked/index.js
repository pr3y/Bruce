import "./no-port-picked-dialog";
export const openNoPortPickedDialog = async (doTryAgain) => {
    const dialog = document.createElement("ewt-no-port-picked-dialog");
    dialog.doTryAgain = doTryAgain;
    document.body.append(dialog);
    return true;
};
