import { IconButton } from "@material/web/iconbutton/internal/icon-button.js";
import { styles as sharedStyles } from "@material/web/iconbutton/internal/shared-styles.js";
import { styles } from "@material/web/iconbutton/internal/standard-styles.js";

declare global {
  interface HTMLElementTagNameMap {
    "ew-icon-button": EwIconButton;
  }
}

export class EwIconButton extends IconButton {
  static override styles = [sharedStyles, styles];
}

customElements.define("ew-icon-button", EwIconButton);
