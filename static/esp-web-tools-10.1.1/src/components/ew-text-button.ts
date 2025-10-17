import { styles as sharedStyles } from "@material/web/button/internal/shared-styles.js";
import { TextButton } from "@material/web/button/internal/text-button.js";
import { styles as textStyles } from "@material/web/button/internal/text-styles.js";

declare global {
  interface HTMLElementTagNameMap {
    "ew-text-button": EwTextButton;
  }
}

export class EwTextButton extends TextButton {
  static override styles = [sharedStyles, textStyles];
}

customElements.define("ew-text-button", EwTextButton);
