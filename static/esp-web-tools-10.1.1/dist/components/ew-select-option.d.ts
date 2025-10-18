import { SelectOptionEl } from "@material/web/select/internal/selectoption/select-option.js";
declare global {
    interface HTMLElementTagNameMap {
        "ew-select-option": EwSelectOption;
    }
}
export declare class EwSelectOption extends SelectOptionEl {
    static styles: import("lit").CSSResult[];
}
