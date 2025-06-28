import { ListItemBase } from "@material/mwc-list/mwc-list-item-base";
import { styles } from "@material/mwc-list/mwc-list-item.css";
export class EwtListItem extends ListItemBase {
}
EwtListItem.styles = [styles];
customElements.define("ewt-list-item", EwtListItem);
