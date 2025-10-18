import { IconButton } from "@material/web/iconbutton/internal/icon-button.js";
import { styles as sharedStyles } from "@material/web/iconbutton/internal/shared-styles.js";
import { styles } from "@material/web/iconbutton/internal/standard-styles.js";
export class EwIconButton extends IconButton {
}
EwIconButton.styles = [sharedStyles, styles];
customElements.define("ew-icon-button", EwIconButton);
