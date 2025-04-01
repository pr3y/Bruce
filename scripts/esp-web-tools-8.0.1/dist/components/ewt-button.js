import { css } from "lit";
import { ButtonBase } from "@material/mwc-button/mwc-button-base";
import { styles } from "@material/mwc-button/styles.css";
export class EwtButton extends ButtonBase {
}
EwtButton.styles = [
    styles,
    css `
      .mdc-button {
        min-width: initial;
      }
    `,
];
customElements.define("ewt-button", EwtButton);
