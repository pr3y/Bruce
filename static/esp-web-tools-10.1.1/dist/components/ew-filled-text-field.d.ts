import { FilledTextField } from "@material/web/textfield/internal/filled-text-field.js";
declare global {
    interface HTMLElementTagNameMap {
        "ew-filled-text-field": EwFilledTextField;
    }
}
export declare class EwFilledTextField extends FilledTextField {
    static styles: import("lit").CSSResult[];
    protected readonly fieldTag: import("lit-html/static").StaticValue;
}
