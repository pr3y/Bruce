import { CircularProgress } from "@material/web/progress/internal/circular-progress.js";
declare global {
    interface HTMLElementTagNameMap {
        "ew-circular-progress": EwCircularProgress;
    }
}
export declare class EwCircularProgress extends CircularProgress {
    static styles: import("lit").CSSResult[];
}
