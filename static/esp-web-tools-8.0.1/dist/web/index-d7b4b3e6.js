import{d as e,r as t,n as o,s as i,$ as r}from"./connect-221663ff.js";var n=function(e,t,o,i){var r,n=arguments.length,l=n<3?t:null===i?i=Object.getOwnPropertyDescriptor(t,o):i;if("object"==typeof Reflect&&"function"==typeof Reflect.decorate)l=Reflect.decorate(e,t,o,i);else for(var s=e.length-1;s>=0;s--)(r=e[s])&&(l=(n<3?r(l):n>3?r(t,o,l):r(t,o))||l);return n>3&&l&&Object.defineProperty(t,o,l),l};let l=class extends i{render(){return r`
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
        ${this.doTryAgain?r`
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
            `:r`
              <ewt-button
                slot="primaryAction"
                dialogAction="close"
                label="Close"
              ></ewt-button>
            `}
      </ewt-dialog>
    `}async _handleClose(){this.parentNode.removeChild(this)}};l.styles=[e,t`
      li + li,
      li > ul {
        margin-top: 8px;
      }
      ol {
        margin-bottom: 0;
      }
    `],l=n([o("ewt-no-port-picked-dialog")],l);const s=async e=>{const t=document.createElement("ewt-no-port-picked-dialog");return t.doTryAgain=e,document.body.append(t),!0};export{s as openNoPortPickedDialog};
