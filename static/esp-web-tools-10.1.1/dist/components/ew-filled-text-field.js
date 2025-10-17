import { styles as filledStyles } from "@material/web/textfield/internal/filled-styles.js";
import { FilledTextField } from "@material/web/textfield/internal/filled-text-field.js";
import { styles as sharedStyles } from "@material/web/textfield/internal/shared-styles.js";
import { literal } from "lit/static-html.js";
export class EwFilledTextField extends FilledTextField {
    constructor() {
        super(...arguments);
        this.fieldTag = literal `md-filled-field`;
    }
}
EwFilledTextField.styles = [sharedStyles, filledStyles];
customElements.define("ew-filled-text-field", EwFilledTextField);
