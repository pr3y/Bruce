export const fileDownload = (href, filename = "") => {
    const a = document.createElement("a");
    a.target = "_blank";
    a.href = href;
    a.download = filename;
    document.body.appendChild(a);
    a.dispatchEvent(new MouseEvent("click"));
    document.body.removeChild(a);
};
export const textDownload = (text, filename = "") => {
    const blob = new Blob([text], { type: "text/plain" });
    const url = URL.createObjectURL(blob);
    fileDownload(url, filename);
    setTimeout(() => URL.revokeObjectURL(url), 0);
};
