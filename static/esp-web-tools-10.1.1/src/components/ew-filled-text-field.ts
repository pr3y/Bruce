import { styles as filledStyles } from "@material/web/textfield/internal/filled-styles.js";
import { FilledTextField } from "@material/web/textfield/internal/filled-text-field.js";
import { styles as sharedStyles } from "@material/web/textfield/internal/shared-styles.js";
import { literal } from "lit/static-html.js";

declare global {
  interface HTMLElementTagNameMap {
    "ew-filled-text-field": EwFilledTextField;
  }
}

export class EwFilledTextField extends FilledTextField {
  static override styles = [sharedStyles, filledStyles];
  protected override readonly fieldTag = literal`md-filled-field`;
}

customElements.define("ew-filled-text-field", EwFilledTextField);
