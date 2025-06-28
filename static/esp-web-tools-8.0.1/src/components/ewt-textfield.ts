import { TextFieldBase } from "@material/mwc-textfield/mwc-textfield-base";
import { styles } from "@material/mwc-textfield/mwc-textfield.css";
import { css } from "lit";

declare global {
  interface HTMLElementTagNameMap {
    "ewt-textfield": EwtTextfield;
  }
}

export class EwtTextfield extends TextFieldBase {
  static override styles = [
    styles,
    // rem -> em conversion
    css`
      .mdc-floating-label {
        line-height: 1.15em;
      }
    `,
  ];
}

customElements.define("ewt-textfield", EwtTextfield);
