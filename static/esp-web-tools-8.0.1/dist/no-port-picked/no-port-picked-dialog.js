var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
import { LitElement, html, css } from "lit";
import { customElement } from "lit/decorators.js";
import "../components/ewt-dialog";
import "../components/ewt-button";
import { dialogStyles } from "../styles";
let EwtNoPortPickedDialog = class EwtNoPortPickedDialog extends LitElement {
    render() {
        return html `
      <ewt-dialog
        open
        heading="No port selected"
        scrimClickAction
        @closed=${this._handleClose}
      >
        <div>
          If you didn't select a port because you didn't see your device listed,
          try the following steps:
        </div>
        <ol>
          <li>
            Make sure that the device is connected to this computer (the one
            that runs the browser that shows this website)
          </li>
          <li>
            Most devices have a tiny light when it is powered on. If yours has
            one, make sure it is on.
          </li>
          <li>
            Make sure you have the right drivers installed. Below are the
            drivers for common chips used in ESP devices:
            <ul>
              <li>
                CP2102 (square chip):
                <a
                  href="https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers"
                  target="_blank"
                  rel="noopener"
                  >driver</a
                >
              </li>
              <li>
                CH341:
                <a
                  href="https://github.com/nodemcu/nodemcu-devkit/tree/master/Drivers"
                  target="_blank"
                  rel="noopener"
                  >driver</a
                >
              </li>
            </ul>
          </li>
        </ol>
        ${this.doTryAgain
            ? html `
              <ewt-button
                slot="primaryAction"
                dialogAction="close"
                label="Try Again"
                @click=${this.doTryAgain}
              ></ewt-button>

              <ewt-button
                no-attention
                slot="secondaryAction"
                dialogAction="close"
                label="Cancel"
              ></ewt-button>
            `
            : html `
              <ewt-button
                slot="primaryAction"
                dialogAction="close"
                label="Close"
              ></ewt-button>
            `}
      </ewt-dialog>
    `;
    }
    async _handleClose() {
        this.parentNode.removeChild(this);
    }
};
EwtNoPortPickedDialog.styles = [
    dialogStyles,
    css `
      li + li,
      li > ul {
        margin-top: 8px;
      }
      ol {
        margin-bottom: 0;
      }
    `,
];
EwtNoPortPickedDialog = __decorate([
    customElement("ewt-no-port-picked-dialog")
], EwtNoPortPickedDialog);
