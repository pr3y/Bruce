import { TextButton } from "@material/web/button/internal/text-button.js";
declare global {
    interface HTMLElementTagNameMap {
        "ew-text-button": EwTextButton;
    }
}
export declare class EwTextButton extends TextButton {
    static styles: import("lit").CSSResult[];
}
