import type { FlashState } from "./const";
import type { EwtInstallDialog } from "./install-dialog";
export declare class InstallButton extends HTMLElement {
    static isSupported: boolean;
    static isAllowed: boolean;
    private static style;
    manifest?: string;
    eraseFirst?: boolean;
    hideProgress?: boolean;
    showLog?: boolean;
    logConsole?: boolean;
    state?: FlashState;
    renderRoot?: ShadowRoot;
    overrides: EwtInstallDialog["overrides"];
    connectedCallback(): void;
}
