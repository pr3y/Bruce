import { ListItemBase } from "@material/mwc-list/mwc-list-item-base";
declare global {
    interface HTMLElementTagNameMap {
        "ewt-list-item": EwtListItem;
    }
}
export declare class EwtListItem extends ListItemBase {
    static styles: import("lit").CSSResult[];
}
