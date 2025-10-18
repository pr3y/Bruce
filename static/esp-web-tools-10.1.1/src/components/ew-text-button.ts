import { css } from "lit";
import { styles as sharedStyles } from "@material/web/button/internal/shared-styles.js";
import { TextButton } from "@material/web/button/internal/text-button.js";
import { styles as textStyles } from "@material/web/button/internal/text-styles.js";

declare global {
  interface HTMLElementTagNameMap {
    "ew-text-button": EwTextButton;
  }
}

export class EwTextButton extends TextButton {
  static override styles = [
    sharedStyles,
    textStyles,
    css`
      :host {
        border: 2px solid var(--md-sys-color-primary);
        border-radius: 8px;
        color: var(--md-sys-color-primary) !important;
        height: 48px;
        font-size: 16px;
        margin-left: 5px;
        margin-right: 5px;
      }
      :host(:hover) {
        background-color: var(--md-sys-color-primary) !important;
      }
      :host(:hover) .label {
        color: white !important;
      }
    `,
  ];

}

customElements.define("ew-text-button", EwTextButton);
