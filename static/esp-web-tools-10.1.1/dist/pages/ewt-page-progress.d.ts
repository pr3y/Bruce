import { LitElement, TemplateResult } from "lit";
import "../components/ew-circular-progress";
declare class EwtPageProgress extends LitElement {
    label: string | TemplateResult;
    progress: number | undefined;
    render(): TemplateResult<1>;
    static styles: import("lit").CSSResult;
}
declare global {
    interface HTMLElementTagNameMap {
        "ewt-page-progress": EwtPageProgress;
    }
}
export {};
