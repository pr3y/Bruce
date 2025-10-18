import { DialogBase } from "@material/mwc-dialog/mwc-dialog-base";
import { styles } from "@material/mwc-dialog/mwc-dialog.css";
import { css } from "lit";

declare global {
  interface HTMLElementTagNameMap {
    "ewt-dialog": EwtDialog;
  }
}

export class EwtDialog extends DialogBase {
  static override styles = [
    styles,
    css`
      .mdc-dialog__title {
        padding-right: 52px;
      }
    `,
  ];
}

customElements.define("ewt-dialog", EwtDialog);
