import { ListItemEl as ListItem } from "@material/web/list/internal/listitem/list-item.js";
import { styles } from "@material/web/list/internal/listitem/list-item-styles.js";

declare global {
  interface HTMLElementTagNameMap {
    "ew-list-item": EwListItem;
  }
}

export class EwListItem extends ListItem {
  static override styles = [styles];
}

customElements.define("ew-list-item", EwListItem);
