import { TextFieldBase } from "@material/mwc-textfield/mwc-textfield-base";
import { styles } from "@material/mwc-textfield/mwc-textfield.css";
import { css } from "lit";
export class EwtTextfield extends TextFieldBase {
}
EwtTextfield.styles = [
    styles,
    // rem -> em conversion
    css `
      .mdc-floating-label {
        line-height: 1.15em;
      }
    `,
];
customElements.define("ewt-textfield", EwtTextfield);
