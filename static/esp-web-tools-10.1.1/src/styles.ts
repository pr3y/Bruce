import { css } from "lit";

// We set font-size to 16px and all the mdc typography styles
// because it defaults to rem, which means that the font-size
// of the host website would influence the ESP Web Tools dialog.

export const dialogStyles = css`
  :host {
    --default-font: 'Inter', sans-serif;
    --text-color: #fff;
    --danger-color: #db4437;

    --md-sys-color-primary: #a020f0;
    --md-sys-color-on-primary: #000;
    --md-ref-typeface-brand: var(--default-font);
    --md-ref-typeface-plain: var(--default-font);

    --md-sys-color-surface: #000;
    --md-sys-color-on-surface: #fff;
    --md-sys-color-on-surface-variant: #fff;
    --md-list-item-leading-icon-color: #a020f0;
    --md-sys-color-surface-container: #000;
    --md-sys-color-surface-container-high: #000;
    --md-sys-color-surface-container-highest: #ccc;
    --md-sys-color-secondary-container: #bbb;

    --md-sys-typescale-headline-font: var(--default-font);
    --md-sys-typescale-title-font: var(--default-font);

    /* --md-text-button-hover-state-layer-opacity: 0.5;
    --md-text-button-hover-state-layer-color: var(--md-sys-color-on-surface);
    --md-text-button-color: var(--md-sys-color-primary); */
  }

  a {
    color: var(--md-sys-color-primary);
  }
`;