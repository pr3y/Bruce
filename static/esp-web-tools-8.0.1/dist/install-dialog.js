var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
import { LitElement, html, css } from "lit";
import { state } from "lit/decorators.js";
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
import { chipIcon, closeIcon, firmwareIcon } from "./components/svg";
import { ImprovSerial } from "improv-wifi-serial-sdk/dist/serial";
import { ImprovSerialCurrentState, PortNotReady, } from "improv-wifi-serial-sdk/dist/const";
import { flash } from "./flash";
import { textDownload } from "./util/file-download";
import { fireEvent } from "./util/fire-event";
import { sleep } from "./util/sleep";
import { downloadManifest } from "./util/manifest";
import { dialogStyles } from "./styles";
const ERROR_ICON = "⚠️";
const OK_ICON = "🎉";
export class EwtInstallDialog extends LitElement {
    constructor() {
        super(...arguments);
        this.logger = console;
        this._state = "DASHBOARD";
        this._installErase = false;
        this._installConfirmed = false;
        this._provisionForce = false;
        this._wasProvisioned = false;
        this._busy = false;
        // -1 = custom
        this._selectedSsid = -1;
        this._handleDisconnect = () => {
            this._state = "ERROR";
            this._error = "Disconnected";
        };
    }
    render() {
        if (!this.port) {
            return html ``;
        }
        let heading;
        let content;
        let hideActions = false;
        let allowClosing = false;
        // During installation phase we temporarily remove the client
        if (this._client === undefined &&
            this._state !== "INSTALL" &&
            this._state !== "LOGS") {
            if (this._error) {
                [heading, content, hideActions] = this._renderError(this._error);
            }
            else {
                content = this._renderProgress("Connecting");
                hideActions = true;
            }
        }
        else if (this._state === "INSTALL") {
            [heading, content, hideActions, allowClosing] = this._renderInstall();
        }
        else if (this._state === "ASK_ERASE") {
            [heading, content] = this._renderAskErase();
        }
        else if (this._state === "ERROR") {
            [heading, content, hideActions] = this._renderError(this._error);
        }
        else if (this._state === "DASHBOARD") {
            [heading, content, hideActions, allowClosing] = this._client
                ? this._renderDashboard()
                : this._renderDashboardNoImprov();
        }
        else if (this._state === "PROVISION") {
            [heading, content, hideActions] = this._renderProvision();
        }
        else if (this._state === "LOGS") {
            [heading, content, hideActions] = this._renderLogs();
        }
        return html `
      <ewt-dialog
        open
        .heading=${heading}
        scrimClickAction
        @closed=${this._handleClose}
        .hideActions=${hideActions}
      >
        ${heading && allowClosing
            ? html `
              <ewt-icon-button dialogAction="close">
                ${closeIcon}
              </ewt-icon-button>
            `
            : ""}
        ${content}
      </ewt-dialog>
    `;
    }
    _renderProgress(label, progress) {
        return html `
      <ewt-page-progress
        .label=${label}
        .progress=${progress}
      ></ewt-page-progress>
    `;
    }
    _renderError(label) {
        const heading = "Error";
        const content = html `
      <ewt-page-message .icon=${ERROR_ICON} .label=${label}></ewt-page-message>
      <ewt-button
        slot="primaryAction"
        dialogAction="ok"
        label="Close"
      ></ewt-button>
    `;
        const hideActions = false;
        return [heading, content, hideActions];
    }
    _renderDashboard() {
        const heading = this._info.name;
        let content;
        let hideActions = true;
        let allowClosing = true;
        content = html `
      <table>
        <tr>
          <td>${firmwareIcon}</td>
          <td>${this._info.firmware}&nbsp;${this._info.version}</td>
        </tr>
        <tr>
          <td>${chipIcon}</td>
          <td>${this._info.chipFamily}</td>
        </tr>
      </table>
      <div class="dashboard-buttons">
        ${!this._isSameVersion
            ? html `
              <div>
                <ewt-button
                  .label=${!this._isSameFirmware
                ? `Install ${this._manifest.name}`
                : `Update ${this._manifest.name}`}
                  @click=${() => {
                if (this._isSameFirmware) {
                    this._startInstall(false);
                }
                else if (this._manifest.new_install_prompt_erase) {
                    this._state = "ASK_ERASE";
                }
                else {
                    this._startInstall(true);
                }
            }}
                ></ewt-button>
              </div>
            `
            : ""}
        ${this._client.nextUrl === undefined
            ? ""
            : html `
              <div>
                <a
                  href=${this._client.nextUrl}
                  class="has-button"
                  target="_blank"
                >
                  <ewt-button label="Visit Device"></ewt-button>
                </a>
              </div>
            `}
        ${!this._manifest.home_assistant_domain ||
            this._client.state !== ImprovSerialCurrentState.PROVISIONED
            ? ""
            : html `
              <div>
                <a
                  href=${`https://my.home-assistant.io/redirect/config_flow_start/?domain=${this._manifest.home_assistant_domain}`}
                  class="has-button"
                  target="_blank"
                >
                  <ewt-button label="Add to Home Assistant"></ewt-button>
                </a>
              </div>
            `}
        <div>
          <ewt-button
            .label=${this._client.state === ImprovSerialCurrentState.READY
            ? "Connect to Wi-Fi"
            : "Change Wi-Fi"}
            @click=${() => {
            this._state = "PROVISION";
            if (this._client.state === ImprovSerialCurrentState.PROVISIONED) {
                this._provisionForce = true;
            }
        }}
          ></ewt-button>
        </div>
        <div>
          <ewt-button
            label="Logs & Console"
            @click=${async () => {
            const client = this._client;
            if (client) {
                await this._closeClientWithoutEvents(client);
                await sleep(100);
            }
            // Also set `null` back to undefined.
            this._client = undefined;
            this._state = "LOGS";
        }}
          ></ewt-button>
        </div>
        ${this._isSameFirmware && this._manifest.funding_url
            ? html `
              <div>
                <a
                  class="button"
                  href=${this._manifest.funding_url}
                  target="_blank"
                >
                  <ewt-button label="Fund Development"></ewt-button>
                </a>
              </div>
            `
            : ""}
        ${this._isSameVersion
            ? html `
              <div>
                <ewt-button
                  class="danger"
                  label="Erase User Data"
                  @click=${() => this._startInstall(true)}
                ></ewt-button>
              </div>
            `
            : ""}
      </div>
    `;
        return [heading, content, hideActions, allowClosing];
    }
    _renderDashboardNoImprov() {
        const heading = "Device Dashboard";
        let content;
        let hideActions = true;
        let allowClosing = true;
        content = html `
      <div class="dashboard-buttons">
        <div>
          <ewt-button
            .label=${`Install ${this._manifest.name}`}
            @click=${() => {
            if (this._manifest.new_install_prompt_erase) {
                this._state = "ASK_ERASE";
            }
            else {
                // Default is to erase a device that does not support Improv Serial
                this._startInstall(true);
            }
        }}
          ></ewt-button>
        </div>

        <div>
          <ewt-button
            label="Logs & Console"
            @click=${async () => {
            // Also set `null` back to undefined.
            this._client = undefined;
            this._state = "LOGS";
        }}
          ></ewt-button>
        </div>
      </div>
    `;
        return [heading, content, hideActions, allowClosing];
    }
    _renderProvision() {
        let heading = "Configure Wi-Fi";
        let content;
        let hideActions = false;
        if (this._busy) {
            return [
                heading,
                this._renderProgress(this._ssids === undefined
                    ? "Scanning for networks"
                    : "Trying to connect"),
                true,
            ];
        }
        if (!this._provisionForce &&
            this._client.state === ImprovSerialCurrentState.PROVISIONED) {
            heading = undefined;
            const showSetupLinks = !this._wasProvisioned &&
                (this._client.nextUrl !== undefined ||
                    "home_assistant_domain" in this._manifest);
            hideActions = showSetupLinks;
            content = html `
        <ewt-page-message
          .icon=${OK_ICON}
          label="Device connected to the network!"
        ></ewt-page-message>
        ${showSetupLinks
                ? html `
              <div class="dashboard-buttons">
                ${this._client.nextUrl === undefined
                    ? ""
                    : html `
                      <div>
                        <a
                          href=${this._client.nextUrl}
                          class="has-button"
                          target="_blank"
                          @click=${() => {
                        this._state = "DASHBOARD";
                    }}
                        >
                          <ewt-button label="Visit Device"></ewt-button>
                        </a>
                      </div>
                    `}
                ${!this._manifest.home_assistant_domain
                    ? ""
                    : html `
                      <div>
                        <a
                          href=${`https://my.home-assistant.io/redirect/config_flow_start/?domain=${this._manifest.home_assistant_domain}`}
                          class="has-button"
                          target="_blank"
                          @click=${() => {
                        this._state = "DASHBOARD";
                    }}
                        >
                          <ewt-button
                            label="Add to Home Assistant"
                          ></ewt-button>
                        </a>
                      </div>
                    `}
                <div>
                  <ewt-button
                    label="Skip"
                    @click=${() => {
                    this._state = "DASHBOARD";
                }}
                  ></ewt-button>
                </div>
              </div>
            `
                : html `
              <ewt-button
                slot="primaryAction"
                label="Continue"
                @click=${() => {
                    this._state = "DASHBOARD";
                }}
              ></ewt-button>
            `}
      `;
        }
        else {
            let error;
            switch (this._client.error) {
                case 3 /* UNABLE_TO_CONNECT */:
                    error = "Unable to connect";
                    break;
                case 0 /* NO_ERROR */:
                // Happens when list SSIDs not supported.
                case 2 /* UNKNOWN_RPC_COMMAND */:
                    break;
                default:
                    error = `Unknown error (${this._client.error})`;
            }
            content = html `
        <div>
          Enter the credentials of the Wi-Fi network that you want your device
          to connect to.
        </div>
        ${error ? html `<p class="error">${error}</p>` : ""}
        ${this._ssids !== null
                ? html `
              <ewt-select
                fixedMenuPosition
                label="Network"
                @selected=${(ev) => {
                    const index = ev.detail.index;
                    // The "Join Other" item is always the last item.
                    this._selectedSsid =
                        index === this._ssids.length ? -1 : index;
                }}
                @closed=${(ev) => ev.stopPropagation()}
              >
                ${this._ssids.map((info, idx) => html `
                    <ewt-list-item
                      .selected=${this._selectedSsid === idx}
                      value=${idx}
                    >
                      ${info.name}
                    </ewt-list-item>
                  `)}
                <ewt-list-item
                  .selected=${this._selectedSsid === -1}
                  value="-1"
                >
                  Join other…
                </ewt-list-item>
              </ewt-select>
            `
                : ""}
        ${
            // Show input box if command not supported or "Join Other" selected
            this._selectedSsid === -1
                ? html `
                <ewt-textfield label="Network Name" name="ssid"></ewt-textfield>
              `
                : ""}
        <ewt-textfield
          label="Password"
          name="password"
          type="password"
        ></ewt-textfield>
        <ewt-button
          slot="primaryAction"
          label="Connect"
          @click=${this._doProvision}
        ></ewt-button>
        <ewt-button
          slot="secondaryAction"
          .label=${this._installState && this._installErase ? "Skip" : "Back"}
          @click=${() => {
                this._state = "DASHBOARD";
            }}
        ></ewt-button>
      `;
        }
        return [heading, content, hideActions];
    }
    _renderAskErase() {
        const heading = "Erase device";
        const content = html `
      <div>
        Do you want to erase the device before installing
        ${this._manifest.name}? All data on the device will be lost.
      </div>
      <ewt-formfield label="Erase device" class="danger">
        <ewt-checkbox></ewt-checkbox>
      </ewt-formfield>
      <ewt-button
        slot="primaryAction"
        label="Next"
        @click=${() => {
            const checkbox = this.shadowRoot.querySelector("ewt-checkbox");
            this._startInstall(checkbox.checked);
        }}
      ></ewt-button>
      <ewt-button
        slot="secondaryAction"
        label="Back"
        @click=${() => {
            this._state = "DASHBOARD";
        }}
      ></ewt-button>
    `;
        return [heading, content];
    }
    _renderInstall() {
        let heading = `${this._installConfirmed ? "Installing" : "Install"} ${this._manifest.name}`;
        let content;
        let hideActions = false;
        const allowClosing = false;
        const isUpdate = !this._installErase && this._isSameFirmware;
        if (!this._installConfirmed && this._isSameVersion) {
            heading = "Erase User Data";
            content = html `
        Do you want to reset your device and erase all user data from your
        device?
        <ewt-button
          class="danger"
          slot="primaryAction"
          label="Erase User Data"
          @click=${this._confirmInstall}
        ></ewt-button>
      `;
        }
        else if (!this._installConfirmed) {
            const action = isUpdate ? "update to" : "install";
            content = html `
        ${isUpdate
                ? html `Your device is running
              ${this._info.firmware}&nbsp;${this._info.version}.<br /><br />`
                : ""}
        Do you want to ${action}
        ${this._manifest.name}&nbsp;${this._manifest.version}?
        ${this._installErase
                ? "All existing data will be erased from your device."
                : ""}
        <ewt-button
          slot="primaryAction"
          label="Install"
          @click=${this._confirmInstall}
        ></ewt-button>
        <ewt-button
          slot="secondaryAction"
          label="Back"
          @click=${() => {
                this._state = "DASHBOARD";
            }}
        ></ewt-button>
      `;
        }
        else if (!this._installState ||
            this._installState.state === "initializing" /* INITIALIZING */ ||
            this._installState.state === "manifest" /* MANIFEST */ ||
            this._installState.state === "preparing" /* PREPARING */) {
            content = this._renderProgress("Preparing installation");
            hideActions = true;
        }
        else if (this._installState.state === "erasing" /* ERASING */) {
            content = this._renderProgress("Erasing");
            hideActions = true;
        }
        else if (this._installState.state === "writing" /* WRITING */ ||
            // When we're finished, keep showing this screen with 100% written
            // until Improv is initialized / not detected.
            (this._installState.state === "finished" /* FINISHED */ &&
                this._client === undefined)) {
            let percentage;
            let undeterminateLabel;
            if (this._installState.state === "finished" /* FINISHED */) {
                // We're done writing and detecting improv, show spinner
                undeterminateLabel = "Wrapping up";
            }
            else if (this._installState.details.percentage < 4) {
                // We're writing the firmware under 4%, show spinner or else we don't show any pixels
                undeterminateLabel = "Installing";
            }
            else {
                // We're writing the firmware over 4%, show progress bar
                percentage = this._installState.details.percentage;
            }
            content = this._renderProgress(html `
          ${undeterminateLabel ? html `${undeterminateLabel}<br />` : ""}
          <br />
          This will take
          ${this._installState.chipFamily === "ESP8266"
                ? "a minute"
                : "2 minutes"}.<br />
          Keep this page visible to prevent slow down
        `, percentage);
            hideActions = true;
        }
        else if (this._installState.state === "finished" /* FINISHED */) {
            heading = undefined;
            const supportsImprov = this._client !== null;
            content = html `
        <ewt-page-message
          .icon=${OK_ICON}
          label="Installation complete!"
        ></ewt-page-message>
        <ewt-button
          slot="primaryAction"
          label="Next"
          @click=${() => {
                this._state =
                    supportsImprov && this._installErase ? "PROVISION" : "DASHBOARD";
            }}
        ></ewt-button>
      `;
        }
        else if (this._installState.state === "error" /* ERROR */) {
            content = html `
        <ewt-page-message
          .icon=${ERROR_ICON}
          .label=${this._installState.message}
        ></ewt-page-message>
        <ewt-button
          slot="primaryAction"
          label="Back"
          @click=${async () => {
                this._initialize();
                this._state = "DASHBOARD";
            }}
        ></ewt-button>
      `;
        }
        return [heading, content, hideActions, allowClosing];
    }
    _renderLogs() {
        let heading = `Logs`;
        let content;
        let hideActions = false;
        content = html `
      <ewt-console .port=${this.port} .logger=${this.logger}></ewt-console>
      <ewt-button
        slot="primaryAction"
        label="Back"
        @click=${async () => {
            await this.shadowRoot.querySelector("ewt-console").disconnect();
            this._state = "DASHBOARD";
            this._initialize();
        }}
      ></ewt-button>
      <ewt-button
        slot="secondaryAction"
        label="Download Logs"
        @click=${() => {
            textDownload(this.shadowRoot.querySelector("ewt-console").logs(), `esp-web-tools-logs.txt`);
            this.shadowRoot.querySelector("ewt-console").reset();
        }}
      ></ewt-button>
      <ewt-button
        slot="secondaryAction"
        label="Reset Device"
        @click=${async () => {
            await this.shadowRoot.querySelector("ewt-console").reset();
        }}
      ></ewt-button>
    `;
        return [heading, content, hideActions];
    }
    willUpdate(changedProps) {
        if (!changedProps.has("_state")) {
            return;
        }
        // Clear errors when changing between pages unless we change
        // to the error page.
        if (this._state !== "ERROR") {
            this._error = undefined;
        }
        // Scan for SSIDs on provision
        if (this._state === "PROVISION") {
            this._ssids = undefined;
            this._busy = true;
            this._client.scan().then((ssids) => {
                this._busy = false;
                this._ssids = ssids;
                this._selectedSsid = ssids.length ? 0 : -1;
            }, () => {
                this._busy = false;
                this._ssids = null;
                this._selectedSsid = -1;
            });
        }
        else {
            // Reset this value if we leave provisioning.
            this._provisionForce = false;
        }
        if (this._state === "INSTALL") {
            this._installConfirmed = false;
            this._installState = undefined;
        }
    }
    firstUpdated(changedProps) {
        super.firstUpdated(changedProps);
        this._initialize();
    }
    updated(changedProps) {
        super.updated(changedProps);
        if (changedProps.has("_state")) {
            this.setAttribute("state", this._state);
        }
        if (this._state !== "PROVISION") {
            return;
        }
        if (changedProps.has("_selectedSsid") && this._selectedSsid === -1) {
            // If we pick "Join other", select SSID input.
            this._focusFormElement("ewt-textfield[name=ssid]");
        }
        else if (changedProps.has("_ssids")) {
            // Form is shown when SSIDs are loaded/marked not supported
            this._focusFormElement();
        }
    }
    _focusFormElement(selector = "ewt-textfield, ewt-select") {
        const formEl = this.shadowRoot.querySelector(selector);
        if (formEl) {
            formEl.updateComplete.then(() => setTimeout(() => formEl.focus(), 100));
        }
    }
    async _initialize(justInstalled = false) {
        if (this.port.readable === null || this.port.writable === null) {
            this._state = "ERROR";
            this._error =
                "Serial port is not readable/writable. Close any other application using it and try again.";
            return;
        }
        try {
            this._manifest = await downloadManifest(this.manifestPath);
        }
        catch (err) {
            this._state = "ERROR";
            this._error = "Failed to download manifest";
            return;
        }
        if (this._manifest.new_install_improv_wait_time === 0) {
            this._client = null;
            return;
        }
        const client = new ImprovSerial(this.port, this.logger);
        client.addEventListener("state-changed", () => {
            this.requestUpdate();
        });
        client.addEventListener("error-changed", () => this.requestUpdate());
        try {
            // If a device was just installed, give new firmware 10 seconds (overridable) to
            // format the rest of the flash and do other stuff.
            const timeout = !justInstalled
                ? 1000
                : this._manifest.new_install_improv_wait_time !== undefined
                    ? this._manifest.new_install_improv_wait_time * 1000
                    : 10000;
            this._info = await client.initialize(timeout);
            this._client = client;
            client.addEventListener("disconnect", this._handleDisconnect);
        }
        catch (err) {
            // Clear old value
            this._info = undefined;
            if (err instanceof PortNotReady) {
                this._state = "ERROR";
                this._error =
                    "Serial port is not ready. Close any other application using it and try again.";
            }
            else {
                this._client = null; // not supported
                this.logger.error("Improv initialization failed.", err);
            }
        }
    }
    _startInstall(erase) {
        this._state = "INSTALL";
        this._installErase = erase;
        this._installConfirmed = false;
    }
    async _confirmInstall() {
        this._installConfirmed = true;
        this._installState = undefined;
        if (this._client) {
            await this._closeClientWithoutEvents(this._client);
        }
        this._client = undefined;
        flash((state) => {
            this._installState = state;
            if (state.state === "finished" /* FINISHED */) {
                sleep(100)
                    .then(() => this._initialize(true))
                    .then(() => this.requestUpdate());
            }
        }, this.port, this.logger, this.manifestPath, this._installErase);
    }
    async _doProvision() {
        this._busy = true;
        this._wasProvisioned =
            this._client.state === ImprovSerialCurrentState.PROVISIONED;
        const ssid = this._selectedSsid === -1
            ? this.shadowRoot.querySelector("ewt-textfield[name=ssid]").value
            : this._ssids[this._selectedSsid].name;
        const password = this.shadowRoot.querySelector("ewt-textfield[name=password]").value;
        try {
            await this._client.provision(ssid, password);
        }
        catch (err) {
            return;
        }
        finally {
            this._busy = false;
            this._provisionForce = false;
        }
    }
    async _handleClose() {
        if (this._client) {
            await this._closeClientWithoutEvents(this._client);
        }
        fireEvent(this, "closed");
        this.parentNode.removeChild(this);
    }
    /**
     * Return if the device runs same firmware as manifest.
     */
    get _isSameFirmware() {
        var _a;
        return !this._info
            ? false
            : ((_a = this.overrides) === null || _a === void 0 ? void 0 : _a.checkSameFirmware)
                ? this.overrides.checkSameFirmware(this._manifest, this._info)
                : this._info.firmware === this._manifest.name;
    }
    /**
     * Return if the device runs same firmware and version as manifest.
     */
    get _isSameVersion() {
        return (this._isSameFirmware && this._info.version === this._manifest.version);
    }
    async _closeClientWithoutEvents(client) {
        client.removeEventListener("disconnect", this._handleDisconnect);
        await client.close();
    }
}
EwtInstallDialog.styles = [
    dialogStyles,
    css `
      :host {
        --mdc-dialog-max-width: 390px;
      }
      ewt-icon-button {
        position: absolute;
        right: 4px;
        top: 10px;
      }
      table {
        border-spacing: 0;
        color: var(--improv-text-color);
        margin-bottom: 16px;
      }
      table svg {
        width: 20px;
        margin-right: 8px;
      }
      ewt-textfield,
      ewt-select {
        display: block;
        margin-top: 16px;
      }
      .dashboard-buttons {
        margin: 0 0 -16px -8px;
      }
      .dashboard-buttons div {
        display: block;
        margin: 4px 0;
      }
      a.has-button {
        text-decoration: none;
      }
      .error {
        color: var(--improv-danger-color);
      }
      .danger {
        --mdc-theme-primary: var(--improv-danger-color);
        --mdc-theme-secondary: var(--improv-danger-color);
      }
      button.link {
        background: none;
        color: inherit;
        border: none;
        padding: 0;
        font: inherit;
        text-align: left;
        text-decoration: underline;
        cursor: pointer;
      }
      :host([state="LOGS"]) ewt-dialog {
        --mdc-dialog-max-width: 90vw;
      }
      ewt-console {
        width: calc(80vw - 48px);
        height: 80vh;
      }
    `,
];
__decorate([
    state()
], EwtInstallDialog.prototype, "_client", void 0);
__decorate([
    state()
], EwtInstallDialog.prototype, "_state", void 0);
__decorate([
    state()
], EwtInstallDialog.prototype, "_installErase", void 0);
__decorate([
    state()
], EwtInstallDialog.prototype, "_installConfirmed", void 0);
__decorate([
    state()
], EwtInstallDialog.prototype, "_installState", void 0);
__decorate([
    state()
], EwtInstallDialog.prototype, "_provisionForce", void 0);
__decorate([
    state()
], EwtInstallDialog.prototype, "_error", void 0);
__decorate([
    state()
], EwtInstallDialog.prototype, "_busy", void 0);
__decorate([
    state()
], EwtInstallDialog.prototype, "_ssids", void 0);
__decorate([
    state()
], EwtInstallDialog.prototype, "_selectedSsid", void 0);
customElements.define("ewt-install-dialog", EwtInstallDialog);
