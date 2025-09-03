import { FormfieldBase } from "@material/mwc-formfield/mwc-formfield-base";
import { styles } from "@material/mwc-formfield/mwc-formfield.css";

declare global {
  interface HTMLElementTagNameMap {
    "ewt-formfield": EwtFormfield;
  }
}

export class EwtFormfield extends FormfieldBase {
  static override styles = [styles];
}

customElements.define("ewt-formfield", EwtFormfield);
