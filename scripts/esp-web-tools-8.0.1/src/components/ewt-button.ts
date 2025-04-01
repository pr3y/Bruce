import { css } from "lit";
import { ButtonBase } from "@material/mwc-button/mwc-button-base";
import { styles } from "@material/mwc-button/styles.css";

declare global {
  interface HTMLElementTagNameMap {
    "ewt-button": EwtButton;
  }
}

export class EwtButton extends ButtonBase {
  static override styles = [
    styles,
    css`
      .mdc-button {
        min-width: initial;
      }
    `,
  ];
}

customElements.define("ewt-button", EwtButton);
