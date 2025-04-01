import { CheckboxBase } from "@material/mwc-checkbox/mwc-checkbox-base";
import { styles } from "@material/mwc-checkbox/mwc-checkbox.css";

declare global {
  interface HTMLElementTagNameMap {
    "ewt-checkbox": EwtCheckbox;
  }
}

export class EwtCheckbox extends CheckboxBase {
  static override styles = [styles];
}

customElements.define("ewt-checkbox", EwtCheckbox);
