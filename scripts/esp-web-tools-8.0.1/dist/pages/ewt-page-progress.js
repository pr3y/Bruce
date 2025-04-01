var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
import { LitElement, html, css } from "lit";
import { property } from "lit/decorators.js";
import "../components/ewt-circular-progress";
class EwtPageProgress extends LitElement {
    render() {
        return html `
      <div>
        <ewt-circular-progress
          active
          ?indeterminate=${this.progress === undefined}
          .progress=${this.progress !== undefined
            ? this.progress / 100
            : undefined}
          density="8"
        ></ewt-circular-progress>
        ${this.progress !== undefined ? html `<div>${this.progress}%</div>` : ""}
      </div>
      ${this.label}
    `;
    }
}
EwtPageProgress.styles = css `
    :host {
      display: flex;
      flex-direction: column;
      text-align: center;
    }
    ewt-circular-progress {
      margin-bottom: 16px;
    }
  `;
__decorate([
    property()
], EwtPageProgress.prototype, "label", void 0);
__decorate([
    property()
], EwtPageProgress.prototype, "progress", void 0);
customElements.define("ewt-page-progress", EwtPageProgress);
