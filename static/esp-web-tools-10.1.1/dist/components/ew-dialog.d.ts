import { Dialog } from "@material/web/dialog/internal/dialog.js";
declare global {
    interface HTMLElementTagNameMap {
        "ew-dialog": EwDialog;
    }
}
export declare class EwDialog extends Dialog {
    static styles: import("lit").CSSResult[];
}
