import { LitElement, html, css } from "lit";
import { customElement } from "lit/decorators.js";
import "../components/ewt-dialog";
import "../components/ewt-button";
import { dialogStyles } from "../styles";

@customElement("ewt-no-port-picked-dialog")
class EwtNoPortPickedDialog extends LitElement {
  public doTryAgain?: () => void;

  public render() {
    return html`
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
          ? html`
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
          : html`
              <ewt-button
                slot="primaryAction"
                dialogAction="close"
                label="Close"
              ></ewt-button>
            `}
      </ewt-dialog>
    `;
  }

  private async _handleClose() {
    this.parentNode!.removeChild(this);
  }

  static styles = [
    dialogStyles,
    css`
      li + li,
      li > ul {
        margin-top: 8px;
      }
      ol {
        margin-bottom: 0;
      }
    `,
  ];
}

declare global {
  interface HTMLElementTagNameMap {
    "ewt-no-port-picked-dialog": EwtNoPortPickedDialog;
  }
}
