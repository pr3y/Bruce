export const fireEvent = <Event extends keyof HTMLElementEventMap>(
  eventTarget: EventTarget,
  type: Event,
  // @ts-ignore
  detail?: HTMLElementEventMap[Event]["detail"],
  options?: {
    bubbles?: boolean;
    cancelable?: boolean;
    composed?: boolean;
  },
): void => {
  options = options || {};
  const event = new CustomEvent(type, {
    bubbles: options.bubbles === undefined ? true : options.bubbles,
    cancelable: Boolean(options.cancelable),
    composed: options.composed === undefined ? true : options.composed,
    detail,
  });
  eventTarget.dispatchEvent(event);
};
