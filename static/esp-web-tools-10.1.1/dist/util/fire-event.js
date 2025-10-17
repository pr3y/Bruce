export const fireEvent = (eventTarget, type, 
// @ts-ignore
detail, options) => {
    options = options || {};
    const event = new CustomEvent(type, {
        bubbles: options.bubbles === undefined ? true : options.bubbles,
        cancelable: Boolean(options.cancelable),
        composed: options.composed === undefined ? true : options.composed,
        detail,
    });
    eventTarget.dispatchEvent(event);
};
