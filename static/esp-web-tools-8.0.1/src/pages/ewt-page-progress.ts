import { LitElement, html, css, TemplateResult } from "lit";
import { property } from "lit/decorators.js";
import "../components/ewt-circular-progress";

class EwtPageProgress extends LitElement {
  @property() label!: string | TemplateResult;

  @property() progress: number | undefined;

  render() {
    return html`
      <div>
        <ewt-circular-progress
          active
          ?indeterminate=${this.progress === undefined}
          .progress=${this.progress !== undefined
            ? this.progress / 100
            : undefined}
          density="8"
        ></ewt-circular-progress>
        ${this.progress !== undefined ? html`<div>${this.progress}%</div>` : ""}
      </div>
      ${this.label}
    `;
  }

  static styles = css`
    :host {
      display: flex;
      flex-direction: column;
      text-align: center;
    }
    ewt-circular-progress {
      margin-bottom: 16px;
    }
  `;
}
customElements.define("ewt-page-progress", EwtPageProgress);

declare global {
  interface HTMLElementTagNameMap {
    "ewt-page-progress": EwtPageProgress;
  }
}
