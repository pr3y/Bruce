import { LitElement, html, css, TemplateResult } from "lit";
import { property } from "lit/decorators.js";

class EwtPageMessage extends LitElement {
  @property() icon!: string;

  @property() label!: string | TemplateResult;

  render() {
    return html`
      <div class="icon">${this.icon}</div>
      ${this.label}
    `;
  }

  static styles = css`
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
}
customElements.define("ewt-page-message", EwtPageMessage);

declare global {
  interface HTMLElementTagNameMap {
    "ewt-page-message": EwtPageMessage;
  }
}
