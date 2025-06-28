/// <reference types="w3c-web-serial" />
import { LitElement, PropertyValues, TemplateResult } from "lit";
import "./components/ewt-button";
import "./components/ewt-checkbox";
import "./components/ewt-console";
import "./components/ewt-dialog";
import "./components/ewt-formfield";
import "./components/ewt-icon-button";
import "./components/ewt-textfield";
import "./components/ewt-select";
import "./components/ewt-list-item";
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
    protected render(): TemplateResult<1>;
    _renderProgress(label: string | TemplateResult, progress?: number): TemplateResult<1>;
    _renderError(label: string): [string, TemplateResult, boolean];
    _renderDashboard(): [string, TemplateResult, boolean, boolean];
    _renderDashboardNoImprov(): [string, TemplateResult, boolean, boolean];
    _renderProvision(): [string | undefined, TemplateResult, boolean];
    _renderAskErase(): [string | undefined, TemplateResult];
    _renderInstall(): [string | undefined, TemplateResult, boolean, boolean];
    _renderLogs(): [string | undefined, TemplateResult, boolean];
    willUpdate(changedProps: PropertyValues): void;
    protected firstUpdated(changedProps: PropertyValues): void;
    protected updated(changedProps: PropertyValues): void;
    private _focusFormElement;
    private _initialize;
    private _startInstall;
    private _confirmInstall;
    private _doProvision;
    private _handleDisconnect;
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
    static styles: import("lit").CSSResult[];
}
declare global {
    interface HTMLElementTagNameMap {
        "ewt-install-dialog": EwtInstallDialog;
    }
}
