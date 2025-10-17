import { Checkbox } from "@material/web/checkbox/internal/checkbox.js";
import { styles } from "@material/web/checkbox/internal/checkbox-styles.js";

declare global {
  interface HTMLElementTagNameMap {
    "ew-checkbox": EwCheckbox;
  }
}

export class EwCheckbox extends Checkbox {
  static override styles = [styles];
}

customElements.define("ew-checkbox", EwCheckbox);
