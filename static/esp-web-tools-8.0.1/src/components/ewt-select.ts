import { SelectBase } from "@material/mwc-select/mwc-select-base";
import { styles } from "@material/mwc-select/mwc-select.css";
import { css } from "lit";

declare global {
  interface HTMLElementTagNameMap {
    "ewt-select": EwtSelect;
  }
}

export class EwtSelect extends SelectBase {
  static override styles = [
    styles,
    // rem -> em conversion
    css`
      .mdc-floating-label {
        line-height: 1.15em;
      }
    `,
  ];
}

customElements.define("ewt-select", EwtSelect);
