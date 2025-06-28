import { IconButtonBase } from "@material/mwc-icon-button/mwc-icon-button-base";
import { styles } from "@material/mwc-icon-button/mwc-icon-button.css";

declare global {
  interface HTMLElementTagNameMap {
    "ewt-icon-button": EwtIconButton;
  }
}

export class EwtIconButton extends IconButtonBase {
  static override styles = [styles];
}

customElements.define("ewt-icon-button", EwtIconButton);
