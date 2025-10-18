export declare const fireEvent: <Event extends keyof HTMLElementEventMap>(eventTarget: EventTarget, type: Event, detail?: HTMLElementEventMap[Event]["detail"], options?: {
    bubbles?: boolean;
    cancelable?: boolean;
    composed?: boolean;
}) => void;
