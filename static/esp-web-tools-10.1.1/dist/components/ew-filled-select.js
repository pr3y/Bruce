import { FilledSelect } from "@material/web/select/internal/filled-select.js";
import { styles } from "@material/web/select/internal/filled-select-styles.js";
import { styles as sharedStyles } from "@material/web/select/internal/shared-styles.js";
export class EwFilledSelect extends FilledSelect {
}
EwFilledSelect.styles = [sharedStyles, styles];
customElements.define("ew-filled-select", EwFilledSelect);
