import { ListItemEl as ListItem } from "@material/web/list/internal/listitem/list-item.js";
declare global {
    interface HTMLElementTagNameMap {
        "ew-list-item": EwListItem;
    }
}
export declare class EwListItem extends ListItem {
    static styles: import("lit").CSSResult[];
}
