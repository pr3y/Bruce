import { __decorate } from "tslib";
import { LitElement, html, css } from "lit";
import { property } from "lit/decorators.js";
class EwtPageMessage extends LitElement {
    render() {
        return html `
      <div class="icon">${this.icon}</div>
      ${this.label}
    `;
    }
}
EwtPageMessage.styles = css `
    :host {
      display: flex;
      flex-direction: column;
      text-align: center;
    }
    .icon {
      font-size: 50px;
      line-height: 80px;
      color: black;
    }
  `;
__decorate([
    property()
], EwtPageMessage.prototype, "icon", void 0);
__decorate([
    property()
], EwtPageMessage.prototype, "label", void 0);
customElements.define("ewt-page-message", EwtPageMessage);
