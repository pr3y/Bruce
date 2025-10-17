import { Divider } from "@material/web/divider/internal/divider.js";
declare global {
    interface HTMLElementTagNameMap {
        "ew-divider": EwDivider;
    }
}
export declare class EwDivider extends Divider {
    static styles: import("lit").CSSResult[];
}
