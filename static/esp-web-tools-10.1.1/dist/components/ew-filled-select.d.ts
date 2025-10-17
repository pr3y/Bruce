import { FilledSelect } from "@material/web/select/internal/filled-select.js";
declare global {
    interface HTMLElementTagNameMap {
        "ew-filled-select": EwFilledSelect;
    }
}
export declare class EwFilledSelect extends FilledSelect {
    static styles: import("lit").CSSResult[];
}
