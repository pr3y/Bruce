import { ListItemBase } from "@material/mwc-list/mwc-list-item-base";
import { styles } from "@material/mwc-list/mwc-list-item.css";

declare global {
  interface HTMLElementTagNameMap {
    "ewt-list-item": EwtListItem;
  }
}

export class EwtListItem extends ListItemBase {
  static override styles = [styles];
}

customElements.define("ewt-list-item", EwtListItem);
