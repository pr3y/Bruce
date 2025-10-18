import { List } from "@material/web/list/internal/list.js";
import { styles } from "@material/web/list/internal/list-styles.js";

declare global {
  interface HTMLElementTagNameMap {
    "ew-list": EwList;
  }
}

export class EwList extends List {
  static override styles = [styles];
}

customElements.define("ew-list", EwList);
