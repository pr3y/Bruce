import { CircularProgressBase } from "@material/mwc-circular-progress/mwc-circular-progress-base";
import { styles } from "@material/mwc-circular-progress/mwc-circular-progress.css";
export class EwtCircularProgress extends CircularProgressBase {
}
EwtCircularProgress.styles = [styles];
customElements.define("ewt-circular-progress", EwtCircularProgress);
