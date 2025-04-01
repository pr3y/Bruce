import { CircularProgressBase } from "@material/mwc-circular-progress/mwc-circular-progress-base";
import { styles } from "@material/mwc-circular-progress/mwc-circular-progress.css";

declare global {
  interface HTMLElementTagNameMap {
    "ewt-circular-progress": EwtCircularProgress;
  }
}

export class EwtCircularProgress extends CircularProgressBase {
  static override styles = [styles];
}

customElements.define("ewt-circular-progress", EwtCircularProgress);
