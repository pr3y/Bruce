import { Checkbox } from "@material/web/checkbox/internal/checkbox.js";
declare global {
    interface HTMLElementTagNameMap {
        "ew-checkbox": EwCheckbox;
    }
}
export declare class EwCheckbox extends Checkbox {
    static styles: import("lit").CSSResult[];
}
