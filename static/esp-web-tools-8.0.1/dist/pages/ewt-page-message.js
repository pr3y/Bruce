var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
import { LitElement, html, css } from "lit";
import { property } from "lit/decorators.js";
import "../components/ewt-circular-progress";
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
    ewt-circular-progress {
      margin-bottom: 16px;
    }
  `;
__decorate([
    property()
], EwtPageMessage.prototype, "icon", void 0);
__decorate([
    property()
], EwtPageMessage.prototype, "label", void 0);
customElements.define("ewt-page-message", EwtPageMessage);
