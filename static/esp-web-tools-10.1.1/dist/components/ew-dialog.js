import { Dialog } from "@material/web/dialog/internal/dialog.js";
import { styles } from "@material/web/dialog/internal/dialog-styles.js";
import { css } from "lit";
const customDialogStyles = css `
:host {
  --_hover-state-layer-opacity: 0.2;
}
  dialog {
    border: 2px solid var(--md-sys-color-primary, #a020f0) !important;
    border-radius: 12px !important;
    box-shadow: 0px 0px 50px 50px rgba(0, 0, 0, 1);
    -webkit-box-shadow: 0px 0px 50px 50px rgba(0, 0, 0, 1);
    -moz-box-shadow: 0px 0px 50px 50px rgba(0, 0, 0, 1);
  }
`;
export class EwDialog extends Dialog {
}
EwDialog.styles = [styles, customDialogStyles];
customElements.define("ew-dialog", EwDialog);
