import { SelectBase } from "@material/mwc-select/mwc-select-base";
import { styles } from "@material/mwc-select/mwc-select.css";
import { css } from "lit";
export class EwtSelect extends SelectBase {
}
EwtSelect.styles = [
    styles,
    // rem -> em conversion
    css `
      .mdc-floating-label {
        line-height: 1.15em;
      }
    `,
];
customElements.define("ewt-select", EwtSelect);
