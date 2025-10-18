import { styles } from "@material/web/menu/internal/menuitem/menu-item-styles.js";
import { SelectOptionEl } from "@material/web/select/internal/selectoption/select-option.js";
export class EwSelectOption extends SelectOptionEl {
}
EwSelectOption.styles = [styles];
customElements.define("ew-select-option", EwSelectOption);
