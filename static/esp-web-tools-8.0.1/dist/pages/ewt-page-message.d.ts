import { LitElement, TemplateResult } from "lit";
import "../components/ewt-circular-progress";
declare class EwtPageMessage extends LitElement {
    icon: string;
    label: string | TemplateResult;
    render(): TemplateResult<1>;
    static styles: import("lit").CSSResult;
}
declare global {
    interface HTMLElementTagNameMap {
        "ewt-page-message": EwtPageMessage;
    }
}
export {};
