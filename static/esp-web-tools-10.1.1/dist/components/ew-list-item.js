import { ListItemEl as ListItem } from "@material/web/list/internal/listitem/list-item.js";
import { styles } from "@material/web/list/internal/listitem/list-item-styles.js";
export class EwListItem extends ListItem {
}
EwListItem.styles = [styles];
customElements.define("ew-list-item", EwListItem);
