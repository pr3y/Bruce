import { Dialog } from "@material/web/dialog/internal/dialog.js";
import { styles } from "@material/web/dialog/internal/dialog-styles.js";

declare global {
  interface HTMLElementTagNameMap {
    "ew-dialog": EwDialog;
  }
}

export class EwDialog extends Dialog {
  static override styles = [styles];
}

customElements.define("ew-dialog", EwDialog);
