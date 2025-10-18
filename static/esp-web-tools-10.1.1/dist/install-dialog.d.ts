import { LitElement, PropertyValues, TemplateResult } from "lit";
import "./components/ew-text-button";
import "./components/ew-list";
import "./components/ew-list-item";
import "./components/ew-divider";
import "./components/ew-checkbox";
import "./components/ewt-console";
import "./components/ew-dialog";
import "./components/ew-icon-button";
import "./components/ew-filled-text-field";
import "./components/ew-filled-select";
import "./components/ew-select-option";
import "./pages/ewt-page-progress";
import "./pages/ewt-page-message";
import { Logger, Manifest } from "./const.js";
import { ImprovSerial } from "improv-wifi-serial-sdk/dist/serial";
export declare class EwtInstallDialog extends LitElement {
    port: SerialPort;
    manifestPath: string;
    logger: Logger;
    overrides?: {
        checkSameFirmware?: (manifest: Manifest, deviceImprov: ImprovSerial["info"]) => boolean;
    };
    private _manifest;
    private _info?;
    private _client?;
    private _state;
    private _installErase;
    private _installConfirmed;
    private _installState?;
    private _provisionForce;
    private _wasProvisioned;
    private _error?;
    private _busy;
    private _ssids?;
    private _selectedSsid;
    private _bodyOverflow;
    protected render(): TemplateResult<1>;
    _renderProgress(label: string | TemplateResult, progress?: number): TemplateResult<1>;
    _renderError(label: string): [string, TemplateResult];
    _renderDashboard(): [string, TemplateResult, boolean];
    _renderDashboardNoImprov(): [string, TemplateResult, boolean];
    _renderProvision(): [string | undefined, TemplateResult];
    _renderAskErase(): [string | undefined, TemplateResult];
    _renderInstall(): [string | undefined, TemplateResult, boolean];
    _renderLogs(): [string | undefined, TemplateResult];
    willUpdate(changedProps: PropertyValues): void;
    private _updateSsids;
    protected firstUpdated(changedProps: PropertyValues): void;
    protected updated(changedProps: PropertyValues): void;
    private _focusFormElement;
    private _initialize;
    private _startInstall;
    private _confirmInstall;
    private _doProvision;
    private _handleDisconnect;
    private _closeDialog;
    private _handleClose;
    /**
     * Return if the device runs same firmware as manifest.
     */
    private get _isSameFirmware();
    /**
     * Return if the device runs same firmware and version as manifest.
     */
    private get _isSameVersion();
    private _closeClientWithoutEvents;
    private _preventDefault;
    static styles: import("lit").CSSResult[];
}
declare global {
    interface HTMLElementTagNameMap {
        "ewt-install-dialog": EwtInstallDialog;
    }
}
