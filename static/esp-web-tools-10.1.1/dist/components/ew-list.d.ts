import { List } from "@material/web/list/internal/list.js";
declare global {
    interface HTMLElementTagNameMap {
        "ew-list": EwList;
    }
}
export declare class EwList extends List {
    static styles: import("lit").CSSResult[];
}
