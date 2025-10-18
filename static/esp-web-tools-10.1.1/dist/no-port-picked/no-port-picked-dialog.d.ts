import { LitElement } from "lit";
import "../components/ew-dialog";
import "../components/ew-text-button";
declare class EwtNoPortPickedDialog extends LitElement {
    doTryAgain?: () => void;
    render(): import("lit-html").TemplateResult<1>;
    private tryAgain;
    private close;
    private _handleClose;
    static styles: import("lit").CSSResult[];
}
declare global {
    interface HTMLElementTagNameMap {
        "ewt-no-port-picked-dialog": EwtNoPortPickedDialog;
    }
}
export {};
