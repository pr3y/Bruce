import { __decorate } from "tslib";
import { LitElement, html, css } from "lit";
import { property } from "lit/decorators.js";
import "../components/ew-circular-progress";
class EwtPageProgress extends LitElement {
    render() {
        return html `
      <div>
        <ew-circular-progress
          active
          ?indeterminate=${this.progress === undefined}
          .value=${this.progress !== undefined
            ? this.progress / 100
            : undefined}
        ></ew-circular-progress>
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
    ew-circular-progress {
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
