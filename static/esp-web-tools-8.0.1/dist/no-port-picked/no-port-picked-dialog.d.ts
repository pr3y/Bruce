import { LitElement } from "lit";
import "../components/ewt-dialog";
import "../components/ewt-button";
declare class EwtNoPortPickedDialog extends LitElement {
    doTryAgain?: () => void;
    render(): import("lit-html").TemplateResult<1>;
    private _handleClose;
    static styles: import("lit").CSSResult[];
}
declare global {
    interface HTMLElementTagNameMap {
        "ewt-no-port-picked-dialog": EwtNoPortPickedDialog;
    }
}
export {};
