import { e as e$1, _ as __decorate, o as o$2, i as i$2, x, a as i$3, n as n$3, t, m as mixinDelegatesAria, b as e$2, E, c as e$3, s as styles$g, D as Divider, d as internals, f as mixinElementInternals, r as r$1, g as isActivationClick, h as dispatchActivationClick, j as redispatchEvent, k as setupFormSubmitter, B, l as EASING, p as e$4, q as i$4, u as t$1, T, v as createAnimationSignal, w as b, y as dialogStyles } from './styles-DmVHveMP.js';

/**
 * @license
 * Copyright 2017 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */
let e;
function r(r) {
  return (n, o) => e$1(n, o, {
    get() {
      return (this.renderRoot ?? e ?? (e = document.createDocumentFragment())).querySelectorAll(r);
    }
  });
}

/**
 * @license
 * Copyright 2017 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */
function n$2(n) {
  return (o, r) => {
    const {
        slot: e
      } = n ?? {},
      s = "slot" + (e ? `[name=${e}]` : ":not([name])");
    return e$1(o, r, {
      get() {
        var _this$renderRoot;
        const t = (_this$renderRoot = this.renderRoot) === null || _this$renderRoot === void 0 ? void 0 : _this$renderRoot.querySelector(s);
        return (t === null || t === void 0 ? void 0 : t.assignedNodes(n)) ?? [];
      }
    });
  };
}

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * Activates the first non-disabled item of a given array of items.
 *
 * @param items {Array<ListItem>} The items from which to activate the
 *     first item.
 * @param isActivatable Function to determine if an item can be  activated.
 *     Defaults to non-disabled items.
 */
function activateFirstItem(items, isActivatable = isItemNotDisabled) {
  // NOTE: These selector functions are static and not on the instance such
  // that multiple operations can be chained and we do not have to re-query
  // the DOM
  const firstItem = getFirstActivatableItem(items, isActivatable);
  if (firstItem) {
    firstItem.tabIndex = 0;
    firstItem.focus();
  }
  return firstItem;
}
/**
 * Activates the last non-disabled item of a given array of items.
 *
 * @param items {Array<ListItem>} The items from which to activate the
 *     last item.
 * @param isActivatable Function to determine if an item can be  activated.
 *     Defaults to non-disabled items.
 * @nocollapse
 */
function activateLastItem(items, isActivatable = isItemNotDisabled) {
  const lastItem = getLastActivatableItem(items, isActivatable);
  if (lastItem) {
    lastItem.tabIndex = 0;
    lastItem.focus();
  }
  return lastItem;
}
/**
 * Retrieves the first activated item of a given array of items.
 *
 * @param items {Array<ListItem>} The items to search.
 * @param isActivatable Function to determine if an item can be  activated.
 *     Defaults to non-disabled items.
 * @return A record of the first activated item including the item and the
 *     index of the item or `null` if none are activated.
 * @nocollapse
 */
function getActiveItem(items, isActivatable = isItemNotDisabled) {
  for (let i = 0; i < items.length; i++) {
    const item = items[i];
    if (item.tabIndex === 0 && isActivatable(item)) {
      return {
        item,
        index: i
      };
    }
  }
  return null;
}
/**
 * Retrieves the first non-disabled item of a given array of items. This
 * the first item that is not disabled.
 *
 * @param items {Array<ListItem>} The items to search.
 * @param isActivatable Function to determine if an item can be  activated.
 *     Defaults to non-disabled items.
 * @return The first activatable item or `null` if none are activatable.
 * @nocollapse
 */
function getFirstActivatableItem(items, isActivatable = isItemNotDisabled) {
  for (const item of items) {
    if (isActivatable(item)) {
      return item;
    }
  }
  return null;
}
/**
 * Retrieves the last non-disabled item of a given array of items.
 *
 * @param items {Array<ListItem>} The items to search.
 * @param isActivatable Function to determine if an item can be  activated.
 *     Defaults to non-disabled items.
 * @return The last activatable item or `null` if none are activatable.
 * @nocollapse
 */
function getLastActivatableItem(items, isActivatable = isItemNotDisabled) {
  for (let i = items.length - 1; i >= 0; i--) {
    const item = items[i];
    if (isActivatable(item)) {
      return item;
    }
  }
  return null;
}
/**
 * Retrieves the next non-disabled item of a given array of items.
 *
 * @param items {Array<ListItem>} The items to search.
 * @param index {{index: number}} The index to search from.
 * @param isActivatable Function to determine if an item can be  activated.
 *     Defaults to non-disabled items.
 * @param wrap If true, then the next item at the end of the list is the first
 *     item. Defaults to true.
 * @return The next activatable item or `null` if none are activatable.
 */
function getNextItem(items, index, isActivatable = isItemNotDisabled, wrap = true) {
  for (let i = 1; i < items.length; i++) {
    const nextIndex = (i + index) % items.length;
    if (nextIndex < index && !wrap) {
      // Return if the index loops back to the beginning and not wrapping.
      return null;
    }
    const item = items[nextIndex];
    if (isActivatable(item)) {
      return item;
    }
  }
  return items[index] ? items[index] : null;
}
/**
 * Retrieves the previous non-disabled item of a given array of items.
 *
 * @param items {Array<ListItem>} The items to search.
 * @param index {{index: number}} The index to search from.
 * @param isActivatable Function to determine if an item can be  activated.
 *     Defaults to non-disabled items.
 * @param wrap If true, then the previous item at the beginning of the list is
 *     the last item. Defaults to true.
 * @return The previous activatable item or `null` if none are activatable.
 */
function getPrevItem(items, index, isActivatable = isItemNotDisabled, wrap = true) {
  for (let i = 1; i < items.length; i++) {
    const prevIndex = (index - i + items.length) % items.length;
    if (prevIndex > index && !wrap) {
      // Return if the index loops back to the end and not wrapping.
      return null;
    }
    const item = items[prevIndex];
    if (isActivatable(item)) {
      return item;
    }
  }
  return items[index] ? items[index] : null;
}
/**
 * Activates the next item and focuses it. If nothing is currently activated,
 * activates the first item.
 */
function activateNextItem(items, activeItemRecord, isActivatable = isItemNotDisabled, wrap = true) {
  if (activeItemRecord) {
    const next = getNextItem(items, activeItemRecord.index, isActivatable, wrap);
    if (next) {
      next.tabIndex = 0;
      next.focus();
    }
    return next;
  } else {
    return activateFirstItem(items, isActivatable);
  }
}
/**
 * Activates the previous item and focuses it. If nothing is currently
 * activated, activates the last item.
 */
function activatePreviousItem(items, activeItemRecord, isActivatable = isItemNotDisabled, wrap = true) {
  if (activeItemRecord) {
    const prev = getPrevItem(items, activeItemRecord.index, isActivatable, wrap);
    if (prev) {
      prev.tabIndex = 0;
      prev.focus();
    }
    return prev;
  } else {
    return activateLastItem(items, isActivatable);
  }
}
/**
 * Creates an event that requests the menu to set `tabindex=0` on the item and
 * focus it. We use this pattern because List keeps track of what element is
 * active in the List by maintaining tabindex. We do not want list items
 * to set tabindex on themselves or focus themselves so that we can organize all
 * that logic in the parent List and Menus, and list item stays as dumb as
 * possible.
 */
function createRequestActivationEvent() {
  return new Event('request-activation', {
    bubbles: true,
    composed: true
  });
}
/**
 * The default `isActivatable` function, which checks if an item is not
 * disabled.
 *
 * @param item The item to check.
 * @return true if `item.disabled` is `false.
 */
function isItemNotDisabled(item) {
  return !item.disabled;
}

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// TODO: move this file to List and make List use this
/**
 * Default keys that trigger navigation.
 */
// tslint:disable:enforce-name-casing Following Enum style
const NavigableKeys = {
  ArrowDown: 'ArrowDown',
  ArrowLeft: 'ArrowLeft',
  ArrowUp: 'ArrowUp',
  ArrowRight: 'ArrowRight',
  Home: 'Home',
  End: 'End'
};
/**
 * A controller that handles list keyboard navigation and item management.
 */
class ListController {
  constructor(config) {
    /**
     * Handles keyboard navigation. Should be bound to the node that will act as
     * the List.
     */
    this.handleKeydown = event => {
      const key = event.key;
      if (event.defaultPrevented || !this.isNavigableKey(key)) {
        return;
      }
      // do not use this.items directly in upcoming calculations so we don't
      // re-query the DOM unnecessarily
      const items = this.items;
      if (!items.length) {
        return;
      }
      const activeItemRecord = getActiveItem(items, this.isActivatable);
      event.preventDefault();
      const isRtl = this.isRtl();
      const inlinePrevious = isRtl ? NavigableKeys.ArrowRight : NavigableKeys.ArrowLeft;
      const inlineNext = isRtl ? NavigableKeys.ArrowLeft : NavigableKeys.ArrowRight;
      let nextActiveItem = null;
      switch (key) {
        // Activate the next item
        case NavigableKeys.ArrowDown:
        case inlineNext:
          nextActiveItem = activateNextItem(items, activeItemRecord, this.isActivatable, this.wrapNavigation());
          break;
        // Activate the previous item
        case NavigableKeys.ArrowUp:
        case inlinePrevious:
          nextActiveItem = activatePreviousItem(items, activeItemRecord, this.isActivatable, this.wrapNavigation());
          break;
        // Activate the first item
        case NavigableKeys.Home:
          nextActiveItem = activateFirstItem(items, this.isActivatable);
          break;
        // Activate the last item
        case NavigableKeys.End:
          nextActiveItem = activateLastItem(items, this.isActivatable);
          break;
      }
      if (nextActiveItem && activeItemRecord && activeItemRecord.item !== nextActiveItem) {
        // If a new item was activated, remove the tabindex of the previous
        // activated item.
        activeItemRecord.item.tabIndex = -1;
      }
    };
    /**
     * Listener to be bound to the `deactivate-items` item event.
     */
    this.onDeactivateItems = () => {
      const items = this.items;
      for (const item of items) {
        this.deactivateItem(item);
      }
    };
    /**
     * Listener to be bound to the `request-activation` item event..
     */
    this.onRequestActivation = event => {
      this.onDeactivateItems();
      const target = event.target;
      this.activateItem(target);
      target.focus();
    };
    /**
     * Listener to be bound to the `slotchange` event for the slot that renders
     * the items.
     */
    this.onSlotchange = () => {
      const items = this.items;
      // Whether we have encountered an item that has been activated
      let encounteredActivated = false;
      for (const item of items) {
        const isActivated = !item.disabled && item.tabIndex > -1;
        if (isActivated && !encounteredActivated) {
          encounteredActivated = true;
          item.tabIndex = 0;
          continue;
        }
        // Deactivate the rest including disabled
        item.tabIndex = -1;
      }
      if (encounteredActivated) {
        return;
      }
      const firstActivatableItem = getFirstActivatableItem(items, this.isActivatable);
      if (!firstActivatableItem) {
        return;
      }
      firstActivatableItem.tabIndex = 0;
    };
    const {
      isItem,
      getPossibleItems,
      isRtl,
      deactivateItem,
      activateItem,
      isNavigableKey,
      isActivatable,
      wrapNavigation
    } = config;
    this.isItem = isItem;
    this.getPossibleItems = getPossibleItems;
    this.isRtl = isRtl;
    this.deactivateItem = deactivateItem;
    this.activateItem = activateItem;
    this.isNavigableKey = isNavigableKey;
    this.isActivatable = isActivatable;
    this.wrapNavigation = wrapNavigation ?? (() => true);
  }
  /**
   * The items being managed by the list. Additionally, attempts to see if the
   * object has a sub-item in the `.item` property.
   */
  get items() {
    const maybeItems = this.getPossibleItems();
    const items = [];
    for (const itemOrParent of maybeItems) {
      const isItem = this.isItem(itemOrParent);
      // if the item is a list item, add it to the list of items
      if (isItem) {
        items.push(itemOrParent);
        continue;
      }
      // If the item exposes an `item` property check if it is a list item.
      const subItem = itemOrParent.item;
      if (subItem && this.isItem(subItem)) {
        items.push(subItem);
      }
    }
    return items;
  }
  /**
   * Activates the next item in the list. If at the end of the list, the first
   * item will be activated.
   *
   * @return The activated list item or `null` if there are no items.
   */
  activateNextItem() {
    const items = this.items;
    const activeItemRecord = getActiveItem(items, this.isActivatable);
    if (activeItemRecord) {
      activeItemRecord.item.tabIndex = -1;
    }
    return activateNextItem(items, activeItemRecord, this.isActivatable, this.wrapNavigation());
  }
  /**
   * Activates the previous item in the list. If at the start of the list, the
   * last item will be activated.
   *
   * @return The activated list item or `null` if there are no items.
   */
  activatePreviousItem() {
    const items = this.items;
    const activeItemRecord = getActiveItem(items, this.isActivatable);
    if (activeItemRecord) {
      activeItemRecord.item.tabIndex = -1;
    }
    return activatePreviousItem(items, activeItemRecord, this.isActivatable, this.wrapNavigation());
  }
}

/**
 * @license
 * Copyright 2021 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
const NAVIGABLE_KEY_SET = new Set(Object.values(NavigableKeys));
// tslint:disable-next-line:enforce-comments-on-exported-symbols
class List extends i$2 {
  /** @export */
  get items() {
    return this.listController.items;
  }
  constructor() {
    super();
    this.listController = new ListController({
      isItem: item => item.hasAttribute('md-list-item'),
      getPossibleItems: () => this.slotItems,
      isRtl: () => getComputedStyle(this).direction === 'rtl',
      deactivateItem: item => {
        item.tabIndex = -1;
      },
      activateItem: item => {
        item.tabIndex = 0;
      },
      isNavigableKey: key => NAVIGABLE_KEY_SET.has(key),
      isActivatable: item => !item.disabled && item.type !== 'text'
    });
    this.internals =
    // Cast needed for closure
    this.attachInternals();
    {
      this.internals.role = 'list';
      this.addEventListener('keydown', this.listController.handleKeydown);
    }
  }
  render() {
    return x`
      <slot
        @deactivate-items=${this.listController.onDeactivateItems}
        @request-activation=${this.listController.onRequestActivation}
        @slotchange=${this.listController.onSlotchange}>
      </slot>
    `;
  }
  /**
   * Activates the next item in the list. If at the end of the list, the first
   * item will be activated.
   *
   * @return The activated list item or `null` if there are no items.
   */
  activateNextItem() {
    return this.listController.activateNextItem();
  }
  /**
   * Activates the previous item in the list. If at the start of the list, the
   * last item will be activated.
   *
   * @return The activated list item or `null` if there are no items.
   */
  activatePreviousItem() {
    return this.listController.activatePreviousItem();
  }
}
__decorate([o$2({
  flatten: true
})], List.prototype, "slotItems", void 0);

/**
 * @license
 * Copyright 2024 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Generated stylesheet for ./list/internal/list-styles.css.
const styles$f = i$3`:host{background:var(--md-list-container-color, var(--md-sys-color-surface, #fef7ff));color:unset;display:flex;flex-direction:column;outline:none;padding:8px 0;position:relative}
`;

class EwList extends List {}
EwList.styles = [styles$f];
customElements.define("ew-list", EwList);

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * An item layout component.
 */
class Item extends i$2 {
  constructor() {
    super(...arguments);
    /**
     * Only needed for SSR.
     *
     * Add this attribute when an item has two lines to avoid a Flash Of Unstyled
     * Content. This attribute is not needed for single line items or items with
     * three or more lines.
     */
    this.multiline = false;
  }
  render() {
    return x`
      <slot name="container"></slot>
      <slot class="non-text" name="start"></slot>
      <div class="text">
        <slot name="overline" @slotchange=${this.handleTextSlotChange}></slot>
        <slot
          class="default-slot"
          @slotchange=${this.handleTextSlotChange}></slot>
        <slot name="headline" @slotchange=${this.handleTextSlotChange}></slot>
        <slot
          name="supporting-text"
          @slotchange=${this.handleTextSlotChange}></slot>
      </div>
      <slot class="non-text" name="trailing-supporting-text"></slot>
      <slot class="non-text" name="end"></slot>
    `;
  }
  handleTextSlotChange() {
    // Check if there's more than one text slot with content. If so, the item is
    // multiline, which has a different min-height than single line items.
    let isMultiline = false;
    let slotsWithContent = 0;
    for (const slot of this.textSlots) {
      if (slotHasContent(slot)) {
        slotsWithContent += 1;
      }
      if (slotsWithContent > 1) {
        isMultiline = true;
        break;
      }
    }
    this.multiline = isMultiline;
  }
}
__decorate([n$3({
  type: Boolean,
  reflect: true
})], Item.prototype, "multiline", void 0);
__decorate([r('.text slot')], Item.prototype, "textSlots", void 0);
function slotHasContent(slot) {
  for (const node of slot.assignedNodes({
    flatten: true
  })) {
    var _node$textContent;
    // Assume there's content if there's an element slotted in
    const isElement = node.nodeType === Node.ELEMENT_NODE;
    // If there's only text nodes for the default slot, check if there's
    // non-whitespace.
    const isTextWithContent = node.nodeType === Node.TEXT_NODE && ((_node$textContent = node.textContent) === null || _node$textContent === void 0 ? void 0 : _node$textContent.match(/\S/));
    if (isElement || isTextWithContent) {
      return true;
    }
  }
  return false;
}

/**
 * @license
 * Copyright 2024 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Generated stylesheet for ./labs/item/internal/item-styles.css.
const styles$e = i$3`:host{color:var(--md-sys-color-on-surface, #1d1b20);font-family:var(--md-sys-typescale-body-large-font, var(--md-ref-typeface-plain, Roboto));font-size:var(--md-sys-typescale-body-large-size, 1rem);font-weight:var(--md-sys-typescale-body-large-weight, var(--md-ref-typeface-weight-regular, 400));line-height:var(--md-sys-typescale-body-large-line-height, 1.5rem);align-items:center;box-sizing:border-box;display:flex;gap:16px;min-height:56px;overflow:hidden;padding:12px 16px;position:relative;text-overflow:ellipsis}:host([multiline]){min-height:72px}[name=overline]{color:var(--md-sys-color-on-surface-variant, #49454f);font-family:var(--md-sys-typescale-label-small-font, var(--md-ref-typeface-plain, Roboto));font-size:var(--md-sys-typescale-label-small-size, 0.6875rem);font-weight:var(--md-sys-typescale-label-small-weight, var(--md-ref-typeface-weight-medium, 500));line-height:var(--md-sys-typescale-label-small-line-height, 1rem)}[name=supporting-text]{color:var(--md-sys-color-on-surface-variant, #49454f);font-family:var(--md-sys-typescale-body-medium-font, var(--md-ref-typeface-plain, Roboto));font-size:var(--md-sys-typescale-body-medium-size, 0.875rem);font-weight:var(--md-sys-typescale-body-medium-weight, var(--md-ref-typeface-weight-regular, 400));line-height:var(--md-sys-typescale-body-medium-line-height, 1.25rem)}[name=trailing-supporting-text]{color:var(--md-sys-color-on-surface-variant, #49454f);font-family:var(--md-sys-typescale-label-small-font, var(--md-ref-typeface-plain, Roboto));font-size:var(--md-sys-typescale-label-small-size, 0.6875rem);font-weight:var(--md-sys-typescale-label-small-weight, var(--md-ref-typeface-weight-medium, 500));line-height:var(--md-sys-typescale-label-small-line-height, 1rem)}[name=container]::slotted(*){inset:0;position:absolute}.default-slot{display:inline}.default-slot,.text ::slotted(*){overflow:hidden;text-overflow:ellipsis}.text{display:flex;flex:1;flex-direction:column;overflow:hidden}
`;

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * An item layout component that can be used inside list items to give them
 * their customizable structure.
 *
 * `<md-item>` does not have any functionality, which must be added by the
 * component using it.
 *
 * All text will wrap unless `white-space: nowrap` is set on the item or any of
 * its children.
 *
 * Slots available:
 * - `<default>`: The headline, or custom content.
 * - `headline`: The first line.
 * - `supporting-text`: Supporting text lines underneath the headline.
 * - `trailing-supporting-text`: A small text snippet at the end of the item.
 * - `start`: Any leading content, such as icons, avatars, or checkboxes.
 * - `end`: Any trailing content, such as icons and buttons.
 * - `container`: Background container content, intended for adding additional
 *     styles, such as ripples or focus rings.
 *
 * @example
 * ```html
 * <md-item>Single line</md-item>
 *
 * <md-item>
 *   <div class="custom-content">...</div>
 * </md-item>
 *
 * <!-- Classic 1 to 3+ line list items -->
 * <md-item>
 *   <md-icon slot="start">image</md-icon>
 *   <div slot="overline">Overline</div>
 *   <div slot="headline">Headline</div>
 *   <div="supporting-text">Supporting text</div>
 *   <div="trailing-supporting-text">Trailing</div>
 *   <md-icon slot="end">image</md-icon>
 * </md-item>
 * ```
 *
 * When wrapping `<md-item>`, forward the available slots to use the same slot
 * structure for the wrapping component (this is what `<md-list-item>` does).
 *
 * @example
 * ```html
 * <md-item>
 *   <slot></slot>
 *   <slot name="overline" slot="overline"></slot>
 *   <slot name="headline" slot="headline"></slot>
 *   <slot name="supporting-text" slot="supporting-text"></slot>
 *   <slot name="trailing-supporting-text"
 *       slot="trailing-supporting-text"></slot>
 *   <slot name="start" slot="start"></slot>
 *   <slot name="end" slot="end"></slot>
 * </md-item>
 * ```
 *
 * @final
 * @suppress {visibility}
 */
let MdItem = class MdItem extends Item {};
MdItem.styles = [styles$e];
MdItem = __decorate([t('md-item')], MdItem);

/**
 * @license
 * Copyright 2020 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */
const a = Symbol.for(""),
  o$1 = t => {
    if ((t === null || t === void 0 ? void 0 : t.r) === a) return t === null || t === void 0 ? void 0 : t._$litStatic$;
  },
  i$1 = (t, ...r) => ({
    _$litStatic$: r.reduce((r, e, a) => r + (t => {
      if (void 0 !== t._$litStatic$) return t._$litStatic$;
      throw Error(`Value passed to 'literal' function must be a 'literal' result: ${t}. Use 'unsafeStatic' to pass non-literal values, but\n            take care to ensure page security.`);
    })(e) + t[a + 1], t[0]),
    r: a
  }),
  l$1 = new Map(),
  n$1 = t => (r, ...e) => {
    const a = e.length;
    let s, i;
    const n = [],
      u = [];
    let c,
      $ = 0,
      f = false;
    for (; $ < a;) {
      for (c = r[$]; $ < a && void 0 !== (i = e[$], s = o$1(i));) c += s + r[++$], f = true;
      $ !== a && u.push(i), n.push(c), $++;
    }
    if ($ === a && n.push(r[a]), f) {
      const t = n.join("$$lit$$");
      void 0 === (r = l$1.get(t)) && (n.raw = n, l$1.set(t, r = n)), e = u;
    }
    return t(r, ...e);
  },
  u$1 = n$1(x);

/**
 * @license
 * Copyright 2022 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Separate variable needed for closure.
const listItemBaseClass = mixinDelegatesAria(i$2);
/**
 * @fires request-activation {Event} Requests the list to set `tabindex=0` on
 * the item and focus it. --bubbles --composed
 */
class ListItemEl extends listItemBaseClass {
  constructor() {
    super(...arguments);
    /**
     * Disables the item and makes it non-selectable and non-interactive.
     */
    this.disabled = false;
    /**
     * Sets the behavior of the list item, defaults to "text". Change to "link" or
     * "button" for interactive items.
     */
    this.type = 'text';
    /**
     * READONLY. Sets the `md-list-item` attribute on the element.
     */
    this.isListItem = true;
    /**
     * Sets the underlying `HTMLAnchorElement`'s `href` resource attribute.
     */
    this.href = '';
    /**
     * Sets the underlying `HTMLAnchorElement`'s `target` attribute when `href` is
     * set.
     */
    this.target = '';
  }
  get isDisabled() {
    return this.disabled && this.type !== 'link';
  }
  willUpdate(changed) {
    if (this.href) {
      this.type = 'link';
    }
    super.willUpdate(changed);
  }
  render() {
    return this.renderListItem(x`
      <md-item>
        <div slot="container">
          ${this.renderRipple()} ${this.renderFocusRing()}
        </div>
        <slot name="start" slot="start"></slot>
        <slot name="end" slot="end"></slot>
        ${this.renderBody()}
      </md-item>
    `);
  }
  /**
   * Renders the root list item.
   *
   * @param content the child content of the list item.
   */
  renderListItem(content) {
    const isAnchor = this.type === 'link';
    let tag;
    switch (this.type) {
      case 'link':
        tag = i$1`a`;
        break;
      case 'button':
        tag = i$1`button`;
        break;
      default:
      case 'text':
        tag = i$1`li`;
        break;
    }
    const isInteractive = this.type !== 'text';
    // TODO(b/265339866): announce "button"/"link" inside of a list item. Until
    // then all are "listitem" roles for correct announcement.
    const target = isAnchor && !!this.target ? this.target : E;
    return u$1`
      <${tag}
        id="item"
        tabindex="${this.isDisabled || !isInteractive ? -1 : 0}"
        ?disabled=${this.isDisabled}
        role="listitem"
        aria-selected=${this.ariaSelected || E}
        aria-checked=${this.ariaChecked || E}
        aria-expanded=${this.ariaExpanded || E}
        aria-haspopup=${this.ariaHasPopup || E}
        class="list-item ${e$3(this.getRenderClasses())}"
        href=${this.href || E}
        target=${target}
        @focus=${this.onFocus}
      >${content}</${tag}>
    `;
  }
  /**
   * Handles rendering of the ripple element.
   */
  renderRipple() {
    if (this.type === 'text') {
      return E;
    }
    return x` <md-ripple
      part="ripple"
      for="item"
      ?disabled=${this.isDisabled}></md-ripple>`;
  }
  /**
   * Handles rendering of the focus ring.
   */
  renderFocusRing() {
    if (this.type === 'text') {
      return E;
    }
    return x` <md-focus-ring
      @visibility-changed=${this.onFocusRingVisibilityChanged}
      part="focus-ring"
      for="item"
      inward></md-focus-ring>`;
  }
  onFocusRingVisibilityChanged(e) {}
  /**
   * Classes applied to the list item root.
   */
  getRenderClasses() {
    return {
      'disabled': this.isDisabled
    };
  }
  /**
   * Handles rendering the headline and supporting text.
   */
  renderBody() {
    return x`
      <slot></slot>
      <slot name="overline" slot="overline"></slot>
      <slot name="headline" slot="headline"></slot>
      <slot name="supporting-text" slot="supporting-text"></slot>
      <slot
        name="trailing-supporting-text"
        slot="trailing-supporting-text"></slot>
    `;
  }
  onFocus() {
    if (this.tabIndex !== -1) {
      return;
    }
    // Handles the case where the user clicks on the element and then tabs.
    this.dispatchEvent(createRequestActivationEvent());
  }
  focus() {
    var _this$listItemRoot;
    // TODO(b/300334509): needed for some cases where delegatesFocus doesn't
    // work programmatically like in FF and select-option
    (_this$listItemRoot = this.listItemRoot) === null || _this$listItemRoot === void 0 || _this$listItemRoot.focus();
  }
  click() {
    if (!this.listItemRoot) {
      // If the element has not finished rendering, call super to ensure click
      // events are dispatched.
      super.click();
      return;
    }
    // Forward click to the element to ensure link <a>.click() works correctly.
    this.listItemRoot.click();
  }
}
/** @nocollapse */
ListItemEl.shadowRootOptions = {
  ...i$2.shadowRootOptions,
  delegatesFocus: true
};
__decorate([n$3({
  type: Boolean,
  reflect: true
})], ListItemEl.prototype, "disabled", void 0);
__decorate([n$3({
  reflect: true
})], ListItemEl.prototype, "type", void 0);
__decorate([n$3({
  type: Boolean,
  attribute: 'md-list-item',
  reflect: true
})], ListItemEl.prototype, "isListItem", void 0);
__decorate([n$3()], ListItemEl.prototype, "href", void 0);
__decorate([n$3()], ListItemEl.prototype, "target", void 0);
__decorate([e$2('.list-item')], ListItemEl.prototype, "listItemRoot", void 0);

/**
 * @license
 * Copyright 2024 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Generated stylesheet for ./list/internal/listitem/list-item-styles.css.
const styles$d = i$3`:host{display:flex;-webkit-tap-highlight-color:rgba(0,0,0,0);--md-ripple-hover-color: var(--md-list-item-hover-state-layer-color, var(--md-sys-color-on-surface, #1d1b20));--md-ripple-hover-opacity: var(--md-list-item-hover-state-layer-opacity, 0.08);--md-ripple-pressed-color: var(--md-list-item-pressed-state-layer-color, var(--md-sys-color-on-surface, #1d1b20));--md-ripple-pressed-opacity: var(--md-list-item-pressed-state-layer-opacity, 0.12)}:host(:is([type=button]:not([disabled]),[type=link])){cursor:pointer}md-focus-ring{z-index:1;--md-focus-ring-shape: 8px}a,button,li{background:none;border:none;cursor:inherit;padding:0;margin:0;text-align:unset;text-decoration:none}.list-item{border-radius:inherit;display:flex;flex:1;max-width:inherit;min-width:inherit;outline:none;-webkit-tap-highlight-color:rgba(0,0,0,0);width:100%}.list-item.interactive{cursor:pointer}.list-item.disabled{opacity:var(--md-list-item-disabled-opacity, 0.3);pointer-events:none}[slot=container]{pointer-events:none}md-ripple{border-radius:inherit}md-item{border-radius:inherit;flex:1;height:100%;color:var(--md-list-item-label-text-color, var(--md-sys-color-on-surface, #1d1b20));font-family:var(--md-list-item-label-text-font, var(--md-sys-typescale-body-large-font, var(--md-ref-typeface-plain, Roboto)));font-size:var(--md-list-item-label-text-size, var(--md-sys-typescale-body-large-size, 1rem));line-height:var(--md-list-item-label-text-line-height, var(--md-sys-typescale-body-large-line-height, 1.5rem));font-weight:var(--md-list-item-label-text-weight, var(--md-sys-typescale-body-large-weight, var(--md-ref-typeface-weight-regular, 400)));min-height:var(--md-list-item-one-line-container-height, 56px);padding-top:var(--md-list-item-top-space, 12px);padding-bottom:var(--md-list-item-bottom-space, 12px);padding-inline-start:var(--md-list-item-leading-space, 16px);padding-inline-end:var(--md-list-item-trailing-space, 16px)}md-item[multiline]{min-height:var(--md-list-item-two-line-container-height, 72px)}[slot=supporting-text]{color:var(--md-list-item-supporting-text-color, var(--md-sys-color-on-surface-variant, #49454f));font-family:var(--md-list-item-supporting-text-font, var(--md-sys-typescale-body-medium-font, var(--md-ref-typeface-plain, Roboto)));font-size:var(--md-list-item-supporting-text-size, var(--md-sys-typescale-body-medium-size, 0.875rem));line-height:var(--md-list-item-supporting-text-line-height, var(--md-sys-typescale-body-medium-line-height, 1.25rem));font-weight:var(--md-list-item-supporting-text-weight, var(--md-sys-typescale-body-medium-weight, var(--md-ref-typeface-weight-regular, 400)))}[slot=trailing-supporting-text]{color:var(--md-list-item-trailing-supporting-text-color, var(--md-sys-color-on-surface-variant, #49454f));font-family:var(--md-list-item-trailing-supporting-text-font, var(--md-sys-typescale-label-small-font, var(--md-ref-typeface-plain, Roboto)));font-size:var(--md-list-item-trailing-supporting-text-size, var(--md-sys-typescale-label-small-size, 0.6875rem));line-height:var(--md-list-item-trailing-supporting-text-line-height, var(--md-sys-typescale-label-small-line-height, 1rem));font-weight:var(--md-list-item-trailing-supporting-text-weight, var(--md-sys-typescale-label-small-weight, var(--md-ref-typeface-weight-medium, 500)))}:is([slot=start],[slot=end])::slotted(*){fill:currentColor}[slot=start]{color:var(--md-list-item-leading-icon-color, var(--md-sys-color-on-surface-variant, #49454f))}[slot=end]{color:var(--md-list-item-trailing-icon-color, var(--md-sys-color-on-surface-variant, #49454f))}@media(forced-colors: active){.disabled slot{color:GrayText}.list-item.disabled{color:GrayText;opacity:1}}
`;

class EwListItem extends ListItemEl {}
EwListItem.styles = [styles$d];
customElements.define("ew-list-item", EwListItem);

class EwDivider extends Divider {}
EwDivider.styles = [styles$g];
customElements.define("ew-divider", EwDivider);

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * A symbol property used to create a constraint validation `Validator`.
 * Required for all `mixinConstraintValidation()` elements.
 */
const createValidator = Symbol('createValidator');
/**
 * A symbol property used to return an anchor for constraint validation popups.
 * Required for all `mixinConstraintValidation()` elements.
 */
const getValidityAnchor = Symbol('getValidityAnchor');
// Private symbol members, used to avoid name clashing.
const privateValidator = Symbol('privateValidator');
const privateSyncValidity = Symbol('privateSyncValidity');
const privateCustomValidationMessage = Symbol('privateCustomValidationMessage');
/**
 * Mixes in constraint validation APIs for an element.
 *
 * See https://developer.mozilla.org/en-US/docs/Web/HTML/Constraint_validation
 * for more details.
 *
 * Implementations must provide a validator to cache and compute its validity,
 * along with a shadow root element to anchor validation popups to.
 *
 * @example
 * ```ts
 * const baseClass = mixinConstraintValidation(
 *   mixinFormAssociated(mixinElementInternals(LitElement))
 * );
 *
 * class MyCheckbox extends baseClass {
 *   \@property({type: Boolean}) checked = false;
 *   \@property({type: Boolean}) required = false;
 *
 *   [createValidator]() {
 *     return new CheckboxValidator(() => this);
 *   }
 *
 *   [getValidityAnchor]() {
 *     return this.renderRoot.querySelector('.root');
 *   }
 * }
 * ```
 *
 * @param base The class to mix functionality into.
 * @return The provided class with `ConstraintValidation` mixed in.
 */
function mixinConstraintValidation(base) {
  var _a;
  class ConstraintValidationElement extends base {
    constructor() {
      super(...arguments);
      /**
       * Needed for Safari, see https://bugs.webkit.org/show_bug.cgi?id=261432
       * Replace with this[internals].validity.customError when resolved.
       */
      this[_a] = '';
    }
    get validity() {
      this[privateSyncValidity]();
      return this[internals].validity;
    }
    get validationMessage() {
      this[privateSyncValidity]();
      return this[internals].validationMessage;
    }
    get willValidate() {
      this[privateSyncValidity]();
      return this[internals].willValidate;
    }
    checkValidity() {
      this[privateSyncValidity]();
      return this[internals].checkValidity();
    }
    reportValidity() {
      this[privateSyncValidity]();
      return this[internals].reportValidity();
    }
    setCustomValidity(error) {
      this[privateCustomValidationMessage] = error;
      this[privateSyncValidity]();
    }
    requestUpdate(name, oldValue, options) {
      super.requestUpdate(name, oldValue, options);
      this[privateSyncValidity]();
    }
    firstUpdated(changed) {
      super.firstUpdated(changed);
      // Sync the validity again when the element first renders, since the
      // validity anchor is now available.
      //
      // Elements that `delegatesFocus: true` to an `<input>` will throw an
      // error in Chrome and Safari when a form tries to submit or call
      // `form.reportValidity()`:
      // "An invalid form control with name='' is not focusable"
      //
      // The validity anchor MUST be provided in `internals.setValidity()` and
      // MUST be the `<input>` element rendered.
      //
      // See https://lit.dev/playground/#gist=6c26e418e0010f7a5aac15005cde8bde
      // for a reproduction.
      this[privateSyncValidity]();
    }
    [(_a = privateCustomValidationMessage, privateSyncValidity)]() {
      if (!this[privateValidator]) {
        this[privateValidator] = this[createValidator]();
      }
      const {
        validity,
        validationMessage: nonCustomValidationMessage
      } = this[privateValidator].getValidity();
      const customError = !!this[privateCustomValidationMessage];
      const validationMessage = this[privateCustomValidationMessage] || nonCustomValidationMessage;
      this[internals].setValidity({
        ...validity,
        customError
      }, validationMessage, this[getValidityAnchor]() ?? undefined);
    }
    [createValidator]() {
      throw new Error('Implement [createValidator]');
    }
    [getValidityAnchor]() {
      throw new Error('Implement [getValidityAnchor]');
    }
  }
  return ConstraintValidationElement;
}

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * A symbol property to retrieve the form value for an element.
 */
const getFormValue = Symbol('getFormValue');
/**
 * A symbol property to retrieve the form state for an element.
 */
const getFormState = Symbol('getFormState');
/**
 * Mixes in form-associated behavior for a class. This allows an element to add
 * values to `<form>` elements.
 *
 * Implementing classes should provide a `[formValue]` to return the current
 * value of the element, as well as reset and restore callbacks.
 *
 * @example
 * ```ts
 * const base = mixinFormAssociated(mixinElementInternals(LitElement));
 *
 * class MyControl extends base {
 *   \@property()
 *   value = '';
 *
 *   override [getFormValue]() {
 *     return this.value;
 *   }
 *
 *   override formResetCallback() {
 *     const defaultValue = this.getAttribute('value');
 *     this.value = defaultValue;
 *   }
 *
 *   override formStateRestoreCallback(state: string) {
 *     this.value = state;
 *   }
 * }
 * ```
 *
 * Elements may optionally provide a `[formState]` if their values do not
 * represent the state of the component.
 *
 * @example
 * ```ts
 * const base = mixinFormAssociated(mixinElementInternals(LitElement));
 *
 * class MyCheckbox extends base {
 *   \@property()
 *   value = 'on';
 *
 *   \@property({type: Boolean})
 *   checked = false;
 *
 *   override [getFormValue]() {
 *     return this.checked ? this.value : null;
 *   }
 *
 *   override [getFormState]() {
 *     return String(this.checked);
 *   }
 *
 *   override formResetCallback() {
 *     const defaultValue = this.hasAttribute('checked');
 *     this.checked = defaultValue;
 *   }
 *
 *   override formStateRestoreCallback(state: string) {
 *     this.checked = Boolean(state);
 *   }
 * }
 * ```
 *
 * IMPORTANT: Requires declares for lit-analyzer
 * @example
 * ```ts
 * const base = mixinFormAssociated(mixinElementInternals(LitElement));
 * class MyControl extends base {
 *   // Writable mixin properties for lit-html binding, needed for lit-analyzer
 *   declare disabled: boolean;
 *   declare name: string;
 * }
 * ```
 *
 * @param base The class to mix functionality into. The base class must use
 *     `mixinElementInternals()`.
 * @return The provided class with `FormAssociated` mixed in.
 */
function mixinFormAssociated(base) {
  class FormAssociatedElement extends base {
    get form() {
      return this[internals].form;
    }
    get labels() {
      return this[internals].labels;
    }
    // Use @property for the `name` and `disabled` properties to add them to the
    // `observedAttributes` array and trigger `attributeChangedCallback()`.
    //
    // We don't use Lit's default getter/setter (`noAccessor: true`) because
    // the attributes need to be updated synchronously to work with synchronous
    // form APIs, and Lit updates attributes async by default.
    get name() {
      return this.getAttribute('name') ?? '';
    }
    set name(name) {
      // Note: setting name to null or empty does not remove the attribute.
      this.setAttribute('name', name);
      // We don't need to call `requestUpdate()` since it's called synchronously
      // in `attributeChangedCallback()`.
    }
    get disabled() {
      return this.hasAttribute('disabled');
    }
    set disabled(disabled) {
      this.toggleAttribute('disabled', disabled);
      // We don't need to call `requestUpdate()` since it's called synchronously
      // in `attributeChangedCallback()`.
    }
    attributeChangedCallback(name, old, value) {
      // Manually `requestUpdate()` for `name` and `disabled` when their
      // attribute or property changes.
      // The properties update their attributes, so this callback is invoked
      // immediately when the properties are set. We call `requestUpdate()` here
      // instead of letting Lit set the properties from the attribute change.
      // That would cause the properties to re-set the attribute and invoke this
      // callback again in a loop. This leads to stale state when Lit tries to
      // determine if a property changed or not.
      if (name === 'name' || name === 'disabled') {
        // Disabled's value is only false if the attribute is missing and null.
        const oldValue = name === 'disabled' ? old !== null : old;
        // Trigger a lit update when the attribute changes.
        this.requestUpdate(name, oldValue);
        return;
      }
      super.attributeChangedCallback(name, old, value);
    }
    requestUpdate(name, oldValue, options) {
      super.requestUpdate(name, oldValue, options);
      // If any properties change, update the form value, which may have changed
      // as well.
      // Update the form value synchronously in `requestUpdate()` rather than
      // `update()` or `updated()`, which are async. This is necessary to ensure
      // that form data is updated in time for synchronous event listeners.
      this[internals].setFormValue(this[getFormValue](), this[getFormState]());
    }
    [getFormValue]() {
      // Closure does not allow abstract symbol members, so a default
      // implementation is needed.
      throw new Error('Implement [getFormValue]');
    }
    [getFormState]() {
      return this[getFormValue]();
    }
    formDisabledCallback(disabled) {
      this.disabled = disabled;
    }
  }
  /** @nocollapse */
  FormAssociatedElement.formAssociated = true;
  __decorate([n$3({
    noAccessor: true
  })], FormAssociatedElement.prototype, "name", null);
  __decorate([n$3({
    type: Boolean,
    noAccessor: true
  })], FormAssociatedElement.prototype, "disabled", null);
  return FormAssociatedElement;
}

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * A class that computes and caches `ValidityStateFlags` for a component with
 * a given `State` interface.
 *
 * Cached performance before computing validity is important since constraint
 * validation must be checked frequently and synchronously when properties
 * change.
 *
 * @template State The expected interface of properties relevant to constraint
 *     validation.
 */
class Validator {
  /**
   * Creates a new validator.
   *
   * @param getCurrentState A callback that returns the current state of
   *     constraint validation-related properties.
   */
  constructor(getCurrentState) {
    this.getCurrentState = getCurrentState;
    /**
     * The current validity state and message. This is cached and returns if
     * constraint validation state does not change.
     */
    this.currentValidity = {
      validity: {},
      validationMessage: ''
    };
  }
  /**
   * Returns the current `ValidityStateFlags` and validation message for the
   * validator.
   *
   * If the constraint validation state has not changed, this will return a
   * cached result. This is important since `getValidity()` can be called
   * frequently in response to synchronous property changes.
   *
   * @return The current validity and validation message.
   */
  getValidity() {
    const state = this.getCurrentState();
    const hasStateChanged = !this.prevState || !this.equals(this.prevState, state);
    if (!hasStateChanged) {
      return this.currentValidity;
    }
    const {
      validity,
      validationMessage
    } = this.computeValidity(state);
    this.prevState = this.copy(state);
    this.currentValidity = {
      validationMessage,
      validity: {
        // Change any `ValidityState` instances into `ValidityStateFlags` since
        // `ValidityState` cannot be easily `{...spread}`.
        badInput: validity.badInput,
        customError: validity.customError,
        patternMismatch: validity.patternMismatch,
        rangeOverflow: validity.rangeOverflow,
        rangeUnderflow: validity.rangeUnderflow,
        stepMismatch: validity.stepMismatch,
        tooLong: validity.tooLong,
        tooShort: validity.tooShort,
        typeMismatch: validity.typeMismatch,
        valueMissing: validity.valueMissing
      }
    };
    return this.currentValidity;
  }
}

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * A validator that provides constraint validation that emulates
 * `<input type="checkbox">` validation.
 */
class CheckboxValidator extends Validator {
  computeValidity(state) {
    if (!this.checkboxControl) {
      // Lazily create the platform input
      this.checkboxControl = document.createElement('input');
      this.checkboxControl.type = 'checkbox';
    }
    this.checkboxControl.checked = state.checked;
    this.checkboxControl.required = state.required;
    return {
      validity: this.checkboxControl.validity,
      validationMessage: this.checkboxControl.validationMessage
    };
  }
  equals(prev, next) {
    return prev.checked === next.checked && prev.required === next.required;
  }
  copy({
    checked,
    required
  }) {
    return {
      checked,
      required
    };
  }
}

/**
 * @license
 * Copyright 2019 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Separate variable needed for closure.
const checkboxBaseClass = mixinDelegatesAria(mixinConstraintValidation(mixinFormAssociated(mixinElementInternals(i$2))));
/**
 * A checkbox component.
 *
 *
 * @fires change {Event} The native `change` event on
 * [`<input>`](https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/change_event)
 * --bubbles
 * @fires input {InputEvent} The native `input` event on
 * [`<input>`](https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/input_event)
 * --bubbles --composed
 */
class Checkbox extends checkboxBaseClass {
  constructor() {
    super();
    /**
     * Whether or not the checkbox is selected.
     */
    this.checked = false;
    /**
     * Whether or not the checkbox is indeterminate.
     *
     * https://developer.mozilla.org/en-US/docs/Web/HTML/Element/input/checkbox#indeterminate_state_checkboxes
     */
    this.indeterminate = false;
    /**
     * When true, require the checkbox to be selected when participating in
     * form submission.
     *
     * https://developer.mozilla.org/en-US/docs/Web/HTML/Element/input/checkbox#validation
     */
    this.required = false;
    /**
     * The value of the checkbox that is submitted with a form when selected.
     *
     * https://developer.mozilla.org/en-US/docs/Web/HTML/Element/input/checkbox#value
     */
    this.value = 'on';
    this.prevChecked = false;
    this.prevDisabled = false;
    this.prevIndeterminate = false;
    {
      this.addEventListener('click', event => {
        if (!isActivationClick(event) || !this.input) {
          return;
        }
        this.focus();
        dispatchActivationClick(this.input);
      });
    }
  }
  update(changed) {
    if (changed.has('checked') || changed.has('disabled') || changed.has('indeterminate')) {
      this.prevChecked = changed.get('checked') ?? this.checked;
      this.prevDisabled = changed.get('disabled') ?? this.disabled;
      this.prevIndeterminate = changed.get('indeterminate') ?? this.indeterminate;
    }
    super.update(changed);
  }
  render() {
    const prevNone = !this.prevChecked && !this.prevIndeterminate;
    const prevChecked = this.prevChecked && !this.prevIndeterminate;
    const prevIndeterminate = this.prevIndeterminate;
    const isChecked = this.checked && !this.indeterminate;
    const isIndeterminate = this.indeterminate;
    const containerClasses = e$3({
      'disabled': this.disabled,
      'selected': isChecked || isIndeterminate,
      'unselected': !isChecked && !isIndeterminate,
      'checked': isChecked,
      'indeterminate': isIndeterminate,
      'prev-unselected': prevNone,
      'prev-checked': prevChecked,
      'prev-indeterminate': prevIndeterminate,
      'prev-disabled': this.prevDisabled
    });
    // Needed for closure conformance
    const {
      ariaLabel,
      ariaInvalid
    } = this;
    // Note: <input> needs to be rendered before the <svg> for
    // form.reportValidity() to work in Chrome.
    return x`
      <div class="container ${containerClasses}">
        <input
          type="checkbox"
          id="input"
          aria-checked=${isIndeterminate ? 'mixed' : E}
          aria-label=${ariaLabel || E}
          aria-invalid=${ariaInvalid || E}
          ?disabled=${this.disabled}
          ?required=${this.required}
          .indeterminate=${this.indeterminate}
          .checked=${this.checked}
          @input=${this.handleInput}
          @change=${this.handleChange} />

        <div class="outline"></div>
        <div class="background"></div>
        <md-focus-ring part="focus-ring" for="input"></md-focus-ring>
        <md-ripple for="input" ?disabled=${this.disabled}></md-ripple>
        <svg class="icon" viewBox="0 0 18 18" aria-hidden="true">
          <rect class="mark short" />
          <rect class="mark long" />
        </svg>
      </div>
    `;
  }
  handleInput(event) {
    const target = event.target;
    this.checked = target.checked;
    this.indeterminate = target.indeterminate;
    // <input> 'input' event bubbles and is composed, don't re-dispatch it.
  }
  handleChange(event) {
    // <input> 'change' event is not composed, re-dispatch it.
    redispatchEvent(this, event);
  }
  [getFormValue]() {
    if (!this.checked || this.indeterminate) {
      return null;
    }
    return this.value;
  }
  [getFormState]() {
    return String(this.checked);
  }
  formResetCallback() {
    // The checked property does not reflect, so the original attribute set by
    // the user is used to determine the default value.
    this.checked = this.hasAttribute('checked');
  }
  formStateRestoreCallback(state) {
    this.checked = state === 'true';
  }
  [createValidator]() {
    return new CheckboxValidator(() => this);
  }
  [getValidityAnchor]() {
    return this.input;
  }
}
/** @nocollapse */
Checkbox.shadowRootOptions = {
  ...i$2.shadowRootOptions,
  delegatesFocus: true
};
__decorate([n$3({
  type: Boolean
})], Checkbox.prototype, "checked", void 0);
__decorate([n$3({
  type: Boolean
})], Checkbox.prototype, "indeterminate", void 0);
__decorate([n$3({
  type: Boolean
})], Checkbox.prototype, "required", void 0);
__decorate([n$3()], Checkbox.prototype, "value", void 0);
__decorate([r$1()], Checkbox.prototype, "prevChecked", void 0);
__decorate([r$1()], Checkbox.prototype, "prevDisabled", void 0);
__decorate([r$1()], Checkbox.prototype, "prevIndeterminate", void 0);
__decorate([e$2('input')], Checkbox.prototype, "input", void 0);

/**
 * @license
 * Copyright 2024 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Generated stylesheet for ./checkbox/internal/checkbox-styles.css.
const styles$c = i$3`:host{border-start-start-radius:var(--md-checkbox-container-shape-start-start, var(--md-checkbox-container-shape, 2px));border-start-end-radius:var(--md-checkbox-container-shape-start-end, var(--md-checkbox-container-shape, 2px));border-end-end-radius:var(--md-checkbox-container-shape-end-end, var(--md-checkbox-container-shape, 2px));border-end-start-radius:var(--md-checkbox-container-shape-end-start, var(--md-checkbox-container-shape, 2px));display:inline-flex;height:var(--md-checkbox-container-size, 18px);position:relative;vertical-align:top;width:var(--md-checkbox-container-size, 18px);-webkit-tap-highlight-color:rgba(0,0,0,0);cursor:pointer}:host([disabled]){cursor:default}:host([touch-target=wrapper]){margin:max(0px,(48px - var(--md-checkbox-container-size, 18px))/2)}md-focus-ring{height:44px;inset:unset;width:44px}input{appearance:none;height:48px;margin:0;opacity:0;outline:none;position:absolute;width:48px;z-index:1;cursor:inherit}:host([touch-target=none]) input{height:100%;width:100%}.container{border-radius:inherit;display:flex;height:100%;place-content:center;place-items:center;position:relative;width:100%}.outline,.background,.icon{inset:0;position:absolute}.outline,.background{border-radius:inherit}.outline{border-color:var(--md-checkbox-outline-color, var(--md-sys-color-on-surface-variant, #49454f));border-style:solid;border-width:var(--md-checkbox-outline-width, 2px);box-sizing:border-box}.background{background-color:var(--md-checkbox-selected-container-color, var(--md-sys-color-primary, #6750a4))}.background,.icon{opacity:0;transition-duration:150ms,50ms;transition-property:transform,opacity;transition-timing-function:cubic-bezier(0.3, 0, 0.8, 0.15),linear;transform:scale(0.6)}:where(.selected) :is(.background,.icon){opacity:1;transition-duration:350ms,50ms;transition-timing-function:cubic-bezier(0.05, 0.7, 0.1, 1),linear;transform:scale(1)}md-ripple{border-radius:var(--md-checkbox-state-layer-shape, var(--md-sys-shape-corner-full, 9999px));height:var(--md-checkbox-state-layer-size, 40px);inset:unset;width:var(--md-checkbox-state-layer-size, 40px);--md-ripple-hover-color: var(--md-checkbox-hover-state-layer-color, var(--md-sys-color-on-surface, #1d1b20));--md-ripple-hover-opacity: var(--md-checkbox-hover-state-layer-opacity, 0.08);--md-ripple-pressed-color: var(--md-checkbox-pressed-state-layer-color, var(--md-sys-color-primary, #6750a4));--md-ripple-pressed-opacity: var(--md-checkbox-pressed-state-layer-opacity, 0.12)}.selected md-ripple{--md-ripple-hover-color: var(--md-checkbox-selected-hover-state-layer-color, var(--md-sys-color-primary, #6750a4));--md-ripple-hover-opacity: var(--md-checkbox-selected-hover-state-layer-opacity, 0.08);--md-ripple-pressed-color: var(--md-checkbox-selected-pressed-state-layer-color, var(--md-sys-color-on-surface, #1d1b20));--md-ripple-pressed-opacity: var(--md-checkbox-selected-pressed-state-layer-opacity, 0.12)}.icon{fill:var(--md-checkbox-selected-icon-color, var(--md-sys-color-on-primary, #fff));height:var(--md-checkbox-icon-size, 18px);width:var(--md-checkbox-icon-size, 18px)}.mark.short{height:2px;transition-property:transform,height;width:2px}.mark.long{height:2px;transition-property:transform,width;width:10px}.mark{animation-duration:150ms;animation-timing-function:cubic-bezier(0.3, 0, 0.8, 0.15);transition-duration:150ms;transition-timing-function:cubic-bezier(0.3, 0, 0.8, 0.15)}.selected .mark{animation-duration:350ms;animation-timing-function:cubic-bezier(0.05, 0.7, 0.1, 1);transition-duration:350ms;transition-timing-function:cubic-bezier(0.05, 0.7, 0.1, 1)}.checked .mark,.prev-checked.unselected .mark{transform:scaleY(-1) translate(7px, -14px) rotate(45deg)}.checked .mark.short,.prev-checked.unselected .mark.short{height:5.6568542495px}.checked .mark.long,.prev-checked.unselected .mark.long{width:11.313708499px}.indeterminate .mark,.prev-indeterminate.unselected .mark{transform:scaleY(-1) translate(4px, -10px) rotate(0deg)}.prev-unselected .mark{transition-property:none}.prev-unselected.checked .mark.long{animation-name:prev-unselected-to-checked}@keyframes prev-unselected-to-checked{from{width:0}}:where(:hover) .outline{border-color:var(--md-checkbox-hover-outline-color, var(--md-sys-color-on-surface, #1d1b20));border-width:var(--md-checkbox-hover-outline-width, 2px)}:where(:hover) .background{background:var(--md-checkbox-selected-hover-container-color, var(--md-sys-color-primary, #6750a4))}:where(:hover) .icon{fill:var(--md-checkbox-selected-hover-icon-color, var(--md-sys-color-on-primary, #fff))}:where(:focus-within) .outline{border-color:var(--md-checkbox-focus-outline-color, var(--md-sys-color-on-surface, #1d1b20));border-width:var(--md-checkbox-focus-outline-width, 2px)}:where(:focus-within) .background{background:var(--md-checkbox-selected-focus-container-color, var(--md-sys-color-primary, #6750a4))}:where(:focus-within) .icon{fill:var(--md-checkbox-selected-focus-icon-color, var(--md-sys-color-on-primary, #fff))}:where(:active) .outline{border-color:var(--md-checkbox-pressed-outline-color, var(--md-sys-color-on-surface, #1d1b20));border-width:var(--md-checkbox-pressed-outline-width, 2px)}:where(:active) .background{background:var(--md-checkbox-selected-pressed-container-color, var(--md-sys-color-primary, #6750a4))}:where(:active) .icon{fill:var(--md-checkbox-selected-pressed-icon-color, var(--md-sys-color-on-primary, #fff))}:where(.disabled,.prev-disabled) :is(.background,.icon,.mark){animation-duration:0s;transition-duration:0s}:where(.disabled) .outline{border-color:var(--md-checkbox-disabled-outline-color, var(--md-sys-color-on-surface, #1d1b20));border-width:var(--md-checkbox-disabled-outline-width, 2px);opacity:var(--md-checkbox-disabled-container-opacity, 0.38)}:where(.selected.disabled) .outline{visibility:hidden}:where(.selected.disabled) .background{background:var(--md-checkbox-selected-disabled-container-color, var(--md-sys-color-on-surface, #1d1b20));opacity:var(--md-checkbox-selected-disabled-container-opacity, 0.38)}:where(.disabled) .icon{fill:var(--md-checkbox-selected-disabled-icon-color, var(--md-sys-color-surface, #fef7ff))}@media(forced-colors: active){.background{background-color:CanvasText}.selected.disabled .background{background-color:GrayText;opacity:1}.outline{border-color:CanvasText}.disabled .outline{border-color:GrayText;opacity:1}.icon{fill:Canvas}}
`;

class EwCheckbox extends Checkbox {}
EwCheckbox.styles = [styles$c];
customElements.define("ew-checkbox", EwCheckbox);

class ColoredConsole {
  constructor(targetElement) {
    this.targetElement = targetElement;
    this.state = {
      bold: false,
      italic: false,
      underline: false,
      strikethrough: false,
      foregroundColor: null,
      backgroundColor: null,
      carriageReturn: false,
      lines: [],
      secret: false
    };
  }
  logs() {
    return this.targetElement.innerText;
  }
  processLine(line) {
    // @ts-expect-error
    const re = /(?:\033|\\033)(?:\[(.*?)[@-~]|\].*?(?:\007|\033\\))/g;
    let i = 0;
    const lineSpan = document.createElement("span");
    lineSpan.classList.add("line");
    const addSpan = content => {
      if (content === "") return;
      const span = document.createElement("span");
      if (this.state.bold) span.classList.add("log-bold");
      if (this.state.italic) span.classList.add("log-italic");
      if (this.state.underline) span.classList.add("log-underline");
      if (this.state.strikethrough) span.classList.add("log-strikethrough");
      if (this.state.secret) span.classList.add("log-secret");
      if (this.state.foregroundColor !== null) span.classList.add(`log-fg-${this.state.foregroundColor}`);
      if (this.state.backgroundColor !== null) span.classList.add(`log-bg-${this.state.backgroundColor}`);
      span.appendChild(document.createTextNode(content));
      lineSpan.appendChild(span);
      if (this.state.secret) {
        const redacted = document.createElement("span");
        redacted.classList.add("log-secret-redacted");
        redacted.appendChild(document.createTextNode("[redacted]"));
        lineSpan.appendChild(redacted);
      }
    };
    while (true) {
      const match = re.exec(line);
      if (match === null) break;
      const j = match.index;
      addSpan(line.substring(i, j));
      i = j + match[0].length;
      if (match[1] === undefined) continue;
      for (const colorCode of match[1].split(";")) {
        switch (parseInt(colorCode)) {
          case 0:
            // reset
            this.state.bold = false;
            this.state.italic = false;
            this.state.underline = false;
            this.state.strikethrough = false;
            this.state.foregroundColor = null;
            this.state.backgroundColor = null;
            this.state.secret = false;
            break;
          case 1:
            this.state.bold = true;
            break;
          case 3:
            this.state.italic = true;
            break;
          case 4:
            this.state.underline = true;
            break;
          case 5:
            this.state.secret = true;
            break;
          case 6:
            this.state.secret = false;
            break;
          case 9:
            this.state.strikethrough = true;
            break;
          case 22:
            this.state.bold = false;
            break;
          case 23:
            this.state.italic = false;
            break;
          case 24:
            this.state.underline = false;
            break;
          case 29:
            this.state.strikethrough = false;
            break;
          case 30:
            this.state.foregroundColor = "black";
            break;
          case 31:
            this.state.foregroundColor = "red";
            break;
          case 32:
            this.state.foregroundColor = "green";
            break;
          case 33:
            this.state.foregroundColor = "yellow";
            break;
          case 34:
            this.state.foregroundColor = "blue";
            break;
          case 35:
            this.state.foregroundColor = "magenta";
            break;
          case 36:
            this.state.foregroundColor = "cyan";
            break;
          case 37:
            this.state.foregroundColor = "white";
            break;
          case 39:
            this.state.foregroundColor = null;
            break;
          case 41:
            this.state.backgroundColor = "red";
            break;
          case 42:
            this.state.backgroundColor = "green";
            break;
          case 43:
            this.state.backgroundColor = "yellow";
            break;
          case 44:
            this.state.backgroundColor = "blue";
            break;
          case 45:
            this.state.backgroundColor = "magenta";
            break;
          case 46:
            this.state.backgroundColor = "cyan";
            break;
          case 47:
            this.state.backgroundColor = "white";
            break;
          case 40:
          case 49:
            this.state.backgroundColor = null;
            break;
        }
      }
    }
    addSpan(line.substring(i));
    return lineSpan;
  }
  processLines() {
    const atBottom = this.targetElement.scrollTop > this.targetElement.scrollHeight - this.targetElement.offsetHeight - 50;
    const prevCarriageReturn = this.state.carriageReturn;
    const fragment = document.createDocumentFragment();
    if (this.state.lines.length == 0) {
      return;
    }
    for (const line of this.state.lines) {
      if (this.state.carriageReturn && line !== "\n") {
        if (fragment.childElementCount) {
          fragment.removeChild(fragment.lastChild);
        }
      }
      fragment.appendChild(this.processLine(line));
      this.state.carriageReturn = line.includes("\r");
    }
    if (prevCarriageReturn && this.state.lines[0] !== "\n") {
      this.targetElement.replaceChild(fragment, this.targetElement.lastChild);
    } else {
      this.targetElement.appendChild(fragment);
    }
    this.state.lines = [];
    // Keep scroll at bottom
    if (atBottom) {
      this.targetElement.scrollTop = this.targetElement.scrollHeight;
    }
  }
  addLine(line) {
    // Processing of lines is deferred for performance reasons
    if (this.state.lines.length == 0) {
      setTimeout(() => this.processLines(), 0);
    }
    this.state.lines.push(line);
  }
}
const coloredConsoleStyles = `
  .log {
    flex: 1;
    background-color: #1c1c1c;
    font-family: "SFMono-Regular", Consolas, "Liberation Mono", Menlo, Courier,
      monospace;
    font-size: 12px;
    padding: 16px;
    overflow: auto;
    line-height: 1.45;
    border-radius: 3px;
    white-space: pre-wrap;
    overflow-wrap: break-word;
    color: #ddd;
  }

  .log-bold {
    font-weight: bold;
  }
  .log-italic {
    font-style: italic;
  }
  .log-underline {
    text-decoration: underline;
  }
  .log-strikethrough {
    text-decoration: line-through;
  }
  .log-underline.log-strikethrough {
    text-decoration: underline line-through;
  }
  .log-secret {
    -webkit-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
  }
  .log-secret-redacted {
    opacity: 0;
    width: 1px;
    font-size: 1px;
  }
  .log-fg-black {
    color: rgb(128, 128, 128);
  }
  .log-fg-red {
    color: rgb(255, 0, 0);
  }
  .log-fg-green {
    color: rgb(0, 255, 0);
  }
  .log-fg-yellow {
    color: rgb(255, 255, 0);
  }
  .log-fg-blue {
    color: rgb(0, 0, 255);
  }
  .log-fg-magenta {
    color: rgb(255, 0, 255);
  }
  .log-fg-cyan {
    color: rgb(0, 255, 255);
  }
  .log-fg-white {
    color: rgb(187, 187, 187);
  }
  .log-bg-black {
    background-color: rgb(0, 0, 0);
  }
  .log-bg-red {
    background-color: rgb(255, 0, 0);
  }
  .log-bg-green {
    background-color: rgb(0, 255, 0);
  }
  .log-bg-yellow {
    background-color: rgb(255, 255, 0);
  }
  .log-bg-blue {
    background-color: rgb(0, 0, 255);
  }
  .log-bg-magenta {
    background-color: rgb(255, 0, 255);
  }
  .log-bg-cyan {
    background-color: rgb(0, 255, 255);
  }
  .log-bg-white {
    background-color: rgb(255, 255, 255);
  }
`;

const sleep$2 = time => new Promise(resolve => setTimeout(resolve, time));

class LineBreakTransformer {
  constructor() {
    this.chunks = "";
  }
  transform(chunk, controller) {
    // Append new chunks to existing chunks.
    this.chunks += chunk;
    // For each line breaks in chunks, send the parsed lines out.
    const lines = this.chunks.split(/\r?\n/);
    this.chunks = lines.pop();
    lines.forEach(line => controller.enqueue(line + "\r\n"));
  }
  flush(controller) {
    // When the stream is closed, flush any remaining chunks out.
    controller.enqueue(this.chunks);
  }
}

class TimestampTransformer {
  transform(chunk, controller) {
    const date = new Date();
    const h = date.getHours().toString().padStart(2, "0");
    const m = date.getMinutes().toString().padStart(2, "0");
    const s = date.getSeconds().toString().padStart(2, "0");
    controller.enqueue(`[${h}:${m}:${s}]${chunk}`);
  }
}

class EwtConsole extends HTMLElement {
  constructor() {
    super(...arguments);
    this.allowInput = true;
  }
  logs() {
    var _a;
    return ((_a = this._console) === null || _a === void 0 ? void 0 : _a.logs()) || "";
  }
  connectedCallback() {
    if (this._console) {
      return;
    }
    const shadowRoot = this.attachShadow({
      mode: "open"
    });
    shadowRoot.innerHTML = `
      <style>
        :host, input {
          background-color: #1c1c1c;
          color: #ddd;
          font-family: "SFMono-Regular", Consolas, "Liberation Mono", Menlo, Courier,
            monospace;
          line-height: 1.45;
          display: flex;
          flex-direction: column;
        }
        form {
          display: flex;
          align-items: center;
          padding: 0 8px 0 16px;
        }
        input {
          flex: 1;
          padding: 4px;
          margin: 0 8px;
          border: 0;
          outline: none;
        }
        ${coloredConsoleStyles}
      </style>
      <div class="log"></div>
      ${this.allowInput ? `<form>
                >
                <input autofocus>
              </form>
            ` : ""}
    `;
    this._console = new ColoredConsole(this.shadowRoot.querySelector("div"));
    if (this.allowInput) {
      const input = this.shadowRoot.querySelector("input");
      this.addEventListener("click", () => {
        var _a;
        // Only focus input if user didn't select some text
        if (((_a = getSelection()) === null || _a === void 0 ? void 0 : _a.toString()) === "") {
          input.focus();
        }
      });
      input.addEventListener("keydown", ev => {
        if (ev.key === "Enter") {
          ev.preventDefault();
          ev.stopPropagation();
          this._sendCommand();
        }
      });
    }
    const abortController = new AbortController();
    const connection = this._connect(abortController.signal);
    this._cancelConnection = () => {
      abortController.abort();
      return connection;
    };
  }
  async _connect(abortSignal) {
    this.logger.debug("Starting console read loop");
    try {
      await this.port.readable.pipeThrough(new TextDecoderStream(), {
        signal: abortSignal
      }).pipeThrough(new TransformStream(new LineBreakTransformer())).pipeThrough(new TransformStream(new TimestampTransformer())).pipeTo(new WritableStream({
        write: chunk => {
          this._console.addLine(chunk.replace("\r", ""));
        }
      }));
      if (!abortSignal.aborted) {
        this._console.addLine("");
        this._console.addLine("");
        this._console.addLine("Terminal disconnected");
      }
    } catch (e) {
      this._console.addLine("");
      this._console.addLine("");
      this._console.addLine(`Terminal disconnected: ${e}`);
    } finally {
      await sleep$2(100);
      this.logger.debug("Finished console read loop");
    }
  }
  async _sendCommand() {
    const input = this.shadowRoot.querySelector("input");
    const command = input.value;
    const encoder = new TextEncoder();
    const writer = this.port.writable.getWriter();
    await writer.write(encoder.encode(command + "\r\n"));
    this._console.addLine(`> ${command}\r\n`);
    input.value = "";
    input.focus();
    try {
      writer.releaseLock();
    } catch (err) {
      console.error("Ignoring release lock error", err);
    }
  }
  async disconnect() {
    if (this._cancelConnection) {
      await this._cancelConnection();
      this._cancelConnection = undefined;
    }
  }
  async reset() {
    this.logger.debug("Triggering reset");
    await this.port.setSignals({
      dataTerminalReady: false,
      requestToSend: true
    });
    await sleep$2(250);
    await this.port.setSignals({
      dataTerminalReady: false,
      requestToSend: false
    });
    await sleep$2(250);
    await new Promise(resolve => setTimeout(resolve, 1000));
  }
}
customElements.define("ewt-console", EwtConsole);

/**
 * @license
 * Copyright 2022 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * Returns `true` if the given element is in a right-to-left direction.
 *
 * @param el Element to determine direction from
 * @param shouldCheck Optional. If `false`, return `false` without checking
 *     direction. Determining the direction of `el` is somewhat expensive, so
 *     this parameter can be used as a conditional guard. Defaults to `true`.
 */
function isRtl(el, shouldCheck = true) {
  return shouldCheck && getComputedStyle(el).getPropertyValue('direction').trim() === 'rtl';
}

/**
 * @license
 * Copyright 2018 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Separate variable needed for closure.
const iconButtonBaseClass = mixinDelegatesAria(mixinElementInternals(i$2));
/**
 * A button for rendering icons.
 *
 * @fires input {InputEvent} Dispatched when a toggle button toggles --bubbles
 * --composed
 * @fires change {Event} Dispatched when a toggle button toggles --bubbles
 */
class IconButton extends iconButtonBaseClass {
  get name() {
    return this.getAttribute('name') ?? '';
  }
  set name(name) {
    this.setAttribute('name', name);
  }
  /**
   * The associated form element with which this element's value will submit.
   */
  get form() {
    return this[internals].form;
  }
  /**
   * The labels this element is associated with.
   */
  get labels() {
    return this[internals].labels;
  }
  constructor() {
    super();
    /**
     * Disables the icon button and makes it non-interactive.
     */
    this.disabled = false;
    /**
     * "Soft-disables" the icon button (disabled but still focusable).
     *
     * Use this when an icon button needs increased visibility when disabled. See
     * https://www.w3.org/WAI/ARIA/apg/practices/keyboard-interface/#kbd_disabled_controls
     * for more guidance on when this is needed.
     */
    this.softDisabled = false;
    /**
     * Flips the icon if it is in an RTL context at startup.
     */
    this.flipIconInRtl = false;
    /**
     * Sets the underlying `HTMLAnchorElement`'s `href` resource attribute.
     */
    this.href = '';
    /**
     * The filename to use when downloading the linked resource.
     * If not specified, the browser will determine a filename.
     * This is only applicable when the icon button is used as a link (`href` is set).
     */
    this.download = '';
    /**
     * Sets the underlying `HTMLAnchorElement`'s `target` attribute.
     */
    this.target = '';
    /**
     * The `aria-label` of the button when the button is toggleable and selected.
     */
    this.ariaLabelSelected = '';
    /**
     * When true, the button will toggle between selected and unselected
     * states
     */
    this.toggle = false;
    /**
     * Sets the selected state. When false, displays the default icon. When true,
     * displays the selected icon, or the default icon If no `slot="selected"`
     * icon is provided.
     */
    this.selected = false;
    /**
     * The default behavior of the button. May be "button", "reset", or "submit"
     * (default).
     */
    this.type = 'submit';
    /**
     * The value added to a form with the button's name when the button submits a
     * form.
     */
    this.value = '';
    this.flipIcon = isRtl(this, this.flipIconInRtl);
    {
      this.addEventListener('click', this.handleClick.bind(this));
    }
  }
  willUpdate() {
    // Link buttons cannot be disabled or soft-disabled.
    if (this.href) {
      this.disabled = false;
      this.softDisabled = false;
    }
  }
  render() {
    const tag = this.href ? i$1`div` : i$1`button`;
    // Needed for closure conformance
    const {
      ariaLabel,
      ariaHasPopup,
      ariaExpanded
    } = this;
    const hasToggledAriaLabel = ariaLabel && this.ariaLabelSelected;
    const ariaPressedValue = !this.toggle ? E : this.selected;
    let ariaLabelValue = E;
    if (!this.href) {
      ariaLabelValue = hasToggledAriaLabel && this.selected ? this.ariaLabelSelected : ariaLabel;
    }
    return u$1`<${tag}
        class="icon-button ${e$3(this.getRenderClasses())}"
        id="button"
        aria-label="${ariaLabelValue || E}"
        aria-haspopup="${!this.href && ariaHasPopup || E}"
        aria-expanded="${!this.href && ariaExpanded || E}"
        aria-pressed="${ariaPressedValue}"
        aria-disabled=${!this.href && this.softDisabled || E}
        ?disabled="${!this.href && this.disabled}"
        @click="${this.handleClickOnChild}">
        ${this.renderFocusRing()}
        ${this.renderRipple()}
        ${!this.selected ? this.renderIcon() : E}
        ${this.selected ? this.renderSelectedIcon() : E}
        ${this.href ? this.renderLink() : this.renderTouchTarget()}
  </${tag}>`;
  }
  renderLink() {
    // Needed for closure conformance
    const {
      ariaLabel
    } = this;
    return x`
      <a
        class="link"
        id="link"
        href="${this.href}"
        download="${this.download || E}"
        target="${this.target || E}"
        aria-label="${ariaLabel || E}">
        ${this.renderTouchTarget()}
      </a>
    `;
  }
  getRenderClasses() {
    return {
      'flip-icon': this.flipIcon,
      'selected': this.toggle && this.selected
    };
  }
  renderIcon() {
    return x`<span class="icon"><slot></slot></span>`;
  }
  renderSelectedIcon() {
    // Use default slot as fallback to not require specifying multiple icons
    return x`<span class="icon icon--selected"
      ><slot name="selected"><slot></slot></slot
    ></span>`;
  }
  renderTouchTarget() {
    return x`<span class="touch"></span>`;
  }
  renderFocusRing() {
    // TODO(b/310046938): use the same id for both elements
    return x`<md-focus-ring
      part="focus-ring"
      for=${this.href ? 'link' : 'button'}></md-focus-ring>`;
  }
  renderRipple() {
    const isRippleDisabled = !this.href && (this.disabled || this.softDisabled);
    // TODO(b/310046938): use the same id for both elements
    return x`<md-ripple
      for=${this.href ? 'link' : E}
      ?disabled="${isRippleDisabled}"></md-ripple>`;
  }
  connectedCallback() {
    this.flipIcon = isRtl(this, this.flipIconInRtl);
    super.connectedCallback();
  }
  /** Handles a click on this element. */
  handleClick(event) {
    // If the icon button is soft-disabled, we need to explicitly prevent the
    // click from propagating to other event listeners as well as prevent the
    // default action.
    if (!this.href && this.softDisabled) {
      event.stopImmediatePropagation();
      event.preventDefault();
      return;
    }
  }
  /**
   * Handles a click on the child <div> or <button> element within this
   * element's shadow DOM.
   */
  async handleClickOnChild(event) {
    // Allow the event to propagate
    await 0;
    if (!this.toggle || this.disabled || this.softDisabled || event.defaultPrevented) {
      return;
    }
    this.selected = !this.selected;
    this.dispatchEvent(new InputEvent('input', {
      bubbles: true,
      composed: true
    }));
    // Bubbles but does not compose to mimic native browser <input> & <select>
    // Additionally, native change event is not an InputEvent.
    this.dispatchEvent(new Event('change', {
      bubbles: true
    }));
  }
}
(() => {
  setupFormSubmitter(IconButton);
})();
/** @nocollapse */
IconButton.formAssociated = true;
/** @nocollapse */
IconButton.shadowRootOptions = {
  mode: 'open',
  delegatesFocus: true
};
__decorate([n$3({
  type: Boolean,
  reflect: true
})], IconButton.prototype, "disabled", void 0);
__decorate([n$3({
  type: Boolean,
  attribute: 'soft-disabled',
  reflect: true
})], IconButton.prototype, "softDisabled", void 0);
__decorate([n$3({
  type: Boolean,
  attribute: 'flip-icon-in-rtl'
})], IconButton.prototype, "flipIconInRtl", void 0);
__decorate([n$3()], IconButton.prototype, "href", void 0);
__decorate([n$3()], IconButton.prototype, "download", void 0);
__decorate([n$3()], IconButton.prototype, "target", void 0);
__decorate([n$3({
  attribute: 'aria-label-selected'
})], IconButton.prototype, "ariaLabelSelected", void 0);
__decorate([n$3({
  type: Boolean
})], IconButton.prototype, "toggle", void 0);
__decorate([n$3({
  type: Boolean,
  reflect: true
})], IconButton.prototype, "selected", void 0);
__decorate([n$3()], IconButton.prototype, "type", void 0);
__decorate([n$3({
  reflect: true
})], IconButton.prototype, "value", void 0);
__decorate([r$1()], IconButton.prototype, "flipIcon", void 0);

/**
 * @license
 * Copyright 2024 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Generated stylesheet for ./iconbutton/internal/shared-styles.css.
const styles$b = i$3`:host{display:inline-flex;outline:none;-webkit-tap-highlight-color:rgba(0,0,0,0);height:var(--_container-height);width:var(--_container-width);justify-content:center}:host([touch-target=wrapper]){margin:max(0px,(48px - var(--_container-height))/2) max(0px,(48px - var(--_container-width))/2)}md-focus-ring{--md-focus-ring-shape-start-start: var(--_container-shape-start-start);--md-focus-ring-shape-start-end: var(--_container-shape-start-end);--md-focus-ring-shape-end-end: var(--_container-shape-end-end);--md-focus-ring-shape-end-start: var(--_container-shape-end-start)}:host(:is([disabled],[soft-disabled])){pointer-events:none}.icon-button{place-items:center;background:none;border:none;box-sizing:border-box;cursor:pointer;display:flex;place-content:center;outline:none;padding:0;position:relative;text-decoration:none;user-select:none;z-index:0;flex:1;border-start-start-radius:var(--_container-shape-start-start);border-start-end-radius:var(--_container-shape-start-end);border-end-start-radius:var(--_container-shape-end-start);border-end-end-radius:var(--_container-shape-end-end)}.icon ::slotted(*){font-size:var(--_icon-size);height:var(--_icon-size);width:var(--_icon-size);font-weight:inherit}md-ripple{z-index:-1;border-start-start-radius:var(--_container-shape-start-start);border-start-end-radius:var(--_container-shape-start-end);border-end-start-radius:var(--_container-shape-end-start);border-end-end-radius:var(--_container-shape-end-end)}.flip-icon .icon{transform:scaleX(-1)}.icon{display:inline-flex}.link{display:grid;height:100%;outline:none;place-items:center;position:absolute;width:100%}.touch{position:absolute;height:max(48px,100%);width:max(48px,100%)}:host([touch-target=none]) .touch{display:none}@media(forced-colors: active){:host(:is([disabled],[soft-disabled])){--_disabled-icon-color: GrayText;--_disabled-icon-opacity: 1}}
`;

/**
 * @license
 * Copyright 2024 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Generated stylesheet for ./iconbutton/internal/standard-styles.css.
const styles$a = i$3`:host{--_disabled-icon-color: var(--md-icon-button-disabled-icon-color, var(--md-sys-color-on-surface, #1d1b20));--_disabled-icon-opacity: var(--md-icon-button-disabled-icon-opacity, 0.38);--_icon-size: var(--md-icon-button-icon-size, 24px);--_selected-focus-icon-color: var(--md-icon-button-selected-focus-icon-color, var(--md-sys-color-primary, #6750a4));--_selected-hover-icon-color: var(--md-icon-button-selected-hover-icon-color, var(--md-sys-color-primary, #6750a4));--_selected-hover-state-layer-color: var(--md-icon-button-selected-hover-state-layer-color, var(--md-sys-color-primary, #6750a4));--_selected-hover-state-layer-opacity: var(--md-icon-button-selected-hover-state-layer-opacity, 0.08);--_selected-icon-color: var(--md-icon-button-selected-icon-color, var(--md-sys-color-primary, #6750a4));--_selected-pressed-icon-color: var(--md-icon-button-selected-pressed-icon-color, var(--md-sys-color-primary, #6750a4));--_selected-pressed-state-layer-color: var(--md-icon-button-selected-pressed-state-layer-color, var(--md-sys-color-primary, #6750a4));--_selected-pressed-state-layer-opacity: var(--md-icon-button-selected-pressed-state-layer-opacity, 0.12);--_state-layer-height: var(--md-icon-button-state-layer-height, 40px);--_state-layer-shape: var(--md-icon-button-state-layer-shape, var(--md-sys-shape-corner-full, 9999px));--_state-layer-width: var(--md-icon-button-state-layer-width, 40px);--_focus-icon-color: var(--md-icon-button-focus-icon-color, var(--md-sys-color-on-surface-variant, #49454f));--_hover-icon-color: var(--md-icon-button-hover-icon-color, var(--md-sys-color-on-surface-variant, #49454f));--_hover-state-layer-color: var(--md-icon-button-hover-state-layer-color, var(--md-sys-color-on-surface-variant, #49454f));--_hover-state-layer-opacity: var(--md-icon-button-hover-state-layer-opacity, 0.08);--_icon-color: var(--md-icon-button-icon-color, var(--md-sys-color-on-surface-variant, #49454f));--_pressed-icon-color: var(--md-icon-button-pressed-icon-color, var(--md-sys-color-on-surface-variant, #49454f));--_pressed-state-layer-color: var(--md-icon-button-pressed-state-layer-color, var(--md-sys-color-on-surface-variant, #49454f));--_pressed-state-layer-opacity: var(--md-icon-button-pressed-state-layer-opacity, 0.12);--_container-shape-start-start: 0;--_container-shape-start-end: 0;--_container-shape-end-end: 0;--_container-shape-end-start: 0;--_container-height: 0;--_container-width: 0;height:var(--_state-layer-height);width:var(--_state-layer-width)}:host([touch-target=wrapper]){margin:max(0px,(48px - var(--_state-layer-height))/2) max(0px,(48px - var(--_state-layer-width))/2)}md-focus-ring{--md-focus-ring-shape-start-start: var(--_state-layer-shape);--md-focus-ring-shape-start-end: var(--_state-layer-shape);--md-focus-ring-shape-end-end: var(--_state-layer-shape);--md-focus-ring-shape-end-start: var(--_state-layer-shape)}.standard{background-color:rgba(0,0,0,0);color:var(--_icon-color);--md-ripple-hover-color: var(--_hover-state-layer-color);--md-ripple-hover-opacity: var(--_hover-state-layer-opacity);--md-ripple-pressed-color: var(--_pressed-state-layer-color);--md-ripple-pressed-opacity: var(--_pressed-state-layer-opacity)}.standard:hover{color:var(--_hover-icon-color)}.standard:focus{color:var(--_focus-icon-color)}.standard:active{color:var(--_pressed-icon-color)}.standard:is(:disabled,[aria-disabled=true]){color:var(--_disabled-icon-color)}md-ripple{border-radius:var(--_state-layer-shape)}.standard:is(:disabled,[aria-disabled=true]){opacity:var(--_disabled-icon-opacity)}.selected{--md-ripple-hover-color: var(--_selected-hover-state-layer-color);--md-ripple-hover-opacity: var(--_selected-hover-state-layer-opacity);--md-ripple-pressed-color: var(--_selected-pressed-state-layer-color);--md-ripple-pressed-opacity: var(--_selected-pressed-state-layer-opacity)}.selected:not(:disabled,[aria-disabled=true]){color:var(--_selected-icon-color)}.selected:not(:disabled,[aria-disabled=true]):hover{color:var(--_selected-hover-icon-color)}.selected:not(:disabled,[aria-disabled=true]):focus{color:var(--_selected-focus-icon-color)}.selected:not(:disabled,[aria-disabled=true]):active{color:var(--_selected-pressed-icon-color)}
`;

class EwIconButton extends IconButton {}
EwIconButton.styles = [styles$b, styles$a];
customElements.define("ew-icon-button", EwIconButton);

/**
 * @license
 * Copyright 2024 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Generated stylesheet for ./textfield/internal/filled-styles.css.
const styles$9 = i$3`:host{--_active-indicator-color: var(--md-filled-text-field-active-indicator-color, var(--md-sys-color-on-surface-variant, #49454f));--_active-indicator-height: var(--md-filled-text-field-active-indicator-height, 1px);--_caret-color: var(--md-filled-text-field-caret-color, var(--md-sys-color-primary, #6750a4));--_container-color: var(--md-filled-text-field-container-color, var(--md-sys-color-surface-container-highest, #e6e0e9));--_disabled-active-indicator-color: var(--md-filled-text-field-disabled-active-indicator-color, var(--md-sys-color-on-surface, #1d1b20));--_disabled-active-indicator-height: var(--md-filled-text-field-disabled-active-indicator-height, 1px);--_disabled-active-indicator-opacity: var(--md-filled-text-field-disabled-active-indicator-opacity, 0.38);--_disabled-container-color: var(--md-filled-text-field-disabled-container-color, var(--md-sys-color-on-surface, #1d1b20));--_disabled-container-opacity: var(--md-filled-text-field-disabled-container-opacity, 0.04);--_disabled-input-text-color: var(--md-filled-text-field-disabled-input-text-color, var(--md-sys-color-on-surface, #1d1b20));--_disabled-input-text-opacity: var(--md-filled-text-field-disabled-input-text-opacity, 0.38);--_disabled-label-text-color: var(--md-filled-text-field-disabled-label-text-color, var(--md-sys-color-on-surface, #1d1b20));--_disabled-label-text-opacity: var(--md-filled-text-field-disabled-label-text-opacity, 0.38);--_disabled-leading-icon-color: var(--md-filled-text-field-disabled-leading-icon-color, var(--md-sys-color-on-surface, #1d1b20));--_disabled-leading-icon-opacity: var(--md-filled-text-field-disabled-leading-icon-opacity, 0.38);--_disabled-supporting-text-color: var(--md-filled-text-field-disabled-supporting-text-color, var(--md-sys-color-on-surface, #1d1b20));--_disabled-supporting-text-opacity: var(--md-filled-text-field-disabled-supporting-text-opacity, 0.38);--_disabled-trailing-icon-color: var(--md-filled-text-field-disabled-trailing-icon-color, var(--md-sys-color-on-surface, #1d1b20));--_disabled-trailing-icon-opacity: var(--md-filled-text-field-disabled-trailing-icon-opacity, 0.38);--_error-active-indicator-color: var(--md-filled-text-field-error-active-indicator-color, var(--md-sys-color-error, #b3261e));--_error-focus-active-indicator-color: var(--md-filled-text-field-error-focus-active-indicator-color, var(--md-sys-color-error, #b3261e));--_error-focus-caret-color: var(--md-filled-text-field-error-focus-caret-color, var(--md-sys-color-error, #b3261e));--_error-focus-input-text-color: var(--md-filled-text-field-error-focus-input-text-color, var(--md-sys-color-on-surface, #1d1b20));--_error-focus-label-text-color: var(--md-filled-text-field-error-focus-label-text-color, var(--md-sys-color-error, #b3261e));--_error-focus-leading-icon-color: var(--md-filled-text-field-error-focus-leading-icon-color, var(--md-sys-color-on-surface-variant, #49454f));--_error-focus-supporting-text-color: var(--md-filled-text-field-error-focus-supporting-text-color, var(--md-sys-color-error, #b3261e));--_error-focus-trailing-icon-color: var(--md-filled-text-field-error-focus-trailing-icon-color, var(--md-sys-color-error, #b3261e));--_error-hover-active-indicator-color: var(--md-filled-text-field-error-hover-active-indicator-color, var(--md-sys-color-on-error-container, #410e0b));--_error-hover-input-text-color: var(--md-filled-text-field-error-hover-input-text-color, var(--md-sys-color-on-surface, #1d1b20));--_error-hover-label-text-color: var(--md-filled-text-field-error-hover-label-text-color, var(--md-sys-color-on-error-container, #410e0b));--_error-hover-leading-icon-color: var(--md-filled-text-field-error-hover-leading-icon-color, var(--md-sys-color-on-surface-variant, #49454f));--_error-hover-state-layer-color: var(--md-filled-text-field-error-hover-state-layer-color, var(--md-sys-color-on-surface, #1d1b20));--_error-hover-state-layer-opacity: var(--md-filled-text-field-error-hover-state-layer-opacity, 0.08);--_error-hover-supporting-text-color: var(--md-filled-text-field-error-hover-supporting-text-color, var(--md-sys-color-error, #b3261e));--_error-hover-trailing-icon-color: var(--md-filled-text-field-error-hover-trailing-icon-color, var(--md-sys-color-on-error-container, #410e0b));--_error-input-text-color: var(--md-filled-text-field-error-input-text-color, var(--md-sys-color-on-surface, #1d1b20));--_error-label-text-color: var(--md-filled-text-field-error-label-text-color, var(--md-sys-color-error, #b3261e));--_error-leading-icon-color: var(--md-filled-text-field-error-leading-icon-color, var(--md-sys-color-on-surface-variant, #49454f));--_error-supporting-text-color: var(--md-filled-text-field-error-supporting-text-color, var(--md-sys-color-error, #b3261e));--_error-trailing-icon-color: var(--md-filled-text-field-error-trailing-icon-color, var(--md-sys-color-error, #b3261e));--_focus-active-indicator-color: var(--md-filled-text-field-focus-active-indicator-color, var(--md-sys-color-primary, #6750a4));--_focus-active-indicator-height: var(--md-filled-text-field-focus-active-indicator-height, 3px);--_focus-input-text-color: var(--md-filled-text-field-focus-input-text-color, var(--md-sys-color-on-surface, #1d1b20));--_focus-label-text-color: var(--md-filled-text-field-focus-label-text-color, var(--md-sys-color-primary, #6750a4));--_focus-leading-icon-color: var(--md-filled-text-field-focus-leading-icon-color, var(--md-sys-color-on-surface-variant, #49454f));--_focus-supporting-text-color: var(--md-filled-text-field-focus-supporting-text-color, var(--md-sys-color-on-surface-variant, #49454f));--_focus-trailing-icon-color: var(--md-filled-text-field-focus-trailing-icon-color, var(--md-sys-color-on-surface-variant, #49454f));--_hover-active-indicator-color: var(--md-filled-text-field-hover-active-indicator-color, var(--md-sys-color-on-surface, #1d1b20));--_hover-active-indicator-height: var(--md-filled-text-field-hover-active-indicator-height, 1px);--_hover-input-text-color: var(--md-filled-text-field-hover-input-text-color, var(--md-sys-color-on-surface, #1d1b20));--_hover-label-text-color: var(--md-filled-text-field-hover-label-text-color, var(--md-sys-color-on-surface-variant, #49454f));--_hover-leading-icon-color: var(--md-filled-text-field-hover-leading-icon-color, var(--md-sys-color-on-surface-variant, #49454f));--_hover-state-layer-color: var(--md-filled-text-field-hover-state-layer-color, var(--md-sys-color-on-surface, #1d1b20));--_hover-state-layer-opacity: var(--md-filled-text-field-hover-state-layer-opacity, 0.08);--_hover-supporting-text-color: var(--md-filled-text-field-hover-supporting-text-color, var(--md-sys-color-on-surface-variant, #49454f));--_hover-trailing-icon-color: var(--md-filled-text-field-hover-trailing-icon-color, var(--md-sys-color-on-surface-variant, #49454f));--_input-text-color: var(--md-filled-text-field-input-text-color, var(--md-sys-color-on-surface, #1d1b20));--_input-text-font: var(--md-filled-text-field-input-text-font, var(--md-sys-typescale-body-large-font, var(--md-ref-typeface-plain, Roboto)));--_input-text-line-height: var(--md-filled-text-field-input-text-line-height, var(--md-sys-typescale-body-large-line-height, 1.5rem));--_input-text-placeholder-color: var(--md-filled-text-field-input-text-placeholder-color, var(--md-sys-color-on-surface-variant, #49454f));--_input-text-prefix-color: var(--md-filled-text-field-input-text-prefix-color, var(--md-sys-color-on-surface-variant, #49454f));--_input-text-size: var(--md-filled-text-field-input-text-size, var(--md-sys-typescale-body-large-size, 1rem));--_input-text-suffix-color: var(--md-filled-text-field-input-text-suffix-color, var(--md-sys-color-on-surface-variant, #49454f));--_input-text-weight: var(--md-filled-text-field-input-text-weight, var(--md-sys-typescale-body-large-weight, var(--md-ref-typeface-weight-regular, 400)));--_label-text-color: var(--md-filled-text-field-label-text-color, var(--md-sys-color-on-surface-variant, #49454f));--_label-text-font: var(--md-filled-text-field-label-text-font, var(--md-sys-typescale-body-large-font, var(--md-ref-typeface-plain, Roboto)));--_label-text-line-height: var(--md-filled-text-field-label-text-line-height, var(--md-sys-typescale-body-large-line-height, 1.5rem));--_label-text-populated-line-height: var(--md-filled-text-field-label-text-populated-line-height, var(--md-sys-typescale-body-small-line-height, 1rem));--_label-text-populated-size: var(--md-filled-text-field-label-text-populated-size, var(--md-sys-typescale-body-small-size, 0.75rem));--_label-text-size: var(--md-filled-text-field-label-text-size, var(--md-sys-typescale-body-large-size, 1rem));--_label-text-weight: var(--md-filled-text-field-label-text-weight, var(--md-sys-typescale-body-large-weight, var(--md-ref-typeface-weight-regular, 400)));--_leading-icon-color: var(--md-filled-text-field-leading-icon-color, var(--md-sys-color-on-surface-variant, #49454f));--_leading-icon-size: var(--md-filled-text-field-leading-icon-size, 24px);--_supporting-text-color: var(--md-filled-text-field-supporting-text-color, var(--md-sys-color-on-surface-variant, #49454f));--_supporting-text-font: var(--md-filled-text-field-supporting-text-font, var(--md-sys-typescale-body-small-font, var(--md-ref-typeface-plain, Roboto)));--_supporting-text-line-height: var(--md-filled-text-field-supporting-text-line-height, var(--md-sys-typescale-body-small-line-height, 1rem));--_supporting-text-size: var(--md-filled-text-field-supporting-text-size, var(--md-sys-typescale-body-small-size, 0.75rem));--_supporting-text-weight: var(--md-filled-text-field-supporting-text-weight, var(--md-sys-typescale-body-small-weight, var(--md-ref-typeface-weight-regular, 400)));--_trailing-icon-color: var(--md-filled-text-field-trailing-icon-color, var(--md-sys-color-on-surface-variant, #49454f));--_trailing-icon-size: var(--md-filled-text-field-trailing-icon-size, 24px);--_container-shape-start-start: var(--md-filled-text-field-container-shape-start-start, var(--md-filled-text-field-container-shape, var(--md-sys-shape-corner-extra-small, 4px)));--_container-shape-start-end: var(--md-filled-text-field-container-shape-start-end, var(--md-filled-text-field-container-shape, var(--md-sys-shape-corner-extra-small, 4px)));--_container-shape-end-end: var(--md-filled-text-field-container-shape-end-end, var(--md-filled-text-field-container-shape, var(--md-sys-shape-corner-none, 0px)));--_container-shape-end-start: var(--md-filled-text-field-container-shape-end-start, var(--md-filled-text-field-container-shape, var(--md-sys-shape-corner-none, 0px)));--_icon-input-space: var(--md-filled-text-field-icon-input-space, 16px);--_leading-space: var(--md-filled-text-field-leading-space, 16px);--_trailing-space: var(--md-filled-text-field-trailing-space, 16px);--_top-space: var(--md-filled-text-field-top-space, 16px);--_bottom-space: var(--md-filled-text-field-bottom-space, 16px);--_input-text-prefix-trailing-space: var(--md-filled-text-field-input-text-prefix-trailing-space, 2px);--_input-text-suffix-leading-space: var(--md-filled-text-field-input-text-suffix-leading-space, 2px);--_with-label-top-space: var(--md-filled-text-field-with-label-top-space, 8px);--_with-label-bottom-space: var(--md-filled-text-field-with-label-bottom-space, 8px);--_focus-caret-color: var(--md-filled-text-field-focus-caret-color, var(--md-sys-color-primary, #6750a4));--_with-leading-icon-leading-space: var(--md-filled-text-field-with-leading-icon-leading-space, 12px);--_with-trailing-icon-trailing-space: var(--md-filled-text-field-with-trailing-icon-trailing-space, 12px);--md-filled-field-active-indicator-color: var(--_active-indicator-color);--md-filled-field-active-indicator-height: var(--_active-indicator-height);--md-filled-field-bottom-space: var(--_bottom-space);--md-filled-field-container-color: var(--_container-color);--md-filled-field-container-shape-end-end: var(--_container-shape-end-end);--md-filled-field-container-shape-end-start: var(--_container-shape-end-start);--md-filled-field-container-shape-start-end: var(--_container-shape-start-end);--md-filled-field-container-shape-start-start: var(--_container-shape-start-start);--md-filled-field-content-color: var(--_input-text-color);--md-filled-field-content-font: var(--_input-text-font);--md-filled-field-content-line-height: var(--_input-text-line-height);--md-filled-field-content-size: var(--_input-text-size);--md-filled-field-content-space: var(--_icon-input-space);--md-filled-field-content-weight: var(--_input-text-weight);--md-filled-field-disabled-active-indicator-color: var(--_disabled-active-indicator-color);--md-filled-field-disabled-active-indicator-height: var(--_disabled-active-indicator-height);--md-filled-field-disabled-active-indicator-opacity: var(--_disabled-active-indicator-opacity);--md-filled-field-disabled-container-color: var(--_disabled-container-color);--md-filled-field-disabled-container-opacity: var(--_disabled-container-opacity);--md-filled-field-disabled-content-color: var(--_disabled-input-text-color);--md-filled-field-disabled-content-opacity: var(--_disabled-input-text-opacity);--md-filled-field-disabled-label-text-color: var(--_disabled-label-text-color);--md-filled-field-disabled-label-text-opacity: var(--_disabled-label-text-opacity);--md-filled-field-disabled-leading-content-color: var(--_disabled-leading-icon-color);--md-filled-field-disabled-leading-content-opacity: var(--_disabled-leading-icon-opacity);--md-filled-field-disabled-supporting-text-color: var(--_disabled-supporting-text-color);--md-filled-field-disabled-supporting-text-opacity: var(--_disabled-supporting-text-opacity);--md-filled-field-disabled-trailing-content-color: var(--_disabled-trailing-icon-color);--md-filled-field-disabled-trailing-content-opacity: var(--_disabled-trailing-icon-opacity);--md-filled-field-error-active-indicator-color: var(--_error-active-indicator-color);--md-filled-field-error-content-color: var(--_error-input-text-color);--md-filled-field-error-focus-active-indicator-color: var(--_error-focus-active-indicator-color);--md-filled-field-error-focus-content-color: var(--_error-focus-input-text-color);--md-filled-field-error-focus-label-text-color: var(--_error-focus-label-text-color);--md-filled-field-error-focus-leading-content-color: var(--_error-focus-leading-icon-color);--md-filled-field-error-focus-supporting-text-color: var(--_error-focus-supporting-text-color);--md-filled-field-error-focus-trailing-content-color: var(--_error-focus-trailing-icon-color);--md-filled-field-error-hover-active-indicator-color: var(--_error-hover-active-indicator-color);--md-filled-field-error-hover-content-color: var(--_error-hover-input-text-color);--md-filled-field-error-hover-label-text-color: var(--_error-hover-label-text-color);--md-filled-field-error-hover-leading-content-color: var(--_error-hover-leading-icon-color);--md-filled-field-error-hover-state-layer-color: var(--_error-hover-state-layer-color);--md-filled-field-error-hover-state-layer-opacity: var(--_error-hover-state-layer-opacity);--md-filled-field-error-hover-supporting-text-color: var(--_error-hover-supporting-text-color);--md-filled-field-error-hover-trailing-content-color: var(--_error-hover-trailing-icon-color);--md-filled-field-error-label-text-color: var(--_error-label-text-color);--md-filled-field-error-leading-content-color: var(--_error-leading-icon-color);--md-filled-field-error-supporting-text-color: var(--_error-supporting-text-color);--md-filled-field-error-trailing-content-color: var(--_error-trailing-icon-color);--md-filled-field-focus-active-indicator-color: var(--_focus-active-indicator-color);--md-filled-field-focus-active-indicator-height: var(--_focus-active-indicator-height);--md-filled-field-focus-content-color: var(--_focus-input-text-color);--md-filled-field-focus-label-text-color: var(--_focus-label-text-color);--md-filled-field-focus-leading-content-color: var(--_focus-leading-icon-color);--md-filled-field-focus-supporting-text-color: var(--_focus-supporting-text-color);--md-filled-field-focus-trailing-content-color: var(--_focus-trailing-icon-color);--md-filled-field-hover-active-indicator-color: var(--_hover-active-indicator-color);--md-filled-field-hover-active-indicator-height: var(--_hover-active-indicator-height);--md-filled-field-hover-content-color: var(--_hover-input-text-color);--md-filled-field-hover-label-text-color: var(--_hover-label-text-color);--md-filled-field-hover-leading-content-color: var(--_hover-leading-icon-color);--md-filled-field-hover-state-layer-color: var(--_hover-state-layer-color);--md-filled-field-hover-state-layer-opacity: var(--_hover-state-layer-opacity);--md-filled-field-hover-supporting-text-color: var(--_hover-supporting-text-color);--md-filled-field-hover-trailing-content-color: var(--_hover-trailing-icon-color);--md-filled-field-label-text-color: var(--_label-text-color);--md-filled-field-label-text-font: var(--_label-text-font);--md-filled-field-label-text-line-height: var(--_label-text-line-height);--md-filled-field-label-text-populated-line-height: var(--_label-text-populated-line-height);--md-filled-field-label-text-populated-size: var(--_label-text-populated-size);--md-filled-field-label-text-size: var(--_label-text-size);--md-filled-field-label-text-weight: var(--_label-text-weight);--md-filled-field-leading-content-color: var(--_leading-icon-color);--md-filled-field-leading-space: var(--_leading-space);--md-filled-field-supporting-text-color: var(--_supporting-text-color);--md-filled-field-supporting-text-font: var(--_supporting-text-font);--md-filled-field-supporting-text-line-height: var(--_supporting-text-line-height);--md-filled-field-supporting-text-size: var(--_supporting-text-size);--md-filled-field-supporting-text-weight: var(--_supporting-text-weight);--md-filled-field-top-space: var(--_top-space);--md-filled-field-trailing-content-color: var(--_trailing-icon-color);--md-filled-field-trailing-space: var(--_trailing-space);--md-filled-field-with-label-bottom-space: var(--_with-label-bottom-space);--md-filled-field-with-label-top-space: var(--_with-label-top-space);--md-filled-field-with-leading-content-leading-space: var(--_with-leading-icon-leading-space);--md-filled-field-with-trailing-content-trailing-space: var(--_with-trailing-icon-trailing-space)}
`;

/**
 * @license
 * Copyright 2021 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * A field component.
 */
class Field extends i$2 {
  constructor() {
    super(...arguments);
    this.disabled = false;
    this.error = false;
    this.focused = false;
    this.label = '';
    this.noAsterisk = false;
    this.populated = false;
    this.required = false;
    this.resizable = false;
    this.supportingText = '';
    this.errorText = '';
    this.count = -1;
    this.max = -1;
    /**
     * Whether or not the field has leading content.
     */
    this.hasStart = false;
    /**
     * Whether or not the field has trailing content.
     */
    this.hasEnd = false;
    this.isAnimating = false;
    /**
     * When set to true, the error text's `role="alert"` will be removed, then
     * re-added after an animation frame. This will re-announce an error message
     * to screen readers.
     */
    this.refreshErrorAlert = false;
    this.disableTransitions = false;
  }
  get counterText() {
    // Count and max are typed as number, but can be set to null when Lit removes
    // their attributes. These getters coerce back to a number for calculations.
    const countAsNumber = this.count ?? -1;
    const maxAsNumber = this.max ?? -1;
    // Counter does not show if count is negative, or max is negative or 0.
    if (countAsNumber < 0 || maxAsNumber <= 0) {
      return '';
    }
    return `${countAsNumber} / ${maxAsNumber}`;
  }
  get supportingOrErrorText() {
    return this.error && this.errorText ? this.errorText : this.supportingText;
  }
  /**
   * Re-announces the field's error supporting text to screen readers.
   *
   * Error text announces to screen readers anytime it is visible and changes.
   * Use the method to re-announce the message when the text has not changed,
   * but announcement is still needed (such as for `reportValidity()`).
   */
  reannounceError() {
    this.refreshErrorAlert = true;
  }
  update(props) {
    // Client-side property updates
    const isDisabledChanging = props.has('disabled') && props.get('disabled') !== undefined;
    if (isDisabledChanging) {
      this.disableTransitions = true;
    }
    // When disabling, remove focus styles if focused.
    if (this.disabled && this.focused) {
      props.set('focused', true);
      this.focused = false;
    }
    // Animate if focused or populated change.
    this.animateLabelIfNeeded({
      wasFocused: props.get('focused'),
      wasPopulated: props.get('populated')
    });
    super.update(props);
  }
  render() {
    var _this$renderOutline, _this$renderBackgroun, _this$renderStateLaye, _this$renderIndicator;
    const floatingLabel = this.renderLabel(/*isFloating*/true);
    const restingLabel = this.renderLabel(/*isFloating*/false);
    const outline = (_this$renderOutline = this.renderOutline) === null || _this$renderOutline === void 0 ? void 0 : _this$renderOutline.call(this, floatingLabel);
    const classes = {
      'disabled': this.disabled,
      'disable-transitions': this.disableTransitions,
      'error': this.error && !this.disabled,
      'focused': this.focused,
      'with-start': this.hasStart,
      'with-end': this.hasEnd,
      'populated': this.populated,
      'resizable': this.resizable,
      'required': this.required,
      'no-label': !this.label
    };
    return x`
      <div class="field ${e$3(classes)}">
        <div class="container-overflow">
          ${(_this$renderBackgroun = this.renderBackground) === null || _this$renderBackgroun === void 0 ? void 0 : _this$renderBackgroun.call(this)}
          <slot name="container"></slot>
          ${(_this$renderStateLaye = this.renderStateLayer) === null || _this$renderStateLaye === void 0 ? void 0 : _this$renderStateLaye.call(this)} ${(_this$renderIndicator = this.renderIndicator) === null || _this$renderIndicator === void 0 ? void 0 : _this$renderIndicator.call(this)} ${outline}
          <div class="container">
            <div class="start">
              <slot name="start"></slot>
            </div>
            <div class="middle">
              <div class="label-wrapper">
                ${restingLabel} ${outline ? E : floatingLabel}
              </div>
              <div class="content">
                <slot></slot>
              </div>
            </div>
            <div class="end">
              <slot name="end"></slot>
            </div>
          </div>
        </div>
        ${this.renderSupportingText()}
      </div>
    `;
  }
  updated(changed) {
    if (changed.has('supportingText') || changed.has('errorText') || changed.has('count') || changed.has('max')) {
      this.updateSlottedAriaDescribedBy();
    }
    if (this.refreshErrorAlert) {
      // The past render cycle removed the role="alert" from the error message.
      // Re-add it after an animation frame to re-announce the error.
      requestAnimationFrame(() => {
        this.refreshErrorAlert = false;
      });
    }
    if (this.disableTransitions) {
      requestAnimationFrame(() => {
        this.disableTransitions = false;
      });
    }
  }
  renderSupportingText() {
    const {
      supportingOrErrorText,
      counterText
    } = this;
    if (!supportingOrErrorText && !counterText) {
      return E;
    }
    // Always render the supporting text span so that our `space-around`
    // container puts the counter at the end.
    const start = x`<span>${supportingOrErrorText}</span>`;
    // Conditionally render counter so we don't render the extra `gap`.
    // TODO(b/244473435): add aria-label and announcements
    const end = counterText ? x`<span class="counter">${counterText}</span>` : E;
    // Announce if there is an error and error text visible.
    // If refreshErrorAlert is true, do not announce. This will remove the
    // role="alert" attribute. Another render cycle will happen after an
    // animation frame to re-add the role.
    const shouldErrorAnnounce = this.error && this.errorText && !this.refreshErrorAlert;
    const role = shouldErrorAnnounce ? 'alert' : E;
    return x`
      <div class="supporting-text" role=${role}>${start}${end}</div>
      <slot
        name="aria-describedby"
        @slotchange=${this.updateSlottedAriaDescribedBy}></slot>
    `;
  }
  updateSlottedAriaDescribedBy() {
    for (const element of this.slottedAriaDescribedBy) {
      B(x`${this.supportingOrErrorText} ${this.counterText}`, element);
      element.setAttribute('hidden', '');
    }
  }
  renderLabel(isFloating) {
    if (!this.label) {
      return E;
    }
    let visible;
    if (isFloating) {
      // Floating label is visible when focused/populated or when animating.
      visible = this.focused || this.populated || this.isAnimating;
    } else {
      // Resting label is visible when unfocused. It is never visible while
      // animating.
      visible = !this.focused && !this.populated && !this.isAnimating;
    }
    const classes = {
      'hidden': !visible,
      'floating': isFloating,
      'resting': !isFloating
    };
    // Add '*' if a label is present and the field is required
    const labelText = `${this.label}${this.required && !this.noAsterisk ? '*' : ''}`;
    return x`
      <span class="label ${e$3(classes)}" aria-hidden=${!visible}
        >${labelText}</span
      >
    `;
  }
  animateLabelIfNeeded({
    wasFocused,
    wasPopulated
  }) {
    var _this$labelAnimation, _this$floatingLabelEl, _this$labelAnimation2;
    if (!this.label) {
      return;
    }
    wasFocused ?? (wasFocused = this.focused);
    wasPopulated ?? (wasPopulated = this.populated);
    const wasFloating = wasFocused || wasPopulated;
    const shouldBeFloating = this.focused || this.populated;
    if (wasFloating === shouldBeFloating) {
      return;
    }
    this.isAnimating = true;
    (_this$labelAnimation = this.labelAnimation) === null || _this$labelAnimation === void 0 || _this$labelAnimation.cancel();
    // Only one label is visible at a time for clearer text rendering.
    // The floating label is visible and used during animation. At the end of
    // the animation, it will either remain visible (if floating) or hide and
    // the resting label will be shown.
    //
    // We don't use forward filling because if the dimensions of the text field
    // change (leading icon removed, density changes, etc), then the animation
    // will be inaccurate.
    //
    // Re-calculating the animation each time will prevent any visual glitches
    // from appearing.
    // TODO(b/241113345): use animation tokens
    this.labelAnimation = (_this$floatingLabelEl = this.floatingLabelEl) === null || _this$floatingLabelEl === void 0 ? void 0 : _this$floatingLabelEl.animate(this.getLabelKeyframes(), {
      duration: 150,
      easing: EASING.STANDARD
    });
    (_this$labelAnimation2 = this.labelAnimation) === null || _this$labelAnimation2 === void 0 || _this$labelAnimation2.addEventListener('finish', () => {
      // At the end of the animation, update the visible label.
      this.isAnimating = false;
    });
  }
  getLabelKeyframes() {
    const {
      floatingLabelEl,
      restingLabelEl
    } = this;
    if (!floatingLabelEl || !restingLabelEl) {
      return [];
    }
    const {
      x: floatingX,
      y: floatingY,
      height: floatingHeight
    } = floatingLabelEl.getBoundingClientRect();
    const {
      x: restingX,
      y: restingY,
      height: restingHeight
    } = restingLabelEl.getBoundingClientRect();
    const floatingScrollWidth = floatingLabelEl.scrollWidth;
    const restingScrollWidth = restingLabelEl.scrollWidth;
    // Scale by width ratio instead of font size since letter-spacing will scale
    // incorrectly. Using the width we can better approximate the adjusted
    // scale and compensate for tracking and overflow.
    // (use scrollWidth instead of width to account for clipped labels)
    const scale = restingScrollWidth / floatingScrollWidth;
    const xDelta = restingX - floatingX;
    // The line-height of the resting and floating label are different. When
    // we move the floating label down to the resting label's position, it won't
    // exactly match because of this. We need to adjust by half of what the
    // final scaled floating label's height will be.
    const yDelta = restingY - floatingY + Math.round((restingHeight - floatingHeight * scale) / 2);
    // Create the two transforms: floating to resting (using the calculations
    // above), and resting to floating (re-setting the transform to initial
    // values).
    const restTransform = `translateX(${xDelta}px) translateY(${yDelta}px) scale(${scale})`;
    const floatTransform = `translateX(0) translateY(0) scale(1)`;
    // Constrain the floating labels width to a scaled percentage of the
    // resting label's width. This will prevent long clipped labels from
    // overflowing the container.
    const restingClientWidth = restingLabelEl.clientWidth;
    const isRestingClipped = restingScrollWidth > restingClientWidth;
    const width = isRestingClipped ? `${restingClientWidth / scale}px` : '';
    if (this.focused || this.populated) {
      return [{
        transform: restTransform,
        width
      }, {
        transform: floatTransform,
        width
      }];
    }
    return [{
      transform: floatTransform,
      width
    }, {
      transform: restTransform,
      width
    }];
  }
  getSurfacePositionClientRect() {
    return this.containerEl.getBoundingClientRect();
  }
}
__decorate([n$3({
  type: Boolean
})], Field.prototype, "disabled", void 0);
__decorate([n$3({
  type: Boolean
})], Field.prototype, "error", void 0);
__decorate([n$3({
  type: Boolean
})], Field.prototype, "focused", void 0);
__decorate([n$3()], Field.prototype, "label", void 0);
__decorate([n$3({
  type: Boolean,
  attribute: 'no-asterisk'
})], Field.prototype, "noAsterisk", void 0);
__decorate([n$3({
  type: Boolean
})], Field.prototype, "populated", void 0);
__decorate([n$3({
  type: Boolean
})], Field.prototype, "required", void 0);
__decorate([n$3({
  type: Boolean
})], Field.prototype, "resizable", void 0);
__decorate([n$3({
  attribute: 'supporting-text'
})], Field.prototype, "supportingText", void 0);
__decorate([n$3({
  attribute: 'error-text'
})], Field.prototype, "errorText", void 0);
__decorate([n$3({
  type: Number
})], Field.prototype, "count", void 0);
__decorate([n$3({
  type: Number
})], Field.prototype, "max", void 0);
__decorate([n$3({
  type: Boolean,
  attribute: 'has-start'
})], Field.prototype, "hasStart", void 0);
__decorate([n$3({
  type: Boolean,
  attribute: 'has-end'
})], Field.prototype, "hasEnd", void 0);
__decorate([o$2({
  slot: 'aria-describedby'
})], Field.prototype, "slottedAriaDescribedBy", void 0);
__decorate([r$1()], Field.prototype, "isAnimating", void 0);
__decorate([r$1()], Field.prototype, "refreshErrorAlert", void 0);
__decorate([r$1()], Field.prototype, "disableTransitions", void 0);
__decorate([e$2('.label.floating')], Field.prototype, "floatingLabelEl", void 0);
__decorate([e$2('.label.resting')], Field.prototype, "restingLabelEl", void 0);
__decorate([e$2('.container')], Field.prototype, "containerEl", void 0);

/**
 * @license
 * Copyright 2021 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * A filled field component.
 */
class FilledField extends Field {
  renderBackground() {
    return x` <div class="background"></div> `;
  }
  renderStateLayer() {
    return x` <div class="state-layer"></div> `;
  }
  renderIndicator() {
    return x`<div class="active-indicator"></div>`;
  }
}

/**
 * @license
 * Copyright 2024 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Generated stylesheet for ./field/internal/filled-styles.css.
const styles$8 = i$3`@layer styles{:host{--_active-indicator-color: var(--md-filled-field-active-indicator-color, var(--md-sys-color-on-surface-variant, #49454f));--_active-indicator-height: var(--md-filled-field-active-indicator-height, 1px);--_bottom-space: var(--md-filled-field-bottom-space, 16px);--_container-color: var(--md-filled-field-container-color, var(--md-sys-color-surface-container-highest, #e6e0e9));--_content-color: var(--md-filled-field-content-color, var(--md-sys-color-on-surface, #1d1b20));--_content-font: var(--md-filled-field-content-font, var(--md-sys-typescale-body-large-font, var(--md-ref-typeface-plain, Roboto)));--_content-line-height: var(--md-filled-field-content-line-height, var(--md-sys-typescale-body-large-line-height, 1.5rem));--_content-size: var(--md-filled-field-content-size, var(--md-sys-typescale-body-large-size, 1rem));--_content-space: var(--md-filled-field-content-space, 16px);--_content-weight: var(--md-filled-field-content-weight, var(--md-sys-typescale-body-large-weight, var(--md-ref-typeface-weight-regular, 400)));--_disabled-active-indicator-color: var(--md-filled-field-disabled-active-indicator-color, var(--md-sys-color-on-surface, #1d1b20));--_disabled-active-indicator-height: var(--md-filled-field-disabled-active-indicator-height, 1px);--_disabled-active-indicator-opacity: var(--md-filled-field-disabled-active-indicator-opacity, 0.38);--_disabled-container-color: var(--md-filled-field-disabled-container-color, var(--md-sys-color-on-surface, #1d1b20));--_disabled-container-opacity: var(--md-filled-field-disabled-container-opacity, 0.04);--_disabled-content-color: var(--md-filled-field-disabled-content-color, var(--md-sys-color-on-surface, #1d1b20));--_disabled-content-opacity: var(--md-filled-field-disabled-content-opacity, 0.38);--_disabled-label-text-color: var(--md-filled-field-disabled-label-text-color, var(--md-sys-color-on-surface, #1d1b20));--_disabled-label-text-opacity: var(--md-filled-field-disabled-label-text-opacity, 0.38);--_disabled-leading-content-color: var(--md-filled-field-disabled-leading-content-color, var(--md-sys-color-on-surface, #1d1b20));--_disabled-leading-content-opacity: var(--md-filled-field-disabled-leading-content-opacity, 0.38);--_disabled-supporting-text-color: var(--md-filled-field-disabled-supporting-text-color, var(--md-sys-color-on-surface, #1d1b20));--_disabled-supporting-text-opacity: var(--md-filled-field-disabled-supporting-text-opacity, 0.38);--_disabled-trailing-content-color: var(--md-filled-field-disabled-trailing-content-color, var(--md-sys-color-on-surface, #1d1b20));--_disabled-trailing-content-opacity: var(--md-filled-field-disabled-trailing-content-opacity, 0.38);--_error-active-indicator-color: var(--md-filled-field-error-active-indicator-color, var(--md-sys-color-error, #b3261e));--_error-content-color: var(--md-filled-field-error-content-color, var(--md-sys-color-on-surface, #1d1b20));--_error-focus-active-indicator-color: var(--md-filled-field-error-focus-active-indicator-color, var(--md-sys-color-error, #b3261e));--_error-focus-content-color: var(--md-filled-field-error-focus-content-color, var(--md-sys-color-on-surface-variant, #49454f));--_error-focus-label-text-color: var(--md-filled-field-error-focus-label-text-color, var(--md-sys-color-error, #b3261e));--_error-focus-leading-content-color: var(--md-filled-field-error-focus-leading-content-color, var(--md-sys-color-on-surface-variant, #49454f));--_error-focus-supporting-text-color: var(--md-filled-field-error-focus-supporting-text-color, var(--md-sys-color-error, #b3261e));--_error-focus-trailing-content-color: var(--md-filled-field-error-focus-trailing-content-color, var(--md-sys-color-error, #b3261e));--_error-hover-active-indicator-color: var(--md-filled-field-error-hover-active-indicator-color, var(--md-sys-color-on-error-container, #410e0b));--_error-hover-content-color: var(--md-filled-field-error-hover-content-color, var(--md-sys-color-on-surface, #1d1b20));--_error-hover-label-text-color: var(--md-filled-field-error-hover-label-text-color, var(--md-sys-color-on-error-container, #410e0b));--_error-hover-leading-content-color: var(--md-filled-field-error-hover-leading-content-color, var(--md-sys-color-on-surface-variant, #49454f));--_error-hover-state-layer-color: var(--md-filled-field-error-hover-state-layer-color, var(--md-sys-color-on-surface, #1d1b20));--_error-hover-state-layer-opacity: var(--md-filled-field-error-hover-state-layer-opacity, 0.08);--_error-hover-supporting-text-color: var(--md-filled-field-error-hover-supporting-text-color, var(--md-sys-color-error, #b3261e));--_error-hover-trailing-content-color: var(--md-filled-field-error-hover-trailing-content-color, var(--md-sys-color-on-error-container, #410e0b));--_error-label-text-color: var(--md-filled-field-error-label-text-color, var(--md-sys-color-error, #b3261e));--_error-leading-content-color: var(--md-filled-field-error-leading-content-color, var(--md-sys-color-on-surface-variant, #49454f));--_error-supporting-text-color: var(--md-filled-field-error-supporting-text-color, var(--md-sys-color-error, #b3261e));--_error-trailing-content-color: var(--md-filled-field-error-trailing-content-color, var(--md-sys-color-error, #b3261e));--_focus-active-indicator-color: var(--md-filled-field-focus-active-indicator-color, var(--md-sys-color-primary, #6750a4));--_focus-active-indicator-height: var(--md-filled-field-focus-active-indicator-height, 3px);--_focus-content-color: var(--md-filled-field-focus-content-color, var(--md-sys-color-on-surface, #1d1b20));--_focus-label-text-color: var(--md-filled-field-focus-label-text-color, var(--md-sys-color-primary, #6750a4));--_focus-leading-content-color: var(--md-filled-field-focus-leading-content-color, var(--md-sys-color-on-surface-variant, #49454f));--_focus-supporting-text-color: var(--md-filled-field-focus-supporting-text-color, var(--md-sys-color-on-surface-variant, #49454f));--_focus-trailing-content-color: var(--md-filled-field-focus-trailing-content-color, var(--md-sys-color-on-surface-variant, #49454f));--_hover-active-indicator-color: var(--md-filled-field-hover-active-indicator-color, var(--md-sys-color-on-surface, #1d1b20));--_hover-active-indicator-height: var(--md-filled-field-hover-active-indicator-height, 1px);--_hover-content-color: var(--md-filled-field-hover-content-color, var(--md-sys-color-on-surface, #1d1b20));--_hover-label-text-color: var(--md-filled-field-hover-label-text-color, var(--md-sys-color-on-surface-variant, #49454f));--_hover-leading-content-color: var(--md-filled-field-hover-leading-content-color, var(--md-sys-color-on-surface-variant, #49454f));--_hover-state-layer-color: var(--md-filled-field-hover-state-layer-color, var(--md-sys-color-on-surface, #1d1b20));--_hover-state-layer-opacity: var(--md-filled-field-hover-state-layer-opacity, 0.08);--_hover-supporting-text-color: var(--md-filled-field-hover-supporting-text-color, var(--md-sys-color-on-surface-variant, #49454f));--_hover-trailing-content-color: var(--md-filled-field-hover-trailing-content-color, var(--md-sys-color-on-surface-variant, #49454f));--_label-text-color: var(--md-filled-field-label-text-color, var(--md-sys-color-on-surface-variant, #49454f));--_label-text-font: var(--md-filled-field-label-text-font, var(--md-sys-typescale-body-large-font, var(--md-ref-typeface-plain, Roboto)));--_label-text-line-height: var(--md-filled-field-label-text-line-height, var(--md-sys-typescale-body-large-line-height, 1.5rem));--_label-text-populated-line-height: var(--md-filled-field-label-text-populated-line-height, var(--md-sys-typescale-body-small-line-height, 1rem));--_label-text-populated-size: var(--md-filled-field-label-text-populated-size, var(--md-sys-typescale-body-small-size, 0.75rem));--_label-text-size: var(--md-filled-field-label-text-size, var(--md-sys-typescale-body-large-size, 1rem));--_label-text-weight: var(--md-filled-field-label-text-weight, var(--md-sys-typescale-body-large-weight, var(--md-ref-typeface-weight-regular, 400)));--_leading-content-color: var(--md-filled-field-leading-content-color, var(--md-sys-color-on-surface-variant, #49454f));--_leading-space: var(--md-filled-field-leading-space, 16px);--_supporting-text-color: var(--md-filled-field-supporting-text-color, var(--md-sys-color-on-surface-variant, #49454f));--_supporting-text-font: var(--md-filled-field-supporting-text-font, var(--md-sys-typescale-body-small-font, var(--md-ref-typeface-plain, Roboto)));--_supporting-text-leading-space: var(--md-filled-field-supporting-text-leading-space, 16px);--_supporting-text-line-height: var(--md-filled-field-supporting-text-line-height, var(--md-sys-typescale-body-small-line-height, 1rem));--_supporting-text-size: var(--md-filled-field-supporting-text-size, var(--md-sys-typescale-body-small-size, 0.75rem));--_supporting-text-top-space: var(--md-filled-field-supporting-text-top-space, 4px);--_supporting-text-trailing-space: var(--md-filled-field-supporting-text-trailing-space, 16px);--_supporting-text-weight: var(--md-filled-field-supporting-text-weight, var(--md-sys-typescale-body-small-weight, var(--md-ref-typeface-weight-regular, 400)));--_top-space: var(--md-filled-field-top-space, 16px);--_trailing-content-color: var(--md-filled-field-trailing-content-color, var(--md-sys-color-on-surface-variant, #49454f));--_trailing-space: var(--md-filled-field-trailing-space, 16px);--_with-label-bottom-space: var(--md-filled-field-with-label-bottom-space, 8px);--_with-label-top-space: var(--md-filled-field-with-label-top-space, 8px);--_with-leading-content-leading-space: var(--md-filled-field-with-leading-content-leading-space, 12px);--_with-trailing-content-trailing-space: var(--md-filled-field-with-trailing-content-trailing-space, 12px);--_container-shape-start-start: var(--md-filled-field-container-shape-start-start, var(--md-filled-field-container-shape, var(--md-sys-shape-corner-extra-small, 4px)));--_container-shape-start-end: var(--md-filled-field-container-shape-start-end, var(--md-filled-field-container-shape, var(--md-sys-shape-corner-extra-small, 4px)));--_container-shape-end-end: var(--md-filled-field-container-shape-end-end, var(--md-filled-field-container-shape, var(--md-sys-shape-corner-none, 0px)));--_container-shape-end-start: var(--md-filled-field-container-shape-end-start, var(--md-filled-field-container-shape, var(--md-sys-shape-corner-none, 0px)))}.background,.state-layer{border-radius:inherit;inset:0;pointer-events:none;position:absolute}.background{background:var(--_container-color)}.state-layer{visibility:hidden}.field:not(.disabled):hover .state-layer{visibility:visible}.label.floating{position:absolute;top:var(--_with-label-top-space)}.field:not(.with-start) .label-wrapper{margin-inline-start:var(--_leading-space)}.field:not(.with-end) .label-wrapper{margin-inline-end:var(--_trailing-space)}.active-indicator{inset:auto 0 0 0;pointer-events:none;position:absolute;width:100%;z-index:1}.active-indicator::before,.active-indicator::after{border-bottom:var(--_active-indicator-height) solid var(--_active-indicator-color);inset:auto 0 0 0;content:"";position:absolute;width:100%}.active-indicator::after{opacity:0;transition:opacity 150ms cubic-bezier(0.2, 0, 0, 1)}.focused .active-indicator::after{opacity:1}.field:not(.with-start) .content ::slotted(*){padding-inline-start:var(--_leading-space)}.field:not(.with-end) .content ::slotted(*){padding-inline-end:var(--_trailing-space)}.field:not(.no-label) .content ::slotted(:not(textarea)){padding-bottom:var(--_with-label-bottom-space);padding-top:calc(var(--_with-label-top-space) + var(--_label-text-populated-line-height))}.field:not(.no-label) .content ::slotted(textarea){margin-bottom:var(--_with-label-bottom-space);margin-top:calc(var(--_with-label-top-space) + var(--_label-text-populated-line-height))}:hover .active-indicator::before{border-bottom-color:var(--_hover-active-indicator-color);border-bottom-width:var(--_hover-active-indicator-height)}.active-indicator::after{border-bottom-color:var(--_focus-active-indicator-color);border-bottom-width:var(--_focus-active-indicator-height)}:hover .state-layer{background:var(--_hover-state-layer-color);opacity:var(--_hover-state-layer-opacity)}.disabled .active-indicator::before{border-bottom-color:var(--_disabled-active-indicator-color);border-bottom-width:var(--_disabled-active-indicator-height);opacity:var(--_disabled-active-indicator-opacity)}.disabled .background{background:var(--_disabled-container-color);opacity:var(--_disabled-container-opacity)}.error .active-indicator::before{border-bottom-color:var(--_error-active-indicator-color)}.error:hover .active-indicator::before{border-bottom-color:var(--_error-hover-active-indicator-color)}.error:hover .state-layer{background:var(--_error-hover-state-layer-color);opacity:var(--_error-hover-state-layer-opacity)}.error .active-indicator::after{border-bottom-color:var(--_error-focus-active-indicator-color)}.resizable .container{bottom:var(--_focus-active-indicator-height);clip-path:inset(var(--_focus-active-indicator-height) 0 0 0)}.resizable .container>*{top:var(--_focus-active-indicator-height)}}@layer hcm{@media(forced-colors: active){.disabled .active-indicator::before{border-color:GrayText;opacity:1}}}
`;

/**
 * @license
 * Copyright 2024 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Generated stylesheet for ./field/internal/shared-styles.css.
const styles$7 = i$3`:host{display:inline-flex;resize:both}.field{display:flex;flex:1;flex-direction:column;writing-mode:horizontal-tb;max-width:100%}.container-overflow{border-start-start-radius:var(--_container-shape-start-start);border-start-end-radius:var(--_container-shape-start-end);border-end-end-radius:var(--_container-shape-end-end);border-end-start-radius:var(--_container-shape-end-start);display:flex;height:100%;position:relative}.container{align-items:center;border-radius:inherit;display:flex;flex:1;max-height:100%;min-height:100%;min-width:min-content;position:relative}.field,.container-overflow{resize:inherit}.resizable:not(.disabled) .container{resize:inherit;overflow:hidden}.disabled{pointer-events:none}slot[name=container]{border-radius:inherit}slot[name=container]::slotted(*){border-radius:inherit;inset:0;pointer-events:none;position:absolute}@layer styles{.start,.middle,.end{display:flex;box-sizing:border-box;height:100%;position:relative}.start{color:var(--_leading-content-color)}.end{color:var(--_trailing-content-color)}.start,.end{align-items:center;justify-content:center}.with-start .start{margin-inline:var(--_with-leading-content-leading-space) var(--_content-space)}.with-end .end{margin-inline:var(--_content-space) var(--_with-trailing-content-trailing-space)}.middle{align-items:stretch;align-self:baseline;flex:1}.content{color:var(--_content-color);display:flex;flex:1;opacity:0;transition:opacity 83ms cubic-bezier(0.2, 0, 0, 1)}.no-label .content,.focused .content,.populated .content{opacity:1;transition-delay:67ms}:is(.disabled,.disable-transitions) .content{transition:none}.content ::slotted(*){all:unset;color:currentColor;font-family:var(--_content-font);font-size:var(--_content-size);line-height:var(--_content-line-height);font-weight:var(--_content-weight);width:100%;overflow-wrap:revert;white-space:revert}.content ::slotted(:not(textarea)){padding-top:var(--_top-space);padding-bottom:var(--_bottom-space)}.content ::slotted(textarea){margin-top:var(--_top-space);margin-bottom:var(--_bottom-space)}:hover .content{color:var(--_hover-content-color)}:hover .start{color:var(--_hover-leading-content-color)}:hover .end{color:var(--_hover-trailing-content-color)}.focused .content{color:var(--_focus-content-color)}.focused .start{color:var(--_focus-leading-content-color)}.focused .end{color:var(--_focus-trailing-content-color)}.disabled .content{color:var(--_disabled-content-color)}.disabled.no-label .content,.disabled.focused .content,.disabled.populated .content{opacity:var(--_disabled-content-opacity)}.disabled .start{color:var(--_disabled-leading-content-color);opacity:var(--_disabled-leading-content-opacity)}.disabled .end{color:var(--_disabled-trailing-content-color);opacity:var(--_disabled-trailing-content-opacity)}.error .content{color:var(--_error-content-color)}.error .start{color:var(--_error-leading-content-color)}.error .end{color:var(--_error-trailing-content-color)}.error:hover .content{color:var(--_error-hover-content-color)}.error:hover .start{color:var(--_error-hover-leading-content-color)}.error:hover .end{color:var(--_error-hover-trailing-content-color)}.error.focused .content{color:var(--_error-focus-content-color)}.error.focused .start{color:var(--_error-focus-leading-content-color)}.error.focused .end{color:var(--_error-focus-trailing-content-color)}}@layer hcm{@media(forced-colors: active){.disabled :is(.start,.content,.end){color:GrayText;opacity:1}}}@layer styles{.label{box-sizing:border-box;color:var(--_label-text-color);overflow:hidden;max-width:100%;text-overflow:ellipsis;white-space:nowrap;z-index:1;font-family:var(--_label-text-font);font-size:var(--_label-text-size);line-height:var(--_label-text-line-height);font-weight:var(--_label-text-weight);width:min-content}.label-wrapper{inset:0;pointer-events:none;position:absolute}.label.resting{position:absolute;top:var(--_top-space)}.label.floating{font-size:var(--_label-text-populated-size);line-height:var(--_label-text-populated-line-height);transform-origin:top left}.label.hidden{opacity:0}.no-label .label{display:none}.label-wrapper{inset:0;position:absolute;text-align:initial}:hover .label{color:var(--_hover-label-text-color)}.focused .label{color:var(--_focus-label-text-color)}.disabled .label{color:var(--_disabled-label-text-color)}.disabled .label:not(.hidden){opacity:var(--_disabled-label-text-opacity)}.error .label{color:var(--_error-label-text-color)}.error:hover .label{color:var(--_error-hover-label-text-color)}.error.focused .label{color:var(--_error-focus-label-text-color)}}@layer hcm{@media(forced-colors: active){.disabled .label:not(.hidden){color:GrayText;opacity:1}}}@layer styles{.supporting-text{color:var(--_supporting-text-color);display:flex;font-family:var(--_supporting-text-font);font-size:var(--_supporting-text-size);line-height:var(--_supporting-text-line-height);font-weight:var(--_supporting-text-weight);gap:16px;justify-content:space-between;padding-inline-start:var(--_supporting-text-leading-space);padding-inline-end:var(--_supporting-text-trailing-space);padding-top:var(--_supporting-text-top-space)}.supporting-text :nth-child(2){flex-shrink:0}:hover .supporting-text{color:var(--_hover-supporting-text-color)}.focus .supporting-text{color:var(--_focus-supporting-text-color)}.disabled .supporting-text{color:var(--_disabled-supporting-text-color);opacity:var(--_disabled-supporting-text-opacity)}.error .supporting-text{color:var(--_error-supporting-text-color)}.error:hover .supporting-text{color:var(--_error-hover-supporting-text-color)}.error.focus .supporting-text{color:var(--_error-focus-supporting-text-color)}}@layer hcm{@media(forced-colors: active){.disabled .supporting-text{color:GrayText;opacity:1}}}
`;

/**
 * @license
 * Copyright 2021 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * TODO(b/228525797): add docs
 * @final
 * @suppress {visibility}
 */
let MdFilledField = class MdFilledField extends FilledField {};
MdFilledField.styles = [styles$7, styles$8];
MdFilledField = __decorate([t('md-filled-field')], MdFilledField);

/**
 * @license
 * Copyright 2020 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */
const f = o => void 0 === o.strings,
  u = {},
  m = (o, t = u) => o._$AH = t;

/**
 * @license
 * Copyright 2020 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */
const l = e$4(class extends i$4 {
  constructor(r) {
    if (super(r), r.type !== t$1.PROPERTY && r.type !== t$1.ATTRIBUTE && r.type !== t$1.BOOLEAN_ATTRIBUTE) throw Error("The `live` directive is not allowed on child or event bindings");
    if (!f(r)) throw Error("`live` bindings can only contain a single expression");
  }
  render(r) {
    return r;
  }
  update(i, [t]) {
    if (t === T || t === E) return t;
    const o = i.element,
      l = i.name;
    if (i.type === t$1.PROPERTY) {
      if (t === o[l]) return T;
    } else if (i.type === t$1.BOOLEAN_ATTRIBUTE) {
      if (!!t === o.hasAttribute(l)) return T;
    } else if (i.type === t$1.ATTRIBUTE && o.getAttribute(l) === t + "") return T;
    return m(i), t;
  }
});

/**
 * @license
 * Copyright 2018 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */
const n = "important",
  i = " !" + n,
  o = e$4(class extends i$4 {
    constructor(t) {
      var _t$strings;
      if (super(t), t.type !== t$1.ATTRIBUTE || "style" !== t.name || ((_t$strings = t.strings) === null || _t$strings === void 0 ? void 0 : _t$strings.length) > 2) throw Error("The `styleMap` directive must be used in the `style` attribute and must be the only part in the attribute.");
    }
    render(t) {
      return Object.keys(t).reduce((e, r) => {
        const s = t[r];
        return null == s ? e : e + `${r = r.includes("-") ? r : r.replace(/(?:^(webkit|moz|ms|o)|)(?=[A-Z])/g, "-$&").toLowerCase()}:${s};`;
      }, "");
    }
    update(e, [r]) {
      const {
        style: s
      } = e.element;
      if (void 0 === this.ft) return this.ft = new Set(Object.keys(r)), this.render(r);
      for (const t of this.ft) null == r[t] && (this.ft.delete(t), t.includes("-") ? s.removeProperty(t) : s[t] = null);
      for (const t in r) {
        const e = r[t];
        if (null != e) {
          this.ft.add(t);
          const r = "string" == typeof e && e.endsWith(i);
          t.includes("-") || r ? s.setProperty(t, r ? e.slice(0, -11) : e, r ? n : "") : s[t] = e;
        }
      }
      return T;
    }
  });

/**
 * @license
 * Copyright 2022 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
const stringConverter = {
  fromAttribute(value) {
    return value ?? '';
  },
  toAttribute(value) {
    return value || null;
  }
};

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * A symbol property used for a callback when validity has been reported.
 */
const onReportValidity = Symbol('onReportValidity');
// Private symbol members, used to avoid name clashing.
const privateCleanupFormListeners = Symbol('privateCleanupFormListeners');
const privateDoNotReportInvalid = Symbol('privateDoNotReportInvalid');
const privateIsSelfReportingValidity = Symbol('privateIsSelfReportingValidity');
const privateCallOnReportValidity = Symbol('privateCallOnReportValidity');
/**
 * Mixes in a callback for constraint validation when validity should be
 * styled and reported to the user.
 *
 * This is commonly used in text-field-like controls that display error styles
 * and error messages.
 *
 * @example
 * ```ts
 * const baseClass = mixinOnReportValidity(
 *   mixinConstraintValidation(
 *     mixinFormAssociated(mixinElementInternals(LitElement)),
 *   ),
 * );
 *
 * class MyField extends baseClass {
 *   \@property({type: Boolean}) error = false;
 *   \@property() errorMessage = '';
 *
 *   [onReportValidity](invalidEvent: Event | null) {
 *     this.error = !!invalidEvent;
 *     this.errorMessage = this.validationMessage;
 *
 *     // Optionally prevent platform popup from displaying
 *     invalidEvent?.preventDefault();
 *   }
 * }
 * ```
 *
 * @param base The class to mix functionality into.
 * @return The provided class with `OnReportValidity` mixed in.
 */
function mixinOnReportValidity(base) {
  var _a, _b, _c;
  class OnReportValidityElement extends base {
    // Mixins must have a constructor with `...args: any[]`
    // tslint:disable-next-line:no-any
    constructor(...args) {
      super(...args);
      /**
       * Used to clean up event listeners when a new form is associated.
       */
      this[_a] = new AbortController();
      /**
       * Used to determine if an invalid event should report validity. Invalid
       * events from `checkValidity()` do not trigger reporting.
       */
      this[_b] = false;
      /**
       * Used to determine if the control is reporting validity from itself, or
       * if a `<form>` is causing the validity report. Forms have different
       * control focusing behavior.
       */
      this[_c] = false;
      this.addEventListener('invalid', invalidEvent => {
        // Listen for invalid events dispatched by a `<form>` when it tries to
        // submit and the element is invalid. We ignore events dispatched when
        // calling `checkValidity()` as well as untrusted events, since the
        // `reportValidity()` and `<form>`-dispatched events are always
        // trusted.
        if (this[privateDoNotReportInvalid] || !invalidEvent.isTrusted) {
          return;
        }
        this.addEventListener('invalid', () => {
          // A normal bubbling phase event listener. By adding it here, we
          // ensure it's the last event listener that is called during the
          // bubbling phase.
          this[privateCallOnReportValidity](invalidEvent);
        }, {
          once: true
        });
      }, {
        // Listen during the capture phase, which will happen before the
        // bubbling phase. That way, we can add a final event listener that
        // will run after other event listeners, and we can check if it was
        // default prevented. This works because invalid does not bubble.
        capture: true
      });
    }
    checkValidity() {
      this[privateDoNotReportInvalid] = true;
      const valid = super.checkValidity();
      this[privateDoNotReportInvalid] = false;
      return valid;
    }
    reportValidity() {
      this[privateIsSelfReportingValidity] = true;
      const valid = super.reportValidity();
      // Constructor's invalid listener will handle reporting invalid events.
      if (valid) {
        this[privateCallOnReportValidity](null);
      }
      this[privateIsSelfReportingValidity] = false;
      return valid;
    }
    [(_a = privateCleanupFormListeners, _b = privateDoNotReportInvalid, _c = privateIsSelfReportingValidity, privateCallOnReportValidity)](invalidEvent) {
      // Since invalid events do not bubble to parent listeners, and because
      // our invalid listeners are added lazily after other listeners, we can
      // reliably read `defaultPrevented` synchronously without worrying
      // about waiting for another listener that could cancel it.
      const wasCanceled = invalidEvent === null || invalidEvent === void 0 ? void 0 : invalidEvent.defaultPrevented;
      if (wasCanceled) {
        return;
      }
      this[onReportValidity](invalidEvent);
      // If an implementation calls invalidEvent.preventDefault() to stop the
      // platform popup from displaying, focusing is also prevented, so we need
      // to manually focus.
      const implementationCanceledFocus = !wasCanceled && (invalidEvent === null || invalidEvent === void 0 ? void 0 : invalidEvent.defaultPrevented);
      if (!implementationCanceledFocus) {
        return;
      }
      // The control should be focused when:
      // - `control.reportValidity()` is called (self-reporting).
      // - a form is reporting validity for its controls and this is the first
      //   invalid control.
      if (this[privateIsSelfReportingValidity] || isFirstInvalidControlInForm(this[internals].form, this)) {
        this.focus();
      }
    }
    [onReportValidity](invalidEvent) {
      throw new Error('Implement [onReportValidity]');
    }
    formAssociatedCallback(form) {
      // can't use super.formAssociatedCallback?.() due to closure
      if (super.formAssociatedCallback) {
        super.formAssociatedCallback(form);
      }
      // Clean up previous form listeners.
      this[privateCleanupFormListeners].abort();
      if (!form) {
        return;
      }
      this[privateCleanupFormListeners] = new AbortController();
      // Add a listener that fires when the form runs constraint validation and
      // the control is valid, so that it may remove its error styles.
      //
      // This happens on `form.reportValidity()` and `form.requestSubmit()`
      // (both when the submit fails and passes).
      addFormReportValidListener(this, form, () => {
        this[privateCallOnReportValidity](null);
      }, this[privateCleanupFormListeners].signal);
    }
  }
  return OnReportValidityElement;
}
/**
 * Add a listener that fires when a form runs constraint validation on a control
 * and it is valid. This is needed to clear previously invalid styles.
 *
 * @param control The control of the form to listen for valid events.
 * @param form The control's form that can run constraint validation.
 * @param onControlValid A listener that is called when the form runs constraint
 *     validation and the control is valid.
 * @param cleanup A cleanup signal to remove the listener.
 */
function addFormReportValidListener(control, form, onControlValid, cleanup) {
  const validateHooks = getFormValidateHooks(form);
  // When a form validates its controls, check if an invalid event is dispatched
  // on the control. If it is not, then inform the control to report its valid
  // state.
  let controlFiredInvalid = false;
  let cleanupInvalidListener;
  let isNextSubmitFromHook = false;
  validateHooks.addEventListener('before', () => {
    isNextSubmitFromHook = true;
    cleanupInvalidListener = new AbortController();
    controlFiredInvalid = false;
    control.addEventListener('invalid', () => {
      controlFiredInvalid = true;
    }, {
      signal: cleanupInvalidListener.signal
    });
  }, {
    signal: cleanup
  });
  validateHooks.addEventListener('after', () => {
    var _cleanupInvalidListen;
    isNextSubmitFromHook = false;
    (_cleanupInvalidListen = cleanupInvalidListener) === null || _cleanupInvalidListen === void 0 || _cleanupInvalidListen.abort();
    if (controlFiredInvalid) {
      return;
    }
    onControlValid();
  }, {
    signal: cleanup
  });
  // The above hooks handle imperatively submitting the form, but not
  // declaratively submitting the form. This happens when:
  // 1. A non-custom element `<button type="submit">` is clicked.
  // 2. Enter is pressed on a non-custom element text editable `<input>`.
  form.addEventListener('submit', () => {
    // This submit was from `form.requestSubmit()`, which already calls the
    // listener.
    if (isNextSubmitFromHook) {
      return;
    }
    onControlValid();
  }, {
    signal: cleanup
  });
  // Note: it is a known limitation that we cannot detect if a form tries to
  // submit declaratively, but fails to do so because an unrelated sibling
  // control failed its constraint validation.
  //
  // Since we cannot detect when that happens, a previously invalid control may
  // not clear its error styling when it becomes valid again.
  //
  // To work around this, call `form.reportValidity()` when submitting a form
  // declaratively. This can be down on the `<button type="submit">`'s click or
  // the text editable `<input>`'s 'Enter' keydown.
}
const FORM_VALIDATE_HOOKS = new WeakMap();
/**
 * Get a hooks `EventTarget` that dispatches 'before' and 'after' events that
 * fire before a form runs constraint validation and immediately after it
 * finishes running constraint validation on its controls.
 *
 * This happens during `form.reportValidity()` and `form.requestSubmit()`.
 *
 * @param form The form to get or set up hooks for.
 * @return A hooks `EventTarget` to add listeners to.
 */
function getFormValidateHooks(form) {
  if (!FORM_VALIDATE_HOOKS.has(form)) {
    // Patch form methods to add event listener hooks. These are needed to react
    // to form behaviors that do not dispatch events, such as a form asking its
    // controls to report their validity.
    //
    // We should only patch the methods once, since multiple controls and other
    // forces may want to patch this method. We cannot reliably clean it up if
    // there are multiple patched and re-patched methods referring holding
    // references to each other.
    //
    // Instead, we never clean up the patch but add and clean up event listeners
    // added to the hooks after the patch.
    const hooks = new EventTarget();
    FORM_VALIDATE_HOOKS.set(form, hooks);
    // Add hooks to support notifying before and after a form has run constraint
    // validation on its controls.
    // Note: `form.submit()` does not run constraint validation per spec.
    for (const methodName of ['reportValidity', 'requestSubmit']) {
      const superMethod = form[methodName];
      form[methodName] = function () {
        hooks.dispatchEvent(new Event('before'));
        const result = Reflect.apply(superMethod, this, arguments);
        hooks.dispatchEvent(new Event('after'));
        return result;
      };
    }
  }
  return FORM_VALIDATE_HOOKS.get(form);
}
/**
 * Checks if a control is the first invalid control in a form.
 *
 * @param form The control's form. When `null`, the control doesn't have a form
 *     and the method returns true.
 * @param control The control to check.
 * @return True if there is no form or if the control is the form's first
 *     invalid control.
 */
function isFirstInvalidControlInForm(form, control) {
  if (!form) {
    return true;
  }
  let firstInvalidControl;
  for (const element of form.elements) {
    if (element.matches(':invalid')) {
      firstInvalidControl = element;
      break;
    }
  }
  return firstInvalidControl === control;
}

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * A validator that provides constraint validation that emulates `<input>` and
 * `<textarea>` validation.
 */
class TextFieldValidator extends Validator {
  computeValidity({
    state,
    renderedControl
  }) {
    let inputOrTextArea = renderedControl;
    if (isInputState(state) && !inputOrTextArea) {
      // Get cached <input> or create it.
      inputOrTextArea = this.inputControl || document.createElement('input');
      // Cache the <input> to re-use it next time.
      this.inputControl = inputOrTextArea;
    } else if (!inputOrTextArea) {
      // Get cached <textarea> or create it.
      inputOrTextArea = this.textAreaControl || document.createElement('textarea');
      // Cache the <textarea> to re-use it next time.
      this.textAreaControl = inputOrTextArea;
    }
    // Set this variable so we can check it for input-specific properties.
    const input = isInputState(state) ? inputOrTextArea : null;
    // Set input's "type" first, since this can change the other properties
    if (input) {
      input.type = state.type;
    }
    if (inputOrTextArea.value !== state.value) {
      // Only programmatically set the value if there's a difference. When using
      // the rendered control, the value will always be up to date. Setting the
      // property (even if it's the same string) will reset the internal <input>
      // dirty flag, making minlength and maxlength validation reset.
      inputOrTextArea.value = state.value;
    }
    inputOrTextArea.required = state.required;
    // The following IDLAttribute properties will always hydrate an attribute,
    // even if set to a the default value ('' or -1). The presence of the
    // attribute triggers constraint validation, so we must remove the attribute
    // when empty.
    if (input) {
      const inputState = state;
      if (inputState.pattern) {
        input.pattern = inputState.pattern;
      } else {
        input.removeAttribute('pattern');
      }
      if (inputState.min) {
        input.min = inputState.min;
      } else {
        input.removeAttribute('min');
      }
      if (inputState.max) {
        input.max = inputState.max;
      } else {
        input.removeAttribute('max');
      }
      if (inputState.step) {
        input.step = inputState.step;
      } else {
        input.removeAttribute('step');
      }
    }
    // Use -1 to represent no minlength and maxlength, which is what the
    // platform input returns. However, it will throw an error if you try to
    // manually set it to -1.
    //
    // While the type is `number`, it may actually be `null` at runtime.
    // `null > -1` is true since `null` coerces to `0`, so we default null and
    // undefined to -1.
    //
    // We set attributes instead of properties since setting a property may
    // throw an out of bounds error in relation to the other property.
    // Attributes will not throw errors while the state is updating.
    if ((state.minLength ?? -1) > -1) {
      inputOrTextArea.setAttribute('minlength', String(state.minLength));
    } else {
      inputOrTextArea.removeAttribute('minlength');
    }
    if ((state.maxLength ?? -1) > -1) {
      inputOrTextArea.setAttribute('maxlength', String(state.maxLength));
    } else {
      inputOrTextArea.removeAttribute('maxlength');
    }
    return {
      validity: inputOrTextArea.validity,
      validationMessage: inputOrTextArea.validationMessage
    };
  }
  equals({
    state: prev
  }, {
    state: next
  }) {
    // Check shared input and textarea properties
    const inputOrTextAreaEqual = prev.type === next.type && prev.value === next.value && prev.required === next.required && prev.minLength === next.minLength && prev.maxLength === next.maxLength;
    if (!isInputState(prev) || !isInputState(next)) {
      // Both are textareas, all relevant properties are equal.
      return inputOrTextAreaEqual;
    }
    // Check additional input-specific properties.
    return inputOrTextAreaEqual && prev.pattern === next.pattern && prev.min === next.min && prev.max === next.max && prev.step === next.step;
  }
  copy({
    state
  }) {
    // Don't hold a reference to the rendered control when copying since we
    // don't use it when checking if the state changed.
    return {
      state: isInputState(state) ? this.copyInput(state) : this.copyTextArea(state),
      renderedControl: null
    };
  }
  copyInput(state) {
    const {
      type,
      pattern,
      min,
      max,
      step
    } = state;
    return {
      ...this.copySharedState(state),
      type,
      pattern,
      min,
      max,
      step
    };
  }
  copyTextArea(state) {
    return {
      ...this.copySharedState(state),
      type: state.type
    };
  }
  copySharedState({
    value,
    required,
    minLength,
    maxLength
  }) {
    return {
      value,
      required,
      minLength,
      maxLength
    };
  }
}
function isInputState(state) {
  return state.type !== 'textarea';
}

/**
 * @license
 * Copyright 2021 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Separate variable needed for closure.
const textFieldBaseClass = mixinDelegatesAria(mixinOnReportValidity(mixinConstraintValidation(mixinFormAssociated(mixinElementInternals(i$2)))));
/**
 * A text field component.
 *
 * @fires select {Event} The native `select` event on
 * [`<input>`](https://developer.mozilla.org/en-US/docs/Web/API/HTMLInputElement/select_event)
 * --bubbles
 * @fires change {Event} The native `change` event on
 * [`<input>`](https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/change_event)
 * --bubbles
 * @fires input {InputEvent} The native `input` event on
 * [`<input>`](https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/input_event)
 * --bubbles --composed
 */
class TextField extends textFieldBaseClass {
  constructor() {
    super(...arguments);
    /**
     * Gets or sets whether or not the text field is in a visually invalid state.
     *
     * This error state overrides the error state controlled by
     * `reportValidity()`.
     */
    this.error = false;
    /**
     * The error message that replaces supporting text when `error` is true. If
     * `errorText` is an empty string, then the supporting text will continue to
     * show.
     *
     * This error message overrides the error message displayed by
     * `reportValidity()`.
     */
    this.errorText = '';
    /**
     * The floating Material label of the textfield component. It informs the user
     * about what information is requested for a text field. It is aligned with
     * the input text, is always visible, and it floats when focused or when text
     * is entered into the textfield. This label also sets accessibilty labels,
     * but the accessible label is overriden by `aria-label`.
     *
     * Learn more about floating labels from the Material Design guidelines:
     * https://m3.material.io/components/text-fields/guidelines
     */
    this.label = '';
    /**
     * Disables the asterisk on the floating label, when the text field is
     * required.
     */
    this.noAsterisk = false;
    /**
     * Indicates that the user must specify a value for the input before the
     * owning form can be submitted and will render an error state when
     * `reportValidity()` is invoked when value is empty. Additionally the
     * floating label will render an asterisk `"*"` when true.
     *
     * https://developer.mozilla.org/en-US/docs/Web/HTML/Attributes/required
     */
    this.required = false;
    /**
     * The current value of the text field. It is always a string.
     */
    this.value = '';
    /**
     * An optional prefix to display before the input value.
     */
    this.prefixText = '';
    /**
     * An optional suffix to display after the input value.
     */
    this.suffixText = '';
    /**
     * Whether or not the text field has a leading icon. Used for SSR.
     */
    this.hasLeadingIcon = false;
    /**
     * Whether or not the text field has a trailing icon. Used for SSR.
     */
    this.hasTrailingIcon = false;
    /**
     * Conveys additional information below the text field, such as how it should
     * be used.
     */
    this.supportingText = '';
    /**
     * Override the input text CSS `direction`. Useful for RTL languages that use
     * LTR notation for fractions.
     */
    this.textDirection = '';
    /**
     * The number of rows to display for a `type="textarea"` text field.
     * Defaults to 2.
     */
    this.rows = 2;
    /**
     * The number of cols to display for a `type="textarea"` text field.
     * Defaults to 20.
     */
    this.cols = 20;
    // <input> properties
    this.inputMode = '';
    /**
     * Defines the greatest value in the range of permitted values.
     *
     * https://developer.mozilla.org/en-US/docs/Web/HTML/Element/input#max
     */
    this.max = '';
    /**
     * The maximum number of characters a user can enter into the text field. Set
     * to -1 for none.
     *
     * https://developer.mozilla.org/en-US/docs/Web/HTML/Element/input#maxlength
     */
    this.maxLength = -1;
    /**
     * Defines the most negative value in the range of permitted values.
     *
     * https://developer.mozilla.org/en-US/docs/Web/HTML/Element/input#min
     */
    this.min = '';
    /**
     * The minimum number of characters a user can enter into the text field. Set
     * to -1 for none.
     *
     * https://developer.mozilla.org/en-US/docs/Web/HTML/Element/input#minlength
     */
    this.minLength = -1;
    /**
     * When true, hide the spinner for `type="number"` text fields.
     */
    this.noSpinner = false;
    /**
     * A regular expression that the text field's value must match to pass
     * constraint validation.
     *
     * https://developer.mozilla.org/en-US/docs/Web/HTML/Element/input#pattern
     */
    this.pattern = '';
    /**
     * Defines the text displayed in the textfield when it has no value. Provides
     * a brief hint to the user as to the expected type of data that should be
     * entered into the control. Unlike `label`, the placeholder is not visible
     * and does not float when the textfield has a value.
     *
     * https://developer.mozilla.org/en-US/docs/Web/HTML/Attributes/placeholder
     */
    this.placeholder = '';
    /**
     * Indicates whether or not a user should be able to edit the text field's
     * value.
     *
     * https://developer.mozilla.org/en-US/docs/Web/HTML/Element/input#readonly
     */
    this.readOnly = false;
    /**
     * Indicates that input accepts multiple email addresses.
     *
     * https://developer.mozilla.org/en-US/docs/Web/HTML/Element/input/email#multiple
     */
    this.multiple = false;
    /**
     * Returns or sets the element's step attribute, which works with min and max
     * to limit the increments at which a numeric or date-time value can be set.
     *
     * https://developer.mozilla.org/en-US/docs/Web/HTML/Element/input#step
     */
    this.step = '';
    /**
     * The `<input>` type to use, defaults to "text". The type greatly changes how
     * the text field behaves.
     *
     * Text fields support a limited number of `<input>` types:
     *
     * - text
     * - textarea
     * - email
     * - number
     * - password
     * - search
     * - tel
     * - url
     *
     * See
     * https://developer.mozilla.org/en-US/docs/Web/HTML/Element/input#input_types
     * for more details on each input type.
     */
    this.type = 'text';
    /**
     * Describes what, if any, type of autocomplete functionality the input
     * should provide.
     *
     * https://developer.mozilla.org/en-US/docs/Web/HTML/Attributes/autocomplete
     */
    this.autocomplete = '';
    /**
     * Returns true when the text field has been interacted with. Native
     * validation errors only display in response to user interactions.
     */
    this.dirty = false;
    this.focused = false;
    /**
     * Whether or not a native error has been reported via `reportValidity()`.
     */
    this.nativeError = false;
    /**
     * The validation message displayed from a native error via
     * `reportValidity()`.
     */
    this.nativeErrorText = '';
  }
  /**
   * Gets or sets the direction in which selection occurred.
   */
  get selectionDirection() {
    return this.getInputOrTextarea().selectionDirection;
  }
  set selectionDirection(value) {
    this.getInputOrTextarea().selectionDirection = value;
  }
  /**
   * Gets or sets the end position or offset of a text selection.
   */
  get selectionEnd() {
    return this.getInputOrTextarea().selectionEnd;
  }
  set selectionEnd(value) {
    this.getInputOrTextarea().selectionEnd = value;
  }
  /**
   * Gets or sets the starting position or offset of a text selection.
   */
  get selectionStart() {
    return this.getInputOrTextarea().selectionStart;
  }
  set selectionStart(value) {
    this.getInputOrTextarea().selectionStart = value;
  }
  /**
   * The text field's value as a number.
   */
  get valueAsNumber() {
    const input = this.getInput();
    if (!input) {
      return NaN;
    }
    return input.valueAsNumber;
  }
  set valueAsNumber(value) {
    const input = this.getInput();
    if (!input) {
      return;
    }
    input.valueAsNumber = value;
    this.value = input.value;
  }
  /**
   * The text field's value as a Date.
   */
  get valueAsDate() {
    const input = this.getInput();
    if (!input) {
      return null;
    }
    return input.valueAsDate;
  }
  set valueAsDate(value) {
    const input = this.getInput();
    if (!input) {
      return;
    }
    input.valueAsDate = value;
    this.value = input.value;
  }
  get hasError() {
    return this.error || this.nativeError;
  }
  /**
   * Selects all the text in the text field.
   *
   * https://developer.mozilla.org/en-US/docs/Web/API/HTMLInputElement/select
   */
  select() {
    this.getInputOrTextarea().select();
  }
  setRangeText(...args) {
    // Calling setRangeText with 1 vs 3-4 arguments has different behavior.
    // Use spread syntax and type casting to ensure correct usage.
    this.getInputOrTextarea().setRangeText(...args);
    this.value = this.getInputOrTextarea().value;
  }
  /**
   * Sets the start and end positions of a selection in the text field.
   *
   * https://developer.mozilla.org/en-US/docs/Web/API/HTMLInputElement/setSelectionRange
   *
   * @param start The offset into the text field for the start of the selection.
   * @param end The offset into the text field for the end of the selection.
   * @param direction The direction in which the selection is performed.
   */
  setSelectionRange(start, end, direction) {
    this.getInputOrTextarea().setSelectionRange(start, end, direction);
  }
  /**
   * Shows the browser picker for an input element of type "date", "time", etc.
   *
   * For a full list of supported types, see:
   * https://developer.mozilla.org/en-US/docs/Web/API/HTMLInputElement/showPicker#browser_compatibility
   *
   * https://developer.mozilla.org/en-US/docs/Web/API/HTMLInputElement/showPicker
   */
  showPicker() {
    const input = this.getInput();
    if (!input) {
      return;
    }
    input.showPicker();
  }
  /**
   * Decrements the value of a numeric type text field by `step` or `n` `step`
   * number of times.
   *
   * https://developer.mozilla.org/en-US/docs/Web/API/HTMLInputElement/stepDown
   *
   * @param stepDecrement The number of steps to decrement, defaults to 1.
   */
  stepDown(stepDecrement) {
    const input = this.getInput();
    if (!input) {
      return;
    }
    input.stepDown(stepDecrement);
    this.value = input.value;
  }
  /**
   * Increments the value of a numeric type text field by `step` or `n` `step`
   * number of times.
   *
   * https://developer.mozilla.org/en-US/docs/Web/API/HTMLInputElement/stepUp
   *
   * @param stepIncrement The number of steps to increment, defaults to 1.
   */
  stepUp(stepIncrement) {
    const input = this.getInput();
    if (!input) {
      return;
    }
    input.stepUp(stepIncrement);
    this.value = input.value;
  }
  /**
   * Reset the text field to its default value.
   */
  reset() {
    this.dirty = false;
    this.value = this.getAttribute('value') ?? '';
    this.nativeError = false;
    this.nativeErrorText = '';
  }
  attributeChangedCallback(attribute, newValue, oldValue) {
    if (attribute === 'value' && this.dirty) {
      // After user input, changing the value attribute no longer updates the
      // text field's value (until reset). This matches native <input> behavior.
      return;
    }
    super.attributeChangedCallback(attribute, newValue, oldValue);
  }
  render() {
    const classes = {
      'disabled': this.disabled,
      'error': !this.disabled && this.hasError,
      'textarea': this.type === 'textarea',
      'no-spinner': this.noSpinner
    };
    return x`
      <span class="text-field ${e$3(classes)}">
        ${this.renderField()}
      </span>
    `;
  }
  updated(changedProperties) {
    // Keep changedProperties arg so that subclasses may call it
    // If a property such as `type` changes and causes the internal <input>
    // value to change without dispatching an event, re-sync it.
    const value = this.getInputOrTextarea().value;
    if (this.value !== value) {
      // Note this is typically inefficient in updated() since it schedules
      // another update. However, it is needed for the <input> to fully render
      // before checking its value.
      this.value = value;
    }
  }
  renderField() {
    return u$1`<${this.fieldTag}
      class="field"
      count=${this.value.length}
      ?disabled=${this.disabled}
      ?error=${this.hasError}
      error-text=${this.getErrorText()}
      ?focused=${this.focused}
      ?has-end=${this.hasTrailingIcon}
      ?has-start=${this.hasLeadingIcon}
      label=${this.label}
      ?no-asterisk=${this.noAsterisk}
      max=${this.maxLength}
      ?populated=${!!this.value}
      ?required=${this.required}
      ?resizable=${this.type === 'textarea'}
      supporting-text=${this.supportingText}
    >
      ${this.renderLeadingIcon()}
      ${this.renderInputOrTextarea()}
      ${this.renderTrailingIcon()}
      <div id="description" slot="aria-describedby"></div>
      <slot name="container" slot="container"></slot>
    </${this.fieldTag}>`;
  }
  renderLeadingIcon() {
    return x`
      <span class="icon leading" slot="start">
        <slot name="leading-icon" @slotchange=${this.handleIconChange}></slot>
      </span>
    `;
  }
  renderTrailingIcon() {
    return x`
      <span class="icon trailing" slot="end">
        <slot name="trailing-icon" @slotchange=${this.handleIconChange}></slot>
      </span>
    `;
  }
  renderInputOrTextarea() {
    const style = {
      'direction': this.textDirection
    };
    const ariaLabel = this.ariaLabel || this.label || E;
    // lit-anaylzer `autocomplete` types are too strict
    // tslint:disable-next-line:no-any
    const autocomplete = this.autocomplete;
    // These properties may be set to null if the attribute is removed, and
    // `null > -1` is incorrectly `true`.
    const hasMaxLength = (this.maxLength ?? -1) > -1;
    const hasMinLength = (this.minLength ?? -1) > -1;
    if (this.type === 'textarea') {
      return x`
        <textarea
          class="input"
          style=${o(style)}
          aria-describedby="description"
          aria-invalid=${this.hasError}
          aria-label=${ariaLabel}
          autocomplete=${autocomplete || E}
          name=${this.name || E}
          ?disabled=${this.disabled}
          maxlength=${hasMaxLength ? this.maxLength : E}
          minlength=${hasMinLength ? this.minLength : E}
          placeholder=${this.placeholder || E}
          ?readonly=${this.readOnly}
          ?required=${this.required}
          rows=${this.rows}
          cols=${this.cols}
          .value=${l(this.value)}
          @change=${this.redispatchEvent}
          @focus=${this.handleFocusChange}
          @blur=${this.handleFocusChange}
          @input=${this.handleInput}
          @select=${this.redispatchEvent}></textarea>
      `;
    }
    const prefix = this.renderPrefix();
    const suffix = this.renderSuffix();
    // TODO(b/243805848): remove `as unknown as number` and `as any` once lit
    // analyzer is fixed
    // tslint:disable-next-line:no-any
    const inputMode = this.inputMode;
    return x`
      <div class="input-wrapper">
        ${prefix}
        <input
          class="input"
          style=${o(style)}
          aria-describedby="description"
          aria-invalid=${this.hasError}
          aria-label=${ariaLabel}
          autocomplete=${autocomplete || E}
          name=${this.name || E}
          ?disabled=${this.disabled}
          inputmode=${inputMode || E}
          max=${this.max || E}
          maxlength=${hasMaxLength ? this.maxLength : E}
          min=${this.min || E}
          minlength=${hasMinLength ? this.minLength : E}
          pattern=${this.pattern || E}
          placeholder=${this.placeholder || E}
          ?readonly=${this.readOnly}
          ?required=${this.required}
          ?multiple=${this.multiple}
          step=${this.step || E}
          type=${this.type}
          .value=${l(this.value)}
          @change=${this.redispatchEvent}
          @focus=${this.handleFocusChange}
          @blur=${this.handleFocusChange}
          @input=${this.handleInput}
          @select=${this.redispatchEvent} />
        ${suffix}
      </div>
    `;
  }
  renderPrefix() {
    return this.renderAffix(this.prefixText, /* isSuffix */false);
  }
  renderSuffix() {
    return this.renderAffix(this.suffixText, /* isSuffix */true);
  }
  renderAffix(text, isSuffix) {
    if (!text) {
      return E;
    }
    const classes = {
      'suffix': isSuffix,
      'prefix': !isSuffix
    };
    return x`<span class="${e$3(classes)}">${text}</span>`;
  }
  getErrorText() {
    return this.error ? this.errorText : this.nativeErrorText;
  }
  handleFocusChange() {
    var _this$inputOrTextarea;
    // When calling focus() or reportValidity() during change, it's possible
    // for blur to be called after the new focus event. Rather than set
    // `this.focused` to true/false on focus/blur, we always set it to whether
    // or not the input itself is focused.
    this.focused = ((_this$inputOrTextarea = this.inputOrTextarea) === null || _this$inputOrTextarea === void 0 ? void 0 : _this$inputOrTextarea.matches(':focus')) ?? false;
  }
  handleInput(event) {
    this.dirty = true;
    this.value = event.target.value;
  }
  redispatchEvent(event) {
    redispatchEvent(this, event);
  }
  getInputOrTextarea() {
    if (!this.inputOrTextarea) {
      // If the input is not yet defined, synchronously render.
      // e.g.
      // const textField = document.createElement('md-outlined-text-field');
      // document.body.appendChild(textField);
      // textField.focus(); // synchronously render
      this.connectedCallback();
      this.scheduleUpdate();
    }
    if (this.isUpdatePending) {
      // If there are pending updates, synchronously perform them. This ensures
      // that constraint validation properties (like `required`) are synced
      // before interacting with input APIs that depend on them.
      this.scheduleUpdate();
    }
    return this.inputOrTextarea;
  }
  getInput() {
    if (this.type === 'textarea') {
      return null;
    }
    return this.getInputOrTextarea();
  }
  handleIconChange() {
    this.hasLeadingIcon = this.leadingIcons.length > 0;
    this.hasTrailingIcon = this.trailingIcons.length > 0;
  }
  [getFormValue]() {
    return this.value;
  }
  formResetCallback() {
    this.reset();
  }
  formStateRestoreCallback(state) {
    this.value = state;
  }
  focus() {
    // Required for the case that the user slots a focusable element into the
    // leading icon slot such as an iconbutton due to how delegatesFocus works.
    this.getInputOrTextarea().focus();
  }
  [createValidator]() {
    return new TextFieldValidator(() => ({
      state: this,
      renderedControl: this.inputOrTextarea
    }));
  }
  [getValidityAnchor]() {
    return this.inputOrTextarea;
  }
  [onReportValidity](invalidEvent) {
    // Prevent default pop-up behavior.
    invalidEvent === null || invalidEvent === void 0 || invalidEvent.preventDefault();
    const prevMessage = this.getErrorText();
    this.nativeError = !!invalidEvent;
    this.nativeErrorText = this.validationMessage;
    if (prevMessage === this.getErrorText()) {
      var _this$field;
      (_this$field = this.field) === null || _this$field === void 0 || _this$field.reannounceError();
    }
  }
}
/** @nocollapse */
TextField.shadowRootOptions = {
  ...i$2.shadowRootOptions,
  delegatesFocus: true
};
__decorate([n$3({
  type: Boolean,
  reflect: true
})], TextField.prototype, "error", void 0);
__decorate([n$3({
  attribute: 'error-text'
})], TextField.prototype, "errorText", void 0);
__decorate([n$3()], TextField.prototype, "label", void 0);
__decorate([n$3({
  type: Boolean,
  attribute: 'no-asterisk'
})], TextField.prototype, "noAsterisk", void 0);
__decorate([n$3({
  type: Boolean,
  reflect: true
})], TextField.prototype, "required", void 0);
__decorate([n$3()], TextField.prototype, "value", void 0);
__decorate([n$3({
  attribute: 'prefix-text'
})], TextField.prototype, "prefixText", void 0);
__decorate([n$3({
  attribute: 'suffix-text'
})], TextField.prototype, "suffixText", void 0);
__decorate([n$3({
  type: Boolean,
  attribute: 'has-leading-icon'
})], TextField.prototype, "hasLeadingIcon", void 0);
__decorate([n$3({
  type: Boolean,
  attribute: 'has-trailing-icon'
})], TextField.prototype, "hasTrailingIcon", void 0);
__decorate([n$3({
  attribute: 'supporting-text'
})], TextField.prototype, "supportingText", void 0);
__decorate([n$3({
  attribute: 'text-direction'
})], TextField.prototype, "textDirection", void 0);
__decorate([n$3({
  type: Number
})], TextField.prototype, "rows", void 0);
__decorate([n$3({
  type: Number
})], TextField.prototype, "cols", void 0);
__decorate([n$3({
  reflect: true
})], TextField.prototype, "inputMode", void 0);
__decorate([n$3()], TextField.prototype, "max", void 0);
__decorate([n$3({
  type: Number
})], TextField.prototype, "maxLength", void 0);
__decorate([n$3()], TextField.prototype, "min", void 0);
__decorate([n$3({
  type: Number
})], TextField.prototype, "minLength", void 0);
__decorate([n$3({
  type: Boolean,
  attribute: 'no-spinner'
})], TextField.prototype, "noSpinner", void 0);
__decorate([n$3()], TextField.prototype, "pattern", void 0);
__decorate([n$3({
  reflect: true,
  converter: stringConverter
})], TextField.prototype, "placeholder", void 0);
__decorate([n$3({
  type: Boolean,
  reflect: true
})], TextField.prototype, "readOnly", void 0);
__decorate([n$3({
  type: Boolean,
  reflect: true
})], TextField.prototype, "multiple", void 0);
__decorate([n$3()], TextField.prototype, "step", void 0);
__decorate([n$3({
  reflect: true
})], TextField.prototype, "type", void 0);
__decorate([n$3({
  reflect: true
})], TextField.prototype, "autocomplete", void 0);
__decorate([r$1()], TextField.prototype, "dirty", void 0);
__decorate([r$1()], TextField.prototype, "focused", void 0);
__decorate([r$1()], TextField.prototype, "nativeError", void 0);
__decorate([r$1()], TextField.prototype, "nativeErrorText", void 0);
__decorate([e$2('.input')], TextField.prototype, "inputOrTextarea", void 0);
__decorate([e$2('.field')], TextField.prototype, "field", void 0);
__decorate([o$2({
  slot: 'leading-icon'
})], TextField.prototype, "leadingIcons", void 0);
__decorate([o$2({
  slot: 'trailing-icon'
})], TextField.prototype, "trailingIcons", void 0);

/**
 * @license
 * Copyright 2021 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * A filled text field component.
 */
class FilledTextField extends TextField {
  constructor() {
    super(...arguments);
    this.fieldTag = i$1`md-filled-field`;
  }
}

/**
 * @license
 * Copyright 2024 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Generated stylesheet for ./textfield/internal/shared-styles.css.
const styles$6 = i$3`:host{display:inline-flex;outline:none;resize:both;text-align:start;-webkit-tap-highlight-color:rgba(0,0,0,0)}.text-field,.field{width:100%}.text-field{display:inline-flex}.field{cursor:text}.disabled .field{cursor:default}.text-field,.textarea .field{resize:inherit}slot[name=container]{border-radius:inherit}.icon{color:currentColor;display:flex;align-items:center;justify-content:center;fill:currentColor;position:relative}.icon ::slotted(*){display:flex;position:absolute}[has-start] .icon.leading{font-size:var(--_leading-icon-size);height:var(--_leading-icon-size);width:var(--_leading-icon-size)}[has-end] .icon.trailing{font-size:var(--_trailing-icon-size);height:var(--_trailing-icon-size);width:var(--_trailing-icon-size)}.input-wrapper{display:flex}.input-wrapper>*{all:inherit;padding:0}.input{caret-color:var(--_caret-color);overflow-x:hidden;text-align:inherit}.input::placeholder{color:currentColor;opacity:1}.input::-webkit-calendar-picker-indicator{display:none}.input::-webkit-search-decoration,.input::-webkit-search-cancel-button{display:none}@media(forced-colors: active){.input{background:none}}.no-spinner .input::-webkit-inner-spin-button,.no-spinner .input::-webkit-outer-spin-button{display:none}.no-spinner .input[type=number]{-moz-appearance:textfield}:focus-within .input{caret-color:var(--_focus-caret-color)}.error:focus-within .input{caret-color:var(--_error-focus-caret-color)}.text-field:not(.disabled) .prefix{color:var(--_input-text-prefix-color)}.text-field:not(.disabled) .suffix{color:var(--_input-text-suffix-color)}.text-field:not(.disabled) .input::placeholder{color:var(--_input-text-placeholder-color)}.prefix,.suffix{text-wrap:nowrap;width:min-content}.prefix{padding-inline-end:var(--_input-text-prefix-trailing-space)}.suffix{padding-inline-start:var(--_input-text-suffix-leading-space)}
`;

class EwFilledTextField extends FilledTextField {
  constructor() {
    super(...arguments);
    this.fieldTag = i$1`md-filled-field`;
  }
}
EwFilledTextField.styles = [styles$6, styles$9];
customElements.define("ew-filled-text-field", EwFilledTextField);

/**
 * @license
 * Copyright 2022 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * A component for elevation.
 */
class Elevation extends i$2 {
  connectedCallback() {
    super.connectedCallback();
    // Needed for VoiceOver, which will create a "group" if the element is a
    // sibling to other content.
    this.setAttribute('aria-hidden', 'true');
  }
  render() {
    return x`<span class="shadow"></span>`;
  }
}

/**
 * @license
 * Copyright 2024 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Generated stylesheet for ./elevation/internal/elevation-styles.css.
const styles$5 = i$3`:host,.shadow,.shadow::before,.shadow::after{border-radius:inherit;inset:0;position:absolute;transition-duration:inherit;transition-property:inherit;transition-timing-function:inherit}:host{display:flex;pointer-events:none;transition-property:box-shadow,opacity}.shadow::before,.shadow::after{content:"";transition-property:box-shadow,opacity;--_level: var(--md-elevation-level, 0);--_shadow-color: var(--md-elevation-shadow-color, var(--md-sys-color-shadow, #000))}.shadow::before{box-shadow:0px calc(1px*(clamp(0,var(--_level),1) + clamp(0,var(--_level) - 3,1) + 2*clamp(0,var(--_level) - 4,1))) calc(1px*(2*clamp(0,var(--_level),1) + clamp(0,var(--_level) - 2,1) + clamp(0,var(--_level) - 4,1))) 0px var(--_shadow-color);opacity:.3}.shadow::after{box-shadow:0px calc(1px*(clamp(0,var(--_level),1) + clamp(0,var(--_level) - 1,1) + 2*clamp(0,var(--_level) - 2,3))) calc(1px*(3*clamp(0,var(--_level),2) + 2*clamp(0,var(--_level) - 2,3))) calc(1px*(clamp(0,var(--_level),4) + 2*clamp(0,var(--_level) - 4,1))) var(--_shadow-color);opacity:.15}
`;

/**
 * @license
 * Copyright 2022 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * The `<md-elevation>` custom element with default styles.
 *
 * Elevation is the relative distance between two surfaces along the z-axis.
 *
 * @final
 * @suppress {visibility}
 */
let MdElevation = class MdElevation extends Elevation {};
MdElevation.styles = [styles$5];
MdElevation = __decorate([t('md-elevation')], MdElevation);

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * Creates an event that closes any parent menus.
 */
function createCloseMenuEvent(initiator, reason) {
  return new CustomEvent('close-menu', {
    bubbles: true,
    composed: true,
    detail: {
      initiator,
      reason,
      itemPath: [initiator]
    }
  });
}
/**
 * Creates a default close menu event used by md-menu.
 */
const createDefaultCloseMenuEvent = createCloseMenuEvent;
/**
 * Keys that are used for selection in menus.
 */
// tslint:disable-next-line:enforce-name-casing We are mimicking enum style
const SelectionKey = {
  SPACE: 'Space',
  ENTER: 'Enter'
};
/**
 * Default close `Reason` kind values.
 */
// tslint:disable-next-line:enforce-name-casing We are mimicking enum style
const CloseReason = {
  CLICK_SELECTION: 'click-selection',
  KEYDOWN: 'keydown'
};
/**
 * Keys that can close menus.
 */
// tslint:disable-next-line:enforce-name-casing We are mimicking enum style
const KeydownCloseKey = {
  ESCAPE: 'Escape',
  SPACE: SelectionKey.SPACE,
  ENTER: SelectionKey.ENTER
};
/**
 * Determines whether the given key code is a key code that should close the
 * menu.
 *
 * @param code The KeyboardEvent code to check.
 * @return Whether or not the key code is in the predetermined list to close the
 * menu.
 */
function isClosableKey(code) {
  return Object.values(KeydownCloseKey).some(value => value === code);
}
/**
 * Determines whether the given key code is a key code that should select a menu
 * item.
 *
 * @param code They KeyboardEvent code to check.
 * @return Whether or not the key code is in the predetermined list to select a
 * menu item.
 */
function isSelectableKey(code) {
  return Object.values(SelectionKey).some(value => value === code);
}
/**
 * Determines whether a target element is contained inside another element's
 * composed tree.
 *
 * @param target The potential contained element.
 * @param container The potential containing element of the target.
 * @returns Whether the target element is contained inside the container's
 * composed subtree
 */
function isElementInSubtree(target, container) {
  // Dispatch a composed, bubbling event to check its path to see if the
  // newly-focused element is contained in container's subtree
  const focusEv = new Event('md-contains', {
    bubbles: true,
    composed: true
  });
  let composedPath = [];
  const listener = ev => {
    composedPath = ev.composedPath();
  };
  container.addEventListener('md-contains', listener);
  target.dispatchEvent(focusEv);
  container.removeEventListener('md-contains', listener);
  const isContained = composedPath.length > 0;
  return isContained;
}
/**
 * Element to focus on when menu is first opened.
 */
// tslint:disable-next-line:enforce-name-casing We are mimicking enum style
const FocusState = {
  NONE: 'none',
  LIST_ROOT: 'list-root',
  FIRST_ITEM: 'first-item',
  LAST_ITEM: 'last-item'
};

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * An enum of supported Menu corners
 */
// tslint:disable-next-line:enforce-name-casing We are mimicking enum style
const Corner = {
  END_START: 'end-start',
  START_START: 'start-start'};
/**
 * Given a surface, an anchor, corners, and some options, this surface will
 * calculate the position of a surface to align the two given corners and keep
 * the surface inside the window viewport. It also provides a StyleInfo map that
 * can be applied to the surface to handle visiblility and position.
 */
class SurfacePositionController {
  /**
   * @param host The host to connect the controller to.
   * @param getProperties A function that returns the properties for the
   * controller.
   */
  constructor(host, getProperties) {
    this.host = host;
    this.getProperties = getProperties;
    // The current styles to apply to the surface.
    this.surfaceStylesInternal = {
      'display': 'none'
    };
    // Previous values stored for change detection. Open change detection is
    // calculated separately so initialize it here.
    this.lastValues = {
      isOpen: false
    };
    this.host.addController(this);
  }
  /**
   * The StyleInfo map to apply to the surface via Lit's stylemap
   */
  get surfaceStyles() {
    return this.surfaceStylesInternal;
  }
  /**
   * Calculates the surface's new position required so that the surface's
   * `surfaceCorner` aligns to the anchor's `anchorCorner` while keeping the
   * surface inside the window viewport. This positioning also respects RTL by
   * checking `getComputedStyle()` on the surface element.
   */
  async position() {
    const {
      surfaceEl,
      anchorEl,
      anchorCorner: anchorCornerRaw,
      surfaceCorner: surfaceCornerRaw,
      positioning,
      xOffset,
      yOffset,
      disableBlockFlip,
      disableInlineFlip,
      repositionStrategy
    } = this.getProperties();
    const anchorCorner = anchorCornerRaw.toLowerCase().trim();
    const surfaceCorner = surfaceCornerRaw.toLowerCase().trim();
    if (!surfaceEl || !anchorEl) {
      return;
    }
    // Store these before we potentially resize the window with the next set of
    // lines
    const windowInnerWidth = window.innerWidth;
    const windowInnerHeight = window.innerHeight;
    const div = document.createElement('div');
    div.style.opacity = '0';
    div.style.position = 'fixed';
    div.style.display = 'block';
    div.style.inset = '0';
    document.body.appendChild(div);
    const scrollbarTestRect = div.getBoundingClientRect();
    div.remove();
    // Calculate the widths of the scrollbars in the inline and block directions
    // to account for window-relative calculations.
    const blockScrollbarHeight = window.innerHeight - scrollbarTestRect.bottom;
    const inlineScrollbarWidth = window.innerWidth - scrollbarTestRect.right;
    // Paint the surface transparently so that we can get the position and the
    // rect info of the surface.
    this.surfaceStylesInternal = {
      'display': 'block',
      'opacity': '0'
    };
    // Wait for it to be visible.
    this.host.requestUpdate();
    await this.host.updateComplete;
    // Safari has a bug that makes popovers render incorrectly if the node is
    // made visible + Animation Frame before calling showPopover().
    // https://bugs.webkit.org/show_bug.cgi?id=264069
    // also the cast is required due to differing TS types in Google and OSS.
    if (surfaceEl.popover && surfaceEl.isConnected) {
      surfaceEl.showPopover();
    }
    const surfaceRect = surfaceEl.getSurfacePositionClientRect ? surfaceEl.getSurfacePositionClientRect() : surfaceEl.getBoundingClientRect();
    const anchorRect = anchorEl.getSurfacePositionClientRect ? anchorEl.getSurfacePositionClientRect() : anchorEl.getBoundingClientRect();
    const [surfaceBlock, surfaceInline] = surfaceCorner.split('-');
    const [anchorBlock, anchorInline] = anchorCorner.split('-');
    // LTR depends on the direction of the SURFACE not the anchor.
    const isLTR = getComputedStyle(surfaceEl).direction === 'ltr';
    /*
     * For more on inline and block dimensions, see MDN article:
     * https://developer.mozilla.org/en-US/docs/Web/CSS/CSS_logical_properties_and_values
     *
     * ┌───── inline/blockDocumentOffset  inlineScrollbarWidth
     * │       │                                    │
     * │     ┌─▼─────┐                              │Document
     * │    ┌┼───────┴──────────────────────────────┼────────┐
     * │    ││                                      │        │
     * └──► ││ ┌───── inline/blockWindowOffset      │        │
     *      ││ │       │                            ▼        │
     *      ││ │     ┌─▼───┐                 Window┌┐        │
     *      └┤ │    ┌┼─────┴───────────────────────┼│        │
     *       │ │    ││                             ││        │
     *       │ └──► ││  ┌──inline/blockAnchorOffset││        │
     *       │      ││  │     │                    ││        │
     *       │      └┤  │  ┌──▼───┐                ││        │
     *       │       │  │ ┌┼──────┤                ││        │
     *       │       │  └─►│Anchor│                ││        │
     *       │       │    └┴──────┘                ││        │
     *       │       │                             ││        │
     *       │       │     ┌───────────────────────┼┼────┐   │
     *       │       │     │ Surface               ││    │   │
     *       │       │     │                       ││    │   │
     *       │       │     │                       ││    │   │
     *       │       │     │                       ││    │   │
     *       │       │     │                       ││    │   │
     *       │      ┌┼─────┼───────────────────────┼│    │   │
     *       │   ┌─►┴──────┼────────────────────────┘    ├┐  │
     *       │   │         │ inline/blockOOBCorrection   ││  │
     *       │   │         │                         │   ││  │
     *       │   │         │                         ├──►├│  │
     *       │   │         │                         │   ││  │
     *       │   │         └────────────────────────┐▼───┼┘  │
     *       │  blockScrollbarHeight                └────┘   │
     *       │                                               │
     *       └───────────────────────────────────────────────┘
     */
    // Calculate the block positioning properties
    let {
      blockInset,
      blockOutOfBoundsCorrection,
      surfaceBlockProperty
    } = this.calculateBlock({
      surfaceRect,
      anchorRect,
      anchorBlock,
      surfaceBlock,
      yOffset,
      positioning,
      windowInnerHeight,
      blockScrollbarHeight
    });
    // If the surface should be out of bounds in the block direction, flip the
    // surface and anchor corner block values and recalculate
    if (blockOutOfBoundsCorrection && !disableBlockFlip) {
      const flippedSurfaceBlock = surfaceBlock === 'start' ? 'end' : 'start';
      const flippedAnchorBlock = anchorBlock === 'start' ? 'end' : 'start';
      const flippedBlock = this.calculateBlock({
        surfaceRect,
        anchorRect,
        anchorBlock: flippedAnchorBlock,
        surfaceBlock: flippedSurfaceBlock,
        yOffset,
        positioning,
        windowInnerHeight,
        blockScrollbarHeight
      });
      // In the case that the flipped verion would require less out of bounds
      // correcting, use the flipped corner block values
      if (blockOutOfBoundsCorrection > flippedBlock.blockOutOfBoundsCorrection) {
        blockInset = flippedBlock.blockInset;
        blockOutOfBoundsCorrection = flippedBlock.blockOutOfBoundsCorrection;
        surfaceBlockProperty = flippedBlock.surfaceBlockProperty;
      }
    }
    // Calculate the inline positioning properties
    let {
      inlineInset,
      inlineOutOfBoundsCorrection,
      surfaceInlineProperty
    } = this.calculateInline({
      surfaceRect,
      anchorRect,
      anchorInline,
      surfaceInline,
      xOffset,
      positioning,
      isLTR,
      windowInnerWidth,
      inlineScrollbarWidth
    });
    // If the surface should be out of bounds in the inline direction, flip the
    // surface and anchor corner inline values and recalculate
    if (inlineOutOfBoundsCorrection && !disableInlineFlip) {
      const flippedSurfaceInline = surfaceInline === 'start' ? 'end' : 'start';
      const flippedAnchorInline = anchorInline === 'start' ? 'end' : 'start';
      const flippedInline = this.calculateInline({
        surfaceRect,
        anchorRect,
        anchorInline: flippedAnchorInline,
        surfaceInline: flippedSurfaceInline,
        xOffset,
        positioning,
        isLTR,
        windowInnerWidth,
        inlineScrollbarWidth
      });
      // In the case that the flipped verion would require less out of bounds
      // correcting, use the flipped corner inline values
      if (Math.abs(inlineOutOfBoundsCorrection) > Math.abs(flippedInline.inlineOutOfBoundsCorrection)) {
        inlineInset = flippedInline.inlineInset;
        inlineOutOfBoundsCorrection = flippedInline.inlineOutOfBoundsCorrection;
        surfaceInlineProperty = flippedInline.surfaceInlineProperty;
      }
    }
    // If we are simply repositioning the surface back inside the viewport,
    // subtract the out of bounds correction values from the positioning.
    if (repositionStrategy === 'move') {
      blockInset = blockInset - blockOutOfBoundsCorrection;
      inlineInset = inlineInset - inlineOutOfBoundsCorrection;
    }
    this.surfaceStylesInternal = {
      'display': 'block',
      'opacity': '1',
      [surfaceBlockProperty]: `${blockInset}px`,
      [surfaceInlineProperty]: `${inlineInset}px`
    };
    // In the case that we are resizing the surface to stay inside the viewport
    // we need to set height and width on the surface.
    if (repositionStrategy === 'resize') {
      // Add a height property to the styles if there is block height correction
      if (blockOutOfBoundsCorrection) {
        this.surfaceStylesInternal['height'] = `${surfaceRect.height - blockOutOfBoundsCorrection}px`;
      }
      // Add a width property to the styles if there is block height correction
      if (inlineOutOfBoundsCorrection) {
        this.surfaceStylesInternal['width'] = `${surfaceRect.width - inlineOutOfBoundsCorrection}px`;
      }
    }
    this.host.requestUpdate();
  }
  /**
   * Calculates the css property, the inset, and the out of bounds correction
   * for the surface in the block direction.
   */
  calculateBlock(config) {
    const {
      surfaceRect,
      anchorRect,
      anchorBlock,
      surfaceBlock,
      yOffset,
      positioning,
      windowInnerHeight,
      blockScrollbarHeight
    } = config;
    // We use number booleans to multiply values rather than `if` / ternary
    // statements because it _heavily_ cuts down on nesting and readability
    const relativeToWindow = positioning === 'fixed' || positioning === 'document' ? 1 : 0;
    const relativeToDocument = positioning === 'document' ? 1 : 0;
    const isSurfaceBlockStart = surfaceBlock === 'start' ? 1 : 0;
    const isSurfaceBlockEnd = surfaceBlock === 'end' ? 1 : 0;
    const isOneBlockEnd = anchorBlock !== surfaceBlock ? 1 : 0;
    // Whether or not to apply the height of the anchor
    const blockAnchorOffset = isOneBlockEnd * anchorRect.height + yOffset;
    // The absolute block position of the anchor relative to window
    const blockTopLayerOffset = isSurfaceBlockStart * anchorRect.top + isSurfaceBlockEnd * (windowInnerHeight - anchorRect.bottom - blockScrollbarHeight);
    const blockDocumentOffset = isSurfaceBlockStart * window.scrollY - isSurfaceBlockEnd * window.scrollY;
    // If the surface's block would be out of bounds of the window, move it back
    // in
    const blockOutOfBoundsCorrection = Math.abs(Math.min(0, windowInnerHeight - blockTopLayerOffset - blockAnchorOffset - surfaceRect.height));
    // The block logical value of the surface
    const blockInset = relativeToWindow * blockTopLayerOffset + relativeToDocument * blockDocumentOffset + blockAnchorOffset;
    const surfaceBlockProperty = surfaceBlock === 'start' ? 'inset-block-start' : 'inset-block-end';
    return {
      blockInset,
      blockOutOfBoundsCorrection,
      surfaceBlockProperty
    };
  }
  /**
   * Calculates the css property, the inset, and the out of bounds correction
   * for the surface in the inline direction.
   */
  calculateInline(config) {
    const {
      isLTR: isLTRBool,
      surfaceInline,
      anchorInline,
      anchorRect,
      surfaceRect,
      xOffset,
      positioning,
      windowInnerWidth,
      inlineScrollbarWidth
    } = config;
    // We use number booleans to multiply values rather than `if` / ternary
    // statements because it _heavily_ cuts down on nesting and readability
    const relativeToWindow = positioning === 'fixed' || positioning === 'document' ? 1 : 0;
    const relativeToDocument = positioning === 'document' ? 1 : 0;
    const isLTR = isLTRBool ? 1 : 0;
    const isRTL = isLTRBool ? 0 : 1;
    const isSurfaceInlineStart = surfaceInline === 'start' ? 1 : 0;
    const isSurfaceInlineEnd = surfaceInline === 'end' ? 1 : 0;
    const isOneInlineEnd = anchorInline !== surfaceInline ? 1 : 0;
    // Whether or not to apply the width of the anchor
    const inlineAnchorOffset = isOneInlineEnd * anchorRect.width + xOffset;
    // The inline position of the anchor relative to window in LTR
    const inlineTopLayerOffsetLTR = isSurfaceInlineStart * anchorRect.left + isSurfaceInlineEnd * (windowInnerWidth - anchorRect.right - inlineScrollbarWidth);
    // The inline position of the anchor relative to window in RTL
    const inlineTopLayerOffsetRTL = isSurfaceInlineStart * (windowInnerWidth - anchorRect.right - inlineScrollbarWidth) + isSurfaceInlineEnd * anchorRect.left;
    // The inline position of the anchor relative to window
    const inlineTopLayerOffset = isLTR * inlineTopLayerOffsetLTR + isRTL * inlineTopLayerOffsetRTL;
    // The inline position of the anchor relative to window in LTR
    const inlineDocumentOffsetLTR = isSurfaceInlineStart * window.scrollX - isSurfaceInlineEnd * window.scrollX;
    // The inline position of the anchor relative to window in RTL
    const inlineDocumentOffsetRTL = isSurfaceInlineEnd * window.scrollX - isSurfaceInlineStart * window.scrollX;
    // The inline position of the anchor relative to window
    const inlineDocumentOffset = isLTR * inlineDocumentOffsetLTR + isRTL * inlineDocumentOffsetRTL;
    // If the surface's inline would be out of bounds of the window, move it
    // back in
    const inlineOutOfBoundsCorrection = Math.abs(Math.min(0, windowInnerWidth - inlineTopLayerOffset - inlineAnchorOffset - surfaceRect.width));
    // The inline logical value of the surface
    const inlineInset = relativeToWindow * inlineTopLayerOffset + inlineAnchorOffset + relativeToDocument * inlineDocumentOffset;
    let surfaceInlineProperty = surfaceInline === 'start' ? 'inset-inline-start' : 'inset-inline-end';
    // There are cases where the element is RTL but the root of the page is not.
    // In these cases we want to not use logical properties.
    if (positioning === 'document' || positioning === 'fixed') {
      if (surfaceInline === 'start' && isLTRBool || surfaceInline === 'end' && !isLTRBool) {
        surfaceInlineProperty = 'left';
      } else {
        surfaceInlineProperty = 'right';
      }
    }
    return {
      inlineInset,
      inlineOutOfBoundsCorrection,
      surfaceInlineProperty
    };
  }
  hostUpdate() {
    this.onUpdate();
  }
  hostUpdated() {
    this.onUpdate();
  }
  /**
   * Checks whether the properties passed into the controller have changed since
   * the last positioning. If so, it will reposition if the surface is open or
   * close it if the surface should close.
   */
  async onUpdate() {
    const props = this.getProperties();
    let hasChanged = false;
    for (const [key, value] of Object.entries(props)) {
      // tslint:disable-next-line
      hasChanged = hasChanged || value !== this.lastValues[key];
      if (hasChanged) break;
    }
    const openChanged = this.lastValues.isOpen !== props.isOpen;
    const hasAnchor = !!props.anchorEl;
    const hasSurface = !!props.surfaceEl;
    if (hasChanged && hasAnchor && hasSurface) {
      // Only update isOpen, because if it's closed, we do not want to waste
      // time on a useless reposition calculation. So save the other "dirty"
      // values until next time it opens.
      this.lastValues.isOpen = props.isOpen;
      if (props.isOpen) {
        // We are going to do a reposition, so save the prop values for future
        // dirty checking.
        this.lastValues = props;
        await this.position();
        props.onOpen();
      } else if (openChanged) {
        await props.beforeClose();
        this.close();
        props.onClose();
      }
    }
  }
  /**
   * Hides the surface.
   */
  close() {
    this.surfaceStylesInternal = {
      'display': 'none'
    };
    this.host.requestUpdate();
    const surfaceEl = this.getProperties().surfaceEl;
    // The following type casts are required due to differing TS types in Google
    // and open source.
    if (surfaceEl !== null && surfaceEl !== void 0 && surfaceEl.popover && surfaceEl !== null && surfaceEl !== void 0 && surfaceEl.isConnected) {
      surfaceEl.hidePopover();
    }
  }
}

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * Indicies to access the TypeaheadRecord tuple type.
 */
const TYPEAHEAD_RECORD = {
  INDEX: 0,
  ITEM: 1,
  TEXT: 2
};
/**
 * This controller listens to `keydown` events and searches the header text of
 * an array of `MenuItem`s with the corresponding entered keys within the buffer
 * time and activates the item.
 *
 * @example
 * ```ts
 * const typeaheadController = new TypeaheadController(() => ({
 *   typeaheadBufferTime: 50,
 *   getItems: () => Array.from(document.querySelectorAll('md-menu-item'))
 * }));
 * html`
 *   <div
 *       @keydown=${typeaheadController.onKeydown}
 *       tabindex="0"
 *       class="activeItemText">
 *     <!-- focusable element that will receive keydown events -->
 *     Apple
 *   </div>
 *   <div>
 *     <md-menu-item active header="Apple"></md-menu-item>
 *     <md-menu-item header="Apricot"></md-menu-item>
 *     <md-menu-item header="Banana"></md-menu-item>
 *     <md-menu-item header="Olive"></md-menu-item>
 *     <md-menu-item header="Orange"></md-menu-item>
 *   </div>
 * `;
 * ```
 */
class TypeaheadController {
  /**
   * @param getProperties A function that returns the options of the typeahead
   * controller:
   *
   * {
   *   getItems: A function that returns an array of menu items to be searched.
   *   typeaheadBufferTime: The maximum time between each keystroke to keep the
   *       current type buffer alive.
   * }
   */
  constructor(getProperties) {
    this.getProperties = getProperties;
    /**
     * Array of tuples that helps with indexing.
     */
    this.typeaheadRecords = [];
    /**
     * Currently-typed text since last buffer timeout
     */
    this.typaheadBuffer = '';
    /**
     * The timeout id from the current buffer's setTimeout
     */
    this.cancelTypeaheadTimeout = 0;
    /**
     * If we are currently "typing"
     */
    this.isTypingAhead = false;
    /**
     * The record of the last active item.
     */
    this.lastActiveRecord = null;
    /**
     * Apply this listener to the element that will receive `keydown` events that
     * should trigger this controller.
     *
     * @param event The native browser `KeyboardEvent` from the `keydown` event.
     */
    this.onKeydown = event => {
      if (this.isTypingAhead) {
        this.typeahead(event);
      } else {
        this.beginTypeahead(event);
      }
    };
    /**
     * Ends the current typeahead and clears the buffer.
     */
    this.endTypeahead = () => {
      this.isTypingAhead = false;
      this.typaheadBuffer = '';
      this.typeaheadRecords = [];
    };
  }
  get items() {
    return this.getProperties().getItems();
  }
  get active() {
    return this.getProperties().active;
  }
  /**
   * Sets up typingahead
   */
  beginTypeahead(event) {
    if (!this.active) {
      return;
    }
    // We don't want to typeahead if the _beginning_ of the typeahead is a menu
    // navigation, or a selection. We will handle "Space" only if it's in the
    // middle of a typeahead
    if (event.code === 'Space' || event.code === 'Enter' || event.code.startsWith('Arrow') || event.code === 'Escape') {
      return;
    }
    this.isTypingAhead = true;
    // Generates the record array data structure which is the index, the element
    // and a normalized header.
    this.typeaheadRecords = this.items.map((el, index) => [index, el, el.typeaheadText.trim().toLowerCase()]);
    this.lastActiveRecord = this.typeaheadRecords.find(record => record[TYPEAHEAD_RECORD.ITEM].tabIndex === 0) ?? null;
    if (this.lastActiveRecord) {
      this.lastActiveRecord[TYPEAHEAD_RECORD.ITEM].tabIndex = -1;
    }
    this.typeahead(event);
  }
  /**
   * Performs the typeahead. Based on the normalized items and the current text
   * buffer, finds the _next_ item with matching text and activates it.
   *
   * @example
   *
   * items: Apple, Banana, Olive, Orange, Cucumber
   * buffer: ''
   * user types: o
   *
   * activates Olive
   *
   * @example
   *
   * items: Apple, Banana, Olive (active), Orange, Cucumber
   * buffer: 'o'
   * user types: l
   *
   * activates Olive
   *
   * @example
   *
   * items: Apple, Banana, Olive (active), Orange, Cucumber
   * buffer: ''
   * user types: o
   *
   * activates Orange
   *
   * @example
   *
   * items: Apple, Banana, Olive, Orange (active), Cucumber
   * buffer: ''
   * user types: o
   *
   * activates Olive
   */
  typeahead(event) {
    if (event.defaultPrevented) return;
    clearTimeout(this.cancelTypeaheadTimeout);
    // Stop typingahead if one of the navigation or selection keys (except for
    // Space) are pressed
    if (event.code === 'Enter' || event.code.startsWith('Arrow') || event.code === 'Escape') {
      this.endTypeahead();
      if (this.lastActiveRecord) {
        this.lastActiveRecord[TYPEAHEAD_RECORD.ITEM].tabIndex = -1;
      }
      return;
    }
    // If Space is pressed, prevent it from selecting and closing the menu
    if (event.code === 'Space') {
      event.preventDefault();
    }
    // Start up a new keystroke buffer timeout
    this.cancelTypeaheadTimeout = setTimeout(this.endTypeahead, this.getProperties().typeaheadBufferTime);
    this.typaheadBuffer += event.key.toLowerCase();
    const lastActiveIndex = this.lastActiveRecord ? this.lastActiveRecord[TYPEAHEAD_RECORD.INDEX] : -1;
    const numRecords = this.typeaheadRecords.length;
    /**
     * Sorting function that will resort the items starting with the given index
     *
     * @example
     *
     * this.typeaheadRecords =
     * 0: [0, <reference>, 'apple']
     * 1: [1, <reference>, 'apricot']
     * 2: [2, <reference>, 'banana']
     * 3: [3, <reference>, 'olive'] <-- lastActiveIndex
     * 4: [4, <reference>, 'orange']
     * 5: [5, <reference>, 'strawberry']
     *
     * this.typeaheadRecords.sort((a,b) => rebaseIndexOnActive(a)
     *                                       - rebaseIndexOnActive(b)) ===
     * 0: [3, <reference>, 'olive'] <-- lastActiveIndex
     * 1: [4, <reference>, 'orange']
     * 2: [5, <reference>, 'strawberry']
     * 3: [0, <reference>, 'apple']
     * 4: [1, <reference>, 'apricot']
     * 5: [2, <reference>, 'banana']
     */
    const rebaseIndexOnActive = record => {
      return (record[TYPEAHEAD_RECORD.INDEX] + numRecords - lastActiveIndex) % numRecords;
    };
    // records filtered and sorted / rebased around the last active index
    const matchingRecords = this.typeaheadRecords.filter(record => !record[TYPEAHEAD_RECORD.ITEM].disabled && record[TYPEAHEAD_RECORD.TEXT].startsWith(this.typaheadBuffer)).sort((a, b) => rebaseIndexOnActive(a) - rebaseIndexOnActive(b));
    // Just leave if there's nothing that matches. Native select will just
    // choose the first thing that starts with the next letter in the alphabet
    // but that's out of scope and hard to localize
    if (matchingRecords.length === 0) {
      clearTimeout(this.cancelTypeaheadTimeout);
      if (this.lastActiveRecord) {
        this.lastActiveRecord[TYPEAHEAD_RECORD.ITEM].tabIndex = -1;
      }
      this.endTypeahead();
      return;
    }
    const isNewQuery = this.typaheadBuffer.length === 1;
    let nextRecord;
    // This is likely the case that someone is trying to "tab" through different
    // entries that start with the same letter
    if (this.lastActiveRecord === matchingRecords[0] && isNewQuery) {
      nextRecord = matchingRecords[1] ?? matchingRecords[0];
    } else {
      nextRecord = matchingRecords[0];
    }
    if (this.lastActiveRecord) {
      this.lastActiveRecord[TYPEAHEAD_RECORD.ITEM].tabIndex = -1;
    }
    this.lastActiveRecord = nextRecord;
    nextRecord[TYPEAHEAD_RECORD.ITEM].tabIndex = 0;
    nextRecord[TYPEAHEAD_RECORD.ITEM].focus();
    return;
  }
}

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * The default value for the typeahead buffer time in Milliseconds.
 */
const DEFAULT_TYPEAHEAD_BUFFER_TIME = 200;
const submenuNavKeys = new Set([NavigableKeys.ArrowDown, NavigableKeys.ArrowUp, NavigableKeys.Home, NavigableKeys.End]);
const menuNavKeys = new Set([NavigableKeys.ArrowLeft, NavigableKeys.ArrowRight, ...submenuNavKeys]);
/**
 * Gets the currently focused element on the page.
 *
 * @param activeDoc The document or shadowroot from which to start the search.
 *    Defaults to `window.document`
 * @return Returns the currently deeply focused element or `null` if none.
 */
function getFocusedElement(activeDoc = document) {
  let activeEl = activeDoc.activeElement;
  // Check for activeElement in the case that an element with a shadow root host
  // is currently focused.
  while (activeEl && (_activeEl = activeEl) !== null && _activeEl !== void 0 && (_activeEl = _activeEl.shadowRoot) !== null && _activeEl !== void 0 && _activeEl.activeElement) {
    var _activeEl;
    activeEl = activeEl.shadowRoot.activeElement;
  }
  return activeEl;
}
/**
 * @fires opening {Event} Fired before the opening animation begins
 * @fires opened {Event} Fired once the menu is open, after any animations
 * @fires closing {Event} Fired before the closing animation begins
 * @fires closed {Event} Fired once the menu is closed, after any animations
 */
class Menu extends i$2 {
  /**
   * Whether the menu is animating upwards or downwards when opening. This is
   * helpful for calculating some animation calculations.
   */
  get openDirection() {
    const menuCornerBlock = this.menuCorner.split('-')[0];
    return menuCornerBlock === 'start' ? 'DOWN' : 'UP';
  }
  /**
   * The element which the menu should align to. If `anchor` is set to a
   * non-empty idref string, then `anchorEl` will resolve to the element with
   * the given id in the same root node. Otherwise, `null`.
   */
  get anchorElement() {
    if (this.anchor) {
      return this.getRootNode().querySelector(`#${this.anchor}`);
    }
    return this.currentAnchorElement;
  }
  set anchorElement(element) {
    this.currentAnchorElement = element;
    this.requestUpdate('anchorElement');
  }
  constructor() {
    super();
    /**
     * The ID of the element in the same root node in which the menu should align
     * to. Overrides setting `anchorElement = elementReference`.
     *
     * __NOTE__: anchor or anchorElement must either be an HTMLElement or resolve
     * to an HTMLElement in order for menu to open.
     */
    this.anchor = '';
    /**
     * Whether the positioning algorithm should calculate relative to the parent
     * of the anchor element (`absolute`), relative to the window (`fixed`), or
     * relative to the document (`document`). `popover` will use the popover API
     * to render the menu in the top-layer. If your browser does not support the
     * popover API, it will fall back to `fixed`.
     *
     * __Examples for `position = 'fixed'`:__
     *
     * - If there is no `position:relative` in the given parent tree and the
     *   surface is `position:absolute`
     * - If the surface is `position:fixed`
     * - If the surface is in the "top layer"
     * - The anchor and the surface do not share a common `position:relative`
     *   ancestor
     *
     * When using `positioning=fixed`, in most cases, the menu should position
     * itself above most other `position:absolute` or `position:fixed` elements
     * when placed inside of them. e.g. using a menu inside of an `md-dialog`.
     *
     * __NOTE__: Fixed menus will not scroll with the page and will be fixed to
     * the window instead.
     *
     * __Examples for `position = 'document'`:__
     *
     * - There is no parent that creates a relative positioning context e.g.
     *   `position: relative`, `position: absolute`, `transform: translate(x, y)`,
     *   etc.
     * - You put the effort into hoisting the menu to the top of the DOM like the
     *   end of the `<body>` to render over everything or in a top-layer.
     * - You are reusing a single `md-menu` element that dynamically renders
     *   content.
     *
     * __Examples for `position = 'popover'`:__
     *
     * - Your browser supports `popover`.
     * - Most cases. Once popover is in browsers, this will become the default.
     */
    this.positioning = 'absolute';
    /**
     * Skips the opening and closing animations.
     */
    this.quick = false;
    /**
     * Displays overflow content like a submenu. Not required in most cases when
     * using `positioning="popover"`.
     *
     * __NOTE__: This may cause adverse effects if you set
     * `md-menu {max-height:...}`
     * and have items overflowing items in the "y" direction.
     */
    this.hasOverflow = false;
    /**
     * Opens the menu and makes it visible. Alternative to the `.show()` and
     * `.close()` methods
     */
    this.open = false;
    /**
     * Offsets the menu's inline alignment from the anchor by the given number in
     * pixels. This value is direction aware and will follow the LTR / RTL
     * direction.
     *
     * e.g. LTR: positive -> right, negative -> left
     *      RTL: positive -> left, negative -> right
     */
    this.xOffset = 0;
    /**
     * Offsets the menu's block alignment from the anchor by the given number in
     * pixels.
     *
     * e.g. positive -> down, negative -> up
     */
    this.yOffset = 0;
    /**
     * Disable the `flip` behavior that usually happens on the horizontal axis
     * when the surface would render outside the viewport.
     */
    this.noHorizontalFlip = false;
    /**
     * Disable the `flip` behavior that usually happens on the vertical axis when
     * the surface would render outside the viewport.
     */
    this.noVerticalFlip = false;
    /**
     * The max time between the keystrokes of the typeahead menu behavior before
     * it clears the typeahead buffer.
     */
    this.typeaheadDelay = DEFAULT_TYPEAHEAD_BUFFER_TIME;
    /**
     * The corner of the anchor which to align the menu in the standard logical
     * property style of <block>-<inline> e.g. `'end-start'`.
     *
     * NOTE: This value may not be respected by the menu positioning algorithm
     * if the menu would render outisde the viewport.
     * Use `no-horizontal-flip` or `no-vertical-flip` to force the usage of the value
     */
    this.anchorCorner = Corner.END_START;
    /**
     * The corner of the menu which to align the anchor in the standard logical
     * property style of <block>-<inline> e.g. `'start-start'`.
     *
     * NOTE: This value may not be respected by the menu positioning algorithm
     * if the menu would render outisde the viewport.
     * Use `no-horizontal-flip` or `no-vertical-flip` to force the usage of the value
     */
    this.menuCorner = Corner.START_START;
    /**
     * Keeps the user clicks outside the menu.
     *
     * NOTE: clicking outside may still cause focusout to close the menu so see
     * `stayOpenOnFocusout`.
     */
    this.stayOpenOnOutsideClick = false;
    /**
     * Keeps the menu open when focus leaves the menu's composed subtree.
     *
     * NOTE: Focusout behavior will stop propagation of the focusout event. Set
     * this property to true to opt-out of menu's focusout handling altogether.
     */
    this.stayOpenOnFocusout = false;
    /**
     * After closing, does not restore focus to the last focused element before
     * the menu was opened.
     */
    this.skipRestoreFocus = false;
    /**
     * The element that should be focused by default once opened.
     *
     * NOTE: When setting default focus to 'LIST_ROOT', remember to change
     * `tabindex` to `0` and change md-menu's display to something other than
     * `display: contents` when necessary.
     */
    this.defaultFocus = FocusState.FIRST_ITEM;
    /**
     * Turns off navigation wrapping. By default, navigating past the end of the
     * menu items will wrap focus back to the beginning and vice versa. Use this
     * for ARIA patterns that do not wrap focus, like combobox.
     */
    this.noNavigationWrap = false;
    this.typeaheadActive = true;
    /**
     * Whether or not the current menu is a submenu and should not handle specific
     * navigation keys.
     *
     * @export
     */
    this.isSubmenu = false;
    /**
     * The event path of the last window pointerdown event.
     */
    this.pointerPath = [];
    /**
     * Whether or not the menu is repositoining due to window / document resize
     */
    this.isRepositioning = false;
    this.openCloseAnimationSignal = createAnimationSignal();
    this.listController = new ListController({
      isItem: maybeItem => {
        return maybeItem.hasAttribute('md-menu-item');
      },
      getPossibleItems: () => this.slotItems,
      isRtl: () => getComputedStyle(this).direction === 'rtl',
      deactivateItem: item => {
        item.selected = false;
        item.tabIndex = -1;
      },
      activateItem: item => {
        item.selected = true;
        item.tabIndex = 0;
      },
      isNavigableKey: key => {
        if (!this.isSubmenu) {
          return menuNavKeys.has(key);
        }
        const isRtl = getComputedStyle(this).direction === 'rtl';
        // we want md-submenu to handle the submenu's left/right arrow exit
        // key so it can close the menu instead of navigate the list.
        // Therefore we need to include all keys but left/right arrow close
        // key
        const arrowOpen = isRtl ? NavigableKeys.ArrowLeft : NavigableKeys.ArrowRight;
        if (key === arrowOpen) {
          return true;
        }
        return submenuNavKeys.has(key);
      },
      wrapNavigation: () => !this.noNavigationWrap
    });
    /**
     * The element that was focused before the menu opened.
     */
    this.lastFocusedElement = null;
    /**
     * Handles typeahead navigation through the menu.
     */
    this.typeaheadController = new TypeaheadController(() => {
      return {
        getItems: () => this.items,
        typeaheadBufferTime: this.typeaheadDelay,
        active: this.typeaheadActive
      };
    });
    this.currentAnchorElement = null;
    this.internals =
    // Cast needed for closure
    this.attachInternals();
    /**
     * Handles positioning the surface and aligning it to the anchor as well as
     * keeping it in the viewport.
     */
    this.menuPositionController = new SurfacePositionController(this, () => {
      return {
        anchorCorner: this.anchorCorner,
        surfaceCorner: this.menuCorner,
        surfaceEl: this.surfaceEl,
        anchorEl: this.anchorElement,
        positioning: this.positioning === 'popover' ? 'document' : this.positioning,
        isOpen: this.open,
        xOffset: this.xOffset,
        yOffset: this.yOffset,
        disableBlockFlip: this.noVerticalFlip,
        disableInlineFlip: this.noHorizontalFlip,
        onOpen: this.onOpened,
        beforeClose: this.beforeClose,
        onClose: this.onClosed,
        // We can't resize components that have overflow like menus with
        // submenus because the overflow-y will show menu items / content
        // outside the bounds of the menu. Popover API fixes this because each
        // submenu is hoisted to the top-layer and are not considered overflow
        // content.
        repositionStrategy: this.hasOverflow && this.positioning !== 'popover' ? 'move' : 'resize'
      };
    });
    this.onWindowResize = () => {
      if (this.isRepositioning || this.positioning !== 'document' && this.positioning !== 'fixed' && this.positioning !== 'popover') {
        return;
      }
      this.isRepositioning = true;
      this.reposition();
      this.isRepositioning = false;
    };
    this.handleFocusout = async event => {
      const anchorEl = this.anchorElement;
      // Do not close if we focused out by clicking on the anchor element. We
      // can't assume anchor buttons can be the related target because of iOS does
      // not focus buttons.
      if (this.stayOpenOnFocusout || !this.open || this.pointerPath.includes(anchorEl)) {
        return;
      }
      if (event.relatedTarget) {
        // Don't close the menu if we are switching focus between menu,
        // md-menu-item, and md-list or if the anchor was click focused, but check
        // if length of pointerPath is 0 because that means something was at least
        // clicked (shift+tab case).
        if (isElementInSubtree(event.relatedTarget, this) || this.pointerPath.length !== 0 && isElementInSubtree(event.relatedTarget, anchorEl)) {
          return;
        }
      } else if (this.pointerPath.includes(this)) {
        // If menu tabindex == -1 and the user clicks on the menu or a divider, we
        // want to keep the menu open.
        return;
      }
      const oldRestoreFocus = this.skipRestoreFocus;
      // allow focus to continue to the next focused object rather than returning
      this.skipRestoreFocus = true;
      this.close();
      // await for close
      await this.updateComplete;
      // return to previous behavior
      this.skipRestoreFocus = oldRestoreFocus;
    };
    /**
     * Saves the last focused element focuses the new element based on
     * `defaultFocus`, and animates open.
     */
    this.onOpened = async () => {
      this.lastFocusedElement = getFocusedElement();
      const items = this.items;
      const activeItemRecord = getActiveItem(items);
      if (activeItemRecord && this.defaultFocus !== FocusState.NONE) {
        activeItemRecord.item.tabIndex = -1;
      }
      let animationAborted = !this.quick;
      if (this.quick) {
        this.dispatchEvent(new Event('opening'));
      } else {
        animationAborted = !!(await this.animateOpen());
      }
      // This must come after the opening animation or else it may focus one of
      // the items before the animation has begun and causes the list to slide
      // (block-padding-of-the-menu)px at the end of the animation
      switch (this.defaultFocus) {
        case FocusState.FIRST_ITEM:
          const first = getFirstActivatableItem(items);
          if (first) {
            first.tabIndex = 0;
            first.focus();
            await first.updateComplete;
          }
          break;
        case FocusState.LAST_ITEM:
          const last = getLastActivatableItem(items);
          if (last) {
            last.tabIndex = 0;
            last.focus();
            await last.updateComplete;
          }
          break;
        case FocusState.LIST_ROOT:
          this.focus();
          break;
        default:
        case FocusState.NONE:
          // Do nothing.
          break;
      }
      if (!animationAborted) {
        this.dispatchEvent(new Event('opened'));
      }
    };
    /**
     * Animates closed.
     */
    this.beforeClose = async () => {
      this.open = false;
      if (!this.skipRestoreFocus) {
        var _this$lastFocusedElem, _this$lastFocusedElem2;
        (_this$lastFocusedElem = this.lastFocusedElement) === null || _this$lastFocusedElem === void 0 || (_this$lastFocusedElem2 = _this$lastFocusedElem.focus) === null || _this$lastFocusedElem2 === void 0 || _this$lastFocusedElem2.call(_this$lastFocusedElem);
      }
      if (!this.quick) {
        await this.animateClose();
      }
    };
    /**
     * Focuses the last focused element.
     */
    this.onClosed = () => {
      if (this.quick) {
        this.dispatchEvent(new Event('closing'));
        this.dispatchEvent(new Event('closed'));
      }
    };
    this.onWindowPointerdown = event => {
      this.pointerPath = event.composedPath();
    };
    /**
     * We cannot listen to window click because Safari on iOS will not bubble a
     * click event on window if the item clicked is not a "clickable" item such as
     * <body>
     */
    this.onDocumentClick = event => {
      if (!this.open) {
        return;
      }
      const path = event.composedPath();
      if (!this.stayOpenOnOutsideClick && !path.includes(this) && !path.includes(this.anchorElement)) {
        this.open = false;
      }
    };
    {
      this.internals.role = 'menu';
      this.addEventListener('keydown', this.handleKeydown);
      // Capture so that we can grab the event before it reaches the menu item
      // istelf. Specifically useful for the case where typeahead encounters a
      // space and we don't want the menu item to close the menu.
      this.addEventListener('keydown', this.captureKeydown, {
        capture: true
      });
      this.addEventListener('focusout', this.handleFocusout);
    }
  }
  /**
   * The menu items associated with this menu. The items must be `MenuItem`s and
   * have both the `md-menu-item` and `md-list-item` attributes.
   */
  get items() {
    return this.listController.items;
  }
  willUpdate(changed) {
    if (!changed.has('open')) {
      return;
    }
    if (this.open) {
      this.removeAttribute('aria-hidden');
      return;
    }
    this.setAttribute('aria-hidden', 'true');
  }
  update(changed) {
    if (changed.has('open')) {
      if (this.open) {
        this.setUpGlobalEventListeners();
      } else {
        this.cleanUpGlobalEventListeners();
      }
    }
    // Firefox does not support popover. Fall-back to using fixed.
    if (changed.has('positioning') && this.positioning === 'popover' &&
    // type required for Google JS conformance
    !this.showPopover) {
      this.positioning = 'fixed';
    }
    super.update(changed);
  }
  connectedCallback() {
    super.connectedCallback();
    if (this.open) {
      this.setUpGlobalEventListeners();
    }
  }
  disconnectedCallback() {
    super.disconnectedCallback();
    this.cleanUpGlobalEventListeners();
  }
  getBoundingClientRect() {
    if (!this.surfaceEl) {
      return super.getBoundingClientRect();
    }
    return this.surfaceEl.getBoundingClientRect();
  }
  getClientRects() {
    if (!this.surfaceEl) {
      return super.getClientRects();
    }
    return this.surfaceEl.getClientRects();
  }
  render() {
    return this.renderSurface();
  }
  /**
   * Renders the positionable surface element and its contents.
   */
  renderSurface() {
    return x`
      <div
        class="menu ${e$3(this.getSurfaceClasses())}"
        style=${o(this.menuPositionController.surfaceStyles)}
        popover=${this.positioning === 'popover' ? 'manual' : E}>
        ${this.renderElevation()}
        <div class="items">
          <div class="item-padding"> ${this.renderMenuItems()} </div>
        </div>
      </div>
    `;
  }
  /**
   * Renders the menu items' slot
   */
  renderMenuItems() {
    return x`<slot
      @close-menu=${this.onCloseMenu}
      @deactivate-items=${this.onDeactivateItems}
      @request-activation=${this.onRequestActivation}
      @deactivate-typeahead=${this.handleDeactivateTypeahead}
      @activate-typeahead=${this.handleActivateTypeahead}
      @stay-open-on-focusout=${this.handleStayOpenOnFocusout}
      @close-on-focusout=${this.handleCloseOnFocusout}
      @slotchange=${this.listController.onSlotchange}></slot>`;
  }
  /**
   * Renders the elevation component.
   */
  renderElevation() {
    return x`<md-elevation part="elevation"></md-elevation>`;
  }
  getSurfaceClasses() {
    return {
      open: this.open,
      fixed: this.positioning === 'fixed',
      'has-overflow': this.hasOverflow
    };
  }
  captureKeydown(event) {
    if (event.target === this && !event.defaultPrevented && isClosableKey(event.code)) {
      event.preventDefault();
      this.close();
    }
    this.typeaheadController.onKeydown(event);
  }
  /**
   * Performs the opening animation:
   *
   * https://direct.googleplex.com/#/spec/295000003+271060003
   *
   * @return A promise that resolve to `true` if the animation was aborted,
   *     `false` if it was not aborted.
   */
  async animateOpen() {
    const surfaceEl = this.surfaceEl;
    const slotEl = this.slotEl;
    if (!surfaceEl || !slotEl) return true;
    const openDirection = this.openDirection;
    this.dispatchEvent(new Event('opening'));
    // needs to be imperative because we don't want to mix animation and Lit
    // render timing
    surfaceEl.classList.toggle('animating', true);
    const signal = this.openCloseAnimationSignal.start();
    const height = surfaceEl.offsetHeight;
    const openingUpwards = openDirection === 'UP';
    const children = this.items;
    const FULL_DURATION = 500;
    const SURFACE_OPACITY_DURATION = 50;
    const ITEM_OPACITY_DURATION = 250;
    // We want to fit every child fade-in animation within the full duration of
    // the animation.
    const DELAY_BETWEEN_ITEMS = (FULL_DURATION - ITEM_OPACITY_DURATION) / children.length;
    const surfaceHeightAnimation = surfaceEl.animate([{
      height: '0px'
    }, {
      height: `${height}px`
    }], {
      duration: FULL_DURATION,
      easing: EASING.EMPHASIZED
    });
    // When we are opening upwards, we want to make sure the last item is always
    // in view, so we need to translate it upwards the opposite direction of the
    // height animation
    const upPositionCorrectionAnimation = slotEl.animate([{
      transform: openingUpwards ? `translateY(-${height}px)` : ''
    }, {
      transform: ''
    }], {
      duration: FULL_DURATION,
      easing: EASING.EMPHASIZED
    });
    const surfaceOpacityAnimation = surfaceEl.animate([{
      opacity: 0
    }, {
      opacity: 1
    }], SURFACE_OPACITY_DURATION);
    const childrenAnimations = [];
    for (let i = 0; i < children.length; i++) {
      // If we are animating upwards, then reverse the children list.
      const directionalIndex = openingUpwards ? children.length - 1 - i : i;
      const child = children[directionalIndex];
      const animation = child.animate([{
        opacity: 0
      }, {
        opacity: 1
      }], {
        duration: ITEM_OPACITY_DURATION,
        delay: DELAY_BETWEEN_ITEMS * i
      });
      // Make them all initially hidden and then clean up at the end of each
      // animation.
      child.classList.toggle('md-menu-hidden', true);
      animation.addEventListener('finish', () => {
        child.classList.toggle('md-menu-hidden', false);
      });
      childrenAnimations.push([child, animation]);
    }
    let resolveAnimation = value => {};
    const animationFinished = new Promise(resolve => {
      resolveAnimation = resolve;
    });
    signal.addEventListener('abort', () => {
      surfaceHeightAnimation.cancel();
      upPositionCorrectionAnimation.cancel();
      surfaceOpacityAnimation.cancel();
      childrenAnimations.forEach(([child, animation]) => {
        child.classList.toggle('md-menu-hidden', false);
        animation.cancel();
      });
      resolveAnimation(true);
    });
    surfaceHeightAnimation.addEventListener('finish', () => {
      surfaceEl.classList.toggle('animating', false);
      this.openCloseAnimationSignal.finish();
      resolveAnimation(false);
    });
    return await animationFinished;
  }
  /**
   * Performs the closing animation:
   *
   * https://direct.googleplex.com/#/spec/295000003+271060003
   */
  animateClose() {
    let resolve;
    // This promise blocks the surface position controller from setting
    // display: none on the surface which will interfere with this animation.
    const animationEnded = new Promise(res => {
      resolve = res;
    });
    const surfaceEl = this.surfaceEl;
    const slotEl = this.slotEl;
    if (!surfaceEl || !slotEl) {
      resolve(false);
      return animationEnded;
    }
    const openDirection = this.openDirection;
    const closingDownwards = openDirection === 'UP';
    this.dispatchEvent(new Event('closing'));
    // needs to be imperative because we don't want to mix animation and Lit
    // render timing
    surfaceEl.classList.toggle('animating', true);
    const signal = this.openCloseAnimationSignal.start();
    const height = surfaceEl.offsetHeight;
    const children = this.items;
    const FULL_DURATION = 150;
    const SURFACE_OPACITY_DURATION = 50;
    // The surface fades away at the very end
    const SURFACE_OPACITY_DELAY = FULL_DURATION - SURFACE_OPACITY_DURATION;
    const ITEM_OPACITY_DURATION = 50;
    const ITEM_OPACITY_INITIAL_DELAY = 50;
    const END_HEIGHT_PERCENTAGE = 0.35;
    // We want to fit every child fade-out animation within the full duration of
    // the animation.
    const DELAY_BETWEEN_ITEMS = (FULL_DURATION - ITEM_OPACITY_INITIAL_DELAY - ITEM_OPACITY_DURATION) / children.length;
    // The mock has the animation shrink to 35%
    const surfaceHeightAnimation = surfaceEl.animate([{
      height: `${height}px`
    }, {
      height: `${height * END_HEIGHT_PERCENTAGE}px`
    }], {
      duration: FULL_DURATION,
      easing: EASING.EMPHASIZED_ACCELERATE
    });
    // When we are closing downwards, we want to make sure the last item is
    // always in view, so we need to translate it upwards the opposite direction
    // of the height animation
    const downPositionCorrectionAnimation = slotEl.animate([{
      transform: ''
    }, {
      transform: closingDownwards ? `translateY(-${height * (1 - END_HEIGHT_PERCENTAGE)}px)` : ''
    }], {
      duration: FULL_DURATION,
      easing: EASING.EMPHASIZED_ACCELERATE
    });
    const surfaceOpacityAnimation = surfaceEl.animate([{
      opacity: 1
    }, {
      opacity: 0
    }], {
      duration: SURFACE_OPACITY_DURATION,
      delay: SURFACE_OPACITY_DELAY
    });
    const childrenAnimations = [];
    for (let i = 0; i < children.length; i++) {
      // If the animation is closing upwards, then reverse the list of
      // children so that we animate in the opposite direction.
      const directionalIndex = closingDownwards ? i : children.length - 1 - i;
      const child = children[directionalIndex];
      const animation = child.animate([{
        opacity: 1
      }, {
        opacity: 0
      }], {
        duration: ITEM_OPACITY_DURATION,
        delay: ITEM_OPACITY_INITIAL_DELAY + DELAY_BETWEEN_ITEMS * i
      });
      // Make sure the items stay hidden at the end of each child animation.
      // We clean this up at the end of the overall animation.
      animation.addEventListener('finish', () => {
        child.classList.toggle('md-menu-hidden', true);
      });
      childrenAnimations.push([child, animation]);
    }
    signal.addEventListener('abort', () => {
      surfaceHeightAnimation.cancel();
      downPositionCorrectionAnimation.cancel();
      surfaceOpacityAnimation.cancel();
      childrenAnimations.forEach(([child, animation]) => {
        animation.cancel();
        child.classList.toggle('md-menu-hidden', false);
      });
      resolve(false);
    });
    surfaceHeightAnimation.addEventListener('finish', () => {
      surfaceEl.classList.toggle('animating', false);
      childrenAnimations.forEach(([child]) => {
        child.classList.toggle('md-menu-hidden', false);
      });
      this.openCloseAnimationSignal.finish();
      this.dispatchEvent(new Event('closed'));
      resolve(true);
    });
    return animationEnded;
  }
  handleKeydown(event) {
    // At any key event, the pointer interaction is done so we need to clear our
    // cached pointerpath. This handles the case where the user clicks on the
    // anchor, and then hits shift+tab
    this.pointerPath = [];
    this.listController.handleKeydown(event);
  }
  setUpGlobalEventListeners() {
    document.addEventListener('click', this.onDocumentClick, {
      capture: true
    });
    window.addEventListener('pointerdown', this.onWindowPointerdown);
    document.addEventListener('resize', this.onWindowResize, {
      passive: true
    });
    window.addEventListener('resize', this.onWindowResize, {
      passive: true
    });
  }
  cleanUpGlobalEventListeners() {
    document.removeEventListener('click', this.onDocumentClick, {
      capture: true
    });
    window.removeEventListener('pointerdown', this.onWindowPointerdown);
    document.removeEventListener('resize', this.onWindowResize);
    window.removeEventListener('resize', this.onWindowResize);
  }
  onCloseMenu() {
    this.close();
  }
  onDeactivateItems(event) {
    event.stopPropagation();
    this.listController.onDeactivateItems();
  }
  onRequestActivation(event) {
    event.stopPropagation();
    this.listController.onRequestActivation(event);
  }
  handleDeactivateTypeahead(event) {
    // stopPropagation so that this does not deactivate any typeaheads in menus
    // nested above it e.g. md-sub-menu
    event.stopPropagation();
    this.typeaheadActive = false;
  }
  handleActivateTypeahead(event) {
    // stopPropagation so that this does not activate any typeaheads in menus
    // nested above it e.g. md-sub-menu
    event.stopPropagation();
    this.typeaheadActive = true;
  }
  handleStayOpenOnFocusout(event) {
    event.stopPropagation();
    this.stayOpenOnFocusout = true;
  }
  handleCloseOnFocusout(event) {
    event.stopPropagation();
    this.stayOpenOnFocusout = false;
  }
  close() {
    this.open = false;
    const maybeSubmenu = this.slotItems;
    maybeSubmenu.forEach(item => {
      var _item$close;
      (_item$close = item.close) === null || _item$close === void 0 || _item$close.call(item);
    });
  }
  show() {
    this.open = true;
  }
  /**
   * Activates the next item in the menu. If at the end of the menu, the first
   * item will be activated.
   *
   * @return The activated menu item or `null` if there are no items.
   */
  activateNextItem() {
    return this.listController.activateNextItem() ?? null;
  }
  /**
   * Activates the previous item in the menu. If at the start of the menu, the
   * last item will be activated.
   *
   * @return The activated menu item or `null` if there are no items.
   */
  activatePreviousItem() {
    return this.listController.activatePreviousItem() ?? null;
  }
  /**
   * Repositions the menu if it is open.
   *
   * Useful for the case where document or window-positioned menus have their
   * anchors moved while open.
   */
  reposition() {
    if (this.open) {
      this.menuPositionController.position();
    }
  }
}
__decorate([e$2('.menu')], Menu.prototype, "surfaceEl", void 0);
__decorate([e$2('slot')], Menu.prototype, "slotEl", void 0);
__decorate([n$3()], Menu.prototype, "anchor", void 0);
__decorate([n$3()], Menu.prototype, "positioning", void 0);
__decorate([n$3({
  type: Boolean
})], Menu.prototype, "quick", void 0);
__decorate([n$3({
  type: Boolean,
  attribute: 'has-overflow'
})], Menu.prototype, "hasOverflow", void 0);
__decorate([n$3({
  type: Boolean,
  reflect: true
})], Menu.prototype, "open", void 0);
__decorate([n$3({
  type: Number,
  attribute: 'x-offset'
})], Menu.prototype, "xOffset", void 0);
__decorate([n$3({
  type: Number,
  attribute: 'y-offset'
})], Menu.prototype, "yOffset", void 0);
__decorate([n$3({
  type: Boolean,
  attribute: 'no-horizontal-flip'
})], Menu.prototype, "noHorizontalFlip", void 0);
__decorate([n$3({
  type: Boolean,
  attribute: 'no-vertical-flip'
})], Menu.prototype, "noVerticalFlip", void 0);
__decorate([n$3({
  type: Number,
  attribute: 'typeahead-delay'
})], Menu.prototype, "typeaheadDelay", void 0);
__decorate([n$3({
  attribute: 'anchor-corner'
})], Menu.prototype, "anchorCorner", void 0);
__decorate([n$3({
  attribute: 'menu-corner'
})], Menu.prototype, "menuCorner", void 0);
__decorate([n$3({
  type: Boolean,
  attribute: 'stay-open-on-outside-click'
})], Menu.prototype, "stayOpenOnOutsideClick", void 0);
__decorate([n$3({
  type: Boolean,
  attribute: 'stay-open-on-focusout'
})], Menu.prototype, "stayOpenOnFocusout", void 0);
__decorate([n$3({
  type: Boolean,
  attribute: 'skip-restore-focus'
})], Menu.prototype, "skipRestoreFocus", void 0);
__decorate([n$3({
  attribute: 'default-focus'
})], Menu.prototype, "defaultFocus", void 0);
__decorate([n$3({
  type: Boolean,
  attribute: 'no-navigation-wrap'
})], Menu.prototype, "noNavigationWrap", void 0);
__decorate([o$2({
  flatten: true
})], Menu.prototype, "slotItems", void 0);
__decorate([r$1()], Menu.prototype, "typeaheadActive", void 0);

/**
 * @license
 * Copyright 2024 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Generated stylesheet for ./menu/internal/menu-styles.css.
const styles$4 = i$3`:host{--md-elevation-level: var(--md-menu-container-elevation, 2);--md-elevation-shadow-color: var(--md-menu-container-shadow-color, var(--md-sys-color-shadow, #000));min-width:112px;color:unset;display:contents}md-focus-ring{--md-focus-ring-shape: var(--md-menu-container-shape, var(--md-sys-shape-corner-extra-small, 4px))}.menu{border-radius:var(--md-menu-container-shape, var(--md-sys-shape-corner-extra-small, 4px));display:none;inset:auto;border:none;padding:0px;overflow:visible;background-color:rgba(0,0,0,0);color:inherit;opacity:0;z-index:20;position:absolute;user-select:none;max-height:inherit;height:inherit;min-width:inherit;max-width:inherit;scrollbar-width:inherit}.menu::backdrop{display:none}.fixed{position:fixed}.items{display:block;list-style-type:none;margin:0;outline:none;box-sizing:border-box;background-color:var(--md-menu-container-color, var(--md-sys-color-surface-container, #f3edf7));height:inherit;max-height:inherit;overflow:auto;min-width:inherit;max-width:inherit;border-radius:inherit;scrollbar-width:inherit}.item-padding{padding-block:var(--md-menu-top-space, 8px) var(--md-menu-bottom-space, 8px)}.has-overflow:not([popover]) .items{overflow:visible}.has-overflow.animating .items,.animating .items{overflow:hidden}.has-overflow.animating .items{pointer-events:none}.animating ::slotted(.md-menu-hidden){opacity:0}slot{display:block;height:inherit;max-height:inherit}::slotted(:is(md-divider,[role=separator])){margin:8px 0}@media(forced-colors: active){.menu{border-style:solid;border-color:CanvasText;border-width:1px}}
`;

/**
 * @license
 * Copyright 2022 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * @summary Menus display a list of choices on a temporary surface.
 *
 * @description
 * Menus appear when users interact with a button, action, or other control.
 *
 * They can be opened from a variety of elements, most commonly icon buttons,
 * buttons, and text fields.
 *
 * md-menu listens for the `close-menu` and `deselect-items` events.
 *
 * - `close-menu` closes the menu when dispatched from a child element.
 * - `deselect-items` deselects all of its immediate menu-item children.
 *
 * @example
 * ```html
 * <div style="position:relative;">
 *   <button
 *       id="anchor"
 *       @click=${() => this.menuRef.value.show()}>
 *     Click to open menu
 *   </button>
 *   <!--
 *     `has-overflow` is required when using a submenu which overflows the
 *     menu's contents.
 *
 *     Additionally, `anchor` ingests an idref which do not pass through shadow
 *     roots. You can also set `.anchorElement` to an element reference if
 *     necessary.
 *   -->
 *   <md-menu anchor="anchor" has-overflow ${ref(menuRef)}>
 *     <md-menu-item headline="This is a headline"></md-menu-item>
 *     <md-sub-menu>
 *       <md-menu-item
 *           slot="item"
 *           headline="this is a submenu item">
 *       </md-menu-item>
 *       <md-menu slot="menu">
 *         <md-menu-item headline="This is an item inside a submenu">
 *         </md-menu-item>
 *       </md-menu>
 *     </md-sub-menu>
 *   </md-menu>
 * </div>
 * ```
 *
 * @final
 * @suppress {visibility}
 */
let MdMenu = class MdMenu extends Menu {};
MdMenu.styles = [styles$4];
MdMenu = __decorate([t('md-menu')], MdMenu);

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * A validator that provides constraint validation that emulates `<select>`
 * validation.
 */
class SelectValidator extends Validator {
  computeValidity(state) {
    if (!this.selectControl) {
      // Lazily create the platform select
      this.selectControl = document.createElement('select');
    }
    B(x`<option value=${state.value}></option>`, this.selectControl);
    this.selectControl.value = state.value;
    this.selectControl.required = state.required;
    return {
      validity: this.selectControl.validity,
      validationMessage: this.selectControl.validationMessage
    };
  }
  equals(prev, next) {
    return prev.value === next.value && prev.required === next.required;
  }
  copy({
    value,
    required
  }) {
    return {
      value,
      required
    };
  }
}

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * Given a list of select options, this function will return an array of
 * SelectOptionRecords that are selected.
 *
 * @return An array of SelectOptionRecords describing the options that are
 * selected.
 */
function getSelectedItems(items) {
  const selectedItemRecords = [];
  for (let i = 0; i < items.length; i++) {
    const item = items[i];
    if (item.selected) {
      selectedItemRecords.push([item, i]);
    }
  }
  return selectedItemRecords;
}

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
var _a;
const VALUE = Symbol('value');
// Separate variable needed for closure.
const selectBaseClass = mixinDelegatesAria(mixinOnReportValidity(mixinConstraintValidation(mixinFormAssociated(mixinElementInternals(i$2)))));
/**
 * @fires change {Event} The native `change` event on
 * [`<input>`](https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/change_event)
 * --bubbles
 * @fires input {InputEvent} The native `input` event on
 * [`<input>`](https://developer.mozilla.org/en-US/docs/Web/API/HTMLElement/input_event)
 * --bubbles --composed
 * @fires opening {Event} Fired when the select's menu is about to open.
 * @fires opened {Event} Fired when the select's menu has finished animations
 * and opened.
 * @fires closing {Event} Fired when the select's menu is about to close.
 * @fires closed {Event} Fired when the select's menu has finished animations
 * and closed.
 */
class Select extends selectBaseClass {
  /**
   * The value of the currently selected option.
   *
   * Note: For SSR, set `[selected]` on the requested option and `displayText`
   * rather than setting `value` setting `value` will incur a DOM query.
   */
  get value() {
    return this[VALUE];
  }
  set value(value) {
    this.lastUserSetValue = value;
    this.select(value);
  }
  get options() {
    var _this$menu;
    // NOTE: this does a DOM query.
    return ((_this$menu = this.menu) === null || _this$menu === void 0 ? void 0 : _this$menu.items) ?? [];
  }
  /**
   * The index of the currently selected option.
   *
   * Note: For SSR, set `[selected]` on the requested option and `displayText`
   * rather than setting `selectedIndex` setting `selectedIndex` will incur a
   * DOM query.
   */
  get selectedIndex() {
    // tslint:disable-next-line:enforce-name-casing
    const [_option, index] = (this.getSelectedOptions() ?? [])[0] ?? [];
    return index ?? -1;
  }
  set selectedIndex(index) {
    this.lastUserSetSelectedIndex = index;
    this.selectIndex(index);
  }
  /**
   * Returns an array of selected options.
   *
   * NOTE: md-select only supports single selection.
   */
  get selectedOptions() {
    return (this.getSelectedOptions() ?? []).map(([option]) => option);
  }
  get hasError() {
    return this.error || this.nativeError;
  }
  constructor() {
    super();
    /**
     * Opens the menu synchronously with no animation.
     */
    this.quick = false;
    /**
     * Whether or not the select is required.
     */
    this.required = false;
    /**
     * The error message that replaces supporting text when `error` is true. If
     * `errorText` is an empty string, then the supporting text will continue to
     * show.
     *
     * This error message overrides the error message displayed by
     * `reportValidity()`.
     */
    this.errorText = '';
    /**
     * The floating label for the field.
     */
    this.label = '';
    /**
     * Disables the asterisk on the floating label, when the select is
     * required.
     */
    this.noAsterisk = false;
    /**
     * Conveys additional information below the select, such as how it should
     * be used.
     */
    this.supportingText = '';
    /**
     * Gets or sets whether or not the select is in a visually invalid state.
     *
     * This error state overrides the error state controlled by
     * `reportValidity()`.
     */
    this.error = false;
    /**
     * Whether or not the underlying md-menu should be position: fixed to display
     * in a top-level manner, or position: absolute.
     *
     * position:fixed is useful for cases where select is inside of another
     * element with stacking context and hidden overflows such as `md-dialog`.
     */
    this.menuPositioning = 'popover';
    /**
     * Clamps the menu-width to the width of the select.
     */
    this.clampMenuWidth = false;
    /**
     * The max time between the keystrokes of the typeahead select / menu behavior
     * before it clears the typeahead buffer.
     */
    this.typeaheadDelay = DEFAULT_TYPEAHEAD_BUFFER_TIME;
    /**
     * Whether or not the text field has a leading icon. Used for SSR.
     */
    this.hasLeadingIcon = false;
    /**
     * Text to display in the field. Only set for SSR.
     */
    this.displayText = '';
    /**
     * Whether the menu should be aligned to the start or the end of the select's
     * textbox.
     */
    this.menuAlign = 'start';
    this[_a] = '';
    /**
     * Used for initializing select when the user sets the `value` directly.
     */
    this.lastUserSetValue = null;
    /**
     * Used for initializing select when the user sets the `selectedIndex`
     * directly.
     */
    this.lastUserSetSelectedIndex = null;
    /**
     * Used for `input` and `change` event change detection.
     */
    this.lastSelectedOption = null;
    // tslint:disable-next-line:enforce-name-casing
    this.lastSelectedOptionRecords = [];
    /**
     * Whether or not a native error has been reported via `reportValidity()`.
     */
    this.nativeError = false;
    /**
     * The validation message displayed from a native error via
     * `reportValidity()`.
     */
    this.nativeErrorText = '';
    this.focused = false;
    this.open = false;
    this.defaultFocus = FocusState.NONE;
    // Have to keep track of previous open because it's state and private and thus
    // cannot be tracked in PropertyValues<this> map.
    this.prevOpen = this.open;
    this.selectWidth = 0;
    this.addEventListener('focus', this.handleFocus.bind(this));
    this.addEventListener('blur', this.handleBlur.bind(this));
  }
  /**
   * Selects an option given the value of the option, and updates MdSelect's
   * value.
   */
  select(value) {
    const optionToSelect = this.options.find(option => option.value === value);
    if (optionToSelect) {
      this.selectItem(optionToSelect);
    }
  }
  /**
   * Selects an option given the index of the option, and updates MdSelect's
   * value.
   */
  selectIndex(index) {
    const optionToSelect = this.options[index];
    if (optionToSelect) {
      this.selectItem(optionToSelect);
    }
  }
  /**
   * Reset the select to its default value.
   */
  reset() {
    for (const option of this.options) {
      option.selected = option.hasAttribute('selected');
    }
    this.updateValueAndDisplayText();
    this.nativeError = false;
    this.nativeErrorText = '';
  }
  [(_a = VALUE, onReportValidity)](invalidEvent) {
    // Prevent default pop-up behavior.
    invalidEvent === null || invalidEvent === void 0 || invalidEvent.preventDefault();
    const prevMessage = this.getErrorText();
    this.nativeError = !!invalidEvent;
    this.nativeErrorText = this.validationMessage;
    if (prevMessage === this.getErrorText()) {
      var _this$field;
      (_this$field = this.field) === null || _this$field === void 0 || _this$field.reannounceError();
    }
  }
  update(changed) {
    // In SSR the options will be ready to query, so try to figure out what
    // the value and display text should be.
    if (!this.hasUpdated) {
      this.initUserSelection();
    }
    // We have just opened the menu.
    // We are only able to check for the select's rect in `update()` instead of
    // having to wait for `updated()` because the menu can never be open on
    // first render since it is not settable and Lit SSR does not support click
    // events which would open the menu.
    if (this.prevOpen !== this.open && this.open) {
      const selectRect = this.getBoundingClientRect();
      this.selectWidth = selectRect.width;
    }
    this.prevOpen = this.open;
    super.update(changed);
  }
  render() {
    return x`
      <span
        class="select ${e$3(this.getRenderClasses())}"
        @focusout=${this.handleFocusout}>
        ${this.renderField()} ${this.renderMenu()}
      </span>
    `;
  }
  async firstUpdated(changed) {
    var _this$menu2;
    await ((_this$menu2 = this.menu) === null || _this$menu2 === void 0 ? void 0 : _this$menu2.updateComplete);
    // If this has been handled on update already due to SSR, try again.
    if (!this.lastSelectedOptionRecords.length) {
      this.initUserSelection();
    }
    // Case for when the DOM is streaming, there are no children, and a child
    // has [selected] set on it, we need to wait for DOM to render something.
    if (!this.lastSelectedOptionRecords.length && true && !this.options.length) {
      setTimeout(() => {
        this.updateValueAndDisplayText();
      });
    }
    super.firstUpdated(changed);
  }
  getRenderClasses() {
    return {
      'disabled': this.disabled,
      'error': this.error,
      'open': this.open
    };
  }
  renderField() {
    const ariaLabel = this.ariaLabel || this.label;
    return u$1`
      <${this.fieldTag}
          aria-haspopup="listbox"
          role="combobox"
          part="field"
          id="field"
          tabindex=${this.disabled ? '-1' : '0'}
          aria-label=${ariaLabel || E}
          aria-describedby="description"
          aria-expanded=${this.open ? 'true' : 'false'}
          aria-controls="listbox"
          class="field"
          label=${this.label}
          ?no-asterisk=${this.noAsterisk}
          .focused=${this.focused || this.open}
          .populated=${!!this.displayText}
          .disabled=${this.disabled}
          .required=${this.required}
          .error=${this.hasError}
          ?has-start=${this.hasLeadingIcon}
          has-end
          supporting-text=${this.supportingText}
          error-text=${this.getErrorText()}
          @keydown=${this.handleKeydown}
          @click=${this.handleClick}>
         ${this.renderFieldContent()}
         <div id="description" slot="aria-describedby"></div>
      </${this.fieldTag}>`;
  }
  renderFieldContent() {
    return [this.renderLeadingIcon(), this.renderLabel(), this.renderTrailingIcon()];
  }
  renderLeadingIcon() {
    return x`
      <span class="icon leading" slot="start">
        <slot name="leading-icon" @slotchange=${this.handleIconChange}></slot>
      </span>
    `;
  }
  renderTrailingIcon() {
    return x`
      <span class="icon trailing" slot="end">
        <slot name="trailing-icon" @slotchange=${this.handleIconChange}>
          <svg height="5" viewBox="7 10 10 5" focusable="false">
            <polygon
              class="down"
              stroke="none"
              fill-rule="evenodd"
              points="7 10 12 15 17 10"></polygon>
            <polygon
              class="up"
              stroke="none"
              fill-rule="evenodd"
              points="7 15 12 10 17 15"></polygon>
          </svg>
        </slot>
      </span>
    `;
  }
  renderLabel() {
    // need to render &nbsp; so that line-height can apply and give it a
    // non-zero height
    return x`<div id="label">${this.displayText || x`&nbsp;`}</div>`;
  }
  renderMenu() {
    const ariaLabel = this.label || this.ariaLabel;
    return x`<div class="menu-wrapper">
      <md-menu
        id="listbox"
        .defaultFocus=${this.defaultFocus}
        role="listbox"
        tabindex="-1"
        aria-label=${ariaLabel || E}
        stay-open-on-focusout
        part="menu"
        exportparts="focus-ring: menu-focus-ring"
        anchor="field"
        style=${o({
      '--__menu-min-width': `${this.selectWidth}px`,
      '--__menu-max-width': this.clampMenuWidth ? `${this.selectWidth}px` : undefined
    })}
        no-navigation-wrap
        .open=${this.open}
        .quick=${this.quick}
        .positioning=${this.menuPositioning}
        .typeaheadDelay=${this.typeaheadDelay}
        .anchorCorner=${this.menuAlign === 'start' ? 'end-start' : 'end-end'}
        .menuCorner=${this.menuAlign === 'start' ? 'start-start' : 'start-end'}
        @opening=${this.handleOpening}
        @opened=${this.redispatchEvent}
        @closing=${this.redispatchEvent}
        @closed=${this.handleClosed}
        @close-menu=${this.handleCloseMenu}
        @request-selection=${this.handleRequestSelection}
        @request-deselection=${this.handleRequestDeselection}>
        ${this.renderMenuContent()}
      </md-menu>
    </div>`;
  }
  renderMenuContent() {
    return x`<slot></slot>`;
  }
  /**
   * Handles opening the select on keydown and typahead selection when the menu
   * is closed.
   */
  handleKeydown(event) {
    if (this.open || this.disabled || !this.menu) {
      return;
    }
    const typeaheadController = this.menu.typeaheadController;
    const isOpenKey = event.code === 'Space' || event.code === 'ArrowDown' || event.code === 'ArrowUp' || event.code === 'End' || event.code === 'Home' || event.code === 'Enter';
    // Do not open if currently typing ahead because the user may be typing the
    // spacebar to match a word with a space
    if (!typeaheadController.isTypingAhead && isOpenKey) {
      event.preventDefault();
      this.open = true;
      // https://www.w3.org/WAI/ARIA/apg/patterns/combobox/examples/combobox-select-only/#kbd_label
      switch (event.code) {
        case 'Space':
        case 'ArrowDown':
        case 'Enter':
          // We will handle focusing last selected item in this.handleOpening()
          this.defaultFocus = FocusState.NONE;
          break;
        case 'End':
          this.defaultFocus = FocusState.LAST_ITEM;
          break;
        case 'ArrowUp':
        case 'Home':
          this.defaultFocus = FocusState.FIRST_ITEM;
          break;
      }
      return;
    }
    const isPrintableKey = event.key.length === 1;
    // Handles typing ahead when the menu is closed by delegating the event to
    // the underlying menu's typeaheadController
    if (isPrintableKey) {
      var _this$labelEl, _this$labelEl$setAttr;
      typeaheadController.onKeydown(event);
      event.preventDefault();
      const {
        lastActiveRecord
      } = typeaheadController;
      if (!lastActiveRecord) {
        return;
      }
      (_this$labelEl = this.labelEl) === null || _this$labelEl === void 0 || (_this$labelEl$setAttr = _this$labelEl.setAttribute) === null || _this$labelEl$setAttr === void 0 || _this$labelEl$setAttr.call(_this$labelEl, 'aria-live', 'polite');
      const hasChanged = this.selectItem(lastActiveRecord[TYPEAHEAD_RECORD.ITEM]);
      if (hasChanged) {
        this.dispatchInteractionEvents();
      }
    }
  }
  handleClick() {
    this.open = !this.open;
  }
  handleFocus() {
    this.focused = true;
  }
  handleBlur() {
    this.focused = false;
  }
  /**
   * Handles closing the menu when the focus leaves the select's subtree.
   */
  handleFocusout(event) {
    // Don't close the menu if we are switching focus between menu,
    // select-option, and field
    if (event.relatedTarget && isElementInSubtree(event.relatedTarget, this)) {
      return;
    }
    this.open = false;
  }
  /**
   * Gets a list of all selected select options as a list item record array.
   *
   * @return An array of selected list option records.
   */
  getSelectedOptions() {
    if (!this.menu) {
      this.lastSelectedOptionRecords = [];
      return null;
    }
    const items = this.menu.items;
    this.lastSelectedOptionRecords = getSelectedItems(items);
    return this.lastSelectedOptionRecords;
  }
  async getUpdateComplete() {
    var _this$menu3;
    await ((_this$menu3 = this.menu) === null || _this$menu3 === void 0 ? void 0 : _this$menu3.updateComplete);
    return super.getUpdateComplete();
  }
  /**
   * Gets the selected options from the DOM, and updates the value and display
   * text to the first selected option's value and headline respectively.
   *
   * @return Whether or not the selected option has changed since last update.
   */
  updateValueAndDisplayText() {
    const selectedOptions = this.getSelectedOptions() ?? [];
    // Used to determine whether or not we need to fire an input / change event
    // which fire whenever the option element changes (value or selectedIndex)
    // on user interaction.
    let hasSelectedOptionChanged = false;
    if (selectedOptions.length) {
      const [firstSelectedOption] = selectedOptions[0];
      hasSelectedOptionChanged = this.lastSelectedOption !== firstSelectedOption;
      this.lastSelectedOption = firstSelectedOption;
      this[VALUE] = firstSelectedOption.value;
      this.displayText = firstSelectedOption.displayText;
    } else {
      hasSelectedOptionChanged = this.lastSelectedOption !== null;
      this.lastSelectedOption = null;
      this[VALUE] = '';
      this.displayText = '';
    }
    return hasSelectedOptionChanged;
  }
  /**
   * Focuses and activates the last selected item upon opening, and resets other
   * active items.
   */
  async handleOpening(e) {
    var _this$labelEl2, _this$labelEl2$remove, _getActiveItem;
    (_this$labelEl2 = this.labelEl) === null || _this$labelEl2 === void 0 || (_this$labelEl2$remove = _this$labelEl2.removeAttribute) === null || _this$labelEl2$remove === void 0 || _this$labelEl2$remove.call(_this$labelEl2, 'aria-live');
    this.redispatchEvent(e);
    // FocusState.NONE means we want to handle focus ourselves and focus the
    // last selected item.
    if (this.defaultFocus !== FocusState.NONE) {
      return;
    }
    const items = this.menu.items;
    const activeItem = (_getActiveItem = getActiveItem(items)) === null || _getActiveItem === void 0 ? void 0 : _getActiveItem.item;
    let [selectedItem] = this.lastSelectedOptionRecords[0] ?? [null];
    // This is true if the user keys through the list but clicks out of the menu
    // thus no close-menu event is fired by an item and we can't clean up in
    // handleCloseMenu.
    if (activeItem && activeItem !== selectedItem) {
      activeItem.tabIndex = -1;
    }
    // in the case that nothing is selected, focus the first item
    selectedItem = selectedItem ?? items[0];
    if (selectedItem) {
      selectedItem.tabIndex = 0;
      selectedItem.focus();
    }
  }
  redispatchEvent(e) {
    redispatchEvent(this, e);
  }
  handleClosed(e) {
    this.open = false;
    this.redispatchEvent(e);
  }
  /**
   * Determines the reason for closing, and updates the UI accordingly.
   */
  handleCloseMenu(event) {
    const reason = event.detail.reason;
    const item = event.detail.itemPath[0];
    this.open = false;
    let hasChanged = false;
    if (reason.kind === 'click-selection') {
      hasChanged = this.selectItem(item);
    } else if (reason.kind === 'keydown' && isSelectableKey(reason.key)) {
      hasChanged = this.selectItem(item);
    } else {
      // This can happen on ESC being pressed
      item.tabIndex = -1;
      item.blur();
    }
    // Dispatch interaction events since selection has been made via keyboard
    // or mouse.
    if (hasChanged) {
      this.dispatchInteractionEvents();
    }
  }
  /**
   * Selects a given option, deselects other options, and updates the UI.
   *
   * @return Whether the last selected option has changed.
   */
  selectItem(item) {
    const selectedOptions = this.getSelectedOptions() ?? [];
    selectedOptions.forEach(([option]) => {
      if (item !== option) {
        option.selected = false;
      }
    });
    item.selected = true;
    return this.updateValueAndDisplayText();
  }
  /**
   * Handles updating selection when an option element requests selection via
   * property / attribute change.
   */
  handleRequestSelection(event) {
    const requestingOptionEl = event.target;
    // No-op if this item is already selected.
    if (this.lastSelectedOptionRecords.some(([option]) => option === requestingOptionEl)) {
      return;
    }
    this.selectItem(requestingOptionEl);
  }
  /**
   * Handles updating selection when an option element requests deselection via
   * property / attribute change.
   */
  handleRequestDeselection(event) {
    const requestingOptionEl = event.target;
    // No-op if this item is not even in the list of tracked selected items.
    if (!this.lastSelectedOptionRecords.some(([option]) => option === requestingOptionEl)) {
      return;
    }
    this.updateValueAndDisplayText();
  }
  /**
   * Attempts to initialize the selected option from user-settable values like
   * SSR, setting `value`, or `selectedIndex` at startup.
   */
  initUserSelection() {
    // User has set `.value` directly, but internals have not yet booted up.
    if (this.lastUserSetValue && !this.lastSelectedOptionRecords.length) {
      this.select(this.lastUserSetValue);
      // User has set `.selectedIndex` directly, but internals have not yet
      // booted up.
    } else if (this.lastUserSetSelectedIndex !== null && !this.lastSelectedOptionRecords.length) {
      this.selectIndex(this.lastUserSetSelectedIndex);
      // Regular boot up!
    } else {
      this.updateValueAndDisplayText();
    }
  }
  handleIconChange() {
    this.hasLeadingIcon = this.leadingIcons.length > 0;
  }
  /**
   * Dispatches the `input` and `change` events.
   */
  dispatchInteractionEvents() {
    this.dispatchEvent(new Event('input', {
      bubbles: true,
      composed: true
    }));
    this.dispatchEvent(new Event('change', {
      bubbles: true
    }));
  }
  getErrorText() {
    return this.error ? this.errorText : this.nativeErrorText;
  }
  [getFormValue]() {
    return this.value;
  }
  formResetCallback() {
    this.reset();
  }
  formStateRestoreCallback(state) {
    this.value = state;
  }
  click() {
    var _this$field2;
    (_this$field2 = this.field) === null || _this$field2 === void 0 || _this$field2.click();
  }
  [createValidator]() {
    return new SelectValidator(() => this);
  }
  [getValidityAnchor]() {
    return this.field;
  }
}
/** @nocollapse */
Select.shadowRootOptions = {
  ...i$2.shadowRootOptions,
  delegatesFocus: true
};
__decorate([n$3({
  type: Boolean
})], Select.prototype, "quick", void 0);
__decorate([n$3({
  type: Boolean
})], Select.prototype, "required", void 0);
__decorate([n$3({
  type: String,
  attribute: 'error-text'
})], Select.prototype, "errorText", void 0);
__decorate([n$3()], Select.prototype, "label", void 0);
__decorate([n$3({
  type: Boolean,
  attribute: 'no-asterisk'
})], Select.prototype, "noAsterisk", void 0);
__decorate([n$3({
  type: String,
  attribute: 'supporting-text'
})], Select.prototype, "supportingText", void 0);
__decorate([n$3({
  type: Boolean,
  reflect: true
})], Select.prototype, "error", void 0);
__decorate([n$3({
  attribute: 'menu-positioning'
})], Select.prototype, "menuPositioning", void 0);
__decorate([n$3({
  type: Boolean,
  attribute: 'clamp-menu-width'
})], Select.prototype, "clampMenuWidth", void 0);
__decorate([n$3({
  type: Number,
  attribute: 'typeahead-delay'
})], Select.prototype, "typeaheadDelay", void 0);
__decorate([n$3({
  type: Boolean,
  attribute: 'has-leading-icon'
})], Select.prototype, "hasLeadingIcon", void 0);
__decorate([n$3({
  attribute: 'display-text'
})], Select.prototype, "displayText", void 0);
__decorate([n$3({
  attribute: 'menu-align'
})], Select.prototype, "menuAlign", void 0);
__decorate([n$3()], Select.prototype, "value", null);
__decorate([n$3({
  type: Number,
  attribute: 'selected-index'
})], Select.prototype, "selectedIndex", null);
__decorate([r$1()], Select.prototype, "nativeError", void 0);
__decorate([r$1()], Select.prototype, "nativeErrorText", void 0);
__decorate([r$1()], Select.prototype, "focused", void 0);
__decorate([r$1()], Select.prototype, "open", void 0);
__decorate([r$1()], Select.prototype, "defaultFocus", void 0);
__decorate([e$2('.field')], Select.prototype, "field", void 0);
__decorate([e$2('md-menu')], Select.prototype, "menu", void 0);
__decorate([e$2('#label')], Select.prototype, "labelEl", void 0);
__decorate([o$2({
  slot: 'leading-icon',
  flatten: true
})], Select.prototype, "leadingIcons", void 0);

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// tslint:disable-next-line:enforce-comments-on-exported-symbols
class FilledSelect extends Select {
  constructor() {
    super(...arguments);
    this.fieldTag = i$1`md-filled-field`;
  }
}

/**
 * @license
 * Copyright 2024 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Generated stylesheet for ./select/internal/filled-select-styles.css.
const styles$3 = i$3`:host{--_text-field-active-indicator-color: var(--md-filled-select-text-field-active-indicator-color, var(--md-sys-color-on-surface-variant, #49454f));--_text-field-active-indicator-height: var(--md-filled-select-text-field-active-indicator-height, 1px);--_text-field-container-color: var(--md-filled-select-text-field-container-color, var(--md-sys-color-surface-container-highest, #e6e0e9));--_text-field-disabled-active-indicator-color: var(--md-filled-select-text-field-disabled-active-indicator-color, var(--md-sys-color-on-surface, #1d1b20));--_text-field-disabled-active-indicator-height: var(--md-filled-select-text-field-disabled-active-indicator-height, 1px);--_text-field-disabled-active-indicator-opacity: var(--md-filled-select-text-field-disabled-active-indicator-opacity, 0.38);--_text-field-disabled-container-color: var(--md-filled-select-text-field-disabled-container-color, var(--md-sys-color-on-surface, #1d1b20));--_text-field-disabled-container-opacity: var(--md-filled-select-text-field-disabled-container-opacity, 0.04);--_text-field-disabled-input-text-color: var(--md-filled-select-text-field-disabled-input-text-color, var(--md-sys-color-on-surface, #1d1b20));--_text-field-disabled-input-text-opacity: var(--md-filled-select-text-field-disabled-input-text-opacity, 0.38);--_text-field-disabled-label-text-color: var(--md-filled-select-text-field-disabled-label-text-color, var(--md-sys-color-on-surface, #1d1b20));--_text-field-disabled-label-text-opacity: var(--md-filled-select-text-field-disabled-label-text-opacity, 0.38);--_text-field-disabled-leading-icon-color: var(--md-filled-select-text-field-disabled-leading-icon-color, var(--md-sys-color-on-surface, #1d1b20));--_text-field-disabled-leading-icon-opacity: var(--md-filled-select-text-field-disabled-leading-icon-opacity, 0.38);--_text-field-disabled-supporting-text-color: var(--md-filled-select-text-field-disabled-supporting-text-color, var(--md-sys-color-on-surface, #1d1b20));--_text-field-disabled-supporting-text-opacity: var(--md-filled-select-text-field-disabled-supporting-text-opacity, 0.38);--_text-field-disabled-trailing-icon-color: var(--md-filled-select-text-field-disabled-trailing-icon-color, var(--md-sys-color-on-surface, #1d1b20));--_text-field-disabled-trailing-icon-opacity: var(--md-filled-select-text-field-disabled-trailing-icon-opacity, 0.38);--_text-field-error-active-indicator-color: var(--md-filled-select-text-field-error-active-indicator-color, var(--md-sys-color-error, #b3261e));--_text-field-error-focus-active-indicator-color: var(--md-filled-select-text-field-error-focus-active-indicator-color, var(--md-sys-color-error, #b3261e));--_text-field-error-focus-input-text-color: var(--md-filled-select-text-field-error-focus-input-text-color, var(--md-sys-color-on-surface, #1d1b20));--_text-field-error-focus-label-text-color: var(--md-filled-select-text-field-error-focus-label-text-color, var(--md-sys-color-error, #b3261e));--_text-field-error-focus-leading-icon-color: var(--md-filled-select-text-field-error-focus-leading-icon-color, var(--md-sys-color-on-surface-variant, #49454f));--_text-field-error-focus-supporting-text-color: var(--md-filled-select-text-field-error-focus-supporting-text-color, var(--md-sys-color-error, #b3261e));--_text-field-error-focus-trailing-icon-color: var(--md-filled-select-text-field-error-focus-trailing-icon-color, var(--md-sys-color-error, #b3261e));--_text-field-error-hover-active-indicator-color: var(--md-filled-select-text-field-error-hover-active-indicator-color, var(--md-sys-color-on-error-container, #410e0b));--_text-field-error-hover-input-text-color: var(--md-filled-select-text-field-error-hover-input-text-color, var(--md-sys-color-on-surface, #1d1b20));--_text-field-error-hover-label-text-color: var(--md-filled-select-text-field-error-hover-label-text-color, var(--md-sys-color-on-error-container, #410e0b));--_text-field-error-hover-leading-icon-color: var(--md-filled-select-text-field-error-hover-leading-icon-color, var(--md-sys-color-on-surface-variant, #49454f));--_text-field-error-hover-state-layer-color: var(--md-filled-select-text-field-error-hover-state-layer-color, var(--md-sys-color-on-surface, #1d1b20));--_text-field-error-hover-state-layer-opacity: var(--md-filled-select-text-field-error-hover-state-layer-opacity, 0.08);--_text-field-error-hover-supporting-text-color: var(--md-filled-select-text-field-error-hover-supporting-text-color, var(--md-sys-color-error, #b3261e));--_text-field-error-hover-trailing-icon-color: var(--md-filled-select-text-field-error-hover-trailing-icon-color, var(--md-sys-color-on-error-container, #410e0b));--_text-field-error-input-text-color: var(--md-filled-select-text-field-error-input-text-color, var(--md-sys-color-on-surface, #1d1b20));--_text-field-error-label-text-color: var(--md-filled-select-text-field-error-label-text-color, var(--md-sys-color-error, #b3261e));--_text-field-error-leading-icon-color: var(--md-filled-select-text-field-error-leading-icon-color, var(--md-sys-color-on-surface-variant, #49454f));--_text-field-error-supporting-text-color: var(--md-filled-select-text-field-error-supporting-text-color, var(--md-sys-color-error, #b3261e));--_text-field-error-trailing-icon-color: var(--md-filled-select-text-field-error-trailing-icon-color, var(--md-sys-color-error, #b3261e));--_text-field-focus-active-indicator-color: var(--md-filled-select-text-field-focus-active-indicator-color, var(--md-sys-color-primary, #6750a4));--_text-field-focus-active-indicator-height: var(--md-filled-select-text-field-focus-active-indicator-height, 3px);--_text-field-focus-input-text-color: var(--md-filled-select-text-field-focus-input-text-color, var(--md-sys-color-on-surface, #1d1b20));--_text-field-focus-label-text-color: var(--md-filled-select-text-field-focus-label-text-color, var(--md-sys-color-primary, #6750a4));--_text-field-focus-leading-icon-color: var(--md-filled-select-text-field-focus-leading-icon-color, var(--md-sys-color-on-surface-variant, #49454f));--_text-field-focus-supporting-text-color: var(--md-filled-select-text-field-focus-supporting-text-color, var(--md-sys-color-on-surface-variant, #49454f));--_text-field-focus-trailing-icon-color: var(--md-filled-select-text-field-focus-trailing-icon-color, var(--md-sys-color-primary, #6750a4));--_text-field-hover-active-indicator-color: var(--md-filled-select-text-field-hover-active-indicator-color, var(--md-sys-color-on-surface, #1d1b20));--_text-field-hover-active-indicator-height: var(--md-filled-select-text-field-hover-active-indicator-height, 1px);--_text-field-hover-input-text-color: var(--md-filled-select-text-field-hover-input-text-color, var(--md-sys-color-on-surface, #1d1b20));--_text-field-hover-label-text-color: var(--md-filled-select-text-field-hover-label-text-color, var(--md-sys-color-on-surface, #1d1b20));--_text-field-hover-leading-icon-color: var(--md-filled-select-text-field-hover-leading-icon-color, var(--md-sys-color-on-surface-variant, #49454f));--_text-field-hover-state-layer-color: var(--md-filled-select-text-field-hover-state-layer-color, var(--md-sys-color-on-surface, #1d1b20));--_text-field-hover-state-layer-opacity: var(--md-filled-select-text-field-hover-state-layer-opacity, 0.08);--_text-field-hover-supporting-text-color: var(--md-filled-select-text-field-hover-supporting-text-color, var(--md-sys-color-on-surface-variant, #49454f));--_text-field-hover-trailing-icon-color: var(--md-filled-select-text-field-hover-trailing-icon-color, var(--md-sys-color-on-surface-variant, #49454f));--_text-field-input-text-color: var(--md-filled-select-text-field-input-text-color, var(--md-sys-color-on-surface, #1d1b20));--_text-field-input-text-font: var(--md-filled-select-text-field-input-text-font, var(--md-sys-typescale-body-large-font, var(--md-ref-typeface-plain, Roboto)));--_text-field-input-text-line-height: var(--md-filled-select-text-field-input-text-line-height, var(--md-sys-typescale-body-large-line-height, 1.5rem));--_text-field-input-text-size: var(--md-filled-select-text-field-input-text-size, var(--md-sys-typescale-body-large-size, 1rem));--_text-field-input-text-weight: var(--md-filled-select-text-field-input-text-weight, var(--md-sys-typescale-body-large-weight, var(--md-ref-typeface-weight-regular, 400)));--_text-field-label-text-color: var(--md-filled-select-text-field-label-text-color, var(--md-sys-color-on-surface-variant, #49454f));--_text-field-label-text-font: var(--md-filled-select-text-field-label-text-font, var(--md-sys-typescale-body-large-font, var(--md-ref-typeface-plain, Roboto)));--_text-field-label-text-line-height: var(--md-filled-select-text-field-label-text-line-height, var(--md-sys-typescale-body-large-line-height, 1.5rem));--_text-field-label-text-populated-line-height: var(--md-filled-select-text-field-label-text-populated-line-height, var(--md-sys-typescale-body-small-line-height, 1rem));--_text-field-label-text-populated-size: var(--md-filled-select-text-field-label-text-populated-size, var(--md-sys-typescale-body-small-size, 0.75rem));--_text-field-label-text-size: var(--md-filled-select-text-field-label-text-size, var(--md-sys-typescale-body-large-size, 1rem));--_text-field-label-text-weight: var(--md-filled-select-text-field-label-text-weight, var(--md-sys-typescale-body-large-weight, var(--md-ref-typeface-weight-regular, 400)));--_text-field-leading-icon-color: var(--md-filled-select-text-field-leading-icon-color, var(--md-sys-color-on-surface-variant, #49454f));--_text-field-leading-icon-size: var(--md-filled-select-text-field-leading-icon-size, 24px);--_text-field-supporting-text-color: var(--md-filled-select-text-field-supporting-text-color, var(--md-sys-color-on-surface-variant, #49454f));--_text-field-supporting-text-font: var(--md-filled-select-text-field-supporting-text-font, var(--md-sys-typescale-body-small-font, var(--md-ref-typeface-plain, Roboto)));--_text-field-supporting-text-line-height: var(--md-filled-select-text-field-supporting-text-line-height, var(--md-sys-typescale-body-small-line-height, 1rem));--_text-field-supporting-text-size: var(--md-filled-select-text-field-supporting-text-size, var(--md-sys-typescale-body-small-size, 0.75rem));--_text-field-supporting-text-weight: var(--md-filled-select-text-field-supporting-text-weight, var(--md-sys-typescale-body-small-weight, var(--md-ref-typeface-weight-regular, 400)));--_text-field-trailing-icon-color: var(--md-filled-select-text-field-trailing-icon-color, var(--md-sys-color-on-surface-variant, #49454f));--_text-field-trailing-icon-size: var(--md-filled-select-text-field-trailing-icon-size, 24px);--_text-field-container-shape-start-start: var(--md-filled-select-text-field-container-shape-start-start, var(--md-filled-select-text-field-container-shape, var(--md-sys-shape-corner-extra-small, 4px)));--_text-field-container-shape-start-end: var(--md-filled-select-text-field-container-shape-start-end, var(--md-filled-select-text-field-container-shape, var(--md-sys-shape-corner-extra-small, 4px)));--_text-field-container-shape-end-end: var(--md-filled-select-text-field-container-shape-end-end, var(--md-filled-select-text-field-container-shape, var(--md-sys-shape-corner-none, 0px)));--_text-field-container-shape-end-start: var(--md-filled-select-text-field-container-shape-end-start, var(--md-filled-select-text-field-container-shape, var(--md-sys-shape-corner-none, 0px)));--md-filled-field-active-indicator-color: var(--_text-field-active-indicator-color);--md-filled-field-active-indicator-height: var(--_text-field-active-indicator-height);--md-filled-field-container-color: var(--_text-field-container-color);--md-filled-field-container-shape-end-end: var(--_text-field-container-shape-end-end);--md-filled-field-container-shape-end-start: var(--_text-field-container-shape-end-start);--md-filled-field-container-shape-start-end: var(--_text-field-container-shape-start-end);--md-filled-field-container-shape-start-start: var(--_text-field-container-shape-start-start);--md-filled-field-content-color: var(--_text-field-input-text-color);--md-filled-field-content-font: var(--_text-field-input-text-font);--md-filled-field-content-line-height: var(--_text-field-input-text-line-height);--md-filled-field-content-size: var(--_text-field-input-text-size);--md-filled-field-content-weight: var(--_text-field-input-text-weight);--md-filled-field-disabled-active-indicator-color: var(--_text-field-disabled-active-indicator-color);--md-filled-field-disabled-active-indicator-height: var(--_text-field-disabled-active-indicator-height);--md-filled-field-disabled-active-indicator-opacity: var(--_text-field-disabled-active-indicator-opacity);--md-filled-field-disabled-container-color: var(--_text-field-disabled-container-color);--md-filled-field-disabled-container-opacity: var(--_text-field-disabled-container-opacity);--md-filled-field-disabled-content-color: var(--_text-field-disabled-input-text-color);--md-filled-field-disabled-content-opacity: var(--_text-field-disabled-input-text-opacity);--md-filled-field-disabled-label-text-color: var(--_text-field-disabled-label-text-color);--md-filled-field-disabled-label-text-opacity: var(--_text-field-disabled-label-text-opacity);--md-filled-field-disabled-leading-content-color: var(--_text-field-disabled-leading-icon-color);--md-filled-field-disabled-leading-content-opacity: var(--_text-field-disabled-leading-icon-opacity);--md-filled-field-disabled-supporting-text-color: var(--_text-field-disabled-supporting-text-color);--md-filled-field-disabled-supporting-text-opacity: var(--_text-field-disabled-supporting-text-opacity);--md-filled-field-disabled-trailing-content-color: var(--_text-field-disabled-trailing-icon-color);--md-filled-field-disabled-trailing-content-opacity: var(--_text-field-disabled-trailing-icon-opacity);--md-filled-field-error-active-indicator-color: var(--_text-field-error-active-indicator-color);--md-filled-field-error-content-color: var(--_text-field-error-input-text-color);--md-filled-field-error-focus-active-indicator-color: var(--_text-field-error-focus-active-indicator-color);--md-filled-field-error-focus-content-color: var(--_text-field-error-focus-input-text-color);--md-filled-field-error-focus-label-text-color: var(--_text-field-error-focus-label-text-color);--md-filled-field-error-focus-leading-content-color: var(--_text-field-error-focus-leading-icon-color);--md-filled-field-error-focus-supporting-text-color: var(--_text-field-error-focus-supporting-text-color);--md-filled-field-error-focus-trailing-content-color: var(--_text-field-error-focus-trailing-icon-color);--md-filled-field-error-hover-active-indicator-color: var(--_text-field-error-hover-active-indicator-color);--md-filled-field-error-hover-content-color: var(--_text-field-error-hover-input-text-color);--md-filled-field-error-hover-label-text-color: var(--_text-field-error-hover-label-text-color);--md-filled-field-error-hover-leading-content-color: var(--_text-field-error-hover-leading-icon-color);--md-filled-field-error-hover-state-layer-color: var(--_text-field-error-hover-state-layer-color);--md-filled-field-error-hover-state-layer-opacity: var(--_text-field-error-hover-state-layer-opacity);--md-filled-field-error-hover-supporting-text-color: var(--_text-field-error-hover-supporting-text-color);--md-filled-field-error-hover-trailing-content-color: var(--_text-field-error-hover-trailing-icon-color);--md-filled-field-error-label-text-color: var(--_text-field-error-label-text-color);--md-filled-field-error-leading-content-color: var(--_text-field-error-leading-icon-color);--md-filled-field-error-supporting-text-color: var(--_text-field-error-supporting-text-color);--md-filled-field-error-trailing-content-color: var(--_text-field-error-trailing-icon-color);--md-filled-field-focus-active-indicator-color: var(--_text-field-focus-active-indicator-color);--md-filled-field-focus-active-indicator-height: var(--_text-field-focus-active-indicator-height);--md-filled-field-focus-content-color: var(--_text-field-focus-input-text-color);--md-filled-field-focus-label-text-color: var(--_text-field-focus-label-text-color);--md-filled-field-focus-leading-content-color: var(--_text-field-focus-leading-icon-color);--md-filled-field-focus-supporting-text-color: var(--_text-field-focus-supporting-text-color);--md-filled-field-focus-trailing-content-color: var(--_text-field-focus-trailing-icon-color);--md-filled-field-hover-active-indicator-color: var(--_text-field-hover-active-indicator-color);--md-filled-field-hover-active-indicator-height: var(--_text-field-hover-active-indicator-height);--md-filled-field-hover-content-color: var(--_text-field-hover-input-text-color);--md-filled-field-hover-label-text-color: var(--_text-field-hover-label-text-color);--md-filled-field-hover-leading-content-color: var(--_text-field-hover-leading-icon-color);--md-filled-field-hover-state-layer-color: var(--_text-field-hover-state-layer-color);--md-filled-field-hover-state-layer-opacity: var(--_text-field-hover-state-layer-opacity);--md-filled-field-hover-supporting-text-color: var(--_text-field-hover-supporting-text-color);--md-filled-field-hover-trailing-content-color: var(--_text-field-hover-trailing-icon-color);--md-filled-field-label-text-color: var(--_text-field-label-text-color);--md-filled-field-label-text-font: var(--_text-field-label-text-font);--md-filled-field-label-text-line-height: var(--_text-field-label-text-line-height);--md-filled-field-label-text-populated-line-height: var(--_text-field-label-text-populated-line-height);--md-filled-field-label-text-populated-size: var(--_text-field-label-text-populated-size);--md-filled-field-label-text-size: var(--_text-field-label-text-size);--md-filled-field-label-text-weight: var(--_text-field-label-text-weight);--md-filled-field-leading-content-color: var(--_text-field-leading-icon-color);--md-filled-field-supporting-text-color: var(--_text-field-supporting-text-color);--md-filled-field-supporting-text-font: var(--_text-field-supporting-text-font);--md-filled-field-supporting-text-line-height: var(--_text-field-supporting-text-line-height);--md-filled-field-supporting-text-size: var(--_text-field-supporting-text-size);--md-filled-field-supporting-text-weight: var(--_text-field-supporting-text-weight);--md-filled-field-trailing-content-color: var(--_text-field-trailing-icon-color)}[has-start] .icon.leading{font-size:var(--_text-field-leading-icon-size);height:var(--_text-field-leading-icon-size);width:var(--_text-field-leading-icon-size)}.icon.trailing{font-size:var(--_text-field-trailing-icon-size);height:var(--_text-field-trailing-icon-size);width:var(--_text-field-trailing-icon-size)}
`;

/**
 * @license
 * Copyright 2024 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Generated stylesheet for ./select/internal/shared-styles.css.
const styles$2 = i$3`:host{color:unset;min-width:210px;display:flex}.field{cursor:default;outline:none}.select{position:relative;flex-direction:column}.icon.trailing svg,.icon ::slotted(*){fill:currentColor}.icon ::slotted(*){width:inherit;height:inherit;font-size:inherit}.icon slot{display:flex;height:100%;width:100%;align-items:center;justify-content:center}.icon.trailing :is(.up,.down){opacity:0;transition:opacity 75ms linear 75ms}.select:not(.open) .down,.select.open .up{opacity:1}.field,.select,md-menu{min-width:inherit;width:inherit;max-width:inherit;display:flex}md-menu{min-width:var(--__menu-min-width);max-width:var(--__menu-max-width, inherit)}.menu-wrapper{width:0px;height:0px;max-width:inherit}md-menu ::slotted(:not[disabled]){cursor:pointer}.field,.select{width:100%}:host{display:inline-flex}:host([disabled]){pointer-events:none}
`;

class EwFilledSelect extends FilledSelect {}
EwFilledSelect.styles = [styles$2, styles$3];
customElements.define("ew-filled-select", EwFilledSelect);

/**
 * @license
 * Copyright 2024 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Generated stylesheet for ./menu/internal/menuitem/menu-item-styles.css.
const styles$1 = i$3`:host{display:flex;--md-ripple-hover-color: var(--md-menu-item-hover-state-layer-color, var(--md-sys-color-on-surface, #1d1b20));--md-ripple-hover-opacity: var(--md-menu-item-hover-state-layer-opacity, 0.08);--md-ripple-pressed-color: var(--md-menu-item-pressed-state-layer-color, var(--md-sys-color-on-surface, #1d1b20));--md-ripple-pressed-opacity: var(--md-menu-item-pressed-state-layer-opacity, 0.12)}:host([disabled]){opacity:var(--md-menu-item-disabled-opacity, 0.3);pointer-events:none}md-focus-ring{z-index:1;--md-focus-ring-shape: 8px}a,button,li{background:none;border:none;padding:0;margin:0;text-align:unset;text-decoration:none}.list-item{border-radius:inherit;display:flex;flex:1;max-width:inherit;min-width:inherit;outline:none;-webkit-tap-highlight-color:rgba(0,0,0,0)}.list-item:not(.disabled){cursor:pointer}[slot=container]{pointer-events:none}md-ripple{border-radius:inherit}md-item{border-radius:inherit;flex:1;color:var(--md-menu-item-label-text-color, var(--md-sys-color-on-surface, #1d1b20));font-family:var(--md-menu-item-label-text-font, var(--md-sys-typescale-body-large-font, var(--md-ref-typeface-plain, Roboto)));font-size:var(--md-menu-item-label-text-size, var(--md-sys-typescale-body-large-size, 1rem));line-height:var(--md-menu-item-label-text-line-height, var(--md-sys-typescale-body-large-line-height, 1.5rem));font-weight:var(--md-menu-item-label-text-weight, var(--md-sys-typescale-body-large-weight, var(--md-ref-typeface-weight-regular, 400)));min-height:var(--md-menu-item-one-line-container-height, 56px);padding-top:var(--md-menu-item-top-space, 12px);padding-bottom:var(--md-menu-item-bottom-space, 12px);padding-inline-start:var(--md-menu-item-leading-space, 16px);padding-inline-end:var(--md-menu-item-trailing-space, 16px)}md-item[multiline]{min-height:var(--md-menu-item-two-line-container-height, 72px)}[slot=supporting-text]{color:var(--md-menu-item-supporting-text-color, var(--md-sys-color-on-surface-variant, #49454f));font-family:var(--md-menu-item-supporting-text-font, var(--md-sys-typescale-body-medium-font, var(--md-ref-typeface-plain, Roboto)));font-size:var(--md-menu-item-supporting-text-size, var(--md-sys-typescale-body-medium-size, 0.875rem));line-height:var(--md-menu-item-supporting-text-line-height, var(--md-sys-typescale-body-medium-line-height, 1.25rem));font-weight:var(--md-menu-item-supporting-text-weight, var(--md-sys-typescale-body-medium-weight, var(--md-ref-typeface-weight-regular, 400)))}[slot=trailing-supporting-text]{color:var(--md-menu-item-trailing-supporting-text-color, var(--md-sys-color-on-surface-variant, #49454f));font-family:var(--md-menu-item-trailing-supporting-text-font, var(--md-sys-typescale-label-small-font, var(--md-ref-typeface-plain, Roboto)));font-size:var(--md-menu-item-trailing-supporting-text-size, var(--md-sys-typescale-label-small-size, 0.6875rem));line-height:var(--md-menu-item-trailing-supporting-text-line-height, var(--md-sys-typescale-label-small-line-height, 1rem));font-weight:var(--md-menu-item-trailing-supporting-text-weight, var(--md-sys-typescale-label-small-weight, var(--md-ref-typeface-weight-medium, 500)))}:is([slot=start],[slot=end])::slotted(*){fill:currentColor}[slot=start]{color:var(--md-menu-item-leading-icon-color, var(--md-sys-color-on-surface-variant, #49454f))}[slot=end]{color:var(--md-menu-item-trailing-icon-color, var(--md-sys-color-on-surface-variant, #49454f))}.list-item{background-color:var(--md-menu-item-container-color, transparent)}.list-item.selected{background-color:var(--md-menu-item-selected-container-color, var(--md-sys-color-secondary-container, #e8def8))}.selected:not(.disabled) ::slotted(*){color:var(--md-menu-item-selected-label-text-color, var(--md-sys-color-on-secondary-container, #1d192b))}@media(forced-colors: active){:host([disabled]),:host([disabled]) slot{color:GrayText;opacity:1}.list-item{position:relative}.list-item.selected::before{content:"";position:absolute;inset:0;box-sizing:border-box;border-radius:inherit;pointer-events:none;border:3px double CanvasText}}
`;

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * A controller that provides most functionality of an element that implements
 * the MenuItem interface.
 */
class MenuItemController {
  /**
   * @param host The MenuItem in which to attach this controller to.
   * @param config The object that configures this controller's behavior.
   */
  constructor(host, config) {
    this.host = host;
    this.internalTypeaheadText = null;
    /**
     * Bind this click listener to the interactive element. Handles closing the
     * menu.
     */
    this.onClick = () => {
      if (this.host.keepOpen) return;
      this.host.dispatchEvent(createDefaultCloseMenuEvent(this.host, {
        kind: CloseReason.CLICK_SELECTION
      }));
    };
    /**
     * Bind this click listener to the interactive element. Handles closing the
     * menu.
     */
    this.onKeydown = event => {
      // Check if the interactive element is an anchor tag. If so, click it.
      if (this.host.href && event.code === 'Enter') {
        const interactiveElement = this.getInteractiveElement();
        if (interactiveElement instanceof HTMLAnchorElement) {
          interactiveElement.click();
        }
      }
      if (event.defaultPrevented) return;
      // If the host has keepOpen = true we should ignore clicks & Space/Enter,
      // however we always maintain the ability to close a menu with a explicit
      // `escape` keypress.
      const keyCode = event.code;
      if (this.host.keepOpen && keyCode !== 'Escape') return;
      if (isClosableKey(keyCode)) {
        event.preventDefault();
        this.host.dispatchEvent(createDefaultCloseMenuEvent(this.host, {
          kind: CloseReason.KEYDOWN,
          key: keyCode
        }));
      }
    };
    this.getHeadlineElements = config.getHeadlineElements;
    this.getSupportingTextElements = config.getSupportingTextElements;
    this.getDefaultElements = config.getDefaultElements;
    this.getInteractiveElement = config.getInteractiveElement;
    this.host.addController(this);
  }
  /**
   * The text that is selectable via typeahead. If not set, defaults to the
   * innerText of the item slotted into the `"headline"` slot, and if there are
   * no slotted elements into headline, then it checks the _default_ slot, and
   * then the `"supporting-text"` slot if nothing is in _default_.
   */
  get typeaheadText() {
    if (this.internalTypeaheadText !== null) {
      return this.internalTypeaheadText;
    }
    const headlineElements = this.getHeadlineElements();
    const textParts = [];
    headlineElements.forEach(headlineElement => {
      if (headlineElement.textContent && headlineElement.textContent.trim()) {
        textParts.push(headlineElement.textContent.trim());
      }
    });
    // If there are no headline elements, check the default slot's text content
    if (textParts.length === 0) {
      this.getDefaultElements().forEach(defaultElement => {
        if (defaultElement.textContent && defaultElement.textContent.trim()) {
          textParts.push(defaultElement.textContent.trim());
        }
      });
    }
    // If there are no headline nor default slot elements, check the
    //supporting-text slot's text content
    if (textParts.length === 0) {
      this.getSupportingTextElements().forEach(supportingTextElement => {
        if (supportingTextElement.textContent && supportingTextElement.textContent.trim()) {
          textParts.push(supportingTextElement.textContent.trim());
        }
      });
    }
    return textParts.join(' ');
  }
  /**
   * The recommended tag name to render as the list item.
   */
  get tagName() {
    const type = this.host.type;
    switch (type) {
      case 'link':
        return 'a';
      case 'button':
        return 'button';
      default:
      case 'menuitem':
      case 'option':
        return 'li';
    }
  }
  /**
   * The recommended role of the menu item.
   */
  get role() {
    return this.host.type === 'option' ? 'option' : 'menuitem';
  }
  hostConnected() {
    this.host.toggleAttribute('md-menu-item', true);
  }
  hostUpdate() {
    if (this.host.href) {
      this.host.type = 'link';
    }
  }
  /**
   * Use to set the typeaheadText when it changes.
   */
  setTypeaheadText(text) {
    this.internalTypeaheadText = text;
  }
}

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * Creates an event fired by a SelectOption to request selection from md-select.
 * Typically fired after `selected` changes from `false` to `true`.
 */
function createRequestSelectionEvent() {
  return new Event('request-selection', {
    bubbles: true,
    composed: true
  });
}
/**
 * Creates an event fired by a SelectOption to request deselection from
 * md-select. Typically fired after `selected` changes from `true` to `false`.
 */
function createRequestDeselectionEvent() {
  return new Event('request-deselection', {
    bubbles: true,
    composed: true
  });
}
/**
 * A controller that provides most functionality and md-select compatibility for
 * an element that implements the SelectOption interface.
 */
class SelectOptionController {
  /**
   * The recommended role of the select option.
   */
  get role() {
    return this.menuItemController.role;
  }
  /**
   * The text that is selectable via typeahead. If not set, defaults to the
   * innerText of the item slotted into the `"headline"` slot, and if there are
   * no slotted elements into headline, then it checks the _default_ slot, and
   * then the `"supporting-text"` slot if nothing is in _default_.
   */
  get typeaheadText() {
    return this.menuItemController.typeaheadText;
  }
  setTypeaheadText(text) {
    this.menuItemController.setTypeaheadText(text);
  }
  /**
   * The text that is displayed in the select field when selected. If not set,
   * defaults to the textContent of the item slotted into the `"headline"` slot,
   * and if there are no slotted elements into headline, then it checks the
   * _default_ slot, and then the `"supporting-text"` slot if nothing is in
   * _default_.
   */
  get displayText() {
    if (this.internalDisplayText !== null) {
      return this.internalDisplayText;
    }
    return this.menuItemController.typeaheadText;
  }
  setDisplayText(text) {
    this.internalDisplayText = text;
  }
  /**
   * @param host The SelectOption in which to attach this controller to.
   * @param config The object that configures this controller's behavior.
   */
  constructor(host, config) {
    this.host = host;
    this.internalDisplayText = null;
    this.firstUpdate = true;
    /**
     * Bind this click listener to the interactive element. Handles closing the
     * menu.
     */
    this.onClick = () => {
      this.menuItemController.onClick();
    };
    /**
     * Bind this click listener to the interactive element. Handles closing the
     * menu.
     */
    this.onKeydown = e => {
      this.menuItemController.onKeydown(e);
    };
    this.lastSelected = this.host.selected;
    this.menuItemController = new MenuItemController(host, config);
    host.addController(this);
  }
  hostUpdate() {
    if (this.lastSelected !== this.host.selected) {
      this.host.ariaSelected = this.host.selected ? 'true' : 'false';
    }
  }
  hostUpdated() {
    // Do not dispatch event on first update / boot-up.
    if (this.lastSelected !== this.host.selected && !this.firstUpdate) {
      // This section is really useful for when the user sets selected on the
      // option programmatically. Most other cases (click and keyboard) are
      // handled by md-select because it needs to coordinate the
      // single-selection behavior.
      if (this.host.selected) {
        this.host.dispatchEvent(createRequestSelectionEvent());
      } else {
        this.host.dispatchEvent(createRequestDeselectionEvent());
      }
    }
    this.lastSelected = this.host.selected;
    this.firstUpdate = false;
  }
}

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Separate variable needed for closure.
const selectOptionBaseClass = mixinDelegatesAria(i$2);
/**
 * @fires close-menu {CustomEvent<{initiator: SelectOption, reason: Reason, itemPath: SelectOption[]}>}
 * Closes the encapsulating menu on closable interaction. --bubbles --composed
 * @fires request-selection {Event} Requests the parent md-select to select this
 * element (and deselect others if single-selection) when `selected` changed to
 * `true`. --bubbles --composed
 * @fires request-deselection {Event} Requests the parent md-select to deselect
 * this element when `selected` changed to `false`. --bubbles --composed
 */
class SelectOptionEl extends selectOptionBaseClass {
  constructor() {
    super(...arguments);
    /**
     * Disables the item and makes it non-selectable and non-interactive.
     */
    this.disabled = false;
    /**
     * READONLY: self-identifies as a menu item and sets its identifying attribute
     */
    this.isMenuItem = true;
    /**
     * Sets the item in the selected visual state when a submenu is opened.
     */
    this.selected = false;
    /**
     * Form value of the option.
     */
    this.value = '';
    this.type = 'option';
    this.selectOptionController = new SelectOptionController(this, {
      getHeadlineElements: () => {
        return this.headlineElements;
      },
      getSupportingTextElements: () => {
        return this.supportingTextElements;
      },
      getDefaultElements: () => {
        return this.defaultElements;
      },
      getInteractiveElement: () => this.listItemRoot
    });
  }
  /**
   * The text that is selectable via typeahead. If not set, defaults to the
   * innerText of the item slotted into the `"headline"` slot.
   */
  get typeaheadText() {
    return this.selectOptionController.typeaheadText;
  }
  set typeaheadText(text) {
    this.selectOptionController.setTypeaheadText(text);
  }
  /**
   * The text that is displayed in the select field when selected. If not set,
   * defaults to the textContent of the item slotted into the `"headline"` slot.
   */
  get displayText() {
    return this.selectOptionController.displayText;
  }
  set displayText(text) {
    this.selectOptionController.setDisplayText(text);
  }
  render() {
    return this.renderListItem(x`
      <md-item>
        <div slot="container">
          ${this.renderRipple()} ${this.renderFocusRing()}
        </div>
        <slot name="start" slot="start"></slot>
        <slot name="end" slot="end"></slot>
        ${this.renderBody()}
      </md-item>
    `);
  }
  /**
   * Renders the root list item.
   *
   * @param content the child content of the list item.
   */
  renderListItem(content) {
    return x`
      <li
        id="item"
        tabindex=${this.disabled ? -1 : 0}
        role=${this.selectOptionController.role}
        aria-label=${this.ariaLabel || E}
        aria-selected=${this.ariaSelected || E}
        aria-checked=${this.ariaChecked || E}
        aria-expanded=${this.ariaExpanded || E}
        aria-haspopup=${this.ariaHasPopup || E}
        class="list-item ${e$3(this.getRenderClasses())}"
        @click=${this.selectOptionController.onClick}
        @keydown=${this.selectOptionController.onKeydown}
        >${content}</li
      >
    `;
  }
  /**
   * Handles rendering of the ripple element.
   */
  renderRipple() {
    return x` <md-ripple
      part="ripple"
      for="item"
      ?disabled=${this.disabled}></md-ripple>`;
  }
  /**
   * Handles rendering of the focus ring.
   */
  renderFocusRing() {
    return x` <md-focus-ring
      part="focus-ring"
      for="item"
      inward></md-focus-ring>`;
  }
  /**
   * Classes applied to the list item root.
   */
  getRenderClasses() {
    return {
      'disabled': this.disabled,
      'selected': this.selected
    };
  }
  /**
   * Handles rendering the headline and supporting text.
   */
  renderBody() {
    return x`
      <slot></slot>
      <slot name="overline" slot="overline"></slot>
      <slot name="headline" slot="headline"></slot>
      <slot name="supporting-text" slot="supporting-text"></slot>
      <slot
        name="trailing-supporting-text"
        slot="trailing-supporting-text"></slot>
    `;
  }
  focus() {
    var _this$listItemRoot;
    // TODO(b/300334509): needed for some cases where delegatesFocus doesn't
    // work programmatically like in FF and select-option
    (_this$listItemRoot = this.listItemRoot) === null || _this$listItemRoot === void 0 || _this$listItemRoot.focus();
  }
}
/** @nocollapse */
SelectOptionEl.shadowRootOptions = {
  ...i$2.shadowRootOptions,
  delegatesFocus: true
};
__decorate([n$3({
  type: Boolean,
  reflect: true
})], SelectOptionEl.prototype, "disabled", void 0);
__decorate([n$3({
  type: Boolean,
  attribute: 'md-menu-item',
  reflect: true
})], SelectOptionEl.prototype, "isMenuItem", void 0);
__decorate([n$3({
  type: Boolean
})], SelectOptionEl.prototype, "selected", void 0);
__decorate([n$3()], SelectOptionEl.prototype, "value", void 0);
__decorate([e$2('.list-item')], SelectOptionEl.prototype, "listItemRoot", void 0);
__decorate([o$2({
  slot: 'headline'
})], SelectOptionEl.prototype, "headlineElements", void 0);
__decorate([o$2({
  slot: 'supporting-text'
})], SelectOptionEl.prototype, "supportingTextElements", void 0);
__decorate([n$2({
  slot: ''
})], SelectOptionEl.prototype, "defaultElements", void 0);
__decorate([n$3({
  attribute: 'typeahead-text'
})], SelectOptionEl.prototype, "typeaheadText", null);
__decorate([n$3({
  attribute: 'display-text'
})], SelectOptionEl.prototype, "displayText", null);

class EwSelectOption extends SelectOptionEl {}
EwSelectOption.styles = [styles$1];
customElements.define("ew-select-option", EwSelectOption);

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Separate variable needed for closure.
const progressBaseClass = mixinDelegatesAria(i$2);
/**
 * A progress component.
 */
class Progress extends progressBaseClass {
  constructor() {
    super(...arguments);
    /**
     * Progress to display, a fraction between 0 and `max`.
     */
    this.value = 0;
    /**
     * Maximum progress to display, defaults to 1.
     */
    this.max = 1;
    /**
     * Whether or not to display indeterminate progress, which gives no indication
     * to how long an activity will take.
     */
    this.indeterminate = false;
    /**
     * Whether or not to render indeterminate mode using 4 colors instead of one.
     */
    this.fourColor = false;
  }
  render() {
    // Needed for closure conformance
    const {
      ariaLabel
    } = this;
    return x`
      <div
        class="progress ${e$3(this.getRenderClasses())}"
        role="progressbar"
        aria-label="${ariaLabel || E}"
        aria-valuemin="0"
        aria-valuemax=${this.max}
        aria-valuenow=${this.indeterminate ? E : this.value}
        >${this.renderIndicator()}</div
      >
    `;
  }
  getRenderClasses() {
    return {
      'indeterminate': this.indeterminate,
      'four-color': this.fourColor
    };
  }
}
__decorate([n$3({
  type: Number
})], Progress.prototype, "value", void 0);
__decorate([n$3({
  type: Number
})], Progress.prototype, "max", void 0);
__decorate([n$3({
  type: Boolean
})], Progress.prototype, "indeterminate", void 0);
__decorate([n$3({
  type: Boolean,
  attribute: 'four-color'
})], Progress.prototype, "fourColor", void 0);

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * A circular progress component.
 */
class CircularProgress extends Progress {
  renderIndicator() {
    if (this.indeterminate) {
      return this.renderIndeterminateContainer();
    }
    return this.renderDeterminateContainer();
  }
  // Determinate mode is rendered with an svg so the progress arc can be
  // easily animated via stroke-dashoffset.
  renderDeterminateContainer() {
    const dashOffset = (1 - this.value / this.max) * 100;
    // note, dash-array/offset are relative to Setting `pathLength` but
    // Chrome seems to render this inaccurately and using a large viewbox helps.
    return x`
      <svg viewBox="0 0 4800 4800">
        <circle class="track" pathLength="100"></circle>
        <circle
          class="active-track"
          pathLength="100"
          stroke-dashoffset=${dashOffset}></circle>
      </svg>
    `;
  }
  // Indeterminate mode rendered with 2 bordered-divs. The borders are
  // clipped into half circles by their containers. The divs are then carefully
  // animated to produce changes to the spinner arc size.
  // This approach has 4.5x the FPS of rendering via svg on Chrome 111.
  // See https://lit.dev/playground/#gist=febb773565272f75408ab06a0eb49746.
  renderIndeterminateContainer() {
    return x` <div class="spinner">
      <div class="left">
        <div class="circle"></div>
      </div>
      <div class="right">
        <div class="circle"></div>
      </div>
    </div>`;
  }
}

/**
 * @license
 * Copyright 2024 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Generated stylesheet for ./progress/internal/circular-progress-styles.css.
const styles = i$3`:host{--_active-indicator-color: var(--md-circular-progress-active-indicator-color, var(--md-sys-color-primary, #6750a4));--_active-indicator-width: var(--md-circular-progress-active-indicator-width, 10);--_four-color-active-indicator-four-color: var(--md-circular-progress-four-color-active-indicator-four-color, var(--md-sys-color-tertiary-container, #ffd8e4));--_four-color-active-indicator-one-color: var(--md-circular-progress-four-color-active-indicator-one-color, var(--md-sys-color-primary, #6750a4));--_four-color-active-indicator-three-color: var(--md-circular-progress-four-color-active-indicator-three-color, var(--md-sys-color-tertiary, #7d5260));--_four-color-active-indicator-two-color: var(--md-circular-progress-four-color-active-indicator-two-color, var(--md-sys-color-primary-container, #eaddff));--_size: var(--md-circular-progress-size, 48px);display:inline-flex;vertical-align:middle;width:var(--_size);height:var(--_size);position:relative;align-items:center;justify-content:center;contain:strict;content-visibility:auto}.progress{flex:1;align-self:stretch;margin:4px}.progress,.spinner,.left,.right,.circle,svg,.track,.active-track{position:absolute;inset:0}svg{transform:rotate(-90deg)}circle{cx:50%;cy:50%;r:calc(50%*(1 - var(--_active-indicator-width)/100));stroke-width:calc(var(--_active-indicator-width)*1%);stroke-dasharray:100;fill:rgba(0,0,0,0)}.active-track{transition:stroke-dashoffset 500ms cubic-bezier(0, 0, 0.2, 1);stroke:var(--_active-indicator-color)}.track{stroke:rgba(0,0,0,0)}.progress.indeterminate{animation:linear infinite linear-rotate;animation-duration:1568.2352941176ms}.spinner{animation:infinite both rotate-arc;animation-duration:5332ms;animation-timing-function:cubic-bezier(0.4, 0, 0.2, 1)}.left{overflow:hidden;inset:0 50% 0 0}.right{overflow:hidden;inset:0 0 0 50%}.circle{box-sizing:border-box;border-radius:50%;border:solid calc(var(--_active-indicator-width)/100*(var(--_size) - 8px));border-color:var(--_active-indicator-color) var(--_active-indicator-color) rgba(0,0,0,0) rgba(0,0,0,0);animation:expand-arc;animation-iteration-count:infinite;animation-fill-mode:both;animation-duration:1333ms,5332ms;animation-timing-function:cubic-bezier(0.4, 0, 0.2, 1)}.four-color .circle{animation-name:expand-arc,four-color}.left .circle{rotate:135deg;inset:0 -100% 0 0}.right .circle{rotate:100deg;inset:0 0 0 -100%;animation-delay:-666.5ms,0ms}@media(forced-colors: active){.active-track{stroke:CanvasText}.circle{border-color:CanvasText CanvasText Canvas Canvas}}@keyframes expand-arc{0%{transform:rotate(265deg)}50%{transform:rotate(130deg)}100%{transform:rotate(265deg)}}@keyframes rotate-arc{12.5%{transform:rotate(135deg)}25%{transform:rotate(270deg)}37.5%{transform:rotate(405deg)}50%{transform:rotate(540deg)}62.5%{transform:rotate(675deg)}75%{transform:rotate(810deg)}87.5%{transform:rotate(945deg)}100%{transform:rotate(1080deg)}}@keyframes linear-rotate{to{transform:rotate(360deg)}}@keyframes four-color{0%{border-top-color:var(--_four-color-active-indicator-one-color);border-right-color:var(--_four-color-active-indicator-one-color)}15%{border-top-color:var(--_four-color-active-indicator-one-color);border-right-color:var(--_four-color-active-indicator-one-color)}25%{border-top-color:var(--_four-color-active-indicator-two-color);border-right-color:var(--_four-color-active-indicator-two-color)}40%{border-top-color:var(--_four-color-active-indicator-two-color);border-right-color:var(--_four-color-active-indicator-two-color)}50%{border-top-color:var(--_four-color-active-indicator-three-color);border-right-color:var(--_four-color-active-indicator-three-color)}65%{border-top-color:var(--_four-color-active-indicator-three-color);border-right-color:var(--_four-color-active-indicator-three-color)}75%{border-top-color:var(--_four-color-active-indicator-four-color);border-right-color:var(--_four-color-active-indicator-four-color)}90%{border-top-color:var(--_four-color-active-indicator-four-color);border-right-color:var(--_four-color-active-indicator-four-color)}100%{border-top-color:var(--_four-color-active-indicator-one-color);border-right-color:var(--_four-color-active-indicator-one-color)}}
`;

class EwCircularProgress extends CircularProgress {}
EwCircularProgress.styles = [styles];
customElements.define("ew-circular-progress", EwCircularProgress);

class EwtPageProgress extends i$2 {
  render() {
    return x`
      <div>
        <ew-circular-progress
          active
          ?indeterminate=${this.progress === undefined}
          .value=${this.progress !== undefined ? this.progress / 100 : undefined}
        ></ew-circular-progress>
        ${this.progress !== undefined ? x`<div>${this.progress}%</div>` : ""}
      </div>
      ${this.label}
    `;
  }
}
EwtPageProgress.styles = i$3`
    :host {
      display: flex;
      flex-direction: column;
      text-align: center;
    }
    ew-circular-progress {
      margin-bottom: 16px;
    }
  `;
__decorate([n$3()], EwtPageProgress.prototype, "label", void 0);
__decorate([n$3()], EwtPageProgress.prototype, "progress", void 0);
customElements.define("ewt-page-progress", EwtPageProgress);

class EwtPageMessage extends i$2 {
  render() {
    return x`
      <div class="icon">${this.icon}</div>
      ${this.label}
    `;
  }
}
EwtPageMessage.styles = i$3`
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
__decorate([n$3()], EwtPageMessage.prototype, "icon", void 0);
__decorate([n$3()], EwtPageMessage.prototype, "label", void 0);
customElements.define("ewt-page-message", EwtPageMessage);

const closeIcon = b`
  <svg width="24" height="24" viewBox="0 0 24 24">
    <path
      fill="currentColor"
      d="M19,6.41L17.59,5L12,10.59L6.41,5L5,6.41L10.59,12L5,17.59L6.41,19L12,13.41L17.59,19L19,17.59L13.41,12L19,6.41Z"
    />
  </svg>
`;
const refreshIcon = b`
  <svg viewBox="0 0 24 24">
    <path
      fill="currentColor"
      d="M17.65,6.35C16.2,4.9 14.21,4 12,4A8,8 0 0,0 4,12A8,8 0 0,0 12,20C15.73,20 18.84,17.45 19.73,14H17.65C16.83,16.33 14.61,18 12,18A6,6 0 0,1 6,12A6,6 0 0,1 12,6C13.66,6 15.14,6.69 16.22,7.78L13,11H20V4L17.65,6.35Z"
    />
  </svg>
`;
const listItemInstallIcon = b`
  <svg slot="start" viewBox="0 0 24 24">
    <path d="M5,20H19V18H5M19,9H15V3H9V9H5L12,16L19,9Z" />
  </svg>
`;
const listItemWifi = b`
  <svg slot="start" viewBox="0 0 24 24">
    <path d="M12,21L15.6,16.2C14.6,15.45 13.35,15 12,15C10.65,15 9.4,15.45 8.4,16.2L12,21M12,3C7.95,3 4.21,4.34 1.2,6.6L3,9C5.5,7.12 8.62,6 12,6C15.38,6 18.5,7.12 21,9L22.8,6.6C19.79,4.34 16.05,3 12,3M12,9C9.3,9 6.81,9.89 4.8,11.4L6.6,13.8C8.1,12.67 9.97,12 12,12C14.03,12 15.9,12.67 17.4,13.8L19.2,11.4C17.19,9.89 14.7,9 12,9Z" />
  </svg>
`;
const listItemConsole = b`
  <svg slot="start" viewBox="0 0 24 24">
    <path d="M20,19V7H4V19H20M20,3A2,2 0 0,1 22,5V19A2,2 0 0,1 20,21H4A2,2 0 0,1 2,19V5C2,3.89 2.9,3 4,3H20M13,17V15H18V17H13M9.58,13L5.57,9H8.4L11.7,12.3C12.09,12.69 12.09,13.33 11.7,13.72L8.42,17H5.59L9.58,13Z" />
  </svg>
`;
const listItemVisitDevice = b`
  <svg slot="start" viewBox="0 0 24 24">
  <path d="M16.36,14C16.44,13.34 16.5,12.68 16.5,12C16.5,11.32 16.44,10.66 16.36,10H19.74C19.9,10.64 20,11.31 20,12C20,12.69 19.9,13.36 19.74,14M14.59,19.56C15.19,18.45 15.65,17.25 15.97,16H18.92C17.96,17.65 16.43,18.93 14.59,19.56M14.34,14H9.66C9.56,13.34 9.5,12.68 9.5,12C9.5,11.32 9.56,10.65 9.66,10H14.34C14.43,10.65 14.5,11.32 14.5,12C14.5,12.68 14.43,13.34 14.34,14M12,19.96C11.17,18.76 10.5,17.43 10.09,16H13.91C13.5,17.43 12.83,18.76 12,19.96M8,8H5.08C6.03,6.34 7.57,5.06 9.4,4.44C8.8,5.55 8.35,6.75 8,8M5.08,16H8C8.35,17.25 8.8,18.45 9.4,19.56C7.57,18.93 6.03,17.65 5.08,16M4.26,14C4.1,13.36 4,12.69 4,12C4,11.31 4.1,10.64 4.26,10H7.64C7.56,10.66 7.5,11.32 7.5,12C7.5,12.68 7.56,13.34 7.64,14M12,4.03C12.83,5.23 13.5,6.57 13.91,8H10.09C10.5,6.57 11.17,5.23 12,4.03M18.92,8H15.97C15.65,6.75 15.19,5.55 14.59,4.44C16.43,5.07 17.96,6.34 18.92,8M12,2C6.47,2 2,6.5 2,12A10,10 0 0,0 12,22A10,10 0 0,0 22,12A10,10 0 0,0 12,2Z" />
  </svg>
`;
const listItemHomeAssistant = b`
  <svg slot="start" viewBox="0 0 24 24">
    <path d="m12.151 1.5882c-.3262 0-.6523.1291-.8996.3867l-8.3848 8.7354c-.0619.0644-.1223.1368-.1807.2154-.0588.0789-.1151.1638-.1688.2534-.2593.4325-.4552.9749-.5232 1.4555-.0026.018-.0076.0369-.0094.0548-.0121.0987-.0184.1944-.0184.2857v8.0124a1.2731 1.2731 0 001.2731 1.2731h7.8313l-3.4484-3.593a1.7399 1.7399 0 111.0803-1.125l2.6847 2.7972v-10.248a1.7399 1.7399 0 111.5276-0v7.187l2.6702-2.782a1.7399 1.7399 0 111.0566 1.1505l-3.7269 3.8831v2.7299h8.174a1.2471 1.2471 0 001.2471-1.2471v-8.0375c0-.0912-.0059-.1868-.0184-.2855-.0603-.4935-.2636-1.0617-.5326-1.5105-.0537-.0896-.1101-.1745-.1684-.253-.0588-.079-.1191-.1513-.181-.2158l-8.3848-8.7363c-.2473-.2577-.5735-.3866-.8995-.3864" />
  </svg>
`;
const listItemEraseUserData = b`
  <svg slot="start" viewBox="0 0 24 24">
    <path d="M15,14C17.67,14 23,15.33 23,18V20H7V18C7,15.33 12.33,14 15,14M15,12A4,4 0 0,1 11,8A4,4 0 0,1 15,4A4,4 0 0,1 19,8A4,4 0 0,1 15,12M5,9.59L7.12,7.46L8.54,8.88L6.41,11L8.54,13.12L7.12,14.54L5,12.41L2.88,14.54L1.46,13.12L3.59,11L1.46,8.88L2.88,7.46L5,9.59Z" />
  </svg>
`;
const listItemFundDevelopment = b`
  <svg slot="start" viewBox="0 0 24 24">
    <path d="M12,21.35L10.55,20.03C5.4,15.36 2,12.27 2,8.5C2,5.41 4.42,3 7.5,3C9.24,3 10.91,3.81 12,5.08C13.09,3.81 14.76,3 16.5,3C19.58,3 22,5.41 22,8.5C22,12.27 18.6,15.36 13.45,20.03L12,21.35Z" />
  </svg>
`;

const SERIAL_PACKET_HEADER = ["I".charCodeAt(0), "M".charCodeAt(0), "P".charCodeAt(0), "R".charCodeAt(0), "O".charCodeAt(0), "V".charCodeAt(0), 1 // protocol version
];
var ImprovSerialMessageType;
(function (ImprovSerialMessageType) {
  ImprovSerialMessageType[ImprovSerialMessageType["CURRENT_STATE"] = 1] = "CURRENT_STATE";
  ImprovSerialMessageType[ImprovSerialMessageType["ERROR_STATE"] = 2] = "ERROR_STATE";
  ImprovSerialMessageType[ImprovSerialMessageType["RPC"] = 3] = "RPC";
  ImprovSerialMessageType[ImprovSerialMessageType["RPC_RESULT"] = 4] = "RPC_RESULT";
})(ImprovSerialMessageType || (ImprovSerialMessageType = {}));
var ImprovSerialCurrentState;
(function (ImprovSerialCurrentState) {
  ImprovSerialCurrentState[ImprovSerialCurrentState["READY"] = 2] = "READY";
  ImprovSerialCurrentState[ImprovSerialCurrentState["PROVISIONING"] = 3] = "PROVISIONING";
  ImprovSerialCurrentState[ImprovSerialCurrentState["PROVISIONED"] = 4] = "PROVISIONED";
})(ImprovSerialCurrentState || (ImprovSerialCurrentState = {}));
const ERROR_MSGS = {
  0x00: "NO_ERROR",
  0x01: "INVALID_RPC_PACKET",
  0x02: "UNKNOWN_RPC_COMMAND",
  0x03: "UNABLE_TO_CONNECT",
  0xfe: "TIMEOUT",
  0xff: "UNKNOWN_ERROR"
};
class PortNotReady extends Error {
  constructor() {
    super("Port is not ready");
  }
}

const sleep$1 = ms => new Promise(resolve => setTimeout(resolve, ms));

const toHex = (value, size = 2) => {
  let hex = value.toString(16).toUpperCase();
  if (hex.startsWith("-")) {
    return "-0x" + hex.substring(1).padStart(size, "0");
  } else {
    return "0x" + hex.padStart(size, "0");
  }
};

const hexFormatter = bytes => "[" + bytes.map(value => toHex(value)).join(", ") + "]";

class ImprovSerial extends EventTarget {
  constructor(port, logger) {
    super();
    this.port = port;
    this.logger = logger;
    this.error = 0 /* ImprovSerialErrorState.NO_ERROR */;
    if (port.readable === null) {
      throw new Error("Port is not readable");
    }
    if (port.writable === null) {
      throw new Error("Port is not writable");
    }
  }
  /**
   * Detect Improv Serial, fetch the state and return the next URL if provisioned.
   * @param timeout Timeout in ms to wait for the device to respond. Default to 1000ms.
   */
  async initialize(timeout = 1000) {
    this.logger.log("Initializing Improv Serial");
    this._processInput();
    // To give the input processing time to start.
    await sleep$1(1000);
    if (this._reader === undefined) {
      throw new PortNotReady();
    }
    try {
      await new Promise(async (resolve, reject) => {
        setTimeout(() => reject(new Error("Improv Wi-Fi Serial not detected")), timeout);
        await this.requestCurrentState();
        resolve(undefined);
      });
      await this.requestInfo();
    } catch (err) {
      await this.close();
      throw err;
    }
    return this.info;
  }
  async close() {
    if (!this._reader) {
      return;
    }
    await new Promise(resolve => {
      this._reader.cancel();
      this.addEventListener("disconnect", resolve, {
        once: true
      });
    });
  }
  /**
   * This command will trigger at least one packet,
   * the Current State and if already provisioned,
   * the same response you would get if device provisioning
   * was successful (see below).
   */
  async requestCurrentState() {
    // Request current state and wait for 5s
    let rpcResult;
    try {
      await new Promise(async (resolve, reject) => {
        this.addEventListener("state-changed", resolve, {
          once: true
        });
        const cleanupAndReject = err => {
          this.removeEventListener("state-changed", resolve);
          reject(err);
        };
        rpcResult = this._sendRPCWithResponse(2 /* ImprovSerialRPCCommand.REQUEST_CURRENT_STATE */, []);
        rpcResult.catch(cleanupAndReject);
      });
    } catch (err) {
      this._rpcFeedback = undefined;
      throw new Error(`Error fetching current state: ${err}`);
    }
    // Only if we are provisioned will we get an rpc result
    if (this.state !== ImprovSerialCurrentState.PROVISIONED) {
      this._rpcFeedback = undefined;
      return;
    }
    const data = await rpcResult;
    this.nextUrl = data[0];
  }
  async requestInfo(timeout) {
    const response = await this._sendRPCWithResponse(3 /* ImprovSerialRPCCommand.REQUEST_INFO */, [], timeout);
    this.info = {
      firmware: response[0],
      version: response[1],
      name: response[3],
      chipFamily: response[2]
    };
  }
  async provision(ssid, password, timeout) {
    const encoder = new TextEncoder();
    const ssidEncoded = encoder.encode(ssid);
    const pwEncoded = encoder.encode(password);
    const data = [ssidEncoded.length, ...ssidEncoded, pwEncoded.length, ...pwEncoded];
    const response = await this._sendRPCWithResponse(1 /* ImprovSerialRPCCommand.SEND_WIFI_SETTINGS */, data, timeout);
    this.nextUrl = response[0];
  }
  async scan() {
    const results = await this._sendRPCWithMultipleResponses(4 /* ImprovSerialRPCCommand.REQUEST_WIFI_NETWORKS */, []);
    const ssids = results.map(([name, rssi, secured]) => ({
      name,
      rssi: parseInt(rssi),
      secured: secured === "YES"
    }));
    ssids.sort((a, b) => a.name.toLocaleLowerCase().localeCompare(b.name.toLocaleLowerCase()));
    return ssids;
  }
  _sendRPC(command, data) {
    this.writePacketToStream(ImprovSerialMessageType.RPC, [command, data.length, ...data]);
  }
  async _sendRPCWithResponse(command, data, timeout) {
    // Commands that receive feedback will finish when either
    // the state changes or the error code becomes not 0.
    if (this._rpcFeedback) {
      throw new Error("Only 1 RPC command that requires feedback can be active");
    }
    return await this._awaitRPCResultWithTimeout(new Promise((resolve, reject) => {
      this._rpcFeedback = {
        command,
        resolve,
        reject
      };
      this._sendRPC(command, data);
    }), timeout);
  }
  async _sendRPCWithMultipleResponses(command, data, timeout) {
    // Commands that receive multiple feedbacks will finish when either
    // the state changes or the error code becomes not 0.
    if (this._rpcFeedback) {
      throw new Error("Only 1 RPC command that requires feedback can be active");
    }
    return await this._awaitRPCResultWithTimeout(new Promise((resolve, reject) => {
      this._rpcFeedback = {
        command,
        resolve,
        reject,
        receivedData: []
      };
      this._sendRPC(command, data);
    }), timeout);
  }
  async _awaitRPCResultWithTimeout(sendRPCPromise, timeout) {
    if (!timeout) {
      return await sendRPCPromise;
    }
    return await new Promise((resolve, reject) => {
      const timeoutRPC = setTimeout(() => this._setError(254 /* ImprovSerialErrorState.TIMEOUT */), timeout);
      sendRPCPromise.finally(() => clearTimeout(timeoutRPC));
      sendRPCPromise.then(resolve, reject);
    });
  }
  async _processInput() {
    // read the data from serial port.
    // current state, error state, rpc result
    this.logger.debug("Starting read loop");
    this._reader = this.port.readable.getReader();
    try {
      let line = [];
      // undefined = not sure if improv packet
      let isImprov;
      // length of improv bytes that we expect
      let improvLength = 0;
      while (true) {
        const {
          value,
          done
        } = await this._reader.read();
        if (done) {
          break;
        }
        if (!value || value.length === 0) {
          continue;
        }
        for (const byte of value) {
          if (isImprov === false) {
            // When it wasn't an improv line, discard everything unti we find new line char
            if (byte === 10) {
              isImprov = undefined;
            }
            continue;
          }
          if (isImprov === true) {
            line.push(byte);
            if (line.length === improvLength) {
              this._handleIncomingPacket(line);
              isImprov = undefined;
              line = [];
            }
            continue;
          }
          if (byte === 10) {
            line = [];
            continue;
          }
          line.push(byte);
          if (line.length !== 9) {
            continue;
          }
          // Check if it's improv
          isImprov = String.fromCharCode(...line.slice(0, 6)) === "IMPROV";
          if (!isImprov) {
            line = [];
            continue;
          }
          // Format:
          // I M P R O V <VERSION> <TYPE> <LENGTH> <DATA> <CHECKSUM>
          // Once we have 9 bytes, we can check if it's an improv packet
          // and extract how much more we need to fetch.
          const packetLength = line[8];
          improvLength = 9 + packetLength + 1; // header + data length + checksum
        }
      }
    } catch (err) {
      this.logger.error("Error while reading serial port", err);
    } finally {
      this._reader.releaseLock();
      this._reader = undefined;
    }
    this.logger.debug("Finished read loop");
    this.dispatchEvent(new Event("disconnect"));
  }
  _handleIncomingPacket(line) {
    const payload = line.slice(6);
    const version = payload[0];
    const packetType = payload[1];
    const packetLength = payload[2];
    const data = payload.slice(3, 3 + packetLength);
    this.logger.debug("PROCESS", {
      version,
      packetType,
      packetLength,
      data: hexFormatter(data)
    });
    if (version !== 1) {
      this.logger.error("Received unsupported version", version);
      return;
    }
    // Verify checksum
    let packetChecksum = payload[3 + packetLength];
    let calculatedChecksum = 0;
    for (let i = 0; i < line.length - 1; i++) {
      calculatedChecksum += line[i];
    }
    calculatedChecksum = calculatedChecksum & 0xff;
    if (calculatedChecksum !== packetChecksum) {
      this.logger.error(`Received invalid checksum ${packetChecksum}. Expected ${calculatedChecksum}`);
      return;
    }
    if (packetType === ImprovSerialMessageType.CURRENT_STATE) {
      this.state = data[0];
      this.dispatchEvent(new CustomEvent("state-changed", {
        detail: this.state
      }));
    } else if (packetType === ImprovSerialMessageType.ERROR_STATE) {
      this._setError(data[0]);
    } else if (packetType === ImprovSerialMessageType.RPC_RESULT) {
      if (!this._rpcFeedback) {
        this.logger.error("Received result while not waiting for one");
        return;
      }
      const rpcCommand = data[0];
      if (rpcCommand !== this._rpcFeedback.command) {
        this.logger.error(`Received result for command ${rpcCommand} but expected ${this._rpcFeedback.command}`);
        return;
      }
      // Chop off rpc command and checksum
      const result = [];
      const totalLength = data[1];
      let idx = 2;
      while (idx < 2 + totalLength) {
        result.push(String.fromCodePoint(...data.slice(idx + 1, idx + data[idx] + 1)));
        idx += data[idx] + 1;
      }
      if ("receivedData" in this._rpcFeedback) {
        if (result.length > 0) {
          this._rpcFeedback.receivedData.push(result);
        } else {
          // Result of 0 means we're done.
          this._rpcFeedback.resolve(this._rpcFeedback.receivedData);
          this._rpcFeedback = undefined;
        }
      } else {
        this._rpcFeedback.resolve(result);
        this._rpcFeedback = undefined;
      }
    } else {
      this.logger.error("Unable to handle packet", payload);
    }
  }
  /**
   * Add header + checksum and write packet to stream
   */
  async writePacketToStream(type, data) {
    const payload = new Uint8Array([...SERIAL_PACKET_HEADER, type, data.length, ...data, 0, 0 // Will be newline
    ]);
    // Calculate checksum
    payload[payload.length - 2] = payload.reduce((sum, cur) => sum + cur, 0) & 0xff;
    payload[payload.length - 1] = 10; // Newline
    this.logger.debug("Writing to stream:", hexFormatter(new Array(...payload)));
    const writer = this.port.writable.getWriter();
    await writer.write(payload);
    try {
      writer.releaseLock();
    } catch (err) {
      console.error("Ignoring release lock error", err);
    }
  }
  // Error is either received from device or is a timeout
  _setError(error) {
    this.error = error;
    if (error > 0 && this._rpcFeedback) {
      this._rpcFeedback.reject(ERROR_MSGS[error] || `UNKNOWN_ERROR (${error})`);
      this._rpcFeedback = undefined;
    }
    this.dispatchEvent(new CustomEvent("error-changed", {
      detail: this.error
    }));
  }
}

/**
 * Represents a Espressif chip error.
 */
class ESPError extends Error {}

/*! pako 2.1.0 https://github.com/nodeca/pako @license (MIT AND Zlib) */
// (C) 1995-2013 Jean-loup Gailly and Mark Adler
// (C) 2014-2017 Vitaly Puzrin and Andrey Tupitsin
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//   claim that you wrote the original software. If you use this software
//   in a product, an acknowledgment in the product documentation would be
//   appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//   misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

/* eslint-disable space-unary-ops */

/* Public constants ==========================================================*/
/* ===========================================================================*/

//const Z_FILTERED          = 1;
//const Z_HUFFMAN_ONLY      = 2;
//const Z_RLE               = 3;
const Z_FIXED$1 = 4;
//const Z_DEFAULT_STRATEGY  = 0;

/* Possible values of the data_type field (though see inflate()) */
const Z_BINARY = 0;
const Z_TEXT = 1;
//const Z_ASCII             = 1; // = Z_TEXT
const Z_UNKNOWN$1 = 2;

/*============================================================================*/

function zero$1(buf) {
  let len = buf.length;
  while (--len >= 0) {
    buf[len] = 0;
  }
}

// From zutil.h

const STORED_BLOCK = 0;
const STATIC_TREES = 1;
const DYN_TREES = 2;
/* The three kinds of block type */

const MIN_MATCH$1 = 3;
const MAX_MATCH$1 = 258;
/* The minimum and maximum match lengths */

// From deflate.h
/* ===========================================================================
 * Internal compression state.
 */

const LENGTH_CODES$1 = 29;
/* number of length codes, not counting the special END_BLOCK code */

const LITERALS$1 = 256;
/* number of literal bytes 0..255 */

const L_CODES$1 = LITERALS$1 + 1 + LENGTH_CODES$1;
/* number of Literal or Length codes, including the END_BLOCK code */

const D_CODES$1 = 30;
/* number of distance codes */

const BL_CODES$1 = 19;
/* number of codes used to transfer the bit lengths */

const HEAP_SIZE$1 = 2 * L_CODES$1 + 1;
/* maximum heap size */

const MAX_BITS$1 = 15;
/* All codes must not exceed MAX_BITS bits */

const Buf_size = 16;
/* size of bit buffer in bi_buf */

/* ===========================================================================
 * Constants
 */

const MAX_BL_BITS = 7;
/* Bit length codes must not exceed MAX_BL_BITS bits */

const END_BLOCK = 256;
/* end of block literal code */

const REP_3_6 = 16;
/* repeat previous bit length 3-6 times (2 bits of repeat count) */

const REPZ_3_10 = 17;
/* repeat a zero length 3-10 times  (3 bits of repeat count) */

const REPZ_11_138 = 18;
/* repeat a zero length 11-138 times  (7 bits of repeat count) */

/* eslint-disable comma-spacing,array-bracket-spacing */
const extra_lbits = /* extra bits for each length code */
new Uint8Array([0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0]);
const extra_dbits = /* extra bits for each distance code */
new Uint8Array([0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13]);
const extra_blbits = /* extra bits for each bit length code */
new Uint8Array([0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 7]);
const bl_order = new Uint8Array([16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15]);
/* eslint-enable comma-spacing,array-bracket-spacing */

/* The lengths of the bit length codes are sent in order of decreasing
 * probability, to avoid transmitting the lengths for unused bit length codes.
 */

/* ===========================================================================
 * Local data. These are initialized only once.
 */

// We pre-fill arrays with 0 to avoid uninitialized gaps

const DIST_CODE_LEN = 512; /* see definition of array dist_code below */

// !!!! Use flat array instead of structure, Freq = i*2, Len = i*2+1
const static_ltree = new Array((L_CODES$1 + 2) * 2);
zero$1(static_ltree);
/* The static literal tree. Since the bit lengths are imposed, there is no
 * need for the L_CODES extra codes used during heap construction. However
 * The codes 286 and 287 are needed to build a canonical tree (see _tr_init
 * below).
 */

const static_dtree = new Array(D_CODES$1 * 2);
zero$1(static_dtree);
/* The static distance tree. (Actually a trivial tree since all codes use
 * 5 bits.)
 */

const _dist_code = new Array(DIST_CODE_LEN);
zero$1(_dist_code);
/* Distance codes. The first 256 values correspond to the distances
 * 3 .. 258, the last 256 values correspond to the top 8 bits of
 * the 15 bit distances.
 */

const _length_code = new Array(MAX_MATCH$1 - MIN_MATCH$1 + 1);
zero$1(_length_code);
/* length code for each normalized match length (0 == MIN_MATCH) */

const base_length = new Array(LENGTH_CODES$1);
zero$1(base_length);
/* First normalized length for each code (0 = MIN_MATCH) */

const base_dist = new Array(D_CODES$1);
zero$1(base_dist);
/* First normalized distance for each code (0 = distance of 1) */

function StaticTreeDesc(static_tree, extra_bits, extra_base, elems, max_length) {
  this.static_tree = static_tree; /* static tree or NULL */
  this.extra_bits = extra_bits; /* extra bits for each code or NULL */
  this.extra_base = extra_base; /* base index for extra_bits */
  this.elems = elems; /* max number of elements in the tree */
  this.max_length = max_length; /* max bit length for the codes */

  // show if `static_tree` has data or dummy - needed for monomorphic objects
  this.has_stree = static_tree && static_tree.length;
}
let static_l_desc;
let static_d_desc;
let static_bl_desc;
function TreeDesc(dyn_tree, stat_desc) {
  this.dyn_tree = dyn_tree; /* the dynamic tree */
  this.max_code = 0; /* largest code with non zero frequency */
  this.stat_desc = stat_desc; /* the corresponding static tree */
}
const d_code = dist => {
  return dist < 256 ? _dist_code[dist] : _dist_code[256 + (dist >>> 7)];
};

/* ===========================================================================
 * Output a short LSB first on the stream.
 * IN assertion: there is enough room in pendingBuf.
 */
const put_short = (s, w) => {
  //    put_byte(s, (uch)((w) & 0xff));
  //    put_byte(s, (uch)((ush)(w) >> 8));
  s.pending_buf[s.pending++] = w & 0xff;
  s.pending_buf[s.pending++] = w >>> 8 & 0xff;
};

/* ===========================================================================
 * Send a value on a given number of bits.
 * IN assertion: length <= 16 and value fits in length bits.
 */
const send_bits = (s, value, length) => {
  if (s.bi_valid > Buf_size - length) {
    s.bi_buf |= value << s.bi_valid & 0xffff;
    put_short(s, s.bi_buf);
    s.bi_buf = value >> Buf_size - s.bi_valid;
    s.bi_valid += length - Buf_size;
  } else {
    s.bi_buf |= value << s.bi_valid & 0xffff;
    s.bi_valid += length;
  }
};
const send_code = (s, c, tree) => {
  send_bits(s, tree[c * 2] /*.Code*/, tree[c * 2 + 1] /*.Len*/);
};

/* ===========================================================================
 * Reverse the first len bits of a code, using straightforward code (a faster
 * method would use a table)
 * IN assertion: 1 <= len <= 15
 */
const bi_reverse = (code, len) => {
  let res = 0;
  do {
    res |= code & 1;
    code >>>= 1;
    res <<= 1;
  } while (--len > 0);
  return res >>> 1;
};

/* ===========================================================================
 * Flush the bit buffer, keeping at most 7 bits in it.
 */
const bi_flush = s => {
  if (s.bi_valid === 16) {
    put_short(s, s.bi_buf);
    s.bi_buf = 0;
    s.bi_valid = 0;
  } else if (s.bi_valid >= 8) {
    s.pending_buf[s.pending++] = s.bi_buf & 0xff;
    s.bi_buf >>= 8;
    s.bi_valid -= 8;
  }
};

/* ===========================================================================
 * Compute the optimal bit lengths for a tree and update the total bit length
 * for the current block.
 * IN assertion: the fields freq and dad are set, heap[heap_max] and
 *    above are the tree nodes sorted by increasing frequency.
 * OUT assertions: the field len is set to the optimal bit length, the
 *     array bl_count contains the frequencies for each bit length.
 *     The length opt_len is updated; static_len is also updated if stree is
 *     not null.
 */
const gen_bitlen = (s, desc) => {
  //    deflate_state *s;
  //    tree_desc *desc;    /* the tree descriptor */

  const tree = desc.dyn_tree;
  const max_code = desc.max_code;
  const stree = desc.stat_desc.static_tree;
  const has_stree = desc.stat_desc.has_stree;
  const extra = desc.stat_desc.extra_bits;
  const base = desc.stat_desc.extra_base;
  const max_length = desc.stat_desc.max_length;
  let h; /* heap index */
  let n, m; /* iterate over the tree elements */
  let bits; /* bit length */
  let xbits; /* extra bits */
  let f; /* frequency */
  let overflow = 0; /* number of elements with bit length too large */

  for (bits = 0; bits <= MAX_BITS$1; bits++) {
    s.bl_count[bits] = 0;
  }

  /* In a first pass, compute the optimal bit lengths (which may
   * overflow in the case of the bit length tree).
   */
  tree[s.heap[s.heap_max] * 2 + 1] /*.Len*/ = 0; /* root of the heap */

  for (h = s.heap_max + 1; h < HEAP_SIZE$1; h++) {
    n = s.heap[h];
    bits = tree[tree[n * 2 + 1] /*.Dad*/ * 2 + 1] /*.Len*/ + 1;
    if (bits > max_length) {
      bits = max_length;
      overflow++;
    }
    tree[n * 2 + 1] /*.Len*/ = bits;
    /* We overwrite tree[n].Dad which is no longer needed */

    if (n > max_code) {
      continue;
    } /* not a leaf node */

    s.bl_count[bits]++;
    xbits = 0;
    if (n >= base) {
      xbits = extra[n - base];
    }
    f = tree[n * 2] /*.Freq*/;
    s.opt_len += f * (bits + xbits);
    if (has_stree) {
      s.static_len += f * (stree[n * 2 + 1] /*.Len*/ + xbits);
    }
  }
  if (overflow === 0) {
    return;
  }

  // Tracev((stderr,"\nbit length overflow\n"));
  /* This happens for example on obj2 and pic of the Calgary corpus */

  /* Find the first bit length which could increase: */
  do {
    bits = max_length - 1;
    while (s.bl_count[bits] === 0) {
      bits--;
    }
    s.bl_count[bits]--; /* move one leaf down the tree */
    s.bl_count[bits + 1] += 2; /* move one overflow item as its brother */
    s.bl_count[max_length]--;
    /* The brother of the overflow item also moves one step up,
     * but this does not affect bl_count[max_length]
     */
    overflow -= 2;
  } while (overflow > 0);

  /* Now recompute all bit lengths, scanning in increasing frequency.
   * h is still equal to HEAP_SIZE. (It is simpler to reconstruct all
   * lengths instead of fixing only the wrong ones. This idea is taken
   * from 'ar' written by Haruhiko Okumura.)
   */
  for (bits = max_length; bits !== 0; bits--) {
    n = s.bl_count[bits];
    while (n !== 0) {
      m = s.heap[--h];
      if (m > max_code) {
        continue;
      }
      if (tree[m * 2 + 1] /*.Len*/ !== bits) {
        // Tracev((stderr,"code %d bits %d->%d\n", m, tree[m].Len, bits));
        s.opt_len += (bits - tree[m * 2 + 1] /*.Len*/) * tree[m * 2] /*.Freq*/;
        tree[m * 2 + 1] /*.Len*/ = bits;
      }
      n--;
    }
  }
};

/* ===========================================================================
 * Generate the codes for a given tree and bit counts (which need not be
 * optimal).
 * IN assertion: the array bl_count contains the bit length statistics for
 * the given tree and the field len is set for all tree elements.
 * OUT assertion: the field code is set for all tree elements of non
 *     zero code length.
 */
const gen_codes = (tree, max_code, bl_count) => {
  //    ct_data *tree;             /* the tree to decorate */
  //    int max_code;              /* largest code with non zero frequency */
  //    ushf *bl_count;            /* number of codes at each bit length */

  const next_code = new Array(MAX_BITS$1 + 1); /* next code value for each bit length */
  let code = 0; /* running code value */
  let bits; /* bit index */
  let n; /* code index */

  /* The distribution counts are first used to generate the code values
   * without bit reversal.
   */
  for (bits = 1; bits <= MAX_BITS$1; bits++) {
    code = code + bl_count[bits - 1] << 1;
    next_code[bits] = code;
  }
  /* Check that the bit counts in bl_count are consistent. The last code
   * must be all ones.
   */
  //Assert (code + bl_count[MAX_BITS]-1 == (1<<MAX_BITS)-1,
  //        "inconsistent bit counts");
  //Tracev((stderr,"\ngen_codes: max_code %d ", max_code));

  for (n = 0; n <= max_code; n++) {
    let len = tree[n * 2 + 1] /*.Len*/;
    if (len === 0) {
      continue;
    }
    /* Now reverse the bits */
    tree[n * 2] /*.Code*/ = bi_reverse(next_code[len]++, len);

    //Tracecv(tree != static_ltree, (stderr,"\nn %3d %c l %2d c %4x (%x) ",
    //     n, (isgraph(n) ? n : ' '), len, tree[n].Code, next_code[len]-1));
  }
};

/* ===========================================================================
 * Initialize the various 'constant' tables.
 */
const tr_static_init = () => {
  let n; /* iterates over tree elements */
  let bits; /* bit counter */
  let length; /* length value */
  let code; /* code value */
  let dist; /* distance index */
  const bl_count = new Array(MAX_BITS$1 + 1);
  /* number of codes at each bit length for an optimal tree */

  // do check in _tr_init()
  //if (static_init_done) return;

  /* For some embedded targets, global variables are not initialized: */
  /*#ifdef NO_INIT_GLOBAL_POINTERS
    static_l_desc.static_tree = static_ltree;
    static_l_desc.extra_bits = extra_lbits;
    static_d_desc.static_tree = static_dtree;
    static_d_desc.extra_bits = extra_dbits;
    static_bl_desc.extra_bits = extra_blbits;
  #endif*/

  /* Initialize the mapping length (0..255) -> length code (0..28) */
  length = 0;
  for (code = 0; code < LENGTH_CODES$1 - 1; code++) {
    base_length[code] = length;
    for (n = 0; n < 1 << extra_lbits[code]; n++) {
      _length_code[length++] = code;
    }
  }
  //Assert (length == 256, "tr_static_init: length != 256");
  /* Note that the length 255 (match length 258) can be represented
   * in two different ways: code 284 + 5 bits or code 285, so we
   * overwrite length_code[255] to use the best encoding:
   */
  _length_code[length - 1] = code;

  /* Initialize the mapping dist (0..32K) -> dist code (0..29) */
  dist = 0;
  for (code = 0; code < 16; code++) {
    base_dist[code] = dist;
    for (n = 0; n < 1 << extra_dbits[code]; n++) {
      _dist_code[dist++] = code;
    }
  }
  //Assert (dist == 256, "tr_static_init: dist != 256");
  dist >>= 7; /* from now on, all distances are divided by 128 */
  for (; code < D_CODES$1; code++) {
    base_dist[code] = dist << 7;
    for (n = 0; n < 1 << extra_dbits[code] - 7; n++) {
      _dist_code[256 + dist++] = code;
    }
  }
  //Assert (dist == 256, "tr_static_init: 256+dist != 512");

  /* Construct the codes of the static literal tree */
  for (bits = 0; bits <= MAX_BITS$1; bits++) {
    bl_count[bits] = 0;
  }
  n = 0;
  while (n <= 143) {
    static_ltree[n * 2 + 1] /*.Len*/ = 8;
    n++;
    bl_count[8]++;
  }
  while (n <= 255) {
    static_ltree[n * 2 + 1] /*.Len*/ = 9;
    n++;
    bl_count[9]++;
  }
  while (n <= 279) {
    static_ltree[n * 2 + 1] /*.Len*/ = 7;
    n++;
    bl_count[7]++;
  }
  while (n <= 287) {
    static_ltree[n * 2 + 1] /*.Len*/ = 8;
    n++;
    bl_count[8]++;
  }
  /* Codes 286 and 287 do not exist, but we must include them in the
   * tree construction to get a canonical Huffman tree (longest code
   * all ones)
   */
  gen_codes(static_ltree, L_CODES$1 + 1, bl_count);

  /* The static distance tree is trivial: */
  for (n = 0; n < D_CODES$1; n++) {
    static_dtree[n * 2 + 1] /*.Len*/ = 5;
    static_dtree[n * 2] /*.Code*/ = bi_reverse(n, 5);
  }

  // Now data ready and we can init static trees
  static_l_desc = new StaticTreeDesc(static_ltree, extra_lbits, LITERALS$1 + 1, L_CODES$1, MAX_BITS$1);
  static_d_desc = new StaticTreeDesc(static_dtree, extra_dbits, 0, D_CODES$1, MAX_BITS$1);
  static_bl_desc = new StaticTreeDesc(new Array(0), extra_blbits, 0, BL_CODES$1, MAX_BL_BITS);

  //static_init_done = true;
};

/* ===========================================================================
 * Initialize a new block.
 */
const init_block = s => {
  let n; /* iterates over tree elements */

  /* Initialize the trees. */
  for (n = 0; n < L_CODES$1; n++) {
    s.dyn_ltree[n * 2] /*.Freq*/ = 0;
  }
  for (n = 0; n < D_CODES$1; n++) {
    s.dyn_dtree[n * 2] /*.Freq*/ = 0;
  }
  for (n = 0; n < BL_CODES$1; n++) {
    s.bl_tree[n * 2] /*.Freq*/ = 0;
  }
  s.dyn_ltree[END_BLOCK * 2] /*.Freq*/ = 1;
  s.opt_len = s.static_len = 0;
  s.sym_next = s.matches = 0;
};

/* ===========================================================================
 * Flush the bit buffer and align the output on a byte boundary
 */
const bi_windup = s => {
  if (s.bi_valid > 8) {
    put_short(s, s.bi_buf);
  } else if (s.bi_valid > 0) {
    //put_byte(s, (Byte)s->bi_buf);
    s.pending_buf[s.pending++] = s.bi_buf;
  }
  s.bi_buf = 0;
  s.bi_valid = 0;
};

/* ===========================================================================
 * Compares to subtrees, using the tree depth as tie breaker when
 * the subtrees have equal frequency. This minimizes the worst case length.
 */
const smaller = (tree, n, m, depth) => {
  const _n2 = n * 2;
  const _m2 = m * 2;
  return tree[_n2] /*.Freq*/ < tree[_m2] /*.Freq*/ || tree[_n2] /*.Freq*/ === tree[_m2] /*.Freq*/ && depth[n] <= depth[m];
};

/* ===========================================================================
 * Restore the heap property by moving down the tree starting at node k,
 * exchanging a node with the smallest of its two sons if necessary, stopping
 * when the heap property is re-established (each father smaller than its
 * two sons).
 */
const pqdownheap = (s, tree, k) => {
  //    deflate_state *s;
  //    ct_data *tree;  /* the tree to restore */
  //    int k;               /* node to move down */

  const v = s.heap[k];
  let j = k << 1; /* left son of k */
  while (j <= s.heap_len) {
    /* Set j to the smallest of the two sons: */
    if (j < s.heap_len && smaller(tree, s.heap[j + 1], s.heap[j], s.depth)) {
      j++;
    }
    /* Exit if v is smaller than both sons */
    if (smaller(tree, v, s.heap[j], s.depth)) {
      break;
    }

    /* Exchange v with the smallest son */
    s.heap[k] = s.heap[j];
    k = j;

    /* And continue down the tree, setting j to the left son of k */
    j <<= 1;
  }
  s.heap[k] = v;
};

// inlined manually
// const SMALLEST = 1;

/* ===========================================================================
 * Send the block data compressed using the given Huffman trees
 */
const compress_block = (s, ltree, dtree) => {
  //    deflate_state *s;
  //    const ct_data *ltree; /* literal tree */
  //    const ct_data *dtree; /* distance tree */

  let dist; /* distance of matched string */
  let lc; /* match length or unmatched char (if dist == 0) */
  let sx = 0; /* running index in sym_buf */
  let code; /* the code to send */
  let extra; /* number of extra bits to send */

  if (s.sym_next !== 0) {
    do {
      dist = s.pending_buf[s.sym_buf + sx++] & 0xff;
      dist += (s.pending_buf[s.sym_buf + sx++] & 0xff) << 8;
      lc = s.pending_buf[s.sym_buf + sx++];
      if (dist === 0) {
        send_code(s, lc, ltree); /* send a literal byte */
        //Tracecv(isgraph(lc), (stderr," '%c' ", lc));
      } else {
        /* Here, lc is the match length - MIN_MATCH */
        code = _length_code[lc];
        send_code(s, code + LITERALS$1 + 1, ltree); /* send the length code */
        extra = extra_lbits[code];
        if (extra !== 0) {
          lc -= base_length[code];
          send_bits(s, lc, extra); /* send the extra length bits */
        }
        dist--; /* dist is now the match distance - 1 */
        code = d_code(dist);
        //Assert (code < D_CODES, "bad d_code");

        send_code(s, code, dtree); /* send the distance code */
        extra = extra_dbits[code];
        if (extra !== 0) {
          dist -= base_dist[code];
          send_bits(s, dist, extra); /* send the extra distance bits */
        }
      } /* literal or match pair ? */

      /* Check that the overlay between pending_buf and sym_buf is ok: */
      //Assert(s->pending < s->lit_bufsize + sx, "pendingBuf overflow");
    } while (sx < s.sym_next);
  }
  send_code(s, END_BLOCK, ltree);
};

/* ===========================================================================
 * Construct one Huffman tree and assigns the code bit strings and lengths.
 * Update the total bit length for the current block.
 * IN assertion: the field freq is set for all tree elements.
 * OUT assertions: the fields len and code are set to the optimal bit length
 *     and corresponding code. The length opt_len is updated; static_len is
 *     also updated if stree is not null. The field max_code is set.
 */
const build_tree = (s, desc) => {
  //    deflate_state *s;
  //    tree_desc *desc; /* the tree descriptor */

  const tree = desc.dyn_tree;
  const stree = desc.stat_desc.static_tree;
  const has_stree = desc.stat_desc.has_stree;
  const elems = desc.stat_desc.elems;
  let n, m; /* iterate over heap elements */
  let max_code = -1; /* largest code with non zero frequency */
  let node; /* new node being created */

  /* Construct the initial heap, with least frequent element in
   * heap[SMALLEST]. The sons of heap[n] are heap[2*n] and heap[2*n+1].
   * heap[0] is not used.
   */
  s.heap_len = 0;
  s.heap_max = HEAP_SIZE$1;
  for (n = 0; n < elems; n++) {
    if (tree[n * 2] /*.Freq*/ !== 0) {
      s.heap[++s.heap_len] = max_code = n;
      s.depth[n] = 0;
    } else {
      tree[n * 2 + 1] /*.Len*/ = 0;
    }
  }

  /* The pkzip format requires that at least one distance code exists,
   * and that at least one bit should be sent even if there is only one
   * possible code. So to avoid special checks later on we force at least
   * two codes of non zero frequency.
   */
  while (s.heap_len < 2) {
    node = s.heap[++s.heap_len] = max_code < 2 ? ++max_code : 0;
    tree[node * 2] /*.Freq*/ = 1;
    s.depth[node] = 0;
    s.opt_len--;
    if (has_stree) {
      s.static_len -= stree[node * 2 + 1] /*.Len*/;
    }
    /* node is 0 or 1 so it does not have extra bits */
  }
  desc.max_code = max_code;

  /* The elements heap[heap_len/2+1 .. heap_len] are leaves of the tree,
   * establish sub-heaps of increasing lengths:
   */
  for (n = s.heap_len >> 1 /*int /2*/; n >= 1; n--) {
    pqdownheap(s, tree, n);
  }

  /* Construct the Huffman tree by repeatedly combining the least two
   * frequent nodes.
   */
  node = elems; /* next internal node of the tree */
  do {
    //pqremove(s, tree, n);  /* n = node of least frequency */
    /*** pqremove ***/
    n = s.heap[1 /*SMALLEST*/];
    s.heap[1 /*SMALLEST*/] = s.heap[s.heap_len--];
    pqdownheap(s, tree, 1 /*SMALLEST*/);
    /***/

    m = s.heap[1 /*SMALLEST*/]; /* m = node of next least frequency */

    s.heap[--s.heap_max] = n; /* keep the nodes sorted by frequency */
    s.heap[--s.heap_max] = m;

    /* Create a new node father of n and m */
    tree[node * 2] /*.Freq*/ = tree[n * 2] /*.Freq*/ + tree[m * 2] /*.Freq*/;
    s.depth[node] = (s.depth[n] >= s.depth[m] ? s.depth[n] : s.depth[m]) + 1;
    tree[n * 2 + 1] /*.Dad*/ = tree[m * 2 + 1] /*.Dad*/ = node;

    /* and insert the new node in the heap */
    s.heap[1 /*SMALLEST*/] = node++;
    pqdownheap(s, tree, 1 /*SMALLEST*/);
  } while (s.heap_len >= 2);
  s.heap[--s.heap_max] = s.heap[1 /*SMALLEST*/];

  /* At this point, the fields freq and dad are set. We can now
   * generate the bit lengths.
   */
  gen_bitlen(s, desc);

  /* The field len is now set, we can generate the bit codes */
  gen_codes(tree, max_code, s.bl_count);
};

/* ===========================================================================
 * Scan a literal or distance tree to determine the frequencies of the codes
 * in the bit length tree.
 */
const scan_tree = (s, tree, max_code) => {
  //    deflate_state *s;
  //    ct_data *tree;   /* the tree to be scanned */
  //    int max_code;    /* and its largest code of non zero frequency */

  let n; /* iterates over all tree elements */
  let prevlen = -1; /* last emitted length */
  let curlen; /* length of current code */

  let nextlen = tree[0 * 2 + 1] /*.Len*/; /* length of next code */

  let count = 0; /* repeat count of the current code */
  let max_count = 7; /* max repeat count */
  let min_count = 4; /* min repeat count */

  if (nextlen === 0) {
    max_count = 138;
    min_count = 3;
  }
  tree[(max_code + 1) * 2 + 1] /*.Len*/ = 0xffff; /* guard */

  for (n = 0; n <= max_code; n++) {
    curlen = nextlen;
    nextlen = tree[(n + 1) * 2 + 1] /*.Len*/;
    if (++count < max_count && curlen === nextlen) {
      continue;
    } else if (count < min_count) {
      s.bl_tree[curlen * 2] /*.Freq*/ += count;
    } else if (curlen !== 0) {
      if (curlen !== prevlen) {
        s.bl_tree[curlen * 2] /*.Freq*/++;
      }
      s.bl_tree[REP_3_6 * 2] /*.Freq*/++;
    } else if (count <= 10) {
      s.bl_tree[REPZ_3_10 * 2] /*.Freq*/++;
    } else {
      s.bl_tree[REPZ_11_138 * 2] /*.Freq*/++;
    }
    count = 0;
    prevlen = curlen;
    if (nextlen === 0) {
      max_count = 138;
      min_count = 3;
    } else if (curlen === nextlen) {
      max_count = 6;
      min_count = 3;
    } else {
      max_count = 7;
      min_count = 4;
    }
  }
};

/* ===========================================================================
 * Send a literal or distance tree in compressed form, using the codes in
 * bl_tree.
 */
const send_tree = (s, tree, max_code) => {
  //    deflate_state *s;
  //    ct_data *tree; /* the tree to be scanned */
  //    int max_code;       /* and its largest code of non zero frequency */

  let n; /* iterates over all tree elements */
  let prevlen = -1; /* last emitted length */
  let curlen; /* length of current code */

  let nextlen = tree[0 * 2 + 1] /*.Len*/; /* length of next code */

  let count = 0; /* repeat count of the current code */
  let max_count = 7; /* max repeat count */
  let min_count = 4; /* min repeat count */

  /* tree[max_code+1].Len = -1; */ /* guard already set */
  if (nextlen === 0) {
    max_count = 138;
    min_count = 3;
  }
  for (n = 0; n <= max_code; n++) {
    curlen = nextlen;
    nextlen = tree[(n + 1) * 2 + 1] /*.Len*/;
    if (++count < max_count && curlen === nextlen) {
      continue;
    } else if (count < min_count) {
      do {
        send_code(s, curlen, s.bl_tree);
      } while (--count !== 0);
    } else if (curlen !== 0) {
      if (curlen !== prevlen) {
        send_code(s, curlen, s.bl_tree);
        count--;
      }
      //Assert(count >= 3 && count <= 6, " 3_6?");
      send_code(s, REP_3_6, s.bl_tree);
      send_bits(s, count - 3, 2);
    } else if (count <= 10) {
      send_code(s, REPZ_3_10, s.bl_tree);
      send_bits(s, count - 3, 3);
    } else {
      send_code(s, REPZ_11_138, s.bl_tree);
      send_bits(s, count - 11, 7);
    }
    count = 0;
    prevlen = curlen;
    if (nextlen === 0) {
      max_count = 138;
      min_count = 3;
    } else if (curlen === nextlen) {
      max_count = 6;
      min_count = 3;
    } else {
      max_count = 7;
      min_count = 4;
    }
  }
};

/* ===========================================================================
 * Construct the Huffman tree for the bit lengths and return the index in
 * bl_order of the last bit length code to send.
 */
const build_bl_tree = s => {
  let max_blindex; /* index of last bit length code of non zero freq */

  /* Determine the bit length frequencies for literal and distance trees */
  scan_tree(s, s.dyn_ltree, s.l_desc.max_code);
  scan_tree(s, s.dyn_dtree, s.d_desc.max_code);

  /* Build the bit length tree: */
  build_tree(s, s.bl_desc);
  /* opt_len now includes the length of the tree representations, except
   * the lengths of the bit lengths codes and the 5+5+4 bits for the counts.
   */

  /* Determine the number of bit length codes to send. The pkzip format
   * requires that at least 4 bit length codes be sent. (appnote.txt says
   * 3 but the actual value used is 4.)
   */
  for (max_blindex = BL_CODES$1 - 1; max_blindex >= 3; max_blindex--) {
    if (s.bl_tree[bl_order[max_blindex] * 2 + 1] /*.Len*/ !== 0) {
      break;
    }
  }
  /* Update opt_len to include the bit length tree and counts */
  s.opt_len += 3 * (max_blindex + 1) + 5 + 5 + 4;
  //Tracev((stderr, "\ndyn trees: dyn %ld, stat %ld",
  //        s->opt_len, s->static_len));

  return max_blindex;
};

/* ===========================================================================
 * Send the header for a block using dynamic Huffman trees: the counts, the
 * lengths of the bit length codes, the literal tree and the distance tree.
 * IN assertion: lcodes >= 257, dcodes >= 1, blcodes >= 4.
 */
const send_all_trees = (s, lcodes, dcodes, blcodes) => {
  //    deflate_state *s;
  //    int lcodes, dcodes, blcodes; /* number of codes for each tree */

  let rank; /* index in bl_order */

  //Assert (lcodes >= 257 && dcodes >= 1 && blcodes >= 4, "not enough codes");
  //Assert (lcodes <= L_CODES && dcodes <= D_CODES && blcodes <= BL_CODES,
  //        "too many codes");
  //Tracev((stderr, "\nbl counts: "));
  send_bits(s, lcodes - 257, 5); /* not +255 as stated in appnote.txt */
  send_bits(s, dcodes - 1, 5);
  send_bits(s, blcodes - 4, 4); /* not -3 as stated in appnote.txt */
  for (rank = 0; rank < blcodes; rank++) {
    //Tracev((stderr, "\nbl code %2d ", bl_order[rank]));
    send_bits(s, s.bl_tree[bl_order[rank] * 2 + 1] /*.Len*/, 3);
  }
  //Tracev((stderr, "\nbl tree: sent %ld", s->bits_sent));

  send_tree(s, s.dyn_ltree, lcodes - 1); /* literal tree */
  //Tracev((stderr, "\nlit tree: sent %ld", s->bits_sent));

  send_tree(s, s.dyn_dtree, dcodes - 1); /* distance tree */
  //Tracev((stderr, "\ndist tree: sent %ld", s->bits_sent));
};

/* ===========================================================================
 * Check if the data type is TEXT or BINARY, using the following algorithm:
 * - TEXT if the two conditions below are satisfied:
 *    a) There are no non-portable control characters belonging to the
 *       "block list" (0..6, 14..25, 28..31).
 *    b) There is at least one printable character belonging to the
 *       "allow list" (9 {TAB}, 10 {LF}, 13 {CR}, 32..255).
 * - BINARY otherwise.
 * - The following partially-portable control characters form a
 *   "gray list" that is ignored in this detection algorithm:
 *   (7 {BEL}, 8 {BS}, 11 {VT}, 12 {FF}, 26 {SUB}, 27 {ESC}).
 * IN assertion: the fields Freq of dyn_ltree are set.
 */
const detect_data_type = s => {
  /* block_mask is the bit mask of block-listed bytes
   * set bits 0..6, 14..25, and 28..31
   * 0xf3ffc07f = binary 11110011111111111100000001111111
   */
  let block_mask = 0xf3ffc07f;
  let n;

  /* Check for non-textual ("block-listed") bytes. */
  for (n = 0; n <= 31; n++, block_mask >>>= 1) {
    if (block_mask & 1 && s.dyn_ltree[n * 2] /*.Freq*/ !== 0) {
      return Z_BINARY;
    }
  }

  /* Check for textual ("allow-listed") bytes. */
  if (s.dyn_ltree[9 * 2] /*.Freq*/ !== 0 || s.dyn_ltree[10 * 2] /*.Freq*/ !== 0 || s.dyn_ltree[13 * 2] /*.Freq*/ !== 0) {
    return Z_TEXT;
  }
  for (n = 32; n < LITERALS$1; n++) {
    if (s.dyn_ltree[n * 2] /*.Freq*/ !== 0) {
      return Z_TEXT;
    }
  }

  /* There are no "block-listed" or "allow-listed" bytes:
   * this stream either is empty or has tolerated ("gray-listed") bytes only.
   */
  return Z_BINARY;
};
let static_init_done = false;

/* ===========================================================================
 * Initialize the tree data structures for a new zlib stream.
 */
const _tr_init$1 = s => {
  if (!static_init_done) {
    tr_static_init();
    static_init_done = true;
  }
  s.l_desc = new TreeDesc(s.dyn_ltree, static_l_desc);
  s.d_desc = new TreeDesc(s.dyn_dtree, static_d_desc);
  s.bl_desc = new TreeDesc(s.bl_tree, static_bl_desc);
  s.bi_buf = 0;
  s.bi_valid = 0;

  /* Initialize the first block of the first file: */
  init_block(s);
};

/* ===========================================================================
 * Send a stored block
 */
const _tr_stored_block$1 = (s, buf, stored_len, last) => {
  //DeflateState *s;
  //charf *buf;       /* input block */
  //ulg stored_len;   /* length of input block */
  //int last;         /* one if this is the last block for a file */

  send_bits(s, (STORED_BLOCK << 1) + (last ? 1 : 0), 3); /* send block type */
  bi_windup(s); /* align on byte boundary */
  put_short(s, stored_len);
  put_short(s, ~stored_len);
  if (stored_len) {
    s.pending_buf.set(s.window.subarray(buf, buf + stored_len), s.pending);
  }
  s.pending += stored_len;
};

/* ===========================================================================
 * Send one empty static block to give enough lookahead for inflate.
 * This takes 10 bits, of which 7 may remain in the bit buffer.
 */
const _tr_align$1 = s => {
  send_bits(s, STATIC_TREES << 1, 3);
  send_code(s, END_BLOCK, static_ltree);
  bi_flush(s);
};

/* ===========================================================================
 * Determine the best encoding for the current block: dynamic trees, static
 * trees or store, and write out the encoded block.
 */
const _tr_flush_block$1 = (s, buf, stored_len, last) => {
  //DeflateState *s;
  //charf *buf;       /* input block, or NULL if too old */
  //ulg stored_len;   /* length of input block */
  //int last;         /* one if this is the last block for a file */

  let opt_lenb, static_lenb; /* opt_len and static_len in bytes */
  let max_blindex = 0; /* index of last bit length code of non zero freq */

  /* Build the Huffman trees unless a stored block is forced */
  if (s.level > 0) {
    /* Check if the file is binary or text */
    if (s.strm.data_type === Z_UNKNOWN$1) {
      s.strm.data_type = detect_data_type(s);
    }

    /* Construct the literal and distance trees */
    build_tree(s, s.l_desc);
    // Tracev((stderr, "\nlit data: dyn %ld, stat %ld", s->opt_len,
    //        s->static_len));

    build_tree(s, s.d_desc);
    // Tracev((stderr, "\ndist data: dyn %ld, stat %ld", s->opt_len,
    //        s->static_len));
    /* At this point, opt_len and static_len are the total bit lengths of
     * the compressed block data, excluding the tree representations.
     */

    /* Build the bit length tree for the above two trees, and get the index
     * in bl_order of the last bit length code to send.
     */
    max_blindex = build_bl_tree(s);

    /* Determine the best encoding. Compute the block lengths in bytes. */
    opt_lenb = s.opt_len + 3 + 7 >>> 3;
    static_lenb = s.static_len + 3 + 7 >>> 3;

    // Tracev((stderr, "\nopt %lu(%lu) stat %lu(%lu) stored %lu lit %u ",
    //        opt_lenb, s->opt_len, static_lenb, s->static_len, stored_len,
    //        s->sym_next / 3));

    if (static_lenb <= opt_lenb) {
      opt_lenb = static_lenb;
    }
  } else {
    // Assert(buf != (char*)0, "lost buf");
    opt_lenb = static_lenb = stored_len + 5; /* force a stored block */
  }
  if (stored_len + 4 <= opt_lenb && buf !== -1) {
    /* 4: two words for the lengths */

    /* The test buf != NULL is only necessary if LIT_BUFSIZE > WSIZE.
     * Otherwise we can't have processed more than WSIZE input bytes since
     * the last block flush, because compression would have been
     * successful. If LIT_BUFSIZE <= WSIZE, it is never too late to
     * transform a block into a stored block.
     */
    _tr_stored_block$1(s, buf, stored_len, last);
  } else if (s.strategy === Z_FIXED$1 || static_lenb === opt_lenb) {
    send_bits(s, (STATIC_TREES << 1) + (last ? 1 : 0), 3);
    compress_block(s, static_ltree, static_dtree);
  } else {
    send_bits(s, (DYN_TREES << 1) + (last ? 1 : 0), 3);
    send_all_trees(s, s.l_desc.max_code + 1, s.d_desc.max_code + 1, max_blindex + 1);
    compress_block(s, s.dyn_ltree, s.dyn_dtree);
  }
  // Assert (s->compressed_len == s->bits_sent, "bad compressed size");
  /* The above check is made mod 2^32, for files larger than 512 MB
   * and uLong implemented on 32 bits.
   */
  init_block(s);
  if (last) {
    bi_windup(s);
  }
  // Tracev((stderr,"\ncomprlen %lu(%lu) ", s->compressed_len>>3,
  //       s->compressed_len-7*last));
};

/* ===========================================================================
 * Save the match info and tally the frequency counts. Return true if
 * the current block must be flushed.
 */
const _tr_tally$1 = (s, dist, lc) => {
  //    deflate_state *s;
  //    unsigned dist;  /* distance of matched string */
  //    unsigned lc;    /* match length-MIN_MATCH or unmatched char (if dist==0) */

  s.pending_buf[s.sym_buf + s.sym_next++] = dist;
  s.pending_buf[s.sym_buf + s.sym_next++] = dist >> 8;
  s.pending_buf[s.sym_buf + s.sym_next++] = lc;
  if (dist === 0) {
    /* lc is the unmatched char */
    s.dyn_ltree[lc * 2] /*.Freq*/++;
  } else {
    s.matches++;
    /* Here, lc is the match length - MIN_MATCH */
    dist--; /* dist = match distance - 1 */
    //Assert((ush)dist < (ush)MAX_DIST(s) &&
    //       (ush)lc <= (ush)(MAX_MATCH-MIN_MATCH) &&
    //       (ush)d_code(dist) < (ush)D_CODES,  "_tr_tally: bad match");

    s.dyn_ltree[(_length_code[lc] + LITERALS$1 + 1) * 2] /*.Freq*/++;
    s.dyn_dtree[d_code(dist) * 2] /*.Freq*/++;
  }
  return s.sym_next === s.sym_end;
};
var _tr_init_1 = _tr_init$1;
var _tr_stored_block_1 = _tr_stored_block$1;
var _tr_flush_block_1 = _tr_flush_block$1;
var _tr_tally_1 = _tr_tally$1;
var _tr_align_1 = _tr_align$1;
var trees = {
  _tr_init: _tr_init_1,
  _tr_stored_block: _tr_stored_block_1,
  _tr_flush_block: _tr_flush_block_1,
  _tr_tally: _tr_tally_1,
  _tr_align: _tr_align_1
};

// Note: adler32 takes 12% for level 0 and 2% for level 6.
// It isn't worth it to make additional optimizations as in original.
// Small size is preferable.

// (C) 1995-2013 Jean-loup Gailly and Mark Adler
// (C) 2014-2017 Vitaly Puzrin and Andrey Tupitsin
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//   claim that you wrote the original software. If you use this software
//   in a product, an acknowledgment in the product documentation would be
//   appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//   misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

const adler32 = (adler, buf, len, pos) => {
  let s1 = adler & 0xffff | 0,
    s2 = adler >>> 16 & 0xffff | 0,
    n = 0;
  while (len !== 0) {
    // Set limit ~ twice less than 5552, to keep
    // s2 in 31-bits, because we force signed ints.
    // in other case %= will fail.
    n = len > 2000 ? 2000 : len;
    len -= n;
    do {
      s1 = s1 + buf[pos++] | 0;
      s2 = s2 + s1 | 0;
    } while (--n);
    s1 %= 65521;
    s2 %= 65521;
  }
  return s1 | s2 << 16 | 0;
};
var adler32_1 = adler32;

// Note: we can't get significant speed boost here.
// So write code to minimize size - no pregenerated tables
// and array tools dependencies.

// (C) 1995-2013 Jean-loup Gailly and Mark Adler
// (C) 2014-2017 Vitaly Puzrin and Andrey Tupitsin
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//   claim that you wrote the original software. If you use this software
//   in a product, an acknowledgment in the product documentation would be
//   appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//   misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

// Use ordinary array, since untyped makes no boost here
const makeTable = () => {
  let c,
    table = [];
  for (var n = 0; n < 256; n++) {
    c = n;
    for (var k = 0; k < 8; k++) {
      c = c & 1 ? 0xEDB88320 ^ c >>> 1 : c >>> 1;
    }
    table[n] = c;
  }
  return table;
};

// Create table on load. Just 255 signed longs. Not a problem.
const crcTable = new Uint32Array(makeTable());
const crc32 = (crc, buf, len, pos) => {
  const t = crcTable;
  const end = pos + len;
  crc ^= -1;
  for (let i = pos; i < end; i++) {
    crc = crc >>> 8 ^ t[(crc ^ buf[i]) & 0xFF];
  }
  return crc ^ -1; // >>> 0;
};
var crc32_1 = crc32;

// (C) 1995-2013 Jean-loup Gailly and Mark Adler
// (C) 2014-2017 Vitaly Puzrin and Andrey Tupitsin
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//   claim that you wrote the original software. If you use this software
//   in a product, an acknowledgment in the product documentation would be
//   appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//   misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

var messages = {
  2: 'need dictionary',
  /* Z_NEED_DICT       2  */
  1: 'stream end',
  /* Z_STREAM_END      1  */
  0: '',
  /* Z_OK              0  */
  '-1': 'file error',
  /* Z_ERRNO         (-1) */
  '-2': 'stream error',
  /* Z_STREAM_ERROR  (-2) */
  '-3': 'data error',
  /* Z_DATA_ERROR    (-3) */
  '-4': 'insufficient memory',
  /* Z_MEM_ERROR     (-4) */
  '-5': 'buffer error',
  /* Z_BUF_ERROR     (-5) */
  '-6': 'incompatible version' /* Z_VERSION_ERROR (-6) */
};

// (C) 1995-2013 Jean-loup Gailly and Mark Adler
// (C) 2014-2017 Vitaly Puzrin and Andrey Tupitsin
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//   claim that you wrote the original software. If you use this software
//   in a product, an acknowledgment in the product documentation would be
//   appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//   misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

var constants$2 = {
  /* Allowed flush values; see deflate() and inflate() below for details */
  Z_NO_FLUSH: 0,
  Z_PARTIAL_FLUSH: 1,
  Z_SYNC_FLUSH: 2,
  Z_FULL_FLUSH: 3,
  Z_FINISH: 4,
  Z_BLOCK: 5,
  Z_TREES: 6,
  /* Return codes for the compression/decompression functions. Negative values
  * are errors, positive values are used for special but normal events.
  */
  Z_OK: 0,
  Z_STREAM_END: 1,
  Z_NEED_DICT: 2,
  Z_STREAM_ERROR: -2,
  Z_DATA_ERROR: -3,
  Z_MEM_ERROR: -4,
  Z_BUF_ERROR: -5,
  Z_DEFAULT_COMPRESSION: -1,
  Z_FILTERED: 1,
  Z_HUFFMAN_ONLY: 2,
  Z_RLE: 3,
  Z_FIXED: 4,
  Z_DEFAULT_STRATEGY: 0,
  //Z_ASCII:                1, // = Z_TEXT (deprecated)
  Z_UNKNOWN: 2,
  /* The deflate compression method */
  Z_DEFLATED: 8
  //Z_NULL:                 null // Use -1 or null inline, depending on var type
};

// (C) 1995-2013 Jean-loup Gailly and Mark Adler
// (C) 2014-2017 Vitaly Puzrin and Andrey Tupitsin
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//   claim that you wrote the original software. If you use this software
//   in a product, an acknowledgment in the product documentation would be
//   appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//   misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

const {
  _tr_init,
  _tr_stored_block,
  _tr_flush_block,
  _tr_tally,
  _tr_align
} = trees;

/* Public constants ==========================================================*/
/* ===========================================================================*/

const {
  Z_NO_FLUSH: Z_NO_FLUSH$2,
  Z_PARTIAL_FLUSH,
  Z_FULL_FLUSH: Z_FULL_FLUSH$1,
  Z_FINISH: Z_FINISH$3,
  Z_BLOCK: Z_BLOCK$1,
  Z_OK: Z_OK$3,
  Z_STREAM_END: Z_STREAM_END$3,
  Z_STREAM_ERROR: Z_STREAM_ERROR$2,
  Z_DATA_ERROR: Z_DATA_ERROR$2,
  Z_BUF_ERROR: Z_BUF_ERROR$1,
  Z_DEFAULT_COMPRESSION: Z_DEFAULT_COMPRESSION$1,
  Z_FILTERED,
  Z_HUFFMAN_ONLY,
  Z_RLE,
  Z_FIXED,
  Z_DEFAULT_STRATEGY: Z_DEFAULT_STRATEGY$1,
  Z_UNKNOWN,
  Z_DEFLATED: Z_DEFLATED$2
} = constants$2;

/*============================================================================*/

const MAX_MEM_LEVEL = 9;
/* Maximum value for memLevel in deflateInit2 */
const MAX_WBITS$1 = 15;
/* 32K LZ77 window */
const DEF_MEM_LEVEL = 8;
const LENGTH_CODES = 29;
/* number of length codes, not counting the special END_BLOCK code */
const LITERALS = 256;
/* number of literal bytes 0..255 */
const L_CODES = LITERALS + 1 + LENGTH_CODES;
/* number of Literal or Length codes, including the END_BLOCK code */
const D_CODES = 30;
/* number of distance codes */
const BL_CODES = 19;
/* number of codes used to transfer the bit lengths */
const HEAP_SIZE = 2 * L_CODES + 1;
/* maximum heap size */
const MAX_BITS = 15;
/* All codes must not exceed MAX_BITS bits */

const MIN_MATCH = 3;
const MAX_MATCH = 258;
const MIN_LOOKAHEAD = MAX_MATCH + MIN_MATCH + 1;
const PRESET_DICT = 0x20;
const INIT_STATE = 42; /* zlib header -> BUSY_STATE */
//#ifdef GZIP
const GZIP_STATE = 57; /* gzip header -> BUSY_STATE | EXTRA_STATE */
//#endif
const EXTRA_STATE = 69; /* gzip extra block -> NAME_STATE */
const NAME_STATE = 73; /* gzip file name -> COMMENT_STATE */
const COMMENT_STATE = 91; /* gzip comment -> HCRC_STATE */
const HCRC_STATE = 103; /* gzip header CRC -> BUSY_STATE */
const BUSY_STATE = 113; /* deflate -> FINISH_STATE */
const FINISH_STATE = 666; /* stream complete */

const BS_NEED_MORE = 1; /* block not completed, need more input or more output */
const BS_BLOCK_DONE = 2; /* block flush performed */
const BS_FINISH_STARTED = 3; /* finish started, need only more output at next deflate */
const BS_FINISH_DONE = 4; /* finish done, accept no more input or output */

const OS_CODE = 0x03; // Unix :) . Don't detect, use this default.

const err = (strm, errorCode) => {
  strm.msg = messages[errorCode];
  return errorCode;
};
const rank = f => {
  return f * 2 - (f > 4 ? 9 : 0);
};
const zero = buf => {
  let len = buf.length;
  while (--len >= 0) {
    buf[len] = 0;
  }
};

/* ===========================================================================
 * Slide the hash table when sliding the window down (could be avoided with 32
 * bit values at the expense of memory usage). We slide even when level == 0 to
 * keep the hash table consistent if we switch back to level > 0 later.
 */
const slide_hash = s => {
  let n, m;
  let p;
  let wsize = s.w_size;
  n = s.hash_size;
  p = n;
  do {
    m = s.head[--p];
    s.head[p] = m >= wsize ? m - wsize : 0;
  } while (--n);
  n = wsize;
  //#ifndef FASTEST
  p = n;
  do {
    m = s.prev[--p];
    s.prev[p] = m >= wsize ? m - wsize : 0;
    /* If n is not on any hash chain, prev[n] is garbage but
     * its value will never be used.
     */
  } while (--n);
  //#endif
};

/* eslint-disable new-cap */
let HASH_ZLIB = (s, prev, data) => (prev << s.hash_shift ^ data) & s.hash_mask;
// This hash causes less collisions, https://github.com/nodeca/pako/issues/135
// But breaks binary compatibility
//let HASH_FAST = (s, prev, data) => ((prev << 8) + (prev >> 8) + (data << 4)) & s.hash_mask;
let HASH = HASH_ZLIB;

/* =========================================================================
 * Flush as much pending output as possible. All deflate() output, except for
 * some deflate_stored() output, goes through this function so some
 * applications may wish to modify it to avoid allocating a large
 * strm->next_out buffer and copying into it. (See also read_buf()).
 */
const flush_pending = strm => {
  const s = strm.state;

  //_tr_flush_bits(s);
  let len = s.pending;
  if (len > strm.avail_out) {
    len = strm.avail_out;
  }
  if (len === 0) {
    return;
  }
  strm.output.set(s.pending_buf.subarray(s.pending_out, s.pending_out + len), strm.next_out);
  strm.next_out += len;
  s.pending_out += len;
  strm.total_out += len;
  strm.avail_out -= len;
  s.pending -= len;
  if (s.pending === 0) {
    s.pending_out = 0;
  }
};
const flush_block_only = (s, last) => {
  _tr_flush_block(s, s.block_start >= 0 ? s.block_start : -1, s.strstart - s.block_start, last);
  s.block_start = s.strstart;
  flush_pending(s.strm);
};
const put_byte = (s, b) => {
  s.pending_buf[s.pending++] = b;
};

/* =========================================================================
 * Put a short in the pending buffer. The 16-bit value is put in MSB order.
 * IN assertion: the stream state is correct and there is enough room in
 * pending_buf.
 */
const putShortMSB = (s, b) => {
  //  put_byte(s, (Byte)(b >> 8));
  //  put_byte(s, (Byte)(b & 0xff));
  s.pending_buf[s.pending++] = b >>> 8 & 0xff;
  s.pending_buf[s.pending++] = b & 0xff;
};

/* ===========================================================================
 * Read a new buffer from the current input stream, update the adler32
 * and total number of bytes read.  All deflate() input goes through
 * this function so some applications may wish to modify it to avoid
 * allocating a large strm->input buffer and copying from it.
 * (See also flush_pending()).
 */
const read_buf = (strm, buf, start, size) => {
  let len = strm.avail_in;
  if (len > size) {
    len = size;
  }
  if (len === 0) {
    return 0;
  }
  strm.avail_in -= len;

  // zmemcpy(buf, strm->next_in, len);
  buf.set(strm.input.subarray(strm.next_in, strm.next_in + len), start);
  if (strm.state.wrap === 1) {
    strm.adler = adler32_1(strm.adler, buf, len, start);
  } else if (strm.state.wrap === 2) {
    strm.adler = crc32_1(strm.adler, buf, len, start);
  }
  strm.next_in += len;
  strm.total_in += len;
  return len;
};

/* ===========================================================================
 * Set match_start to the longest match starting at the given string and
 * return its length. Matches shorter or equal to prev_length are discarded,
 * in which case the result is equal to prev_length and match_start is
 * garbage.
 * IN assertions: cur_match is the head of the hash chain for the current
 *   string (strstart) and its distance is <= MAX_DIST, and prev_length >= 1
 * OUT assertion: the match length is not greater than s->lookahead.
 */
const longest_match = (s, cur_match) => {
  let chain_length = s.max_chain_length; /* max hash chain length */
  let scan = s.strstart; /* current string */
  let match; /* matched string */
  let len; /* length of current match */
  let best_len = s.prev_length; /* best match length so far */
  let nice_match = s.nice_match; /* stop if match long enough */
  const limit = s.strstart > s.w_size - MIN_LOOKAHEAD ? s.strstart - (s.w_size - MIN_LOOKAHEAD) : 0 /*NIL*/;
  const _win = s.window; // shortcut

  const wmask = s.w_mask;
  const prev = s.prev;

  /* Stop when cur_match becomes <= limit. To simplify the code,
   * we prevent matches with the string of window index 0.
   */

  const strend = s.strstart + MAX_MATCH;
  let scan_end1 = _win[scan + best_len - 1];
  let scan_end = _win[scan + best_len];

  /* The code is optimized for HASH_BITS >= 8 and MAX_MATCH-2 multiple of 16.
   * It is easy to get rid of this optimization if necessary.
   */
  // Assert(s->hash_bits >= 8 && MAX_MATCH == 258, "Code too clever");

  /* Do not waste too much time if we already have a good match: */
  if (s.prev_length >= s.good_match) {
    chain_length >>= 2;
  }
  /* Do not look for matches beyond the end of the input. This is necessary
   * to make deflate deterministic.
   */
  if (nice_match > s.lookahead) {
    nice_match = s.lookahead;
  }

  // Assert((ulg)s->strstart <= s->window_size-MIN_LOOKAHEAD, "need lookahead");

  do {
    // Assert(cur_match < s->strstart, "no future");
    match = cur_match;

    /* Skip to next match if the match length cannot increase
     * or if the match length is less than 2.  Note that the checks below
     * for insufficient lookahead only occur occasionally for performance
     * reasons.  Therefore uninitialized memory will be accessed, and
     * conditional jumps will be made that depend on those values.
     * However the length of the match is limited to the lookahead, so
     * the output of deflate is not affected by the uninitialized values.
     */

    if (_win[match + best_len] !== scan_end || _win[match + best_len - 1] !== scan_end1 || _win[match] !== _win[scan] || _win[++match] !== _win[scan + 1]) {
      continue;
    }

    /* The check at best_len-1 can be removed because it will be made
     * again later. (This heuristic is not always a win.)
     * It is not necessary to compare scan[2] and match[2] since they
     * are always equal when the other bytes match, given that
     * the hash keys are equal and that HASH_BITS >= 8.
     */
    scan += 2;
    match++;
    // Assert(*scan == *match, "match[2]?");

    /* We check for insufficient lookahead only every 8th comparison;
     * the 256th check will be made at strstart+258.
     */
    do {
      /*jshint noempty:false*/
    } while (_win[++scan] === _win[++match] && _win[++scan] === _win[++match] && _win[++scan] === _win[++match] && _win[++scan] === _win[++match] && _win[++scan] === _win[++match] && _win[++scan] === _win[++match] && _win[++scan] === _win[++match] && _win[++scan] === _win[++match] && scan < strend);

    // Assert(scan <= s->window+(unsigned)(s->window_size-1), "wild scan");

    len = MAX_MATCH - (strend - scan);
    scan = strend - MAX_MATCH;
    if (len > best_len) {
      s.match_start = cur_match;
      best_len = len;
      if (len >= nice_match) {
        break;
      }
      scan_end1 = _win[scan + best_len - 1];
      scan_end = _win[scan + best_len];
    }
  } while ((cur_match = prev[cur_match & wmask]) > limit && --chain_length !== 0);
  if (best_len <= s.lookahead) {
    return best_len;
  }
  return s.lookahead;
};

/* ===========================================================================
 * Fill the window when the lookahead becomes insufficient.
 * Updates strstart and lookahead.
 *
 * IN assertion: lookahead < MIN_LOOKAHEAD
 * OUT assertions: strstart <= window_size-MIN_LOOKAHEAD
 *    At least one byte has been read, or avail_in == 0; reads are
 *    performed for at least two bytes (required for the zip translate_eol
 *    option -- not supported here).
 */
const fill_window = s => {
  const _w_size = s.w_size;
  let n, more, str;

  //Assert(s->lookahead < MIN_LOOKAHEAD, "already enough lookahead");

  do {
    more = s.window_size - s.lookahead - s.strstart;

    // JS ints have 32 bit, block below not needed
    /* Deal with !@#$% 64K limit: */
    //if (sizeof(int) <= 2) {
    //    if (more == 0 && s->strstart == 0 && s->lookahead == 0) {
    //        more = wsize;
    //
    //  } else if (more == (unsigned)(-1)) {
    //        /* Very unlikely, but possible on 16 bit machine if
    //         * strstart == 0 && lookahead == 1 (input done a byte at time)
    //         */
    //        more--;
    //    }
    //}

    /* If the window is almost full and there is insufficient lookahead,
     * move the upper half to the lower one to make room in the upper half.
     */
    if (s.strstart >= _w_size + (_w_size - MIN_LOOKAHEAD)) {
      s.window.set(s.window.subarray(_w_size, _w_size + _w_size - more), 0);
      s.match_start -= _w_size;
      s.strstart -= _w_size;
      /* we now have strstart >= MAX_DIST */
      s.block_start -= _w_size;
      if (s.insert > s.strstart) {
        s.insert = s.strstart;
      }
      slide_hash(s);
      more += _w_size;
    }
    if (s.strm.avail_in === 0) {
      break;
    }

    /* If there was no sliding:
     *    strstart <= WSIZE+MAX_DIST-1 && lookahead <= MIN_LOOKAHEAD - 1 &&
     *    more == window_size - lookahead - strstart
     * => more >= window_size - (MIN_LOOKAHEAD-1 + WSIZE + MAX_DIST-1)
     * => more >= window_size - 2*WSIZE + 2
     * In the BIG_MEM or MMAP case (not yet supported),
     *   window_size == input_size + MIN_LOOKAHEAD  &&
     *   strstart + s->lookahead <= input_size => more >= MIN_LOOKAHEAD.
     * Otherwise, window_size == 2*WSIZE so more >= 2.
     * If there was sliding, more >= WSIZE. So in all cases, more >= 2.
     */
    //Assert(more >= 2, "more < 2");
    n = read_buf(s.strm, s.window, s.strstart + s.lookahead, more);
    s.lookahead += n;

    /* Initialize the hash value now that we have some input: */
    if (s.lookahead + s.insert >= MIN_MATCH) {
      str = s.strstart - s.insert;
      s.ins_h = s.window[str];

      /* UPDATE_HASH(s, s->ins_h, s->window[str + 1]); */
      s.ins_h = HASH(s, s.ins_h, s.window[str + 1]);
      //#if MIN_MATCH != 3
      //        Call update_hash() MIN_MATCH-3 more times
      //#endif
      while (s.insert) {
        /* UPDATE_HASH(s, s->ins_h, s->window[str + MIN_MATCH-1]); */
        s.ins_h = HASH(s, s.ins_h, s.window[str + MIN_MATCH - 1]);
        s.prev[str & s.w_mask] = s.head[s.ins_h];
        s.head[s.ins_h] = str;
        str++;
        s.insert--;
        if (s.lookahead + s.insert < MIN_MATCH) {
          break;
        }
      }
    }
    /* If the whole input has less than MIN_MATCH bytes, ins_h is garbage,
     * but this is not important since only literal bytes will be emitted.
     */
  } while (s.lookahead < MIN_LOOKAHEAD && s.strm.avail_in !== 0);

  /* If the WIN_INIT bytes after the end of the current data have never been
   * written, then zero those bytes in order to avoid memory check reports of
   * the use of uninitialized (or uninitialised as Julian writes) bytes by
   * the longest match routines.  Update the high water mark for the next
   * time through here.  WIN_INIT is set to MAX_MATCH since the longest match
   * routines allow scanning to strstart + MAX_MATCH, ignoring lookahead.
   */
  //  if (s.high_water < s.window_size) {
  //    const curr = s.strstart + s.lookahead;
  //    let init = 0;
  //
  //    if (s.high_water < curr) {
  //      /* Previous high water mark below current data -- zero WIN_INIT
  //       * bytes or up to end of window, whichever is less.
  //       */
  //      init = s.window_size - curr;
  //      if (init > WIN_INIT)
  //        init = WIN_INIT;
  //      zmemzero(s->window + curr, (unsigned)init);
  //      s->high_water = curr + init;
  //    }
  //    else if (s->high_water < (ulg)curr + WIN_INIT) {
  //      /* High water mark at or above current data, but below current data
  //       * plus WIN_INIT -- zero out to current data plus WIN_INIT, or up
  //       * to end of window, whichever is less.
  //       */
  //      init = (ulg)curr + WIN_INIT - s->high_water;
  //      if (init > s->window_size - s->high_water)
  //        init = s->window_size - s->high_water;
  //      zmemzero(s->window + s->high_water, (unsigned)init);
  //      s->high_water += init;
  //    }
  //  }
  //
  //  Assert((ulg)s->strstart <= s->window_size - MIN_LOOKAHEAD,
  //    "not enough room for search");
};

/* ===========================================================================
 * Copy without compression as much as possible from the input stream, return
 * the current block state.
 *
 * In case deflateParams() is used to later switch to a non-zero compression
 * level, s->matches (otherwise unused when storing) keeps track of the number
 * of hash table slides to perform. If s->matches is 1, then one hash table
 * slide will be done when switching. If s->matches is 2, the maximum value
 * allowed here, then the hash table will be cleared, since two or more slides
 * is the same as a clear.
 *
 * deflate_stored() is written to minimize the number of times an input byte is
 * copied. It is most efficient with large input and output buffers, which
 * maximizes the opportunites to have a single copy from next_in to next_out.
 */
const deflate_stored = (s, flush) => {
  /* Smallest worthy block size when not flushing or finishing. By default
   * this is 32K. This can be as small as 507 bytes for memLevel == 1. For
   * large input and output buffers, the stored block size will be larger.
   */
  let min_block = s.pending_buf_size - 5 > s.w_size ? s.w_size : s.pending_buf_size - 5;

  /* Copy as many min_block or larger stored blocks directly to next_out as
   * possible. If flushing, copy the remaining available input to next_out as
   * stored blocks, if there is enough space.
   */
  let len,
    left,
    have,
    last = 0;
  let used = s.strm.avail_in;
  do {
    /* Set len to the maximum size block that we can copy directly with the
     * available input data and output space. Set left to how much of that
     * would be copied from what's left in the window.
     */
    len = 65535 /* MAX_STORED */; /* maximum deflate stored block length */
    have = s.bi_valid + 42 >> 3; /* number of header bytes */
    if (s.strm.avail_out < have) {
      /* need room for header */
      break;
    }
    /* maximum stored block length that will fit in avail_out: */
    have = s.strm.avail_out - have;
    left = s.strstart - s.block_start; /* bytes left in window */
    if (len > left + s.strm.avail_in) {
      len = left + s.strm.avail_in; /* limit len to the input */
    }
    if (len > have) {
      len = have; /* limit len to the output */
    }

    /* If the stored block would be less than min_block in length, or if
     * unable to copy all of the available input when flushing, then try
     * copying to the window and the pending buffer instead. Also don't
     * write an empty block when flushing -- deflate() does that.
     */
    if (len < min_block && (len === 0 && flush !== Z_FINISH$3 || flush === Z_NO_FLUSH$2 || len !== left + s.strm.avail_in)) {
      break;
    }

    /* Make a dummy stored block in pending to get the header bytes,
     * including any pending bits. This also updates the debugging counts.
     */
    last = flush === Z_FINISH$3 && len === left + s.strm.avail_in ? 1 : 0;
    _tr_stored_block(s, 0, 0, last);

    /* Replace the lengths in the dummy stored block with len. */
    s.pending_buf[s.pending - 4] = len;
    s.pending_buf[s.pending - 3] = len >> 8;
    s.pending_buf[s.pending - 2] = ~len;
    s.pending_buf[s.pending - 1] = ~len >> 8;

    /* Write the stored block header bytes. */
    flush_pending(s.strm);

    //#ifdef ZLIB_DEBUG
    //    /* Update debugging counts for the data about to be copied. */
    //    s->compressed_len += len << 3;
    //    s->bits_sent += len << 3;
    //#endif

    /* Copy uncompressed bytes from the window to next_out. */
    if (left) {
      if (left > len) {
        left = len;
      }
      //zmemcpy(s->strm->next_out, s->window + s->block_start, left);
      s.strm.output.set(s.window.subarray(s.block_start, s.block_start + left), s.strm.next_out);
      s.strm.next_out += left;
      s.strm.avail_out -= left;
      s.strm.total_out += left;
      s.block_start += left;
      len -= left;
    }

    /* Copy uncompressed bytes directly from next_in to next_out, updating
     * the check value.
     */
    if (len) {
      read_buf(s.strm, s.strm.output, s.strm.next_out, len);
      s.strm.next_out += len;
      s.strm.avail_out -= len;
      s.strm.total_out += len;
    }
  } while (last === 0);

  /* Update the sliding window with the last s->w_size bytes of the copied
   * data, or append all of the copied data to the existing window if less
   * than s->w_size bytes were copied. Also update the number of bytes to
   * insert in the hash tables, in the event that deflateParams() switches to
   * a non-zero compression level.
   */
  used -= s.strm.avail_in; /* number of input bytes directly copied */
  if (used) {
    /* If any input was used, then no unused input remains in the window,
     * therefore s->block_start == s->strstart.
     */
    if (used >= s.w_size) {
      /* supplant the previous history */
      s.matches = 2; /* clear hash */
      //zmemcpy(s->window, s->strm->next_in - s->w_size, s->w_size);
      s.window.set(s.strm.input.subarray(s.strm.next_in - s.w_size, s.strm.next_in), 0);
      s.strstart = s.w_size;
      s.insert = s.strstart;
    } else {
      if (s.window_size - s.strstart <= used) {
        /* Slide the window down. */
        s.strstart -= s.w_size;
        //zmemcpy(s->window, s->window + s->w_size, s->strstart);
        s.window.set(s.window.subarray(s.w_size, s.w_size + s.strstart), 0);
        if (s.matches < 2) {
          s.matches++; /* add a pending slide_hash() */
        }
        if (s.insert > s.strstart) {
          s.insert = s.strstart;
        }
      }
      //zmemcpy(s->window + s->strstart, s->strm->next_in - used, used);
      s.window.set(s.strm.input.subarray(s.strm.next_in - used, s.strm.next_in), s.strstart);
      s.strstart += used;
      s.insert += used > s.w_size - s.insert ? s.w_size - s.insert : used;
    }
    s.block_start = s.strstart;
  }
  if (s.high_water < s.strstart) {
    s.high_water = s.strstart;
  }

  /* If the last block was written to next_out, then done. */
  if (last) {
    return BS_FINISH_DONE;
  }

  /* If flushing and all input has been consumed, then done. */
  if (flush !== Z_NO_FLUSH$2 && flush !== Z_FINISH$3 && s.strm.avail_in === 0 && s.strstart === s.block_start) {
    return BS_BLOCK_DONE;
  }

  /* Fill the window with any remaining input. */
  have = s.window_size - s.strstart;
  if (s.strm.avail_in > have && s.block_start >= s.w_size) {
    /* Slide the window down. */
    s.block_start -= s.w_size;
    s.strstart -= s.w_size;
    //zmemcpy(s->window, s->window + s->w_size, s->strstart);
    s.window.set(s.window.subarray(s.w_size, s.w_size + s.strstart), 0);
    if (s.matches < 2) {
      s.matches++; /* add a pending slide_hash() */
    }
    have += s.w_size; /* more space now */
    if (s.insert > s.strstart) {
      s.insert = s.strstart;
    }
  }
  if (have > s.strm.avail_in) {
    have = s.strm.avail_in;
  }
  if (have) {
    read_buf(s.strm, s.window, s.strstart, have);
    s.strstart += have;
    s.insert += have > s.w_size - s.insert ? s.w_size - s.insert : have;
  }
  if (s.high_water < s.strstart) {
    s.high_water = s.strstart;
  }

  /* There was not enough avail_out to write a complete worthy or flushed
   * stored block to next_out. Write a stored block to pending instead, if we
   * have enough input for a worthy block, or if flushing and there is enough
   * room for the remaining input as a stored block in the pending buffer.
   */
  have = s.bi_valid + 42 >> 3; /* number of header bytes */
  /* maximum stored block length that will fit in pending: */
  have = s.pending_buf_size - have > 65535 /* MAX_STORED */ ? 65535 /* MAX_STORED */ : s.pending_buf_size - have;
  min_block = have > s.w_size ? s.w_size : have;
  left = s.strstart - s.block_start;
  if (left >= min_block || (left || flush === Z_FINISH$3) && flush !== Z_NO_FLUSH$2 && s.strm.avail_in === 0 && left <= have) {
    len = left > have ? have : left;
    last = flush === Z_FINISH$3 && s.strm.avail_in === 0 && len === left ? 1 : 0;
    _tr_stored_block(s, s.block_start, len, last);
    s.block_start += len;
    flush_pending(s.strm);
  }

  /* We've done all we can with the available input and output. */
  return last ? BS_FINISH_STARTED : BS_NEED_MORE;
};

/* ===========================================================================
 * Compress as much as possible from the input stream, return the current
 * block state.
 * This function does not perform lazy evaluation of matches and inserts
 * new strings in the dictionary only for unmatched strings or for short
 * matches. It is used only for the fast compression options.
 */
const deflate_fast = (s, flush) => {
  let hash_head; /* head of the hash chain */
  let bflush; /* set if current block must be flushed */

  for (;;) {
    /* Make sure that we always have enough lookahead, except
     * at the end of the input file. We need MAX_MATCH bytes
     * for the next match, plus MIN_MATCH bytes to insert the
     * string following the next match.
     */
    if (s.lookahead < MIN_LOOKAHEAD) {
      fill_window(s);
      if (s.lookahead < MIN_LOOKAHEAD && flush === Z_NO_FLUSH$2) {
        return BS_NEED_MORE;
      }
      if (s.lookahead === 0) {
        break; /* flush the current block */
      }
    }

    /* Insert the string window[strstart .. strstart+2] in the
     * dictionary, and set hash_head to the head of the hash chain:
     */
    hash_head = 0 /*NIL*/;
    if (s.lookahead >= MIN_MATCH) {
      /*** INSERT_STRING(s, s.strstart, hash_head); ***/
      s.ins_h = HASH(s, s.ins_h, s.window[s.strstart + MIN_MATCH - 1]);
      hash_head = s.prev[s.strstart & s.w_mask] = s.head[s.ins_h];
      s.head[s.ins_h] = s.strstart;
      /***/
    }

    /* Find the longest match, discarding those <= prev_length.
     * At this point we have always match_length < MIN_MATCH
     */
    if (hash_head !== 0 /*NIL*/ && s.strstart - hash_head <= s.w_size - MIN_LOOKAHEAD) {
      /* To simplify the code, we prevent matches with the string
       * of window index 0 (in particular we have to avoid a match
       * of the string with itself at the start of the input file).
       */
      s.match_length = longest_match(s, hash_head);
      /* longest_match() sets match_start */
    }
    if (s.match_length >= MIN_MATCH) {
      // check_match(s, s.strstart, s.match_start, s.match_length); // for debug only

      /*** _tr_tally_dist(s, s.strstart - s.match_start,
                     s.match_length - MIN_MATCH, bflush); ***/
      bflush = _tr_tally(s, s.strstart - s.match_start, s.match_length - MIN_MATCH);
      s.lookahead -= s.match_length;

      /* Insert new strings in the hash table only if the match length
       * is not too large. This saves time but degrades compression.
       */
      if (s.match_length <= s.max_lazy_match /*max_insert_length*/ && s.lookahead >= MIN_MATCH) {
        s.match_length--; /* string at strstart already in table */
        do {
          s.strstart++;
          /*** INSERT_STRING(s, s.strstart, hash_head); ***/
          s.ins_h = HASH(s, s.ins_h, s.window[s.strstart + MIN_MATCH - 1]);
          hash_head = s.prev[s.strstart & s.w_mask] = s.head[s.ins_h];
          s.head[s.ins_h] = s.strstart;
          /***/
          /* strstart never exceeds WSIZE-MAX_MATCH, so there are
           * always MIN_MATCH bytes ahead.
           */
        } while (--s.match_length !== 0);
        s.strstart++;
      } else {
        s.strstart += s.match_length;
        s.match_length = 0;
        s.ins_h = s.window[s.strstart];
        /* UPDATE_HASH(s, s.ins_h, s.window[s.strstart+1]); */
        s.ins_h = HASH(s, s.ins_h, s.window[s.strstart + 1]);

        //#if MIN_MATCH != 3
        //                Call UPDATE_HASH() MIN_MATCH-3 more times
        //#endif
        /* If lookahead < MIN_MATCH, ins_h is garbage, but it does not
         * matter since it will be recomputed at next deflate call.
         */
      }
    } else {
      /* No match, output a literal byte */
      //Tracevv((stderr,"%c", s.window[s.strstart]));
      /*** _tr_tally_lit(s, s.window[s.strstart], bflush); ***/
      bflush = _tr_tally(s, 0, s.window[s.strstart]);
      s.lookahead--;
      s.strstart++;
    }
    if (bflush) {
      /*** FLUSH_BLOCK(s, 0); ***/
      flush_block_only(s, false);
      if (s.strm.avail_out === 0) {
        return BS_NEED_MORE;
      }
      /***/
    }
  }
  s.insert = s.strstart < MIN_MATCH - 1 ? s.strstart : MIN_MATCH - 1;
  if (flush === Z_FINISH$3) {
    /*** FLUSH_BLOCK(s, 1); ***/
    flush_block_only(s, true);
    if (s.strm.avail_out === 0) {
      return BS_FINISH_STARTED;
    }
    /***/
    return BS_FINISH_DONE;
  }
  if (s.sym_next) {
    /*** FLUSH_BLOCK(s, 0); ***/
    flush_block_only(s, false);
    if (s.strm.avail_out === 0) {
      return BS_NEED_MORE;
    }
    /***/
  }
  return BS_BLOCK_DONE;
};

/* ===========================================================================
 * Same as above, but achieves better compression. We use a lazy
 * evaluation for matches: a match is finally adopted only if there is
 * no better match at the next window position.
 */
const deflate_slow = (s, flush) => {
  let hash_head; /* head of hash chain */
  let bflush; /* set if current block must be flushed */

  let max_insert;

  /* Process the input block. */
  for (;;) {
    /* Make sure that we always have enough lookahead, except
     * at the end of the input file. We need MAX_MATCH bytes
     * for the next match, plus MIN_MATCH bytes to insert the
     * string following the next match.
     */
    if (s.lookahead < MIN_LOOKAHEAD) {
      fill_window(s);
      if (s.lookahead < MIN_LOOKAHEAD && flush === Z_NO_FLUSH$2) {
        return BS_NEED_MORE;
      }
      if (s.lookahead === 0) {
        break;
      } /* flush the current block */
    }

    /* Insert the string window[strstart .. strstart+2] in the
     * dictionary, and set hash_head to the head of the hash chain:
     */
    hash_head = 0 /*NIL*/;
    if (s.lookahead >= MIN_MATCH) {
      /*** INSERT_STRING(s, s.strstart, hash_head); ***/
      s.ins_h = HASH(s, s.ins_h, s.window[s.strstart + MIN_MATCH - 1]);
      hash_head = s.prev[s.strstart & s.w_mask] = s.head[s.ins_h];
      s.head[s.ins_h] = s.strstart;
      /***/
    }

    /* Find the longest match, discarding those <= prev_length.
     */
    s.prev_length = s.match_length;
    s.prev_match = s.match_start;
    s.match_length = MIN_MATCH - 1;
    if (hash_head !== 0 /*NIL*/ && s.prev_length < s.max_lazy_match && s.strstart - hash_head <= s.w_size - MIN_LOOKAHEAD /*MAX_DIST(s)*/) {
      /* To simplify the code, we prevent matches with the string
       * of window index 0 (in particular we have to avoid a match
       * of the string with itself at the start of the input file).
       */
      s.match_length = longest_match(s, hash_head);
      /* longest_match() sets match_start */

      if (s.match_length <= 5 && (s.strategy === Z_FILTERED || s.match_length === MIN_MATCH && s.strstart - s.match_start > 4096 /*TOO_FAR*/)) {
        /* If prev_match is also MIN_MATCH, match_start is garbage
         * but we will ignore the current match anyway.
         */
        s.match_length = MIN_MATCH - 1;
      }
    }
    /* If there was a match at the previous step and the current
     * match is not better, output the previous match:
     */
    if (s.prev_length >= MIN_MATCH && s.match_length <= s.prev_length) {
      max_insert = s.strstart + s.lookahead - MIN_MATCH;
      /* Do not insert strings in hash table beyond this. */

      //check_match(s, s.strstart-1, s.prev_match, s.prev_length);

      /***_tr_tally_dist(s, s.strstart - 1 - s.prev_match,
                     s.prev_length - MIN_MATCH, bflush);***/
      bflush = _tr_tally(s, s.strstart - 1 - s.prev_match, s.prev_length - MIN_MATCH);
      /* Insert in hash table all strings up to the end of the match.
       * strstart-1 and strstart are already inserted. If there is not
       * enough lookahead, the last two strings are not inserted in
       * the hash table.
       */
      s.lookahead -= s.prev_length - 1;
      s.prev_length -= 2;
      do {
        if (++s.strstart <= max_insert) {
          /*** INSERT_STRING(s, s.strstart, hash_head); ***/
          s.ins_h = HASH(s, s.ins_h, s.window[s.strstart + MIN_MATCH - 1]);
          hash_head = s.prev[s.strstart & s.w_mask] = s.head[s.ins_h];
          s.head[s.ins_h] = s.strstart;
          /***/
        }
      } while (--s.prev_length !== 0);
      s.match_available = 0;
      s.match_length = MIN_MATCH - 1;
      s.strstart++;
      if (bflush) {
        /*** FLUSH_BLOCK(s, 0); ***/
        flush_block_only(s, false);
        if (s.strm.avail_out === 0) {
          return BS_NEED_MORE;
        }
        /***/
      }
    } else if (s.match_available) {
      /* If there was no match at the previous position, output a
       * single literal. If there was a match but the current match
       * is longer, truncate the previous match to a single literal.
       */
      //Tracevv((stderr,"%c", s->window[s->strstart-1]));
      /*** _tr_tally_lit(s, s.window[s.strstart-1], bflush); ***/
      bflush = _tr_tally(s, 0, s.window[s.strstart - 1]);
      if (bflush) {
        /*** FLUSH_BLOCK_ONLY(s, 0) ***/
        flush_block_only(s, false);
        /***/
      }
      s.strstart++;
      s.lookahead--;
      if (s.strm.avail_out === 0) {
        return BS_NEED_MORE;
      }
    } else {
      /* There is no previous match to compare with, wait for
       * the next step to decide.
       */
      s.match_available = 1;
      s.strstart++;
      s.lookahead--;
    }
  }
  //Assert (flush != Z_NO_FLUSH, "no flush?");
  if (s.match_available) {
    //Tracevv((stderr,"%c", s->window[s->strstart-1]));
    /*** _tr_tally_lit(s, s.window[s.strstart-1], bflush); ***/
    bflush = _tr_tally(s, 0, s.window[s.strstart - 1]);
    s.match_available = 0;
  }
  s.insert = s.strstart < MIN_MATCH - 1 ? s.strstart : MIN_MATCH - 1;
  if (flush === Z_FINISH$3) {
    /*** FLUSH_BLOCK(s, 1); ***/
    flush_block_only(s, true);
    if (s.strm.avail_out === 0) {
      return BS_FINISH_STARTED;
    }
    /***/
    return BS_FINISH_DONE;
  }
  if (s.sym_next) {
    /*** FLUSH_BLOCK(s, 0); ***/
    flush_block_only(s, false);
    if (s.strm.avail_out === 0) {
      return BS_NEED_MORE;
    }
    /***/
  }
  return BS_BLOCK_DONE;
};

/* ===========================================================================
 * For Z_RLE, simply look for runs of bytes, generate matches only of distance
 * one.  Do not maintain a hash table.  (It will be regenerated if this run of
 * deflate switches away from Z_RLE.)
 */
const deflate_rle = (s, flush) => {
  let bflush; /* set if current block must be flushed */
  let prev; /* byte at distance one to match */
  let scan, strend; /* scan goes up to strend for length of run */

  const _win = s.window;
  for (;;) {
    /* Make sure that we always have enough lookahead, except
     * at the end of the input file. We need MAX_MATCH bytes
     * for the longest run, plus one for the unrolled loop.
     */
    if (s.lookahead <= MAX_MATCH) {
      fill_window(s);
      if (s.lookahead <= MAX_MATCH && flush === Z_NO_FLUSH$2) {
        return BS_NEED_MORE;
      }
      if (s.lookahead === 0) {
        break;
      } /* flush the current block */
    }

    /* See how many times the previous byte repeats */
    s.match_length = 0;
    if (s.lookahead >= MIN_MATCH && s.strstart > 0) {
      scan = s.strstart - 1;
      prev = _win[scan];
      if (prev === _win[++scan] && prev === _win[++scan] && prev === _win[++scan]) {
        strend = s.strstart + MAX_MATCH;
        do {
          /*jshint noempty:false*/
        } while (prev === _win[++scan] && prev === _win[++scan] && prev === _win[++scan] && prev === _win[++scan] && prev === _win[++scan] && prev === _win[++scan] && prev === _win[++scan] && prev === _win[++scan] && scan < strend);
        s.match_length = MAX_MATCH - (strend - scan);
        if (s.match_length > s.lookahead) {
          s.match_length = s.lookahead;
        }
      }
      //Assert(scan <= s->window+(uInt)(s->window_size-1), "wild scan");
    }

    /* Emit match if have run of MIN_MATCH or longer, else emit literal */
    if (s.match_length >= MIN_MATCH) {
      //check_match(s, s.strstart, s.strstart - 1, s.match_length);

      /*** _tr_tally_dist(s, 1, s.match_length - MIN_MATCH, bflush); ***/
      bflush = _tr_tally(s, 1, s.match_length - MIN_MATCH);
      s.lookahead -= s.match_length;
      s.strstart += s.match_length;
      s.match_length = 0;
    } else {
      /* No match, output a literal byte */
      //Tracevv((stderr,"%c", s->window[s->strstart]));
      /*** _tr_tally_lit(s, s.window[s.strstart], bflush); ***/
      bflush = _tr_tally(s, 0, s.window[s.strstart]);
      s.lookahead--;
      s.strstart++;
    }
    if (bflush) {
      /*** FLUSH_BLOCK(s, 0); ***/
      flush_block_only(s, false);
      if (s.strm.avail_out === 0) {
        return BS_NEED_MORE;
      }
      /***/
    }
  }
  s.insert = 0;
  if (flush === Z_FINISH$3) {
    /*** FLUSH_BLOCK(s, 1); ***/
    flush_block_only(s, true);
    if (s.strm.avail_out === 0) {
      return BS_FINISH_STARTED;
    }
    /***/
    return BS_FINISH_DONE;
  }
  if (s.sym_next) {
    /*** FLUSH_BLOCK(s, 0); ***/
    flush_block_only(s, false);
    if (s.strm.avail_out === 0) {
      return BS_NEED_MORE;
    }
    /***/
  }
  return BS_BLOCK_DONE;
};

/* ===========================================================================
 * For Z_HUFFMAN_ONLY, do not look for matches.  Do not maintain a hash table.
 * (It will be regenerated if this run of deflate switches away from Huffman.)
 */
const deflate_huff = (s, flush) => {
  let bflush; /* set if current block must be flushed */

  for (;;) {
    /* Make sure that we have a literal to write. */
    if (s.lookahead === 0) {
      fill_window(s);
      if (s.lookahead === 0) {
        if (flush === Z_NO_FLUSH$2) {
          return BS_NEED_MORE;
        }
        break; /* flush the current block */
      }
    }

    /* Output a literal byte */
    s.match_length = 0;
    //Tracevv((stderr,"%c", s->window[s->strstart]));
    /*** _tr_tally_lit(s, s.window[s.strstart], bflush); ***/
    bflush = _tr_tally(s, 0, s.window[s.strstart]);
    s.lookahead--;
    s.strstart++;
    if (bflush) {
      /*** FLUSH_BLOCK(s, 0); ***/
      flush_block_only(s, false);
      if (s.strm.avail_out === 0) {
        return BS_NEED_MORE;
      }
      /***/
    }
  }
  s.insert = 0;
  if (flush === Z_FINISH$3) {
    /*** FLUSH_BLOCK(s, 1); ***/
    flush_block_only(s, true);
    if (s.strm.avail_out === 0) {
      return BS_FINISH_STARTED;
    }
    /***/
    return BS_FINISH_DONE;
  }
  if (s.sym_next) {
    /*** FLUSH_BLOCK(s, 0); ***/
    flush_block_only(s, false);
    if (s.strm.avail_out === 0) {
      return BS_NEED_MORE;
    }
    /***/
  }
  return BS_BLOCK_DONE;
};

/* Values for max_lazy_match, good_match and max_chain_length, depending on
 * the desired pack level (0..9). The values given below have been tuned to
 * exclude worst case performance for pathological files. Better values may be
 * found for specific files.
 */
function Config(good_length, max_lazy, nice_length, max_chain, func) {
  this.good_length = good_length;
  this.max_lazy = max_lazy;
  this.nice_length = nice_length;
  this.max_chain = max_chain;
  this.func = func;
}
const configuration_table = [/*      good lazy nice chain */
new Config(0, 0, 0, 0, deflate_stored), /* 0 store only */
new Config(4, 4, 8, 4, deflate_fast), /* 1 max speed, no lazy matches */
new Config(4, 5, 16, 8, deflate_fast), /* 2 */
new Config(4, 6, 32, 32, deflate_fast), /* 3 */

new Config(4, 4, 16, 16, deflate_slow), /* 4 lazy matches */
new Config(8, 16, 32, 32, deflate_slow), /* 5 */
new Config(8, 16, 128, 128, deflate_slow), /* 6 */
new Config(8, 32, 128, 256, deflate_slow), /* 7 */
new Config(32, 128, 258, 1024, deflate_slow), /* 8 */
new Config(32, 258, 258, 4096, deflate_slow) /* 9 max compression */];

/* ===========================================================================
 * Initialize the "longest match" routines for a new zlib stream
 */
const lm_init = s => {
  s.window_size = 2 * s.w_size;

  /*** CLEAR_HASH(s); ***/
  zero(s.head); // Fill with NIL (= 0);

  /* Set the default configuration parameters:
   */
  s.max_lazy_match = configuration_table[s.level].max_lazy;
  s.good_match = configuration_table[s.level].good_length;
  s.nice_match = configuration_table[s.level].nice_length;
  s.max_chain_length = configuration_table[s.level].max_chain;
  s.strstart = 0;
  s.block_start = 0;
  s.lookahead = 0;
  s.insert = 0;
  s.match_length = s.prev_length = MIN_MATCH - 1;
  s.match_available = 0;
  s.ins_h = 0;
};
function DeflateState() {
  this.strm = null; /* pointer back to this zlib stream */
  this.status = 0; /* as the name implies */
  this.pending_buf = null; /* output still pending */
  this.pending_buf_size = 0; /* size of pending_buf */
  this.pending_out = 0; /* next pending byte to output to the stream */
  this.pending = 0; /* nb of bytes in the pending buffer */
  this.wrap = 0; /* bit 0 true for zlib, bit 1 true for gzip */
  this.gzhead = null; /* gzip header information to write */
  this.gzindex = 0; /* where in extra, name, or comment */
  this.method = Z_DEFLATED$2; /* can only be DEFLATED */
  this.last_flush = -1; /* value of flush param for previous deflate call */

  this.w_size = 0; /* LZ77 window size (32K by default) */
  this.w_bits = 0; /* log2(w_size)  (8..16) */
  this.w_mask = 0; /* w_size - 1 */

  this.window = null;
  /* Sliding window. Input bytes are read into the second half of the window,
   * and move to the first half later to keep a dictionary of at least wSize
   * bytes. With this organization, matches are limited to a distance of
   * wSize-MAX_MATCH bytes, but this ensures that IO is always
   * performed with a length multiple of the block size.
   */

  this.window_size = 0;
  /* Actual size of window: 2*wSize, except when the user input buffer
   * is directly used as sliding window.
   */

  this.prev = null;
  /* Link to older string with same hash index. To limit the size of this
   * array to 64K, this link is maintained only for the last 32K strings.
   * An index in this array is thus a window index modulo 32K.
   */

  this.head = null; /* Heads of the hash chains or NIL. */

  this.ins_h = 0; /* hash index of string to be inserted */
  this.hash_size = 0; /* number of elements in hash table */
  this.hash_bits = 0; /* log2(hash_size) */
  this.hash_mask = 0; /* hash_size-1 */

  this.hash_shift = 0;
  /* Number of bits by which ins_h must be shifted at each input
   * step. It must be such that after MIN_MATCH steps, the oldest
   * byte no longer takes part in the hash key, that is:
   *   hash_shift * MIN_MATCH >= hash_bits
   */

  this.block_start = 0;
  /* Window position at the beginning of the current output block. Gets
   * negative when the window is moved backwards.
   */

  this.match_length = 0; /* length of best match */
  this.prev_match = 0; /* previous match */
  this.match_available = 0; /* set if previous match exists */
  this.strstart = 0; /* start of string to insert */
  this.match_start = 0; /* start of matching string */
  this.lookahead = 0; /* number of valid bytes ahead in window */

  this.prev_length = 0;
  /* Length of the best match at previous step. Matches not greater than this
   * are discarded. This is used in the lazy match evaluation.
   */

  this.max_chain_length = 0;
  /* To speed up deflation, hash chains are never searched beyond this
   * length.  A higher limit improves compression ratio but degrades the
   * speed.
   */

  this.max_lazy_match = 0;
  /* Attempt to find a better match only when the current match is strictly
   * smaller than this value. This mechanism is used only for compression
   * levels >= 4.
   */
  // That's alias to max_lazy_match, don't use directly
  //this.max_insert_length = 0;
  /* Insert new strings in the hash table only if the match length is not
   * greater than this length. This saves time but degrades compression.
   * max_insert_length is used only for compression levels <= 3.
   */

  this.level = 0; /* compression level (1..9) */
  this.strategy = 0; /* favor or force Huffman coding*/

  this.good_match = 0;
  /* Use a faster search when the previous match is longer than this */

  this.nice_match = 0; /* Stop searching when current match exceeds this */

  /* used by trees.c: */

  /* Didn't use ct_data typedef below to suppress compiler warning */

  // struct ct_data_s dyn_ltree[HEAP_SIZE];   /* literal and length tree */
  // struct ct_data_s dyn_dtree[2*D_CODES+1]; /* distance tree */
  // struct ct_data_s bl_tree[2*BL_CODES+1];  /* Huffman tree for bit lengths */

  // Use flat array of DOUBLE size, with interleaved fata,
  // because JS does not support effective
  this.dyn_ltree = new Uint16Array(HEAP_SIZE * 2);
  this.dyn_dtree = new Uint16Array((2 * D_CODES + 1) * 2);
  this.bl_tree = new Uint16Array((2 * BL_CODES + 1) * 2);
  zero(this.dyn_ltree);
  zero(this.dyn_dtree);
  zero(this.bl_tree);
  this.l_desc = null; /* desc. for literal tree */
  this.d_desc = null; /* desc. for distance tree */
  this.bl_desc = null; /* desc. for bit length tree */

  //ush bl_count[MAX_BITS+1];
  this.bl_count = new Uint16Array(MAX_BITS + 1);
  /* number of codes at each bit length for an optimal tree */

  //int heap[2*L_CODES+1];      /* heap used to build the Huffman trees */
  this.heap = new Uint16Array(2 * L_CODES + 1); /* heap used to build the Huffman trees */
  zero(this.heap);
  this.heap_len = 0; /* number of elements in the heap */
  this.heap_max = 0; /* element of largest frequency */
  /* The sons of heap[n] are heap[2*n] and heap[2*n+1]. heap[0] is not used.
   * The same heap array is used to build all trees.
   */

  this.depth = new Uint16Array(2 * L_CODES + 1); //uch depth[2*L_CODES+1];
  zero(this.depth);
  /* Depth of each subtree used as tie breaker for trees of equal frequency
   */

  this.sym_buf = 0; /* buffer for distances and literals/lengths */

  this.lit_bufsize = 0;
  /* Size of match buffer for literals/lengths.  There are 4 reasons for
   * limiting lit_bufsize to 64K:
   *   - frequencies can be kept in 16 bit counters
   *   - if compression is not successful for the first block, all input
   *     data is still in the window so we can still emit a stored block even
   *     when input comes from standard input.  (This can also be done for
   *     all blocks if lit_bufsize is not greater than 32K.)
   *   - if compression is not successful for a file smaller than 64K, we can
   *     even emit a stored file instead of a stored block (saving 5 bytes).
   *     This is applicable only for zip (not gzip or zlib).
   *   - creating new Huffman trees less frequently may not provide fast
   *     adaptation to changes in the input data statistics. (Take for
   *     example a binary file with poorly compressible code followed by
   *     a highly compressible string table.) Smaller buffer sizes give
   *     fast adaptation but have of course the overhead of transmitting
   *     trees more frequently.
   *   - I can't count above 4
   */

  this.sym_next = 0; /* running index in sym_buf */
  this.sym_end = 0; /* symbol table full when sym_next reaches this */

  this.opt_len = 0; /* bit length of current block with optimal trees */
  this.static_len = 0; /* bit length of current block with static trees */
  this.matches = 0; /* number of string matches in current block */
  this.insert = 0; /* bytes at end of window left to insert */

  this.bi_buf = 0;
  /* Output buffer. bits are inserted starting at the bottom (least
   * significant bits).
   */
  this.bi_valid = 0;
  /* Number of valid bits in bi_buf.  All bits above the last valid bit
   * are always zero.
   */

  // Used for window memory init. We safely ignore it for JS. That makes
  // sense only for pointers and memory check tools.
  //this.high_water = 0;
  /* High water mark offset in window for initialized bytes -- bytes above
   * this are set to zero in order to avoid memory check warnings when
   * longest match routines access bytes past the input.  This is then
   * updated to the new high water mark.
   */
}

/* =========================================================================
 * Check for a valid deflate stream state. Return 0 if ok, 1 if not.
 */
const deflateStateCheck = strm => {
  if (!strm) {
    return 1;
  }
  const s = strm.state;
  if (!s || s.strm !== strm || s.status !== INIT_STATE &&
  //#ifdef GZIP
  s.status !== GZIP_STATE &&
  //#endif
  s.status !== EXTRA_STATE && s.status !== NAME_STATE && s.status !== COMMENT_STATE && s.status !== HCRC_STATE && s.status !== BUSY_STATE && s.status !== FINISH_STATE) {
    return 1;
  }
  return 0;
};
const deflateResetKeep = strm => {
  if (deflateStateCheck(strm)) {
    return err(strm, Z_STREAM_ERROR$2);
  }
  strm.total_in = strm.total_out = 0;
  strm.data_type = Z_UNKNOWN;
  const s = strm.state;
  s.pending = 0;
  s.pending_out = 0;
  if (s.wrap < 0) {
    s.wrap = -s.wrap;
    /* was made negative by deflate(..., Z_FINISH); */
  }
  s.status =
  //#ifdef GZIP
  s.wrap === 2 ? GZIP_STATE :
  //#endif
  s.wrap ? INIT_STATE : BUSY_STATE;
  strm.adler = s.wrap === 2 ? 0 // crc32(0, Z_NULL, 0)
  : 1; // adler32(0, Z_NULL, 0)
  s.last_flush = -2;
  _tr_init(s);
  return Z_OK$3;
};
const deflateReset = strm => {
  const ret = deflateResetKeep(strm);
  if (ret === Z_OK$3) {
    lm_init(strm.state);
  }
  return ret;
};
const deflateSetHeader = (strm, head) => {
  if (deflateStateCheck(strm) || strm.state.wrap !== 2) {
    return Z_STREAM_ERROR$2;
  }
  strm.state.gzhead = head;
  return Z_OK$3;
};
const deflateInit2 = (strm, level, method, windowBits, memLevel, strategy) => {
  if (!strm) {
    // === Z_NULL
    return Z_STREAM_ERROR$2;
  }
  let wrap = 1;
  if (level === Z_DEFAULT_COMPRESSION$1) {
    level = 6;
  }
  if (windowBits < 0) {
    /* suppress zlib wrapper */
    wrap = 0;
    windowBits = -windowBits;
  } else if (windowBits > 15) {
    wrap = 2; /* write gzip wrapper instead */
    windowBits -= 16;
  }
  if (memLevel < 1 || memLevel > MAX_MEM_LEVEL || method !== Z_DEFLATED$2 || windowBits < 8 || windowBits > 15 || level < 0 || level > 9 || strategy < 0 || strategy > Z_FIXED || windowBits === 8 && wrap !== 1) {
    return err(strm, Z_STREAM_ERROR$2);
  }
  if (windowBits === 8) {
    windowBits = 9;
  }
  /* until 256-byte window bug fixed */

  const s = new DeflateState();
  strm.state = s;
  s.strm = strm;
  s.status = INIT_STATE; /* to pass state test in deflateReset() */

  s.wrap = wrap;
  s.gzhead = null;
  s.w_bits = windowBits;
  s.w_size = 1 << s.w_bits;
  s.w_mask = s.w_size - 1;
  s.hash_bits = memLevel + 7;
  s.hash_size = 1 << s.hash_bits;
  s.hash_mask = s.hash_size - 1;
  s.hash_shift = ~~((s.hash_bits + MIN_MATCH - 1) / MIN_MATCH);
  s.window = new Uint8Array(s.w_size * 2);
  s.head = new Uint16Array(s.hash_size);
  s.prev = new Uint16Array(s.w_size);

  // Don't need mem init magic for JS.
  //s.high_water = 0;  /* nothing written to s->window yet */

  s.lit_bufsize = 1 << memLevel + 6; /* 16K elements by default */

  /* We overlay pending_buf and sym_buf. This works since the average size
   * for length/distance pairs over any compressed block is assured to be 31
   * bits or less.
   *
   * Analysis: The longest fixed codes are a length code of 8 bits plus 5
   * extra bits, for lengths 131 to 257. The longest fixed distance codes are
   * 5 bits plus 13 extra bits, for distances 16385 to 32768. The longest
   * possible fixed-codes length/distance pair is then 31 bits total.
   *
   * sym_buf starts one-fourth of the way into pending_buf. So there are
   * three bytes in sym_buf for every four bytes in pending_buf. Each symbol
   * in sym_buf is three bytes -- two for the distance and one for the
   * literal/length. As each symbol is consumed, the pointer to the next
   * sym_buf value to read moves forward three bytes. From that symbol, up to
   * 31 bits are written to pending_buf. The closest the written pending_buf
   * bits gets to the next sym_buf symbol to read is just before the last
   * code is written. At that time, 31*(n-2) bits have been written, just
   * after 24*(n-2) bits have been consumed from sym_buf. sym_buf starts at
   * 8*n bits into pending_buf. (Note that the symbol buffer fills when n-1
   * symbols are written.) The closest the writing gets to what is unread is
   * then n+14 bits. Here n is lit_bufsize, which is 16384 by default, and
   * can range from 128 to 32768.
   *
   * Therefore, at a minimum, there are 142 bits of space between what is
   * written and what is read in the overlain buffers, so the symbols cannot
   * be overwritten by the compressed data. That space is actually 139 bits,
   * due to the three-bit fixed-code block header.
   *
   * That covers the case where either Z_FIXED is specified, forcing fixed
   * codes, or when the use of fixed codes is chosen, because that choice
   * results in a smaller compressed block than dynamic codes. That latter
   * condition then assures that the above analysis also covers all dynamic
   * blocks. A dynamic-code block will only be chosen to be emitted if it has
   * fewer bits than a fixed-code block would for the same set of symbols.
   * Therefore its average symbol length is assured to be less than 31. So
   * the compressed data for a dynamic block also cannot overwrite the
   * symbols from which it is being constructed.
   */

  s.pending_buf_size = s.lit_bufsize * 4;
  s.pending_buf = new Uint8Array(s.pending_buf_size);

  // It is offset from `s.pending_buf` (size is `s.lit_bufsize * 2`)
  //s->sym_buf = s->pending_buf + s->lit_bufsize;
  s.sym_buf = s.lit_bufsize;

  //s->sym_end = (s->lit_bufsize - 1) * 3;
  s.sym_end = (s.lit_bufsize - 1) * 3;
  /* We avoid equality with lit_bufsize*3 because of wraparound at 64K
   * on 16 bit machines and because stored blocks are restricted to
   * 64K-1 bytes.
   */

  s.level = level;
  s.strategy = strategy;
  s.method = method;
  return deflateReset(strm);
};
const deflateInit = (strm, level) => {
  return deflateInit2(strm, level, Z_DEFLATED$2, MAX_WBITS$1, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY$1);
};

/* ========================================================================= */
const deflate$2 = (strm, flush) => {
  if (deflateStateCheck(strm) || flush > Z_BLOCK$1 || flush < 0) {
    return strm ? err(strm, Z_STREAM_ERROR$2) : Z_STREAM_ERROR$2;
  }
  const s = strm.state;
  if (!strm.output || strm.avail_in !== 0 && !strm.input || s.status === FINISH_STATE && flush !== Z_FINISH$3) {
    return err(strm, strm.avail_out === 0 ? Z_BUF_ERROR$1 : Z_STREAM_ERROR$2);
  }
  const old_flush = s.last_flush;
  s.last_flush = flush;

  /* Flush as much pending output as possible */
  if (s.pending !== 0) {
    flush_pending(strm);
    if (strm.avail_out === 0) {
      /* Since avail_out is 0, deflate will be called again with
       * more output space, but possibly with both pending and
       * avail_in equal to zero. There won't be anything to do,
       * but this is not an error situation so make sure we
       * return OK instead of BUF_ERROR at next call of deflate:
       */
      s.last_flush = -1;
      return Z_OK$3;
    }

    /* Make sure there is something to do and avoid duplicate consecutive
     * flushes. For repeated and useless calls with Z_FINISH, we keep
     * returning Z_STREAM_END instead of Z_BUF_ERROR.
     */
  } else if (strm.avail_in === 0 && rank(flush) <= rank(old_flush) && flush !== Z_FINISH$3) {
    return err(strm, Z_BUF_ERROR$1);
  }

  /* User must not provide more input after the first FINISH: */
  if (s.status === FINISH_STATE && strm.avail_in !== 0) {
    return err(strm, Z_BUF_ERROR$1);
  }

  /* Write the header */
  if (s.status === INIT_STATE && s.wrap === 0) {
    s.status = BUSY_STATE;
  }
  if (s.status === INIT_STATE) {
    /* zlib header */
    let header = Z_DEFLATED$2 + (s.w_bits - 8 << 4) << 8;
    let level_flags = -1;
    if (s.strategy >= Z_HUFFMAN_ONLY || s.level < 2) {
      level_flags = 0;
    } else if (s.level < 6) {
      level_flags = 1;
    } else if (s.level === 6) {
      level_flags = 2;
    } else {
      level_flags = 3;
    }
    header |= level_flags << 6;
    if (s.strstart !== 0) {
      header |= PRESET_DICT;
    }
    header += 31 - header % 31;
    putShortMSB(s, header);

    /* Save the adler32 of the preset dictionary: */
    if (s.strstart !== 0) {
      putShortMSB(s, strm.adler >>> 16);
      putShortMSB(s, strm.adler & 0xffff);
    }
    strm.adler = 1; // adler32(0L, Z_NULL, 0);
    s.status = BUSY_STATE;

    /* Compression must start with an empty pending buffer */
    flush_pending(strm);
    if (s.pending !== 0) {
      s.last_flush = -1;
      return Z_OK$3;
    }
  }
  //#ifdef GZIP
  if (s.status === GZIP_STATE) {
    /* gzip header */
    strm.adler = 0; //crc32(0L, Z_NULL, 0);
    put_byte(s, 31);
    put_byte(s, 139);
    put_byte(s, 8);
    if (!s.gzhead) {
      // s->gzhead == Z_NULL
      put_byte(s, 0);
      put_byte(s, 0);
      put_byte(s, 0);
      put_byte(s, 0);
      put_byte(s, 0);
      put_byte(s, s.level === 9 ? 2 : s.strategy >= Z_HUFFMAN_ONLY || s.level < 2 ? 4 : 0);
      put_byte(s, OS_CODE);
      s.status = BUSY_STATE;

      /* Compression must start with an empty pending buffer */
      flush_pending(strm);
      if (s.pending !== 0) {
        s.last_flush = -1;
        return Z_OK$3;
      }
    } else {
      put_byte(s, (s.gzhead.text ? 1 : 0) + (s.gzhead.hcrc ? 2 : 0) + (!s.gzhead.extra ? 0 : 4) + (!s.gzhead.name ? 0 : 8) + (!s.gzhead.comment ? 0 : 16));
      put_byte(s, s.gzhead.time & 0xff);
      put_byte(s, s.gzhead.time >> 8 & 0xff);
      put_byte(s, s.gzhead.time >> 16 & 0xff);
      put_byte(s, s.gzhead.time >> 24 & 0xff);
      put_byte(s, s.level === 9 ? 2 : s.strategy >= Z_HUFFMAN_ONLY || s.level < 2 ? 4 : 0);
      put_byte(s, s.gzhead.os & 0xff);
      if (s.gzhead.extra && s.gzhead.extra.length) {
        put_byte(s, s.gzhead.extra.length & 0xff);
        put_byte(s, s.gzhead.extra.length >> 8 & 0xff);
      }
      if (s.gzhead.hcrc) {
        strm.adler = crc32_1(strm.adler, s.pending_buf, s.pending, 0);
      }
      s.gzindex = 0;
      s.status = EXTRA_STATE;
    }
  }
  if (s.status === EXTRA_STATE) {
    if (s.gzhead.extra /* != Z_NULL*/) {
      let beg = s.pending; /* start of bytes to update crc */
      let left = (s.gzhead.extra.length & 0xffff) - s.gzindex;
      while (s.pending + left > s.pending_buf_size) {
        let copy = s.pending_buf_size - s.pending;
        // zmemcpy(s.pending_buf + s.pending,
        //    s.gzhead.extra + s.gzindex, copy);
        s.pending_buf.set(s.gzhead.extra.subarray(s.gzindex, s.gzindex + copy), s.pending);
        s.pending = s.pending_buf_size;
        //--- HCRC_UPDATE(beg) ---//
        if (s.gzhead.hcrc && s.pending > beg) {
          strm.adler = crc32_1(strm.adler, s.pending_buf, s.pending - beg, beg);
        }
        //---//
        s.gzindex += copy;
        flush_pending(strm);
        if (s.pending !== 0) {
          s.last_flush = -1;
          return Z_OK$3;
        }
        beg = 0;
        left -= copy;
      }
      // JS specific: s.gzhead.extra may be TypedArray or Array for backward compatibility
      //              TypedArray.slice and TypedArray.from don't exist in IE10-IE11
      let gzhead_extra = new Uint8Array(s.gzhead.extra);
      // zmemcpy(s->pending_buf + s->pending,
      //     s->gzhead->extra + s->gzindex, left);
      s.pending_buf.set(gzhead_extra.subarray(s.gzindex, s.gzindex + left), s.pending);
      s.pending += left;
      //--- HCRC_UPDATE(beg) ---//
      if (s.gzhead.hcrc && s.pending > beg) {
        strm.adler = crc32_1(strm.adler, s.pending_buf, s.pending - beg, beg);
      }
      //---//
      s.gzindex = 0;
    }
    s.status = NAME_STATE;
  }
  if (s.status === NAME_STATE) {
    if (s.gzhead.name /* != Z_NULL*/) {
      let beg = s.pending; /* start of bytes to update crc */
      let val;
      do {
        if (s.pending === s.pending_buf_size) {
          //--- HCRC_UPDATE(beg) ---//
          if (s.gzhead.hcrc && s.pending > beg) {
            strm.adler = crc32_1(strm.adler, s.pending_buf, s.pending - beg, beg);
          }
          //---//
          flush_pending(strm);
          if (s.pending !== 0) {
            s.last_flush = -1;
            return Z_OK$3;
          }
          beg = 0;
        }
        // JS specific: little magic to add zero terminator to end of string
        if (s.gzindex < s.gzhead.name.length) {
          val = s.gzhead.name.charCodeAt(s.gzindex++) & 0xff;
        } else {
          val = 0;
        }
        put_byte(s, val);
      } while (val !== 0);
      //--- HCRC_UPDATE(beg) ---//
      if (s.gzhead.hcrc && s.pending > beg) {
        strm.adler = crc32_1(strm.adler, s.pending_buf, s.pending - beg, beg);
      }
      //---//
      s.gzindex = 0;
    }
    s.status = COMMENT_STATE;
  }
  if (s.status === COMMENT_STATE) {
    if (s.gzhead.comment /* != Z_NULL*/) {
      let beg = s.pending; /* start of bytes to update crc */
      let val;
      do {
        if (s.pending === s.pending_buf_size) {
          //--- HCRC_UPDATE(beg) ---//
          if (s.gzhead.hcrc && s.pending > beg) {
            strm.adler = crc32_1(strm.adler, s.pending_buf, s.pending - beg, beg);
          }
          //---//
          flush_pending(strm);
          if (s.pending !== 0) {
            s.last_flush = -1;
            return Z_OK$3;
          }
          beg = 0;
        }
        // JS specific: little magic to add zero terminator to end of string
        if (s.gzindex < s.gzhead.comment.length) {
          val = s.gzhead.comment.charCodeAt(s.gzindex++) & 0xff;
        } else {
          val = 0;
        }
        put_byte(s, val);
      } while (val !== 0);
      //--- HCRC_UPDATE(beg) ---//
      if (s.gzhead.hcrc && s.pending > beg) {
        strm.adler = crc32_1(strm.adler, s.pending_buf, s.pending - beg, beg);
      }
      //---//
    }
    s.status = HCRC_STATE;
  }
  if (s.status === HCRC_STATE) {
    if (s.gzhead.hcrc) {
      if (s.pending + 2 > s.pending_buf_size) {
        flush_pending(strm);
        if (s.pending !== 0) {
          s.last_flush = -1;
          return Z_OK$3;
        }
      }
      put_byte(s, strm.adler & 0xff);
      put_byte(s, strm.adler >> 8 & 0xff);
      strm.adler = 0; //crc32(0L, Z_NULL, 0);
    }
    s.status = BUSY_STATE;

    /* Compression must start with an empty pending buffer */
    flush_pending(strm);
    if (s.pending !== 0) {
      s.last_flush = -1;
      return Z_OK$3;
    }
  }
  //#endif

  /* Start a new block or continue the current one.
   */
  if (strm.avail_in !== 0 || s.lookahead !== 0 || flush !== Z_NO_FLUSH$2 && s.status !== FINISH_STATE) {
    let bstate = s.level === 0 ? deflate_stored(s, flush) : s.strategy === Z_HUFFMAN_ONLY ? deflate_huff(s, flush) : s.strategy === Z_RLE ? deflate_rle(s, flush) : configuration_table[s.level].func(s, flush);
    if (bstate === BS_FINISH_STARTED || bstate === BS_FINISH_DONE) {
      s.status = FINISH_STATE;
    }
    if (bstate === BS_NEED_MORE || bstate === BS_FINISH_STARTED) {
      if (strm.avail_out === 0) {
        s.last_flush = -1;
        /* avoid BUF_ERROR next call, see above */
      }
      return Z_OK$3;
      /* If flush != Z_NO_FLUSH && avail_out == 0, the next call
       * of deflate should use the same flush parameter to make sure
       * that the flush is complete. So we don't have to output an
       * empty block here, this will be done at next call. This also
       * ensures that for a very small output buffer, we emit at most
       * one empty block.
       */
    }
    if (bstate === BS_BLOCK_DONE) {
      if (flush === Z_PARTIAL_FLUSH) {
        _tr_align(s);
      } else if (flush !== Z_BLOCK$1) {
        /* FULL_FLUSH or SYNC_FLUSH */

        _tr_stored_block(s, 0, 0, false);
        /* For a full flush, this empty block will be recognized
         * as a special marker by inflate_sync().
         */
        if (flush === Z_FULL_FLUSH$1) {
          /*** CLEAR_HASH(s); ***/ /* forget history */
          zero(s.head); // Fill with NIL (= 0);

          if (s.lookahead === 0) {
            s.strstart = 0;
            s.block_start = 0;
            s.insert = 0;
          }
        }
      }
      flush_pending(strm);
      if (strm.avail_out === 0) {
        s.last_flush = -1; /* avoid BUF_ERROR at next call, see above */
        return Z_OK$3;
      }
    }
  }
  if (flush !== Z_FINISH$3) {
    return Z_OK$3;
  }
  if (s.wrap <= 0) {
    return Z_STREAM_END$3;
  }

  /* Write the trailer */
  if (s.wrap === 2) {
    put_byte(s, strm.adler & 0xff);
    put_byte(s, strm.adler >> 8 & 0xff);
    put_byte(s, strm.adler >> 16 & 0xff);
    put_byte(s, strm.adler >> 24 & 0xff);
    put_byte(s, strm.total_in & 0xff);
    put_byte(s, strm.total_in >> 8 & 0xff);
    put_byte(s, strm.total_in >> 16 & 0xff);
    put_byte(s, strm.total_in >> 24 & 0xff);
  } else {
    putShortMSB(s, strm.adler >>> 16);
    putShortMSB(s, strm.adler & 0xffff);
  }
  flush_pending(strm);
  /* If avail_out is zero, the application will call deflate again
   * to flush the rest.
   */
  if (s.wrap > 0) {
    s.wrap = -s.wrap;
  }
  /* write the trailer only once! */
  return s.pending !== 0 ? Z_OK$3 : Z_STREAM_END$3;
};
const deflateEnd = strm => {
  if (deflateStateCheck(strm)) {
    return Z_STREAM_ERROR$2;
  }
  const status = strm.state.status;
  strm.state = null;
  return status === BUSY_STATE ? err(strm, Z_DATA_ERROR$2) : Z_OK$3;
};

/* =========================================================================
 * Initializes the compression dictionary from the given byte
 * sequence without producing any compressed output.
 */
const deflateSetDictionary = (strm, dictionary) => {
  let dictLength = dictionary.length;
  if (deflateStateCheck(strm)) {
    return Z_STREAM_ERROR$2;
  }
  const s = strm.state;
  const wrap = s.wrap;
  if (wrap === 2 || wrap === 1 && s.status !== INIT_STATE || s.lookahead) {
    return Z_STREAM_ERROR$2;
  }

  /* when using zlib wrappers, compute Adler-32 for provided dictionary */
  if (wrap === 1) {
    /* adler32(strm->adler, dictionary, dictLength); */
    strm.adler = adler32_1(strm.adler, dictionary, dictLength, 0);
  }
  s.wrap = 0; /* avoid computing Adler-32 in read_buf */

  /* if dictionary would fill window, just replace the history */
  if (dictLength >= s.w_size) {
    if (wrap === 0) {
      /* already empty otherwise */
      /*** CLEAR_HASH(s); ***/
      zero(s.head); // Fill with NIL (= 0);
      s.strstart = 0;
      s.block_start = 0;
      s.insert = 0;
    }
    /* use the tail */
    // dictionary = dictionary.slice(dictLength - s.w_size);
    let tmpDict = new Uint8Array(s.w_size);
    tmpDict.set(dictionary.subarray(dictLength - s.w_size, dictLength), 0);
    dictionary = tmpDict;
    dictLength = s.w_size;
  }
  /* insert dictionary into window and hash */
  const avail = strm.avail_in;
  const next = strm.next_in;
  const input = strm.input;
  strm.avail_in = dictLength;
  strm.next_in = 0;
  strm.input = dictionary;
  fill_window(s);
  while (s.lookahead >= MIN_MATCH) {
    let str = s.strstart;
    let n = s.lookahead - (MIN_MATCH - 1);
    do {
      /* UPDATE_HASH(s, s->ins_h, s->window[str + MIN_MATCH-1]); */
      s.ins_h = HASH(s, s.ins_h, s.window[str + MIN_MATCH - 1]);
      s.prev[str & s.w_mask] = s.head[s.ins_h];
      s.head[s.ins_h] = str;
      str++;
    } while (--n);
    s.strstart = str;
    s.lookahead = MIN_MATCH - 1;
    fill_window(s);
  }
  s.strstart += s.lookahead;
  s.block_start = s.strstart;
  s.insert = s.lookahead;
  s.lookahead = 0;
  s.match_length = s.prev_length = MIN_MATCH - 1;
  s.match_available = 0;
  strm.next_in = next;
  strm.input = input;
  strm.avail_in = avail;
  s.wrap = wrap;
  return Z_OK$3;
};
var deflateInit_1 = deflateInit;
var deflateInit2_1 = deflateInit2;
var deflateReset_1 = deflateReset;
var deflateResetKeep_1 = deflateResetKeep;
var deflateSetHeader_1 = deflateSetHeader;
var deflate_2$1 = deflate$2;
var deflateEnd_1 = deflateEnd;
var deflateSetDictionary_1 = deflateSetDictionary;
var deflateInfo = 'pako deflate (from Nodeca project)';

/* Not implemented
module.exports.deflateBound = deflateBound;
module.exports.deflateCopy = deflateCopy;
module.exports.deflateGetDictionary = deflateGetDictionary;
module.exports.deflateParams = deflateParams;
module.exports.deflatePending = deflatePending;
module.exports.deflatePrime = deflatePrime;
module.exports.deflateTune = deflateTune;
*/

var deflate_1$2 = {
  deflateInit: deflateInit_1,
  deflateInit2: deflateInit2_1,
  deflateReset: deflateReset_1,
  deflateResetKeep: deflateResetKeep_1,
  deflateSetHeader: deflateSetHeader_1,
  deflate: deflate_2$1,
  deflateEnd: deflateEnd_1,
  deflateSetDictionary: deflateSetDictionary_1,
  deflateInfo: deflateInfo
};
const _has = (obj, key) => {
  return Object.prototype.hasOwnProperty.call(obj, key);
};
var assign = function (obj /*from1, from2, from3, ...*/) {
  const sources = Array.prototype.slice.call(arguments, 1);
  while (sources.length) {
    const source = sources.shift();
    if (!source) {
      continue;
    }
    if (typeof source !== 'object') {
      throw new TypeError(source + 'must be non-object');
    }
    for (const p in source) {
      if (_has(source, p)) {
        obj[p] = source[p];
      }
    }
  }
  return obj;
};

// Join array of chunks to single array.
var flattenChunks = chunks => {
  // calculate data length
  let len = 0;
  for (let i = 0, l = chunks.length; i < l; i++) {
    len += chunks[i].length;
  }

  // join chunks
  const result = new Uint8Array(len);
  for (let i = 0, pos = 0, l = chunks.length; i < l; i++) {
    let chunk = chunks[i];
    result.set(chunk, pos);
    pos += chunk.length;
  }
  return result;
};
var common = {
  assign: assign,
  flattenChunks: flattenChunks
};

// String encode/decode helpers

// Quick check if we can use fast array to bin string conversion
//
// - apply(Array) can fail on Android 2.2
// - apply(Uint8Array) can fail on iOS 5.1 Safari
//
let STR_APPLY_UIA_OK = true;
try {
  String.fromCharCode.apply(null, new Uint8Array(1));
} catch (__) {
  STR_APPLY_UIA_OK = false;
}

// Table with utf8 lengths (calculated by first byte of sequence)
// Note, that 5 & 6-byte values and some 4-byte values can not be represented in JS,
// because max possible codepoint is 0x10ffff
const _utf8len = new Uint8Array(256);
for (let q = 0; q < 256; q++) {
  _utf8len[q] = q >= 252 ? 6 : q >= 248 ? 5 : q >= 240 ? 4 : q >= 224 ? 3 : q >= 192 ? 2 : 1;
}
_utf8len[254] = _utf8len[254] = 1; // Invalid sequence start

// convert string to array (typed, when possible)
var string2buf = str => {
  if (typeof TextEncoder === 'function' && TextEncoder.prototype.encode) {
    return new TextEncoder().encode(str);
  }
  let buf,
    c,
    c2,
    m_pos,
    i,
    str_len = str.length,
    buf_len = 0;

  // count binary size
  for (m_pos = 0; m_pos < str_len; m_pos++) {
    c = str.charCodeAt(m_pos);
    if ((c & 0xfc00) === 0xd800 && m_pos + 1 < str_len) {
      c2 = str.charCodeAt(m_pos + 1);
      if ((c2 & 0xfc00) === 0xdc00) {
        c = 0x10000 + (c - 0xd800 << 10) + (c2 - 0xdc00);
        m_pos++;
      }
    }
    buf_len += c < 0x80 ? 1 : c < 0x800 ? 2 : c < 0x10000 ? 3 : 4;
  }

  // allocate buffer
  buf = new Uint8Array(buf_len);

  // convert
  for (i = 0, m_pos = 0; i < buf_len; m_pos++) {
    c = str.charCodeAt(m_pos);
    if ((c & 0xfc00) === 0xd800 && m_pos + 1 < str_len) {
      c2 = str.charCodeAt(m_pos + 1);
      if ((c2 & 0xfc00) === 0xdc00) {
        c = 0x10000 + (c - 0xd800 << 10) + (c2 - 0xdc00);
        m_pos++;
      }
    }
    if (c < 0x80) {
      /* one byte */
      buf[i++] = c;
    } else if (c < 0x800) {
      /* two bytes */
      buf[i++] = 0xC0 | c >>> 6;
      buf[i++] = 0x80 | c & 0x3f;
    } else if (c < 0x10000) {
      /* three bytes */
      buf[i++] = 0xE0 | c >>> 12;
      buf[i++] = 0x80 | c >>> 6 & 0x3f;
      buf[i++] = 0x80 | c & 0x3f;
    } else {
      /* four bytes */
      buf[i++] = 0xf0 | c >>> 18;
      buf[i++] = 0x80 | c >>> 12 & 0x3f;
      buf[i++] = 0x80 | c >>> 6 & 0x3f;
      buf[i++] = 0x80 | c & 0x3f;
    }
  }
  return buf;
};

// Helper
const buf2binstring = (buf, len) => {
  // On Chrome, the arguments in a function call that are allowed is `65534`.
  // If the length of the buffer is smaller than that, we can use this optimization,
  // otherwise we will take a slower path.
  if (len < 65534) {
    if (buf.subarray && STR_APPLY_UIA_OK) {
      return String.fromCharCode.apply(null, buf.length === len ? buf : buf.subarray(0, len));
    }
  }
  let result = '';
  for (let i = 0; i < len; i++) {
    result += String.fromCharCode(buf[i]);
  }
  return result;
};

// convert array to string
var buf2string = (buf, max) => {
  const len = max || buf.length;
  if (typeof TextDecoder === 'function' && TextDecoder.prototype.decode) {
    return new TextDecoder().decode(buf.subarray(0, max));
  }
  let i, out;

  // Reserve max possible length (2 words per char)
  // NB: by unknown reasons, Array is significantly faster for
  //     String.fromCharCode.apply than Uint16Array.
  const utf16buf = new Array(len * 2);
  for (out = 0, i = 0; i < len;) {
    let c = buf[i++];
    // quick process ascii
    if (c < 0x80) {
      utf16buf[out++] = c;
      continue;
    }
    let c_len = _utf8len[c];
    // skip 5 & 6 byte codes
    if (c_len > 4) {
      utf16buf[out++] = 0xfffd;
      i += c_len - 1;
      continue;
    }

    // apply mask on first byte
    c &= c_len === 2 ? 0x1f : c_len === 3 ? 0x0f : 0x07;
    // join the rest
    while (c_len > 1 && i < len) {
      c = c << 6 | buf[i++] & 0x3f;
      c_len--;
    }

    // terminated by end of string?
    if (c_len > 1) {
      utf16buf[out++] = 0xfffd;
      continue;
    }
    if (c < 0x10000) {
      utf16buf[out++] = c;
    } else {
      c -= 0x10000;
      utf16buf[out++] = 0xd800 | c >> 10 & 0x3ff;
      utf16buf[out++] = 0xdc00 | c & 0x3ff;
    }
  }
  return buf2binstring(utf16buf, out);
};

// Calculate max possible position in utf8 buffer,
// that will not break sequence. If that's not possible
// - (very small limits) return max size as is.
//
// buf[] - utf8 bytes array
// max   - length limit (mandatory);
var utf8border = (buf, max) => {
  max = max || buf.length;
  if (max > buf.length) {
    max = buf.length;
  }

  // go back from last position, until start of sequence found
  let pos = max - 1;
  while (pos >= 0 && (buf[pos] & 0xC0) === 0x80) {
    pos--;
  }

  // Very small and broken sequence,
  // return max, because we should return something anyway.
  if (pos < 0) {
    return max;
  }

  // If we came to start of buffer - that means buffer is too small,
  // return max too.
  if (pos === 0) {
    return max;
  }
  return pos + _utf8len[buf[pos]] > max ? pos : max;
};
var strings = {
  string2buf: string2buf,
  buf2string: buf2string,
  utf8border: utf8border
};

// (C) 1995-2013 Jean-loup Gailly and Mark Adler
// (C) 2014-2017 Vitaly Puzrin and Andrey Tupitsin
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//   claim that you wrote the original software. If you use this software
//   in a product, an acknowledgment in the product documentation would be
//   appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//   misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

function ZStream() {
  /* next input byte */
  this.input = null; // JS specific, because we have no pointers
  this.next_in = 0;
  /* number of bytes available at input */
  this.avail_in = 0;
  /* total number of input bytes read so far */
  this.total_in = 0;
  /* next output byte should be put there */
  this.output = null; // JS specific, because we have no pointers
  this.next_out = 0;
  /* remaining free space at output */
  this.avail_out = 0;
  /* total number of bytes output so far */
  this.total_out = 0;
  /* last error message, NULL if no error */
  this.msg = '' /*Z_NULL*/;
  /* not visible by applications */
  this.state = null;
  /* best guess about the data type: binary or text */
  this.data_type = 2 /*Z_UNKNOWN*/;
  /* adler32 value of the uncompressed data */
  this.adler = 0;
}
var zstream = ZStream;
const toString$1 = Object.prototype.toString;

/* Public constants ==========================================================*/
/* ===========================================================================*/

const {
  Z_NO_FLUSH: Z_NO_FLUSH$1,
  Z_SYNC_FLUSH,
  Z_FULL_FLUSH,
  Z_FINISH: Z_FINISH$2,
  Z_OK: Z_OK$2,
  Z_STREAM_END: Z_STREAM_END$2,
  Z_DEFAULT_COMPRESSION,
  Z_DEFAULT_STRATEGY,
  Z_DEFLATED: Z_DEFLATED$1
} = constants$2;

/* ===========================================================================*/

/**
 * class Deflate
 *
 * Generic JS-style wrapper for zlib calls. If you don't need
 * streaming behaviour - use more simple functions: [[deflate]],
 * [[deflateRaw]] and [[gzip]].
 **/

/* internal
 * Deflate.chunks -> Array
 *
 * Chunks of output data, if [[Deflate#onData]] not overridden.
 **/

/**
 * Deflate.result -> Uint8Array
 *
 * Compressed result, generated by default [[Deflate#onData]]
 * and [[Deflate#onEnd]] handlers. Filled after you push last chunk
 * (call [[Deflate#push]] with `Z_FINISH` / `true` param).
 **/

/**
 * Deflate.err -> Number
 *
 * Error code after deflate finished. 0 (Z_OK) on success.
 * You will not need it in real life, because deflate errors
 * are possible only on wrong options or bad `onData` / `onEnd`
 * custom handlers.
 **/

/**
 * Deflate.msg -> String
 *
 * Error message, if [[Deflate.err]] != 0
 **/

/**
 * new Deflate(options)
 * - options (Object): zlib deflate options.
 *
 * Creates new deflator instance with specified params. Throws exception
 * on bad params. Supported options:
 *
 * - `level`
 * - `windowBits`
 * - `memLevel`
 * - `strategy`
 * - `dictionary`
 *
 * [http://zlib.net/manual.html#Advanced](http://zlib.net/manual.html#Advanced)
 * for more information on these.
 *
 * Additional options, for internal needs:
 *
 * - `chunkSize` - size of generated data chunks (16K by default)
 * - `raw` (Boolean) - do raw deflate
 * - `gzip` (Boolean) - create gzip wrapper
 * - `header` (Object) - custom header for gzip
 *   - `text` (Boolean) - true if compressed data believed to be text
 *   - `time` (Number) - modification time, unix timestamp
 *   - `os` (Number) - operation system code
 *   - `extra` (Array) - array of bytes with extra data (max 65536)
 *   - `name` (String) - file name (binary string)
 *   - `comment` (String) - comment (binary string)
 *   - `hcrc` (Boolean) - true if header crc should be added
 *
 * ##### Example:
 *
 * ```javascript
 * const pako = require('pako')
 *   , chunk1 = new Uint8Array([1,2,3,4,5,6,7,8,9])
 *   , chunk2 = new Uint8Array([10,11,12,13,14,15,16,17,18,19]);
 *
 * const deflate = new pako.Deflate({ level: 3});
 *
 * deflate.push(chunk1, false);
 * deflate.push(chunk2, true);  // true -> last chunk
 *
 * if (deflate.err) { throw new Error(deflate.err); }
 *
 * console.log(deflate.result);
 * ```
 **/
function Deflate$1(options) {
  this.options = common.assign({
    level: Z_DEFAULT_COMPRESSION,
    method: Z_DEFLATED$1,
    chunkSize: 16384,
    windowBits: 15,
    memLevel: 8,
    strategy: Z_DEFAULT_STRATEGY
  }, options || {});
  let opt = this.options;
  if (opt.raw && opt.windowBits > 0) {
    opt.windowBits = -opt.windowBits;
  } else if (opt.gzip && opt.windowBits > 0 && opt.windowBits < 16) {
    opt.windowBits += 16;
  }
  this.err = 0; // error code, if happens (0 = Z_OK)
  this.msg = ''; // error message
  this.ended = false; // used to avoid multiple onEnd() calls
  this.chunks = []; // chunks of compressed data

  this.strm = new zstream();
  this.strm.avail_out = 0;
  let status = deflate_1$2.deflateInit2(this.strm, opt.level, opt.method, opt.windowBits, opt.memLevel, opt.strategy);
  if (status !== Z_OK$2) {
    throw new Error(messages[status]);
  }
  if (opt.header) {
    deflate_1$2.deflateSetHeader(this.strm, opt.header);
  }
  if (opt.dictionary) {
    let dict;
    // Convert data if needed
    if (typeof opt.dictionary === 'string') {
      // If we need to compress text, change encoding to utf8.
      dict = strings.string2buf(opt.dictionary);
    } else if (toString$1.call(opt.dictionary) === '[object ArrayBuffer]') {
      dict = new Uint8Array(opt.dictionary);
    } else {
      dict = opt.dictionary;
    }
    status = deflate_1$2.deflateSetDictionary(this.strm, dict);
    if (status !== Z_OK$2) {
      throw new Error(messages[status]);
    }
    this._dict_set = true;
  }
}

/**
 * Deflate#push(data[, flush_mode]) -> Boolean
 * - data (Uint8Array|ArrayBuffer|String): input data. Strings will be
 *   converted to utf8 byte sequence.
 * - flush_mode (Number|Boolean): 0..6 for corresponding Z_NO_FLUSH..Z_TREE modes.
 *   See constants. Skipped or `false` means Z_NO_FLUSH, `true` means Z_FINISH.
 *
 * Sends input data to deflate pipe, generating [[Deflate#onData]] calls with
 * new compressed chunks. Returns `true` on success. The last data block must
 * have `flush_mode` Z_FINISH (or `true`). That will flush internal pending
 * buffers and call [[Deflate#onEnd]].
 *
 * On fail call [[Deflate#onEnd]] with error code and return false.
 *
 * ##### Example
 *
 * ```javascript
 * push(chunk, false); // push one of data chunks
 * ...
 * push(chunk, true);  // push last chunk
 * ```
 **/
Deflate$1.prototype.push = function (data, flush_mode) {
  const strm = this.strm;
  const chunkSize = this.options.chunkSize;
  let status, _flush_mode;
  if (this.ended) {
    return false;
  }
  if (flush_mode === ~~flush_mode) _flush_mode = flush_mode;else _flush_mode = flush_mode === true ? Z_FINISH$2 : Z_NO_FLUSH$1;

  // Convert data if needed
  if (typeof data === 'string') {
    // If we need to compress text, change encoding to utf8.
    strm.input = strings.string2buf(data);
  } else if (toString$1.call(data) === '[object ArrayBuffer]') {
    strm.input = new Uint8Array(data);
  } else {
    strm.input = data;
  }
  strm.next_in = 0;
  strm.avail_in = strm.input.length;
  for (;;) {
    if (strm.avail_out === 0) {
      strm.output = new Uint8Array(chunkSize);
      strm.next_out = 0;
      strm.avail_out = chunkSize;
    }

    // Make sure avail_out > 6 to avoid repeating markers
    if ((_flush_mode === Z_SYNC_FLUSH || _flush_mode === Z_FULL_FLUSH) && strm.avail_out <= 6) {
      this.onData(strm.output.subarray(0, strm.next_out));
      strm.avail_out = 0;
      continue;
    }
    status = deflate_1$2.deflate(strm, _flush_mode);

    // Ended => flush and finish
    if (status === Z_STREAM_END$2) {
      if (strm.next_out > 0) {
        this.onData(strm.output.subarray(0, strm.next_out));
      }
      status = deflate_1$2.deflateEnd(this.strm);
      this.onEnd(status);
      this.ended = true;
      return status === Z_OK$2;
    }

    // Flush if out buffer full
    if (strm.avail_out === 0) {
      this.onData(strm.output);
      continue;
    }

    // Flush if requested and has data
    if (_flush_mode > 0 && strm.next_out > 0) {
      this.onData(strm.output.subarray(0, strm.next_out));
      strm.avail_out = 0;
      continue;
    }
    if (strm.avail_in === 0) break;
  }
  return true;
};

/**
 * Deflate#onData(chunk) -> Void
 * - chunk (Uint8Array): output data.
 *
 * By default, stores data blocks in `chunks[]` property and glue
 * those in `onEnd`. Override this handler, if you need another behaviour.
 **/
Deflate$1.prototype.onData = function (chunk) {
  this.chunks.push(chunk);
};

/**
 * Deflate#onEnd(status) -> Void
 * - status (Number): deflate status. 0 (Z_OK) on success,
 *   other if not.
 *
 * Called once after you tell deflate that the input stream is
 * complete (Z_FINISH). By default - join collected chunks,
 * free memory and fill `results` / `err` properties.
 **/
Deflate$1.prototype.onEnd = function (status) {
  // On success - join
  if (status === Z_OK$2) {
    this.result = common.flattenChunks(this.chunks);
  }
  this.chunks = [];
  this.err = status;
  this.msg = this.strm.msg;
};

/**
 * deflate(data[, options]) -> Uint8Array
 * - data (Uint8Array|ArrayBuffer|String): input data to compress.
 * - options (Object): zlib deflate options.
 *
 * Compress `data` with deflate algorithm and `options`.
 *
 * Supported options are:
 *
 * - level
 * - windowBits
 * - memLevel
 * - strategy
 * - dictionary
 *
 * [http://zlib.net/manual.html#Advanced](http://zlib.net/manual.html#Advanced)
 * for more information on these.
 *
 * Sugar (options):
 *
 * - `raw` (Boolean) - say that we work with raw stream, if you don't wish to specify
 *   negative windowBits implicitly.
 *
 * ##### Example:
 *
 * ```javascript
 * const pako = require('pako')
 * const data = new Uint8Array([1,2,3,4,5,6,7,8,9]);
 *
 * console.log(pako.deflate(data));
 * ```
 **/
function deflate$1(input, options) {
  const deflator = new Deflate$1(options);
  deflator.push(input, true);

  // That will never happens, if you don't cheat with options :)
  if (deflator.err) {
    throw deflator.msg || messages[deflator.err];
  }
  return deflator.result;
}
var deflate_2 = deflate$1;
var deflate_1$1 = {
  deflate: deflate_2};

// (C) 1995-2013 Jean-loup Gailly and Mark Adler
// (C) 2014-2017 Vitaly Puzrin and Andrey Tupitsin
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//   claim that you wrote the original software. If you use this software
//   in a product, an acknowledgment in the product documentation would be
//   appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//   misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

// See state defs from inflate.js
const BAD$1 = 16209; /* got a data error -- remain here until reset */
const TYPE$1 = 16191; /* i: waiting for type bits, including last-flag bit */

/*
   Decode literal, length, and distance codes and write out the resulting
   literal and match bytes until either not enough input or output is
   available, an end-of-block is encountered, or a data error is encountered.
   When large enough input and output buffers are supplied to inflate(), for
   example, a 16K input buffer and a 64K output buffer, more than 95% of the
   inflate execution time is spent in this routine.

   Entry assumptions:

        state.mode === LEN
        strm.avail_in >= 6
        strm.avail_out >= 258
        start >= strm.avail_out
        state.bits < 8

   On return, state.mode is one of:

        LEN -- ran out of enough output space or enough available input
        TYPE -- reached end of block code, inflate() to interpret next block
        BAD -- error in block data

   Notes:

    - The maximum input bits used by a length/distance pair is 15 bits for the
      length code, 5 bits for the length extra, 15 bits for the distance code,
      and 13 bits for the distance extra.  This totals 48 bits, or six bytes.
      Therefore if strm.avail_in >= 6, then there is enough input to avoid
      checking for available input while decoding.

    - The maximum bytes that a single length/distance pair can output is 258
      bytes, which is the maximum length that can be coded.  inflate_fast()
      requires strm.avail_out >= 258 for each loop to avoid checking for
      output space.
 */
var inffast = function inflate_fast(strm, start) {
  let _in; /* local strm.input */
  let last; /* have enough input while in < last */
  let _out; /* local strm.output */
  let beg; /* inflate()'s initial strm.output */
  let end; /* while out < end, enough space available */
  //#ifdef INFLATE_STRICT
  let dmax; /* maximum distance from zlib header */
  //#endif
  let wsize; /* window size or zero if not using window */
  let whave; /* valid bytes in the window */
  let wnext; /* window write index */
  // Use `s_window` instead `window`, avoid conflict with instrumentation tools
  let s_window; /* allocated sliding window, if wsize != 0 */
  let hold; /* local strm.hold */
  let bits; /* local strm.bits */
  let lcode; /* local strm.lencode */
  let dcode; /* local strm.distcode */
  let lmask; /* mask for first level of length codes */
  let dmask; /* mask for first level of distance codes */
  let here; /* retrieved table entry */
  let op; /* code bits, operation, extra bits, or */
  /*  window position, window bytes to copy */
  let len; /* match length, unused bytes */
  let dist; /* match distance */
  let from; /* where to copy match from */
  let from_source;
  let input, output; // JS specific, because we have no pointers

  /* copy state to local variables */
  const state = strm.state;
  //here = state.here;
  _in = strm.next_in;
  input = strm.input;
  last = _in + (strm.avail_in - 5);
  _out = strm.next_out;
  output = strm.output;
  beg = _out - (start - strm.avail_out);
  end = _out + (strm.avail_out - 257);
  //#ifdef INFLATE_STRICT
  dmax = state.dmax;
  //#endif
  wsize = state.wsize;
  whave = state.whave;
  wnext = state.wnext;
  s_window = state.window;
  hold = state.hold;
  bits = state.bits;
  lcode = state.lencode;
  dcode = state.distcode;
  lmask = (1 << state.lenbits) - 1;
  dmask = (1 << state.distbits) - 1;

  /* decode literals and length/distances until end-of-block or not enough
     input data or output space */

  top: do {
    if (bits < 15) {
      hold += input[_in++] << bits;
      bits += 8;
      hold += input[_in++] << bits;
      bits += 8;
    }
    here = lcode[hold & lmask];
    dolen: for (;;) {
      // Goto emulation
      op = here >>> 24 /*here.bits*/;
      hold >>>= op;
      bits -= op;
      op = here >>> 16 & 0xff /*here.op*/;
      if (op === 0) {
        /* literal */
        //Tracevv((stderr, here.val >= 0x20 && here.val < 0x7f ?
        //        "inflate:         literal '%c'\n" :
        //        "inflate:         literal 0x%02x\n", here.val));
        output[_out++] = here & 0xffff /*here.val*/;
      } else if (op & 16) {
        /* length base */
        len = here & 0xffff /*here.val*/;
        op &= 15; /* number of extra bits */
        if (op) {
          if (bits < op) {
            hold += input[_in++] << bits;
            bits += 8;
          }
          len += hold & (1 << op) - 1;
          hold >>>= op;
          bits -= op;
        }
        //Tracevv((stderr, "inflate:         length %u\n", len));
        if (bits < 15) {
          hold += input[_in++] << bits;
          bits += 8;
          hold += input[_in++] << bits;
          bits += 8;
        }
        here = dcode[hold & dmask];
        dodist: for (;;) {
          // goto emulation
          op = here >>> 24 /*here.bits*/;
          hold >>>= op;
          bits -= op;
          op = here >>> 16 & 0xff /*here.op*/;
          if (op & 16) {
            /* distance base */
            dist = here & 0xffff /*here.val*/;
            op &= 15; /* number of extra bits */
            if (bits < op) {
              hold += input[_in++] << bits;
              bits += 8;
              if (bits < op) {
                hold += input[_in++] << bits;
                bits += 8;
              }
            }
            dist += hold & (1 << op) - 1;
            //#ifdef INFLATE_STRICT
            if (dist > dmax) {
              strm.msg = 'invalid distance too far back';
              state.mode = BAD$1;
              break top;
            }
            //#endif
            hold >>>= op;
            bits -= op;
            //Tracevv((stderr, "inflate:         distance %u\n", dist));
            op = _out - beg; /* max distance in output */
            if (dist > op) {
              /* see if copy from window */
              op = dist - op; /* distance back in window */
              if (op > whave) {
                if (state.sane) {
                  strm.msg = 'invalid distance too far back';
                  state.mode = BAD$1;
                  break top;
                }

                // (!) This block is disabled in zlib defaults,
                // don't enable it for binary compatibility
                //#ifdef INFLATE_ALLOW_INVALID_DISTANCE_TOOFAR_ARRR
                //                if (len <= op - whave) {
                //                  do {
                //                    output[_out++] = 0;
                //                  } while (--len);
                //                  continue top;
                //                }
                //                len -= op - whave;
                //                do {
                //                  output[_out++] = 0;
                //                } while (--op > whave);
                //                if (op === 0) {
                //                  from = _out - dist;
                //                  do {
                //                    output[_out++] = output[from++];
                //                  } while (--len);
                //                  continue top;
                //                }
                //#endif
              }
              from = 0; // window index
              from_source = s_window;
              if (wnext === 0) {
                /* very common case */
                from += wsize - op;
                if (op < len) {
                  /* some from window */
                  len -= op;
                  do {
                    output[_out++] = s_window[from++];
                  } while (--op);
                  from = _out - dist; /* rest from output */
                  from_source = output;
                }
              } else if (wnext < op) {
                /* wrap around window */
                from += wsize + wnext - op;
                op -= wnext;
                if (op < len) {
                  /* some from end of window */
                  len -= op;
                  do {
                    output[_out++] = s_window[from++];
                  } while (--op);
                  from = 0;
                  if (wnext < len) {
                    /* some from start of window */
                    op = wnext;
                    len -= op;
                    do {
                      output[_out++] = s_window[from++];
                    } while (--op);
                    from = _out - dist; /* rest from output */
                    from_source = output;
                  }
                }
              } else {
                /* contiguous in window */
                from += wnext - op;
                if (op < len) {
                  /* some from window */
                  len -= op;
                  do {
                    output[_out++] = s_window[from++];
                  } while (--op);
                  from = _out - dist; /* rest from output */
                  from_source = output;
                }
              }
              while (len > 2) {
                output[_out++] = from_source[from++];
                output[_out++] = from_source[from++];
                output[_out++] = from_source[from++];
                len -= 3;
              }
              if (len) {
                output[_out++] = from_source[from++];
                if (len > 1) {
                  output[_out++] = from_source[from++];
                }
              }
            } else {
              from = _out - dist; /* copy direct from output */
              do {
                /* minimum length is three */
                output[_out++] = output[from++];
                output[_out++] = output[from++];
                output[_out++] = output[from++];
                len -= 3;
              } while (len > 2);
              if (len) {
                output[_out++] = output[from++];
                if (len > 1) {
                  output[_out++] = output[from++];
                }
              }
            }
          } else if ((op & 64) === 0) {
            /* 2nd level distance code */
            here = dcode[(here & 0xffff /*here.val*/) + (hold & (1 << op) - 1)];
            continue dodist;
          } else {
            strm.msg = 'invalid distance code';
            state.mode = BAD$1;
            break top;
          }
          break; // need to emulate goto via "continue"
        }
      } else if ((op & 64) === 0) {
        /* 2nd level length code */
        here = lcode[(here & 0xffff /*here.val*/) + (hold & (1 << op) - 1)];
        continue dolen;
      } else if (op & 32) {
        /* end-of-block */
        //Tracevv((stderr, "inflate:         end of block\n"));
        state.mode = TYPE$1;
        break top;
      } else {
        strm.msg = 'invalid literal/length code';
        state.mode = BAD$1;
        break top;
      }
      break; // need to emulate goto via "continue"
    }
  } while (_in < last && _out < end);

  /* return unused bytes (on entry, bits < 8, so in won't go too far back) */
  len = bits >> 3;
  _in -= len;
  bits -= len << 3;
  hold &= (1 << bits) - 1;

  /* update state and return */
  strm.next_in = _in;
  strm.next_out = _out;
  strm.avail_in = _in < last ? 5 + (last - _in) : 5 - (_in - last);
  strm.avail_out = _out < end ? 257 + (end - _out) : 257 - (_out - end);
  state.hold = hold;
  state.bits = bits;
  return;
};

// (C) 1995-2013 Jean-loup Gailly and Mark Adler
// (C) 2014-2017 Vitaly Puzrin and Andrey Tupitsin
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//   claim that you wrote the original software. If you use this software
//   in a product, an acknowledgment in the product documentation would be
//   appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//   misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

const MAXBITS = 15;
const ENOUGH_LENS$1 = 852;
const ENOUGH_DISTS$1 = 592;
//const ENOUGH = (ENOUGH_LENS+ENOUGH_DISTS);

const CODES$1 = 0;
const LENS$1 = 1;
const DISTS$1 = 2;
const lbase = new Uint16Array([/* Length codes 257..285 base */
3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0]);
const lext = new Uint8Array([/* Length codes 257..285 extra */
16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 16, 72, 78]);
const dbase = new Uint16Array([/* Distance codes 0..29 base */
1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577, 0, 0]);
const dext = new Uint8Array([/* Distance codes 0..29 extra */
16, 16, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 64, 64]);
const inflate_table = (type, lens, lens_index, codes, table, table_index, work, opts) => {
  const bits = opts.bits;
  //here = opts.here; /* table entry for duplication */

  let len = 0; /* a code's length in bits */
  let sym = 0; /* index of code symbols */
  let min = 0,
    max = 0; /* minimum and maximum code lengths */
  let root = 0; /* number of index bits for root table */
  let curr = 0; /* number of index bits for current table */
  let drop = 0; /* code bits to drop for sub-table */
  let left = 0; /* number of prefix codes available */
  let used = 0; /* code entries in table used */
  let huff = 0; /* Huffman code */
  let incr; /* for incrementing code, index */
  let fill; /* index for replicating entries */
  let low; /* low bits for current root entry */
  let mask; /* mask for low root bits */
  let next; /* next available space in table */
  let base = null; /* base value table to use */
  //  let shoextra;    /* extra bits table to use */
  let match; /* use base and extra for symbol >= match */
  const count = new Uint16Array(MAXBITS + 1); //[MAXBITS+1];    /* number of codes of each length */
  const offs = new Uint16Array(MAXBITS + 1); //[MAXBITS+1];     /* offsets in table for each length */
  let extra = null;
  let here_bits, here_op, here_val;

  /*
   Process a set of code lengths to create a canonical Huffman code.  The
   code lengths are lens[0..codes-1].  Each length corresponds to the
   symbols 0..codes-1.  The Huffman code is generated by first sorting the
   symbols by length from short to long, and retaining the symbol order
   for codes with equal lengths.  Then the code starts with all zero bits
   for the first code of the shortest length, and the codes are integer
   increments for the same length, and zeros are appended as the length
   increases.  For the deflate format, these bits are stored backwards
   from their more natural integer increment ordering, and so when the
   decoding tables are built in the large loop below, the integer codes
   are incremented backwards.
    This routine assumes, but does not check, that all of the entries in
   lens[] are in the range 0..MAXBITS.  The caller must assure this.
   1..MAXBITS is interpreted as that code length.  zero means that that
   symbol does not occur in this code.
    The codes are sorted by computing a count of codes for each length,
   creating from that a table of starting indices for each length in the
   sorted table, and then entering the symbols in order in the sorted
   table.  The sorted table is work[], with that space being provided by
   the caller.
    The length counts are used for other purposes as well, i.e. finding
   the minimum and maximum length codes, determining if there are any
   codes at all, checking for a valid set of lengths, and looking ahead
   at length counts to determine sub-table sizes when building the
   decoding tables.
   */

  /* accumulate lengths for codes (assumes lens[] all in 0..MAXBITS) */
  for (len = 0; len <= MAXBITS; len++) {
    count[len] = 0;
  }
  for (sym = 0; sym < codes; sym++) {
    count[lens[lens_index + sym]]++;
  }

  /* bound code lengths, force root to be within code lengths */
  root = bits;
  for (max = MAXBITS; max >= 1; max--) {
    if (count[max] !== 0) {
      break;
    }
  }
  if (root > max) {
    root = max;
  }
  if (max === 0) {
    /* no symbols to code at all */
    //table.op[opts.table_index] = 64;  //here.op = (var char)64;    /* invalid code marker */
    //table.bits[opts.table_index] = 1;   //here.bits = (var char)1;
    //table.val[opts.table_index++] = 0;   //here.val = (var short)0;
    table[table_index++] = 1 << 24 | 64 << 16 | 0;

    //table.op[opts.table_index] = 64;
    //table.bits[opts.table_index] = 1;
    //table.val[opts.table_index++] = 0;
    table[table_index++] = 1 << 24 | 64 << 16 | 0;
    opts.bits = 1;
    return 0; /* no symbols, but wait for decoding to report error */
  }
  for (min = 1; min < max; min++) {
    if (count[min] !== 0) {
      break;
    }
  }
  if (root < min) {
    root = min;
  }

  /* check for an over-subscribed or incomplete set of lengths */
  left = 1;
  for (len = 1; len <= MAXBITS; len++) {
    left <<= 1;
    left -= count[len];
    if (left < 0) {
      return -1;
    } /* over-subscribed */
  }
  if (left > 0 && (type === CODES$1 || max !== 1)) {
    return -1; /* incomplete set */
  }

  /* generate offsets into symbol table for each length for sorting */
  offs[1] = 0;
  for (len = 1; len < MAXBITS; len++) {
    offs[len + 1] = offs[len] + count[len];
  }

  /* sort symbols by length, by symbol order within each length */
  for (sym = 0; sym < codes; sym++) {
    if (lens[lens_index + sym] !== 0) {
      work[offs[lens[lens_index + sym]]++] = sym;
    }
  }

  /*
   Create and fill in decoding tables.  In this loop, the table being
   filled is at next and has curr index bits.  The code being used is huff
   with length len.  That code is converted to an index by dropping drop
   bits off of the bottom.  For codes where len is less than drop + curr,
   those top drop + curr - len bits are incremented through all values to
   fill the table with replicated entries.
    root is the number of index bits for the root table.  When len exceeds
   root, sub-tables are created pointed to by the root entry with an index
   of the low root bits of huff.  This is saved in low to check for when a
   new sub-table should be started.  drop is zero when the root table is
   being filled, and drop is root when sub-tables are being filled.
    When a new sub-table is needed, it is necessary to look ahead in the
   code lengths to determine what size sub-table is needed.  The length
   counts are used for this, and so count[] is decremented as codes are
   entered in the tables.
    used keeps track of how many table entries have been allocated from the
   provided *table space.  It is checked for LENS and DIST tables against
   the constants ENOUGH_LENS and ENOUGH_DISTS to guard against changes in
   the initial root table size constants.  See the comments in inftrees.h
   for more information.
    sym increments through all symbols, and the loop terminates when
   all codes of length max, i.e. all codes, have been processed.  This
   routine permits incomplete codes, so another loop after this one fills
   in the rest of the decoding tables with invalid code markers.
   */

  /* set up for code type */
  // poor man optimization - use if-else instead of switch,
  // to avoid deopts in old v8
  if (type === CODES$1) {
    base = extra = work; /* dummy value--not used */
    match = 20;
  } else if (type === LENS$1) {
    base = lbase;
    extra = lext;
    match = 257;
  } else {
    /* DISTS */
    base = dbase;
    extra = dext;
    match = 0;
  }

  /* initialize opts for loop */
  huff = 0; /* starting code */
  sym = 0; /* starting code symbol */
  len = min; /* starting code length */
  next = table_index; /* current table to fill in */
  curr = root; /* current table index bits */
  drop = 0; /* current bits to drop from code for index */
  low = -1; /* trigger new sub-table when len > root */
  used = 1 << root; /* use root table entries */
  mask = used - 1; /* mask for comparing low */

  /* check available table space */
  if (type === LENS$1 && used > ENOUGH_LENS$1 || type === DISTS$1 && used > ENOUGH_DISTS$1) {
    return 1;
  }

  /* process all codes and make table entries */
  for (;;) {
    /* create table entry */
    here_bits = len - drop;
    if (work[sym] + 1 < match) {
      here_op = 0;
      here_val = work[sym];
    } else if (work[sym] >= match) {
      here_op = extra[work[sym] - match];
      here_val = base[work[sym] - match];
    } else {
      here_op = 32 + 64; /* end of block */
      here_val = 0;
    }

    /* replicate for those indices with low len bits equal to huff */
    incr = 1 << len - drop;
    fill = 1 << curr;
    min = fill; /* save offset to next table */
    do {
      fill -= incr;
      table[next + (huff >> drop) + fill] = here_bits << 24 | here_op << 16 | here_val | 0;
    } while (fill !== 0);

    /* backwards increment the len-bit code huff */
    incr = 1 << len - 1;
    while (huff & incr) {
      incr >>= 1;
    }
    if (incr !== 0) {
      huff &= incr - 1;
      huff += incr;
    } else {
      huff = 0;
    }

    /* go to next symbol, update count, len */
    sym++;
    if (--count[len] === 0) {
      if (len === max) {
        break;
      }
      len = lens[lens_index + work[sym]];
    }

    /* create new sub-table if needed */
    if (len > root && (huff & mask) !== low) {
      /* if first time, transition to sub-tables */
      if (drop === 0) {
        drop = root;
      }

      /* increment past last table */
      next += min; /* here min is 1 << curr */

      /* determine length of next table */
      curr = len - drop;
      left = 1 << curr;
      while (curr + drop < max) {
        left -= count[curr + drop];
        if (left <= 0) {
          break;
        }
        curr++;
        left <<= 1;
      }

      /* check for enough space */
      used += 1 << curr;
      if (type === LENS$1 && used > ENOUGH_LENS$1 || type === DISTS$1 && used > ENOUGH_DISTS$1) {
        return 1;
      }

      /* point entry in root table to sub-table */
      low = huff & mask;
      /*table.op[low] = curr;
      table.bits[low] = root;
      table.val[low] = next - opts.table_index;*/
      table[low] = root << 24 | curr << 16 | next - table_index | 0;
    }
  }

  /* fill in remaining table entry if code is incomplete (guaranteed to have
   at most one remaining entry, since if the code is incomplete, the
   maximum code length that was allowed to get this far is one bit) */
  if (huff !== 0) {
    //table.op[next + huff] = 64;            /* invalid code marker */
    //table.bits[next + huff] = len - drop;
    //table.val[next + huff] = 0;
    table[next + huff] = len - drop << 24 | 64 << 16 | 0;
  }

  /* set return parameters */
  //opts.table_index += used;
  opts.bits = root;
  return 0;
};
var inftrees = inflate_table;

// (C) 1995-2013 Jean-loup Gailly and Mark Adler
// (C) 2014-2017 Vitaly Puzrin and Andrey Tupitsin
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//   claim that you wrote the original software. If you use this software
//   in a product, an acknowledgment in the product documentation would be
//   appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//   misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

const CODES = 0;
const LENS = 1;
const DISTS = 2;

/* Public constants ==========================================================*/
/* ===========================================================================*/

const {
  Z_FINISH: Z_FINISH$1,
  Z_BLOCK,
  Z_TREES,
  Z_OK: Z_OK$1,
  Z_STREAM_END: Z_STREAM_END$1,
  Z_NEED_DICT: Z_NEED_DICT$1,
  Z_STREAM_ERROR: Z_STREAM_ERROR$1,
  Z_DATA_ERROR: Z_DATA_ERROR$1,
  Z_MEM_ERROR: Z_MEM_ERROR$1,
  Z_BUF_ERROR,
  Z_DEFLATED
} = constants$2;

/* STATES ====================================================================*/
/* ===========================================================================*/

const HEAD = 16180; /* i: waiting for magic header */
const FLAGS = 16181; /* i: waiting for method and flags (gzip) */
const TIME = 16182; /* i: waiting for modification time (gzip) */
const OS = 16183; /* i: waiting for extra flags and operating system (gzip) */
const EXLEN = 16184; /* i: waiting for extra length (gzip) */
const EXTRA = 16185; /* i: waiting for extra bytes (gzip) */
const NAME = 16186; /* i: waiting for end of file name (gzip) */
const COMMENT = 16187; /* i: waiting for end of comment (gzip) */
const HCRC = 16188; /* i: waiting for header crc (gzip) */
const DICTID = 16189; /* i: waiting for dictionary check value */
const DICT = 16190; /* waiting for inflateSetDictionary() call */
const TYPE = 16191; /* i: waiting for type bits, including last-flag bit */
const TYPEDO = 16192; /* i: same, but skip check to exit inflate on new block */
const STORED = 16193; /* i: waiting for stored size (length and complement) */
const COPY_ = 16194; /* i/o: same as COPY below, but only first time in */
const COPY = 16195; /* i/o: waiting for input or output to copy stored block */
const TABLE = 16196; /* i: waiting for dynamic block table lengths */
const LENLENS = 16197; /* i: waiting for code length code lengths */
const CODELENS = 16198; /* i: waiting for length/lit and distance code lengths */
const LEN_ = 16199; /* i: same as LEN below, but only first time in */
const LEN = 16200; /* i: waiting for length/lit/eob code */
const LENEXT = 16201; /* i: waiting for length extra bits */
const DIST = 16202; /* i: waiting for distance code */
const DISTEXT = 16203; /* i: waiting for distance extra bits */
const MATCH = 16204; /* o: waiting for output space to copy string */
const LIT = 16205; /* o: waiting for output space to write literal */
const CHECK = 16206; /* i: waiting for 32-bit check value */
const LENGTH = 16207; /* i: waiting for 32-bit length (gzip) */
const DONE = 16208; /* finished check, done -- remain here until reset */
const BAD = 16209; /* got a data error -- remain here until reset */
const MEM = 16210; /* got an inflate() memory error -- remain here until reset */
const SYNC = 16211; /* looking for synchronization bytes to restart inflate() */

/* ===========================================================================*/

const ENOUGH_LENS = 852;
const ENOUGH_DISTS = 592;
//const ENOUGH =  (ENOUGH_LENS+ENOUGH_DISTS);

const MAX_WBITS = 15;
/* 32K LZ77 window */
const DEF_WBITS = MAX_WBITS;
const zswap32 = q => {
  return (q >>> 24 & 0xff) + (q >>> 8 & 0xff00) + ((q & 0xff00) << 8) + ((q & 0xff) << 24);
};
function InflateState() {
  this.strm = null; /* pointer back to this zlib stream */
  this.mode = 0; /* current inflate mode */
  this.last = false; /* true if processing last block */
  this.wrap = 0; /* bit 0 true for zlib, bit 1 true for gzip,
                    bit 2 true to validate check value */
  this.havedict = false; /* true if dictionary provided */
  this.flags = 0; /* gzip header method and flags (0 if zlib), or
                     -1 if raw or no header yet */
  this.dmax = 0; /* zlib header max distance (INFLATE_STRICT) */
  this.check = 0; /* protected copy of check value */
  this.total = 0; /* protected copy of output count */
  // TODO: may be {}
  this.head = null; /* where to save gzip header information */

  /* sliding window */
  this.wbits = 0; /* log base 2 of requested window size */
  this.wsize = 0; /* window size or zero if not using window */
  this.whave = 0; /* valid bytes in the window */
  this.wnext = 0; /* window write index */
  this.window = null; /* allocated sliding window, if needed */

  /* bit accumulator */
  this.hold = 0; /* input bit accumulator */
  this.bits = 0; /* number of bits in "in" */

  /* for string and stored block copying */
  this.length = 0; /* literal or length of data to copy */
  this.offset = 0; /* distance back to copy string from */

  /* for table and code decoding */
  this.extra = 0; /* extra bits needed */

  /* fixed and dynamic code tables */
  this.lencode = null; /* starting table for length/literal codes */
  this.distcode = null; /* starting table for distance codes */
  this.lenbits = 0; /* index bits for lencode */
  this.distbits = 0; /* index bits for distcode */

  /* dynamic table building */
  this.ncode = 0; /* number of code length code lengths */
  this.nlen = 0; /* number of length code lengths */
  this.ndist = 0; /* number of distance code lengths */
  this.have = 0; /* number of code lengths in lens[] */
  this.next = null; /* next available space in codes[] */

  this.lens = new Uint16Array(320); /* temporary storage for code lengths */
  this.work = new Uint16Array(288); /* work area for code table building */

  /*
   because we don't have pointers in js, we use lencode and distcode directly
   as buffers so we don't need codes
  */
  //this.codes = new Int32Array(ENOUGH);       /* space for code tables */
  this.lendyn = null; /* dynamic table for length/literal codes (JS specific) */
  this.distdyn = null; /* dynamic table for distance codes (JS specific) */
  this.sane = 0; /* if false, allow invalid distance too far */
  this.back = 0; /* bits back of last unprocessed length/lit */
  this.was = 0; /* initial length of match */
}
const inflateStateCheck = strm => {
  if (!strm) {
    return 1;
  }
  const state = strm.state;
  if (!state || state.strm !== strm || state.mode < HEAD || state.mode > SYNC) {
    return 1;
  }
  return 0;
};
const inflateResetKeep = strm => {
  if (inflateStateCheck(strm)) {
    return Z_STREAM_ERROR$1;
  }
  const state = strm.state;
  strm.total_in = strm.total_out = state.total = 0;
  strm.msg = ''; /*Z_NULL*/
  if (state.wrap) {
    /* to support ill-conceived Java test suite */
    strm.adler = state.wrap & 1;
  }
  state.mode = HEAD;
  state.last = 0;
  state.havedict = 0;
  state.flags = -1;
  state.dmax = 32768;
  state.head = null /*Z_NULL*/;
  state.hold = 0;
  state.bits = 0;
  //state.lencode = state.distcode = state.next = state.codes;
  state.lencode = state.lendyn = new Int32Array(ENOUGH_LENS);
  state.distcode = state.distdyn = new Int32Array(ENOUGH_DISTS);
  state.sane = 1;
  state.back = -1;
  //Tracev((stderr, "inflate: reset\n"));
  return Z_OK$1;
};
const inflateReset = strm => {
  if (inflateStateCheck(strm)) {
    return Z_STREAM_ERROR$1;
  }
  const state = strm.state;
  state.wsize = 0;
  state.whave = 0;
  state.wnext = 0;
  return inflateResetKeep(strm);
};
const inflateReset2 = (strm, windowBits) => {
  let wrap;

  /* get the state */
  if (inflateStateCheck(strm)) {
    return Z_STREAM_ERROR$1;
  }
  const state = strm.state;

  /* extract wrap request from windowBits parameter */
  if (windowBits < 0) {
    wrap = 0;
    windowBits = -windowBits;
  } else {
    wrap = (windowBits >> 4) + 5;
    if (windowBits < 48) {
      windowBits &= 15;
    }
  }

  /* set number of window bits, free window if different */
  if (windowBits && (windowBits < 8 || windowBits > 15)) {
    return Z_STREAM_ERROR$1;
  }
  if (state.window !== null && state.wbits !== windowBits) {
    state.window = null;
  }

  /* update state and reset the rest of it */
  state.wrap = wrap;
  state.wbits = windowBits;
  return inflateReset(strm);
};
const inflateInit2 = (strm, windowBits) => {
  if (!strm) {
    return Z_STREAM_ERROR$1;
  }
  //strm.msg = Z_NULL;                 /* in case we return an error */

  const state = new InflateState();

  //if (state === Z_NULL) return Z_MEM_ERROR;
  //Tracev((stderr, "inflate: allocated\n"));
  strm.state = state;
  state.strm = strm;
  state.window = null /*Z_NULL*/;
  state.mode = HEAD; /* to pass state test in inflateReset2() */
  const ret = inflateReset2(strm, windowBits);
  if (ret !== Z_OK$1) {
    strm.state = null /*Z_NULL*/;
  }
  return ret;
};
const inflateInit = strm => {
  return inflateInit2(strm, DEF_WBITS);
};

/*
 Return state with length and distance decoding tables and index sizes set to
 fixed code decoding.  Normally this returns fixed tables from inffixed.h.
 If BUILDFIXED is defined, then instead this routine builds the tables the
 first time it's called, and returns those tables the first time and
 thereafter.  This reduces the size of the code by about 2K bytes, in
 exchange for a little execution time.  However, BUILDFIXED should not be
 used for threaded applications, since the rewriting of the tables and virgin
 may not be thread-safe.
 */
let virgin = true;
let lenfix, distfix; // We have no pointers in JS, so keep tables separate

const fixedtables = state => {
  /* build fixed huffman tables if first call (may not be thread safe) */
  if (virgin) {
    lenfix = new Int32Array(512);
    distfix = new Int32Array(32);

    /* literal/length table */
    let sym = 0;
    while (sym < 144) {
      state.lens[sym++] = 8;
    }
    while (sym < 256) {
      state.lens[sym++] = 9;
    }
    while (sym < 280) {
      state.lens[sym++] = 7;
    }
    while (sym < 288) {
      state.lens[sym++] = 8;
    }
    inftrees(LENS, state.lens, 0, 288, lenfix, 0, state.work, {
      bits: 9
    });

    /* distance table */
    sym = 0;
    while (sym < 32) {
      state.lens[sym++] = 5;
    }
    inftrees(DISTS, state.lens, 0, 32, distfix, 0, state.work, {
      bits: 5
    });

    /* do this just once */
    virgin = false;
  }
  state.lencode = lenfix;
  state.lenbits = 9;
  state.distcode = distfix;
  state.distbits = 5;
};

/*
 Update the window with the last wsize (normally 32K) bytes written before
 returning.  If window does not exist yet, create it.  This is only called
 when a window is already in use, or when output has been written during this
 inflate call, but the end of the deflate stream has not been reached yet.
 It is also called to create a window for dictionary data when a dictionary
 is loaded.

 Providing output buffers larger than 32K to inflate() should provide a speed
 advantage, since only the last 32K of output is copied to the sliding window
 upon return from inflate(), and since all distances after the first 32K of
 output will fall in the output data, making match copies simpler and faster.
 The advantage may be dependent on the size of the processor's data caches.
 */
const updatewindow = (strm, src, end, copy) => {
  let dist;
  const state = strm.state;

  /* if it hasn't been done already, allocate space for the window */
  if (state.window === null) {
    state.wsize = 1 << state.wbits;
    state.wnext = 0;
    state.whave = 0;
    state.window = new Uint8Array(state.wsize);
  }

  /* copy state->wsize or less output bytes into the circular window */
  if (copy >= state.wsize) {
    state.window.set(src.subarray(end - state.wsize, end), 0);
    state.wnext = 0;
    state.whave = state.wsize;
  } else {
    dist = state.wsize - state.wnext;
    if (dist > copy) {
      dist = copy;
    }
    //zmemcpy(state->window + state->wnext, end - copy, dist);
    state.window.set(src.subarray(end - copy, end - copy + dist), state.wnext);
    copy -= dist;
    if (copy) {
      //zmemcpy(state->window, end - copy, copy);
      state.window.set(src.subarray(end - copy, end), 0);
      state.wnext = copy;
      state.whave = state.wsize;
    } else {
      state.wnext += dist;
      if (state.wnext === state.wsize) {
        state.wnext = 0;
      }
      if (state.whave < state.wsize) {
        state.whave += dist;
      }
    }
  }
  return 0;
};
const inflate$2 = (strm, flush) => {
  let state;
  let input, output; // input/output buffers
  let next; /* next input INDEX */
  let put; /* next output INDEX */
  let have, left; /* available input and output */
  let hold; /* bit buffer */
  let bits; /* bits in bit buffer */
  let _in, _out; /* save starting available input and output */
  let copy; /* number of stored or match bytes to copy */
  let from; /* where to copy match bytes from */
  let from_source;
  let here = 0; /* current decoding table entry */
  let here_bits, here_op, here_val; // paked "here" denormalized (JS specific)
  //let last;                   /* parent table entry */
  let last_bits, last_op, last_val; // paked "last" denormalized (JS specific)
  let len; /* length to copy for repeats, bits to drop */
  let ret; /* return code */
  const hbuf = new Uint8Array(4); /* buffer for gzip header crc calculation */
  let opts;
  let n; // temporary variable for NEED_BITS

  const order = /* permutation of code lengths */
  new Uint8Array([16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15]);
  if (inflateStateCheck(strm) || !strm.output || !strm.input && strm.avail_in !== 0) {
    return Z_STREAM_ERROR$1;
  }
  state = strm.state;
  if (state.mode === TYPE) {
    state.mode = TYPEDO;
  } /* skip check */

  //--- LOAD() ---
  put = strm.next_out;
  output = strm.output;
  left = strm.avail_out;
  next = strm.next_in;
  input = strm.input;
  have = strm.avail_in;
  hold = state.hold;
  bits = state.bits;
  //---

  _in = have;
  _out = left;
  ret = Z_OK$1;
  inf_leave:
  // goto emulation
  for (;;) {
    switch (state.mode) {
      case HEAD:
        if (state.wrap === 0) {
          state.mode = TYPEDO;
          break;
        }
        //=== NEEDBITS(16);
        while (bits < 16) {
          if (have === 0) {
            break inf_leave;
          }
          have--;
          hold += input[next++] << bits;
          bits += 8;
        }
        //===//
        if (state.wrap & 2 && hold === 0x8b1f) {
          /* gzip header */
          if (state.wbits === 0) {
            state.wbits = 15;
          }
          state.check = 0 /*crc32(0L, Z_NULL, 0)*/;
          //=== CRC2(state.check, hold);
          hbuf[0] = hold & 0xff;
          hbuf[1] = hold >>> 8 & 0xff;
          state.check = crc32_1(state.check, hbuf, 2, 0);
          //===//

          //=== INITBITS();
          hold = 0;
          bits = 0;
          //===//
          state.mode = FLAGS;
          break;
        }
        if (state.head) {
          state.head.done = false;
        }
        if (!(state.wrap & 1) || /* check if zlib header allowed */
        (((hold & 0xff /*BITS(8)*/) << 8) + (hold >> 8)) % 31) {
          strm.msg = 'incorrect header check';
          state.mode = BAD;
          break;
        }
        if ((hold & 0x0f /*BITS(4)*/) !== Z_DEFLATED) {
          strm.msg = 'unknown compression method';
          state.mode = BAD;
          break;
        }
        //--- DROPBITS(4) ---//
        hold >>>= 4;
        bits -= 4;
        //---//
        len = (hold & 0x0f /*BITS(4)*/) + 8;
        if (state.wbits === 0) {
          state.wbits = len;
        }
        if (len > 15 || len > state.wbits) {
          strm.msg = 'invalid window size';
          state.mode = BAD;
          break;
        }

        // !!! pako patch. Force use `options.windowBits` if passed.
        // Required to always use max window size by default.
        state.dmax = 1 << state.wbits;
        //state.dmax = 1 << len;

        state.flags = 0; /* indicate zlib header */
        //Tracev((stderr, "inflate:   zlib header ok\n"));
        strm.adler = state.check = 1 /*adler32(0L, Z_NULL, 0)*/;
        state.mode = hold & 0x200 ? DICTID : TYPE;
        //=== INITBITS();
        hold = 0;
        bits = 0;
        //===//
        break;
      case FLAGS:
        //=== NEEDBITS(16); */
        while (bits < 16) {
          if (have === 0) {
            break inf_leave;
          }
          have--;
          hold += input[next++] << bits;
          bits += 8;
        }
        //===//
        state.flags = hold;
        if ((state.flags & 0xff) !== Z_DEFLATED) {
          strm.msg = 'unknown compression method';
          state.mode = BAD;
          break;
        }
        if (state.flags & 0xe000) {
          strm.msg = 'unknown header flags set';
          state.mode = BAD;
          break;
        }
        if (state.head) {
          state.head.text = hold >> 8 & 1;
        }
        if (state.flags & 0x0200 && state.wrap & 4) {
          //=== CRC2(state.check, hold);
          hbuf[0] = hold & 0xff;
          hbuf[1] = hold >>> 8 & 0xff;
          state.check = crc32_1(state.check, hbuf, 2, 0);
          //===//
        }
        //=== INITBITS();
        hold = 0;
        bits = 0;
        //===//
        state.mode = TIME;
      /* falls through */
      case TIME:
        //=== NEEDBITS(32); */
        while (bits < 32) {
          if (have === 0) {
            break inf_leave;
          }
          have--;
          hold += input[next++] << bits;
          bits += 8;
        }
        //===//
        if (state.head) {
          state.head.time = hold;
        }
        if (state.flags & 0x0200 && state.wrap & 4) {
          //=== CRC4(state.check, hold)
          hbuf[0] = hold & 0xff;
          hbuf[1] = hold >>> 8 & 0xff;
          hbuf[2] = hold >>> 16 & 0xff;
          hbuf[3] = hold >>> 24 & 0xff;
          state.check = crc32_1(state.check, hbuf, 4, 0);
          //===
        }
        //=== INITBITS();
        hold = 0;
        bits = 0;
        //===//
        state.mode = OS;
      /* falls through */
      case OS:
        //=== NEEDBITS(16); */
        while (bits < 16) {
          if (have === 0) {
            break inf_leave;
          }
          have--;
          hold += input[next++] << bits;
          bits += 8;
        }
        //===//
        if (state.head) {
          state.head.xflags = hold & 0xff;
          state.head.os = hold >> 8;
        }
        if (state.flags & 0x0200 && state.wrap & 4) {
          //=== CRC2(state.check, hold);
          hbuf[0] = hold & 0xff;
          hbuf[1] = hold >>> 8 & 0xff;
          state.check = crc32_1(state.check, hbuf, 2, 0);
          //===//
        }
        //=== INITBITS();
        hold = 0;
        bits = 0;
        //===//
        state.mode = EXLEN;
      /* falls through */
      case EXLEN:
        if (state.flags & 0x0400) {
          //=== NEEDBITS(16); */
          while (bits < 16) {
            if (have === 0) {
              break inf_leave;
            }
            have--;
            hold += input[next++] << bits;
            bits += 8;
          }
          //===//
          state.length = hold;
          if (state.head) {
            state.head.extra_len = hold;
          }
          if (state.flags & 0x0200 && state.wrap & 4) {
            //=== CRC2(state.check, hold);
            hbuf[0] = hold & 0xff;
            hbuf[1] = hold >>> 8 & 0xff;
            state.check = crc32_1(state.check, hbuf, 2, 0);
            //===//
          }
          //=== INITBITS();
          hold = 0;
          bits = 0;
          //===//
        } else if (state.head) {
          state.head.extra = null /*Z_NULL*/;
        }
        state.mode = EXTRA;
      /* falls through */
      case EXTRA:
        if (state.flags & 0x0400) {
          copy = state.length;
          if (copy > have) {
            copy = have;
          }
          if (copy) {
            if (state.head) {
              len = state.head.extra_len - state.length;
              if (!state.head.extra) {
                // Use untyped array for more convenient processing later
                state.head.extra = new Uint8Array(state.head.extra_len);
              }
              state.head.extra.set(input.subarray(next,
              // extra field is limited to 65536 bytes
              // - no need for additional size check
              next + copy), /*len + copy > state.head.extra_max - len ? state.head.extra_max : copy,*/
              len);
              //zmemcpy(state.head.extra + len, next,
              //        len + copy > state.head.extra_max ?
              //        state.head.extra_max - len : copy);
            }
            if (state.flags & 0x0200 && state.wrap & 4) {
              state.check = crc32_1(state.check, input, copy, next);
            }
            have -= copy;
            next += copy;
            state.length -= copy;
          }
          if (state.length) {
            break inf_leave;
          }
        }
        state.length = 0;
        state.mode = NAME;
      /* falls through */
      case NAME:
        if (state.flags & 0x0800) {
          if (have === 0) {
            break inf_leave;
          }
          copy = 0;
          do {
            // TODO: 2 or 1 bytes?
            len = input[next + copy++];
            /* use constant limit because in js we should not preallocate memory */
            if (state.head && len && state.length < 65536 /*state.head.name_max*/) {
              state.head.name += String.fromCharCode(len);
            }
          } while (len && copy < have);
          if (state.flags & 0x0200 && state.wrap & 4) {
            state.check = crc32_1(state.check, input, copy, next);
          }
          have -= copy;
          next += copy;
          if (len) {
            break inf_leave;
          }
        } else if (state.head) {
          state.head.name = null;
        }
        state.length = 0;
        state.mode = COMMENT;
      /* falls through */
      case COMMENT:
        if (state.flags & 0x1000) {
          if (have === 0) {
            break inf_leave;
          }
          copy = 0;
          do {
            len = input[next + copy++];
            /* use constant limit because in js we should not preallocate memory */
            if (state.head && len && state.length < 65536 /*state.head.comm_max*/) {
              state.head.comment += String.fromCharCode(len);
            }
          } while (len && copy < have);
          if (state.flags & 0x0200 && state.wrap & 4) {
            state.check = crc32_1(state.check, input, copy, next);
          }
          have -= copy;
          next += copy;
          if (len) {
            break inf_leave;
          }
        } else if (state.head) {
          state.head.comment = null;
        }
        state.mode = HCRC;
      /* falls through */
      case HCRC:
        if (state.flags & 0x0200) {
          //=== NEEDBITS(16); */
          while (bits < 16) {
            if (have === 0) {
              break inf_leave;
            }
            have--;
            hold += input[next++] << bits;
            bits += 8;
          }
          //===//
          if (state.wrap & 4 && hold !== (state.check & 0xffff)) {
            strm.msg = 'header crc mismatch';
            state.mode = BAD;
            break;
          }
          //=== INITBITS();
          hold = 0;
          bits = 0;
          //===//
        }
        if (state.head) {
          state.head.hcrc = state.flags >> 9 & 1;
          state.head.done = true;
        }
        strm.adler = state.check = 0;
        state.mode = TYPE;
        break;
      case DICTID:
        //=== NEEDBITS(32); */
        while (bits < 32) {
          if (have === 0) {
            break inf_leave;
          }
          have--;
          hold += input[next++] << bits;
          bits += 8;
        }
        //===//
        strm.adler = state.check = zswap32(hold);
        //=== INITBITS();
        hold = 0;
        bits = 0;
        //===//
        state.mode = DICT;
      /* falls through */
      case DICT:
        if (state.havedict === 0) {
          //--- RESTORE() ---
          strm.next_out = put;
          strm.avail_out = left;
          strm.next_in = next;
          strm.avail_in = have;
          state.hold = hold;
          state.bits = bits;
          //---
          return Z_NEED_DICT$1;
        }
        strm.adler = state.check = 1 /*adler32(0L, Z_NULL, 0)*/;
        state.mode = TYPE;
      /* falls through */
      case TYPE:
        if (flush === Z_BLOCK || flush === Z_TREES) {
          break inf_leave;
        }
      /* falls through */
      case TYPEDO:
        if (state.last) {
          //--- BYTEBITS() ---//
          hold >>>= bits & 7;
          bits -= bits & 7;
          //---//
          state.mode = CHECK;
          break;
        }
        //=== NEEDBITS(3); */
        while (bits < 3) {
          if (have === 0) {
            break inf_leave;
          }
          have--;
          hold += input[next++] << bits;
          bits += 8;
        }
        //===//
        state.last = hold & 0x01 /*BITS(1)*/;
        //--- DROPBITS(1) ---//
        hold >>>= 1;
        bits -= 1;
        //---//

        switch (hold & 0x03 /*BITS(2)*/) {
          case 0:
            /* stored block */
            //Tracev((stderr, "inflate:     stored block%s\n",
            //        state.last ? " (last)" : ""));
            state.mode = STORED;
            break;
          case 1:
            /* fixed block */
            fixedtables(state);
            //Tracev((stderr, "inflate:     fixed codes block%s\n",
            //        state.last ? " (last)" : ""));
            state.mode = LEN_; /* decode codes */
            if (flush === Z_TREES) {
              //--- DROPBITS(2) ---//
              hold >>>= 2;
              bits -= 2;
              //---//
              break inf_leave;
            }
            break;
          case 2:
            /* dynamic block */
            //Tracev((stderr, "inflate:     dynamic codes block%s\n",
            //        state.last ? " (last)" : ""));
            state.mode = TABLE;
            break;
          case 3:
            strm.msg = 'invalid block type';
            state.mode = BAD;
        }
        //--- DROPBITS(2) ---//
        hold >>>= 2;
        bits -= 2;
        //---//
        break;
      case STORED:
        //--- BYTEBITS() ---// /* go to byte boundary */
        hold >>>= bits & 7;
        bits -= bits & 7;
        //---//
        //=== NEEDBITS(32); */
        while (bits < 32) {
          if (have === 0) {
            break inf_leave;
          }
          have--;
          hold += input[next++] << bits;
          bits += 8;
        }
        //===//
        if ((hold & 0xffff) !== (hold >>> 16 ^ 0xffff)) {
          strm.msg = 'invalid stored block lengths';
          state.mode = BAD;
          break;
        }
        state.length = hold & 0xffff;
        //Tracev((stderr, "inflate:       stored length %u\n",
        //        state.length));
        //=== INITBITS();
        hold = 0;
        bits = 0;
        //===//
        state.mode = COPY_;
        if (flush === Z_TREES) {
          break inf_leave;
        }
      /* falls through */
      case COPY_:
        state.mode = COPY;
      /* falls through */
      case COPY:
        copy = state.length;
        if (copy) {
          if (copy > have) {
            copy = have;
          }
          if (copy > left) {
            copy = left;
          }
          if (copy === 0) {
            break inf_leave;
          }
          //--- zmemcpy(put, next, copy); ---
          output.set(input.subarray(next, next + copy), put);
          //---//
          have -= copy;
          next += copy;
          left -= copy;
          put += copy;
          state.length -= copy;
          break;
        }
        //Tracev((stderr, "inflate:       stored end\n"));
        state.mode = TYPE;
        break;
      case TABLE:
        //=== NEEDBITS(14); */
        while (bits < 14) {
          if (have === 0) {
            break inf_leave;
          }
          have--;
          hold += input[next++] << bits;
          bits += 8;
        }
        //===//
        state.nlen = (hold & 0x1f /*BITS(5)*/) + 257;
        //--- DROPBITS(5) ---//
        hold >>>= 5;
        bits -= 5;
        //---//
        state.ndist = (hold & 0x1f /*BITS(5)*/) + 1;
        //--- DROPBITS(5) ---//
        hold >>>= 5;
        bits -= 5;
        //---//
        state.ncode = (hold & 0x0f /*BITS(4)*/) + 4;
        //--- DROPBITS(4) ---//
        hold >>>= 4;
        bits -= 4;
        //---//
        //#ifndef PKZIP_BUG_WORKAROUND
        if (state.nlen > 286 || state.ndist > 30) {
          strm.msg = 'too many length or distance symbols';
          state.mode = BAD;
          break;
        }
        //#endif
        //Tracev((stderr, "inflate:       table sizes ok\n"));
        state.have = 0;
        state.mode = LENLENS;
      /* falls through */
      case LENLENS:
        while (state.have < state.ncode) {
          //=== NEEDBITS(3);
          while (bits < 3) {
            if (have === 0) {
              break inf_leave;
            }
            have--;
            hold += input[next++] << bits;
            bits += 8;
          }
          //===//
          state.lens[order[state.have++]] = hold & 0x07; //BITS(3);
          //--- DROPBITS(3) ---//
          hold >>>= 3;
          bits -= 3;
          //---//
        }
        while (state.have < 19) {
          state.lens[order[state.have++]] = 0;
        }
        // We have separate tables & no pointers. 2 commented lines below not needed.
        //state.next = state.codes;
        //state.lencode = state.next;
        // Switch to use dynamic table
        state.lencode = state.lendyn;
        state.lenbits = 7;
        opts = {
          bits: state.lenbits
        };
        ret = inftrees(CODES, state.lens, 0, 19, state.lencode, 0, state.work, opts);
        state.lenbits = opts.bits;
        if (ret) {
          strm.msg = 'invalid code lengths set';
          state.mode = BAD;
          break;
        }
        //Tracev((stderr, "inflate:       code lengths ok\n"));
        state.have = 0;
        state.mode = CODELENS;
      /* falls through */
      case CODELENS:
        while (state.have < state.nlen + state.ndist) {
          for (;;) {
            here = state.lencode[hold & (1 << state.lenbits) - 1]; /*BITS(state.lenbits)*/
            here_bits = here >>> 24;
            here_op = here >>> 16 & 0xff;
            here_val = here & 0xffff;
            if (here_bits <= bits) {
              break;
            }
            //--- PULLBYTE() ---//
            if (have === 0) {
              break inf_leave;
            }
            have--;
            hold += input[next++] << bits;
            bits += 8;
            //---//
          }
          if (here_val < 16) {
            //--- DROPBITS(here.bits) ---//
            hold >>>= here_bits;
            bits -= here_bits;
            //---//
            state.lens[state.have++] = here_val;
          } else {
            if (here_val === 16) {
              //=== NEEDBITS(here.bits + 2);
              n = here_bits + 2;
              while (bits < n) {
                if (have === 0) {
                  break inf_leave;
                }
                have--;
                hold += input[next++] << bits;
                bits += 8;
              }
              //===//
              //--- DROPBITS(here.bits) ---//
              hold >>>= here_bits;
              bits -= here_bits;
              //---//
              if (state.have === 0) {
                strm.msg = 'invalid bit length repeat';
                state.mode = BAD;
                break;
              }
              len = state.lens[state.have - 1];
              copy = 3 + (hold & 0x03); //BITS(2);
              //--- DROPBITS(2) ---//
              hold >>>= 2;
              bits -= 2;
              //---//
            } else if (here_val === 17) {
              //=== NEEDBITS(here.bits + 3);
              n = here_bits + 3;
              while (bits < n) {
                if (have === 0) {
                  break inf_leave;
                }
                have--;
                hold += input[next++] << bits;
                bits += 8;
              }
              //===//
              //--- DROPBITS(here.bits) ---//
              hold >>>= here_bits;
              bits -= here_bits;
              //---//
              len = 0;
              copy = 3 + (hold & 0x07); //BITS(3);
              //--- DROPBITS(3) ---//
              hold >>>= 3;
              bits -= 3;
              //---//
            } else {
              //=== NEEDBITS(here.bits + 7);
              n = here_bits + 7;
              while (bits < n) {
                if (have === 0) {
                  break inf_leave;
                }
                have--;
                hold += input[next++] << bits;
                bits += 8;
              }
              //===//
              //--- DROPBITS(here.bits) ---//
              hold >>>= here_bits;
              bits -= here_bits;
              //---//
              len = 0;
              copy = 11 + (hold & 0x7f); //BITS(7);
              //--- DROPBITS(7) ---//
              hold >>>= 7;
              bits -= 7;
              //---//
            }
            if (state.have + copy > state.nlen + state.ndist) {
              strm.msg = 'invalid bit length repeat';
              state.mode = BAD;
              break;
            }
            while (copy--) {
              state.lens[state.have++] = len;
            }
          }
        }

        /* handle error breaks in while */
        if (state.mode === BAD) {
          break;
        }

        /* check for end-of-block code (better have one) */
        if (state.lens[256] === 0) {
          strm.msg = 'invalid code -- missing end-of-block';
          state.mode = BAD;
          break;
        }

        /* build code tables -- note: do not change the lenbits or distbits
           values here (9 and 6) without reading the comments in inftrees.h
           concerning the ENOUGH constants, which depend on those values */
        state.lenbits = 9;
        opts = {
          bits: state.lenbits
        };
        ret = inftrees(LENS, state.lens, 0, state.nlen, state.lencode, 0, state.work, opts);
        // We have separate tables & no pointers. 2 commented lines below not needed.
        // state.next_index = opts.table_index;
        state.lenbits = opts.bits;
        // state.lencode = state.next;

        if (ret) {
          strm.msg = 'invalid literal/lengths set';
          state.mode = BAD;
          break;
        }
        state.distbits = 6;
        //state.distcode.copy(state.codes);
        // Switch to use dynamic table
        state.distcode = state.distdyn;
        opts = {
          bits: state.distbits
        };
        ret = inftrees(DISTS, state.lens, state.nlen, state.ndist, state.distcode, 0, state.work, opts);
        // We have separate tables & no pointers. 2 commented lines below not needed.
        // state.next_index = opts.table_index;
        state.distbits = opts.bits;
        // state.distcode = state.next;

        if (ret) {
          strm.msg = 'invalid distances set';
          state.mode = BAD;
          break;
        }
        //Tracev((stderr, 'inflate:       codes ok\n'));
        state.mode = LEN_;
        if (flush === Z_TREES) {
          break inf_leave;
        }
      /* falls through */
      case LEN_:
        state.mode = LEN;
      /* falls through */
      case LEN:
        if (have >= 6 && left >= 258) {
          //--- RESTORE() ---
          strm.next_out = put;
          strm.avail_out = left;
          strm.next_in = next;
          strm.avail_in = have;
          state.hold = hold;
          state.bits = bits;
          //---
          inffast(strm, _out);
          //--- LOAD() ---
          put = strm.next_out;
          output = strm.output;
          left = strm.avail_out;
          next = strm.next_in;
          input = strm.input;
          have = strm.avail_in;
          hold = state.hold;
          bits = state.bits;
          //---

          if (state.mode === TYPE) {
            state.back = -1;
          }
          break;
        }
        state.back = 0;
        for (;;) {
          here = state.lencode[hold & (1 << state.lenbits) - 1]; /*BITS(state.lenbits)*/
          here_bits = here >>> 24;
          here_op = here >>> 16 & 0xff;
          here_val = here & 0xffff;
          if (here_bits <= bits) {
            break;
          }
          //--- PULLBYTE() ---//
          if (have === 0) {
            break inf_leave;
          }
          have--;
          hold += input[next++] << bits;
          bits += 8;
          //---//
        }
        if (here_op && (here_op & 0xf0) === 0) {
          last_bits = here_bits;
          last_op = here_op;
          last_val = here_val;
          for (;;) {
            here = state.lencode[last_val + ((hold & (1 << last_bits + last_op) - 1 /*BITS(last.bits + last.op)*/) >> last_bits)];
            here_bits = here >>> 24;
            here_op = here >>> 16 & 0xff;
            here_val = here & 0xffff;
            if (last_bits + here_bits <= bits) {
              break;
            }
            //--- PULLBYTE() ---//
            if (have === 0) {
              break inf_leave;
            }
            have--;
            hold += input[next++] << bits;
            bits += 8;
            //---//
          }
          //--- DROPBITS(last.bits) ---//
          hold >>>= last_bits;
          bits -= last_bits;
          //---//
          state.back += last_bits;
        }
        //--- DROPBITS(here.bits) ---//
        hold >>>= here_bits;
        bits -= here_bits;
        //---//
        state.back += here_bits;
        state.length = here_val;
        if (here_op === 0) {
          //Tracevv((stderr, here.val >= 0x20 && here.val < 0x7f ?
          //        "inflate:         literal '%c'\n" :
          //        "inflate:         literal 0x%02x\n", here.val));
          state.mode = LIT;
          break;
        }
        if (here_op & 32) {
          //Tracevv((stderr, "inflate:         end of block\n"));
          state.back = -1;
          state.mode = TYPE;
          break;
        }
        if (here_op & 64) {
          strm.msg = 'invalid literal/length code';
          state.mode = BAD;
          break;
        }
        state.extra = here_op & 15;
        state.mode = LENEXT;
      /* falls through */
      case LENEXT:
        if (state.extra) {
          //=== NEEDBITS(state.extra);
          n = state.extra;
          while (bits < n) {
            if (have === 0) {
              break inf_leave;
            }
            have--;
            hold += input[next++] << bits;
            bits += 8;
          }
          //===//
          state.length += hold & (1 << state.extra) - 1 /*BITS(state.extra)*/;
          //--- DROPBITS(state.extra) ---//
          hold >>>= state.extra;
          bits -= state.extra;
          //---//
          state.back += state.extra;
        }
        //Tracevv((stderr, "inflate:         length %u\n", state.length));
        state.was = state.length;
        state.mode = DIST;
      /* falls through */
      case DIST:
        for (;;) {
          here = state.distcode[hold & (1 << state.distbits) - 1]; /*BITS(state.distbits)*/
          here_bits = here >>> 24;
          here_op = here >>> 16 & 0xff;
          here_val = here & 0xffff;
          if (here_bits <= bits) {
            break;
          }
          //--- PULLBYTE() ---//
          if (have === 0) {
            break inf_leave;
          }
          have--;
          hold += input[next++] << bits;
          bits += 8;
          //---//
        }
        if ((here_op & 0xf0) === 0) {
          last_bits = here_bits;
          last_op = here_op;
          last_val = here_val;
          for (;;) {
            here = state.distcode[last_val + ((hold & (1 << last_bits + last_op) - 1 /*BITS(last.bits + last.op)*/) >> last_bits)];
            here_bits = here >>> 24;
            here_op = here >>> 16 & 0xff;
            here_val = here & 0xffff;
            if (last_bits + here_bits <= bits) {
              break;
            }
            //--- PULLBYTE() ---//
            if (have === 0) {
              break inf_leave;
            }
            have--;
            hold += input[next++] << bits;
            bits += 8;
            //---//
          }
          //--- DROPBITS(last.bits) ---//
          hold >>>= last_bits;
          bits -= last_bits;
          //---//
          state.back += last_bits;
        }
        //--- DROPBITS(here.bits) ---//
        hold >>>= here_bits;
        bits -= here_bits;
        //---//
        state.back += here_bits;
        if (here_op & 64) {
          strm.msg = 'invalid distance code';
          state.mode = BAD;
          break;
        }
        state.offset = here_val;
        state.extra = here_op & 15;
        state.mode = DISTEXT;
      /* falls through */
      case DISTEXT:
        if (state.extra) {
          //=== NEEDBITS(state.extra);
          n = state.extra;
          while (bits < n) {
            if (have === 0) {
              break inf_leave;
            }
            have--;
            hold += input[next++] << bits;
            bits += 8;
          }
          //===//
          state.offset += hold & (1 << state.extra) - 1 /*BITS(state.extra)*/;
          //--- DROPBITS(state.extra) ---//
          hold >>>= state.extra;
          bits -= state.extra;
          //---//
          state.back += state.extra;
        }
        //#ifdef INFLATE_STRICT
        if (state.offset > state.dmax) {
          strm.msg = 'invalid distance too far back';
          state.mode = BAD;
          break;
        }
        //#endif
        //Tracevv((stderr, "inflate:         distance %u\n", state.offset));
        state.mode = MATCH;
      /* falls through */
      case MATCH:
        if (left === 0) {
          break inf_leave;
        }
        copy = _out - left;
        if (state.offset > copy) {
          /* copy from window */
          copy = state.offset - copy;
          if (copy > state.whave) {
            if (state.sane) {
              strm.msg = 'invalid distance too far back';
              state.mode = BAD;
              break;
            }
            // (!) This block is disabled in zlib defaults,
            // don't enable it for binary compatibility
            //#ifdef INFLATE_ALLOW_INVALID_DISTANCE_TOOFAR_ARRR
            //          Trace((stderr, "inflate.c too far\n"));
            //          copy -= state.whave;
            //          if (copy > state.length) { copy = state.length; }
            //          if (copy > left) { copy = left; }
            //          left -= copy;
            //          state.length -= copy;
            //          do {
            //            output[put++] = 0;
            //          } while (--copy);
            //          if (state.length === 0) { state.mode = LEN; }
            //          break;
            //#endif
          }
          if (copy > state.wnext) {
            copy -= state.wnext;
            from = state.wsize - copy;
          } else {
            from = state.wnext - copy;
          }
          if (copy > state.length) {
            copy = state.length;
          }
          from_source = state.window;
        } else {
          /* copy from output */
          from_source = output;
          from = put - state.offset;
          copy = state.length;
        }
        if (copy > left) {
          copy = left;
        }
        left -= copy;
        state.length -= copy;
        do {
          output[put++] = from_source[from++];
        } while (--copy);
        if (state.length === 0) {
          state.mode = LEN;
        }
        break;
      case LIT:
        if (left === 0) {
          break inf_leave;
        }
        output[put++] = state.length;
        left--;
        state.mode = LEN;
        break;
      case CHECK:
        if (state.wrap) {
          //=== NEEDBITS(32);
          while (bits < 32) {
            if (have === 0) {
              break inf_leave;
            }
            have--;
            // Use '|' instead of '+' to make sure that result is signed
            hold |= input[next++] << bits;
            bits += 8;
          }
          //===//
          _out -= left;
          strm.total_out += _out;
          state.total += _out;
          if (state.wrap & 4 && _out) {
            strm.adler = state.check = /*UPDATE_CHECK(state.check, put - _out, _out);*/
            state.flags ? crc32_1(state.check, output, _out, put - _out) : adler32_1(state.check, output, _out, put - _out);
          }
          _out = left;
          // NB: crc32 stored as signed 32-bit int, zswap32 returns signed too
          if (state.wrap & 4 && (state.flags ? hold : zswap32(hold)) !== state.check) {
            strm.msg = 'incorrect data check';
            state.mode = BAD;
            break;
          }
          //=== INITBITS();
          hold = 0;
          bits = 0;
          //===//
          //Tracev((stderr, "inflate:   check matches trailer\n"));
        }
        state.mode = LENGTH;
      /* falls through */
      case LENGTH:
        if (state.wrap && state.flags) {
          //=== NEEDBITS(32);
          while (bits < 32) {
            if (have === 0) {
              break inf_leave;
            }
            have--;
            hold += input[next++] << bits;
            bits += 8;
          }
          //===//
          if (state.wrap & 4 && hold !== (state.total & 0xffffffff)) {
            strm.msg = 'incorrect length check';
            state.mode = BAD;
            break;
          }
          //=== INITBITS();
          hold = 0;
          bits = 0;
          //===//
          //Tracev((stderr, "inflate:   length matches trailer\n"));
        }
        state.mode = DONE;
      /* falls through */
      case DONE:
        ret = Z_STREAM_END$1;
        break inf_leave;
      case BAD:
        ret = Z_DATA_ERROR$1;
        break inf_leave;
      case MEM:
        return Z_MEM_ERROR$1;
      case SYNC:
      /* falls through */
      default:
        return Z_STREAM_ERROR$1;
    }
  }

  // inf_leave <- here is real place for "goto inf_leave", emulated via "break inf_leave"

  /*
     Return from inflate(), updating the total counts and the check value.
     If there was no progress during the inflate() call, return a buffer
     error.  Call updatewindow() to create and/or update the window state.
     Note: a memory error from inflate() is non-recoverable.
   */

  //--- RESTORE() ---
  strm.next_out = put;
  strm.avail_out = left;
  strm.next_in = next;
  strm.avail_in = have;
  state.hold = hold;
  state.bits = bits;
  //---

  if (state.wsize || _out !== strm.avail_out && state.mode < BAD && (state.mode < CHECK || flush !== Z_FINISH$1)) {
    if (updatewindow(strm, strm.output, strm.next_out, _out - strm.avail_out)) ;
  }
  _in -= strm.avail_in;
  _out -= strm.avail_out;
  strm.total_in += _in;
  strm.total_out += _out;
  state.total += _out;
  if (state.wrap & 4 && _out) {
    strm.adler = state.check = /*UPDATE_CHECK(state.check, strm.next_out - _out, _out);*/
    state.flags ? crc32_1(state.check, output, _out, strm.next_out - _out) : adler32_1(state.check, output, _out, strm.next_out - _out);
  }
  strm.data_type = state.bits + (state.last ? 64 : 0) + (state.mode === TYPE ? 128 : 0) + (state.mode === LEN_ || state.mode === COPY_ ? 256 : 0);
  if ((_in === 0 && _out === 0 || flush === Z_FINISH$1) && ret === Z_OK$1) {
    ret = Z_BUF_ERROR;
  }
  return ret;
};
const inflateEnd = strm => {
  if (inflateStateCheck(strm)) {
    return Z_STREAM_ERROR$1;
  }
  let state = strm.state;
  if (state.window) {
    state.window = null;
  }
  strm.state = null;
  return Z_OK$1;
};
const inflateGetHeader = (strm, head) => {
  /* check state */
  if (inflateStateCheck(strm)) {
    return Z_STREAM_ERROR$1;
  }
  const state = strm.state;
  if ((state.wrap & 2) === 0) {
    return Z_STREAM_ERROR$1;
  }

  /* save header structure */
  state.head = head;
  head.done = false;
  return Z_OK$1;
};
const inflateSetDictionary = (strm, dictionary) => {
  const dictLength = dictionary.length;
  let state;
  let dictid;
  let ret;

  /* check state */
  if (inflateStateCheck(strm)) {
    return Z_STREAM_ERROR$1;
  }
  state = strm.state;
  if (state.wrap !== 0 && state.mode !== DICT) {
    return Z_STREAM_ERROR$1;
  }

  /* check for correct dictionary identifier */
  if (state.mode === DICT) {
    dictid = 1; /* adler32(0, null, 0)*/
    /* dictid = adler32(dictid, dictionary, dictLength); */
    dictid = adler32_1(dictid, dictionary, dictLength, 0);
    if (dictid !== state.check) {
      return Z_DATA_ERROR$1;
    }
  }
  /* copy dictionary to window using updatewindow(), which will amend the
   existing dictionary if appropriate */
  ret = updatewindow(strm, dictionary, dictLength, dictLength);
  if (ret) {
    state.mode = MEM;
    return Z_MEM_ERROR$1;
  }
  state.havedict = 1;
  // Tracev((stderr, "inflate:   dictionary set\n"));
  return Z_OK$1;
};
var inflateReset_1 = inflateReset;
var inflateReset2_1 = inflateReset2;
var inflateResetKeep_1 = inflateResetKeep;
var inflateInit_1 = inflateInit;
var inflateInit2_1 = inflateInit2;
var inflate_2$1 = inflate$2;
var inflateEnd_1 = inflateEnd;
var inflateGetHeader_1 = inflateGetHeader;
var inflateSetDictionary_1 = inflateSetDictionary;
var inflateInfo = 'pako inflate (from Nodeca project)';

/* Not implemented
module.exports.inflateCodesUsed = inflateCodesUsed;
module.exports.inflateCopy = inflateCopy;
module.exports.inflateGetDictionary = inflateGetDictionary;
module.exports.inflateMark = inflateMark;
module.exports.inflatePrime = inflatePrime;
module.exports.inflateSync = inflateSync;
module.exports.inflateSyncPoint = inflateSyncPoint;
module.exports.inflateUndermine = inflateUndermine;
module.exports.inflateValidate = inflateValidate;
*/

var inflate_1$2 = {
  inflateReset: inflateReset_1,
  inflateReset2: inflateReset2_1,
  inflateResetKeep: inflateResetKeep_1,
  inflateInit: inflateInit_1,
  inflateInit2: inflateInit2_1,
  inflate: inflate_2$1,
  inflateEnd: inflateEnd_1,
  inflateGetHeader: inflateGetHeader_1,
  inflateSetDictionary: inflateSetDictionary_1,
  inflateInfo: inflateInfo
};

// (C) 1995-2013 Jean-loup Gailly and Mark Adler
// (C) 2014-2017 Vitaly Puzrin and Andrey Tupitsin
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//   claim that you wrote the original software. If you use this software
//   in a product, an acknowledgment in the product documentation would be
//   appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//   misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

function GZheader() {
  /* true if compressed data believed to be text */
  this.text = 0;
  /* modification time */
  this.time = 0;
  /* extra flags (not used when writing a gzip file) */
  this.xflags = 0;
  /* operating system */
  this.os = 0;
  /* pointer to extra field or Z_NULL if none */
  this.extra = null;
  /* extra field length (valid if extra != Z_NULL) */
  this.extra_len = 0; // Actually, we don't need it in JS,
  // but leave for few code modifications

  //
  // Setup limits is not necessary because in js we should not preallocate memory
  // for inflate use constant limit in 65536 bytes
  //

  /* space at extra (only when reading header) */
  // this.extra_max  = 0;
  /* pointer to zero-terminated file name or Z_NULL */
  this.name = '';
  /* space at name (only when reading header) */
  // this.name_max   = 0;
  /* pointer to zero-terminated comment or Z_NULL */
  this.comment = '';
  /* space at comment (only when reading header) */
  // this.comm_max   = 0;
  /* true if there was or will be a header crc */
  this.hcrc = 0;
  /* true when done reading gzip header (not used when writing a gzip file) */
  this.done = false;
}
var gzheader = GZheader;
const toString = Object.prototype.toString;

/* Public constants ==========================================================*/
/* ===========================================================================*/

const {
  Z_NO_FLUSH,
  Z_FINISH,
  Z_OK,
  Z_STREAM_END,
  Z_NEED_DICT,
  Z_STREAM_ERROR,
  Z_DATA_ERROR,
  Z_MEM_ERROR
} = constants$2;

/* ===========================================================================*/

/**
 * class Inflate
 *
 * Generic JS-style wrapper for zlib calls. If you don't need
 * streaming behaviour - use more simple functions: [[inflate]]
 * and [[inflateRaw]].
 **/

/* internal
 * inflate.chunks -> Array
 *
 * Chunks of output data, if [[Inflate#onData]] not overridden.
 **/

/**
 * Inflate.result -> Uint8Array|String
 *
 * Uncompressed result, generated by default [[Inflate#onData]]
 * and [[Inflate#onEnd]] handlers. Filled after you push last chunk
 * (call [[Inflate#push]] with `Z_FINISH` / `true` param).
 **/

/**
 * Inflate.err -> Number
 *
 * Error code after inflate finished. 0 (Z_OK) on success.
 * Should be checked if broken data possible.
 **/

/**
 * Inflate.msg -> String
 *
 * Error message, if [[Inflate.err]] != 0
 **/

/**
 * new Inflate(options)
 * - options (Object): zlib inflate options.
 *
 * Creates new inflator instance with specified params. Throws exception
 * on bad params. Supported options:
 *
 * - `windowBits`
 * - `dictionary`
 *
 * [http://zlib.net/manual.html#Advanced](http://zlib.net/manual.html#Advanced)
 * for more information on these.
 *
 * Additional options, for internal needs:
 *
 * - `chunkSize` - size of generated data chunks (16K by default)
 * - `raw` (Boolean) - do raw inflate
 * - `to` (String) - if equal to 'string', then result will be converted
 *   from utf8 to utf16 (javascript) string. When string output requested,
 *   chunk length can differ from `chunkSize`, depending on content.
 *
 * By default, when no options set, autodetect deflate/gzip data format via
 * wrapper header.
 *
 * ##### Example:
 *
 * ```javascript
 * const pako = require('pako')
 * const chunk1 = new Uint8Array([1,2,3,4,5,6,7,8,9])
 * const chunk2 = new Uint8Array([10,11,12,13,14,15,16,17,18,19]);
 *
 * const inflate = new pako.Inflate({ level: 3});
 *
 * inflate.push(chunk1, false);
 * inflate.push(chunk2, true);  // true -> last chunk
 *
 * if (inflate.err) { throw new Error(inflate.err); }
 *
 * console.log(inflate.result);
 * ```
 **/
function Inflate$1(options) {
  this.options = common.assign({
    chunkSize: 1024 * 64,
    windowBits: 15,
    to: ''
  }, options || {});
  const opt = this.options;

  // Force window size for `raw` data, if not set directly,
  // because we have no header for autodetect.
  if (opt.raw && opt.windowBits >= 0 && opt.windowBits < 16) {
    opt.windowBits = -opt.windowBits;
    if (opt.windowBits === 0) {
      opt.windowBits = -15;
    }
  }

  // If `windowBits` not defined (and mode not raw) - set autodetect flag for gzip/deflate
  if (opt.windowBits >= 0 && opt.windowBits < 16 && !(options && options.windowBits)) {
    opt.windowBits += 32;
  }

  // Gzip header has no info about windows size, we can do autodetect only
  // for deflate. So, if window size not set, force it to max when gzip possible
  if (opt.windowBits > 15 && opt.windowBits < 48) {
    // bit 3 (16) -> gzipped data
    // bit 4 (32) -> autodetect gzip/deflate
    if ((opt.windowBits & 15) === 0) {
      opt.windowBits |= 15;
    }
  }
  this.err = 0; // error code, if happens (0 = Z_OK)
  this.msg = ''; // error message
  this.ended = false; // used to avoid multiple onEnd() calls
  this.chunks = []; // chunks of compressed data

  this.strm = new zstream();
  this.strm.avail_out = 0;
  let status = inflate_1$2.inflateInit2(this.strm, opt.windowBits);
  if (status !== Z_OK) {
    throw new Error(messages[status]);
  }
  this.header = new gzheader();
  inflate_1$2.inflateGetHeader(this.strm, this.header);

  // Setup dictionary
  if (opt.dictionary) {
    // Convert data if needed
    if (typeof opt.dictionary === 'string') {
      opt.dictionary = strings.string2buf(opt.dictionary);
    } else if (toString.call(opt.dictionary) === '[object ArrayBuffer]') {
      opt.dictionary = new Uint8Array(opt.dictionary);
    }
    if (opt.raw) {
      //In raw mode we need to set the dictionary early
      status = inflate_1$2.inflateSetDictionary(this.strm, opt.dictionary);
      if (status !== Z_OK) {
        throw new Error(messages[status]);
      }
    }
  }
}

/**
 * Inflate#push(data[, flush_mode]) -> Boolean
 * - data (Uint8Array|ArrayBuffer): input data
 * - flush_mode (Number|Boolean): 0..6 for corresponding Z_NO_FLUSH..Z_TREE
 *   flush modes. See constants. Skipped or `false` means Z_NO_FLUSH,
 *   `true` means Z_FINISH.
 *
 * Sends input data to inflate pipe, generating [[Inflate#onData]] calls with
 * new output chunks. Returns `true` on success. If end of stream detected,
 * [[Inflate#onEnd]] will be called.
 *
 * `flush_mode` is not needed for normal operation, because end of stream
 * detected automatically. You may try to use it for advanced things, but
 * this functionality was not tested.
 *
 * On fail call [[Inflate#onEnd]] with error code and return false.
 *
 * ##### Example
 *
 * ```javascript
 * push(chunk, false); // push one of data chunks
 * ...
 * push(chunk, true);  // push last chunk
 * ```
 **/
Inflate$1.prototype.push = function (data, flush_mode) {
  const strm = this.strm;
  const chunkSize = this.options.chunkSize;
  const dictionary = this.options.dictionary;
  let status, _flush_mode, last_avail_out;
  if (this.ended) return false;
  if (flush_mode === ~~flush_mode) _flush_mode = flush_mode;else _flush_mode = flush_mode === true ? Z_FINISH : Z_NO_FLUSH;

  // Convert data if needed
  if (toString.call(data) === '[object ArrayBuffer]') {
    strm.input = new Uint8Array(data);
  } else {
    strm.input = data;
  }
  strm.next_in = 0;
  strm.avail_in = strm.input.length;
  for (;;) {
    if (strm.avail_out === 0) {
      strm.output = new Uint8Array(chunkSize);
      strm.next_out = 0;
      strm.avail_out = chunkSize;
    }
    status = inflate_1$2.inflate(strm, _flush_mode);
    if (status === Z_NEED_DICT && dictionary) {
      status = inflate_1$2.inflateSetDictionary(strm, dictionary);
      if (status === Z_OK) {
        status = inflate_1$2.inflate(strm, _flush_mode);
      } else if (status === Z_DATA_ERROR) {
        // Replace code with more verbose
        status = Z_NEED_DICT;
      }
    }

    // Skip snyc markers if more data follows and not raw mode
    while (strm.avail_in > 0 && status === Z_STREAM_END && strm.state.wrap > 0 && data[strm.next_in] !== 0) {
      inflate_1$2.inflateReset(strm);
      status = inflate_1$2.inflate(strm, _flush_mode);
    }
    switch (status) {
      case Z_STREAM_ERROR:
      case Z_DATA_ERROR:
      case Z_NEED_DICT:
      case Z_MEM_ERROR:
        this.onEnd(status);
        this.ended = true;
        return false;
    }

    // Remember real `avail_out` value, because we may patch out buffer content
    // to align utf8 strings boundaries.
    last_avail_out = strm.avail_out;
    if (strm.next_out) {
      if (strm.avail_out === 0 || status === Z_STREAM_END) {
        if (this.options.to === 'string') {
          let next_out_utf8 = strings.utf8border(strm.output, strm.next_out);
          let tail = strm.next_out - next_out_utf8;
          let utf8str = strings.buf2string(strm.output, next_out_utf8);

          // move tail & realign counters
          strm.next_out = tail;
          strm.avail_out = chunkSize - tail;
          if (tail) strm.output.set(strm.output.subarray(next_out_utf8, next_out_utf8 + tail), 0);
          this.onData(utf8str);
        } else {
          this.onData(strm.output.length === strm.next_out ? strm.output : strm.output.subarray(0, strm.next_out));
        }
      }
    }

    // Must repeat iteration if out buffer is full
    if (status === Z_OK && last_avail_out === 0) continue;

    // Finalize if end of stream reached.
    if (status === Z_STREAM_END) {
      status = inflate_1$2.inflateEnd(this.strm);
      this.onEnd(status);
      this.ended = true;
      return true;
    }
    if (strm.avail_in === 0) break;
  }
  return true;
};

/**
 * Inflate#onData(chunk) -> Void
 * - chunk (Uint8Array|String): output data. When string output requested,
 *   each chunk will be string.
 *
 * By default, stores data blocks in `chunks[]` property and glue
 * those in `onEnd`. Override this handler, if you need another behaviour.
 **/
Inflate$1.prototype.onData = function (chunk) {
  this.chunks.push(chunk);
};

/**
 * Inflate#onEnd(status) -> Void
 * - status (Number): inflate status. 0 (Z_OK) on success,
 *   other if not.
 *
 * Called either after you tell inflate that the input stream is
 * complete (Z_FINISH). By default - join collected chunks,
 * free memory and fill `results` / `err` properties.
 **/
Inflate$1.prototype.onEnd = function (status) {
  // On success - join
  if (status === Z_OK) {
    if (this.options.to === 'string') {
      this.result = this.chunks.join('');
    } else {
      this.result = common.flattenChunks(this.chunks);
    }
  }
  this.chunks = [];
  this.err = status;
  this.msg = this.strm.msg;
};

/**
 * ungzip(data[, options]) -> Uint8Array|String
 * - data (Uint8Array|ArrayBuffer): input data to decompress.
 * - options (Object): zlib inflate options.
 *
 * Just shortcut to [[inflate]], because it autodetects format
 * by header.content. Done for convenience.
 **/

var Inflate_1$1 = Inflate$1;
var inflate_1$1 = {
  Inflate: Inflate_1$1};
const {
  deflate} = deflate_1$1;
const {
  Inflate} = inflate_1$1;
var deflate_1 = deflate;
var Inflate_1 = Inflate;

/* global SerialPort, ParityType, FlowControlType */
/**
 * Wrapper class around Webserial API to communicate with the serial device.
 * @param {typeof import("w3c-web-serial").SerialPort} device - Requested device prompted by the browser.
 *
 * ```
 * const port = await navigator.serial.requestPort();
 * ```
 */
class Transport {
  constructor(device, tracing = false, enableSlipReader = true) {
    this.device = device;
    this.tracing = tracing;
    this.slipReaderEnabled = false;
    this.baudrate = 0;
    this.traceLog = "";
    this.lastTraceTime = Date.now();
    this.buffer = new Uint8Array(0);
    this.SLIP_END = 0xc0;
    this.SLIP_ESC = 0xdb;
    this.SLIP_ESC_END = 0xdc;
    this.SLIP_ESC_ESC = 0xdd;
    this._DTR_state = false;
    this.slipReaderEnabled = enableSlipReader;
  }
  /**
   * Request the serial device vendor ID and Product ID as string.
   * @returns {string} Return the device VendorID and ProductID from SerialPortInfo as formatted string.
   */
  getInfo() {
    const info = this.device.getInfo();
    return info.usbVendorId && info.usbProductId ? `WebSerial VendorID 0x${info.usbVendorId.toString(16)} ProductID 0x${info.usbProductId.toString(16)}` : "";
  }
  /**
   * Request the serial device product id from SerialPortInfo.
   * @returns {number | undefined} Return the product ID.
   */
  getPid() {
    return this.device.getInfo().usbProductId;
  }
  /**
   * Format received or sent data for tracing output.
   * @param {string} message Message to format as trace line.
   */
  trace(message) {
    const delta = Date.now() - this.lastTraceTime;
    const prefix = `TRACE ${delta.toFixed(3)}`;
    const traceMessage = `${prefix} ${message}`;
    console.log(traceMessage);
    this.traceLog += traceMessage + "\n";
  }
  async returnTrace() {
    try {
      await navigator.clipboard.writeText(this.traceLog);
      console.log("Text copied to clipboard!");
    } catch (err) {
      console.error("Failed to copy text:", err);
    }
  }
  hexify(s) {
    return Array.from(s).map(byte => byte.toString(16).padStart(2, "0")).join("").padEnd(16, " ");
  }
  hexConvert(uint8Array, autoSplit = true) {
    if (autoSplit && uint8Array.length > 16) {
      let result = "";
      let s = uint8Array;
      while (s.length > 0) {
        const line = s.slice(0, 16);
        const asciiLine = String.fromCharCode(...line).split("").map(c => c === " " || c >= " " && c <= "~" && c !== "  " ? c : ".").join("");
        s = s.slice(16);
        result += `\n    ${this.hexify(line.slice(0, 8))} ${this.hexify(line.slice(8))} | ${asciiLine}`;
      }
      return result;
    } else {
      return this.hexify(uint8Array);
    }
  }
  /**
   * Format data packet using the Serial Line Internet Protocol (SLIP).
   * @param {Uint8Array} data Binary unsigned 8 bit array data to format.
   * @returns {Uint8Array} Formatted unsigned 8 bit data array.
   */
  slipWriter(data) {
    const outData = [];
    outData.push(0xc0);
    for (let i = 0; i < data.length; i++) {
      if (data[i] === 0xdb) {
        outData.push(0xdb, 0xdd);
      } else if (data[i] === 0xc0) {
        outData.push(0xdb, 0xdc);
      } else {
        outData.push(data[i]);
      }
    }
    outData.push(0xc0);
    return new Uint8Array(outData);
  }
  /**
   * Write binary data to device using the WebSerial device writable stream.
   * @param {Uint8Array} data 8 bit unsigned data array to write to device.
   */
  async write(data) {
    const outData = this.slipWriter(data);
    if (this.device.writable) {
      const writer = this.device.writable.getWriter();
      if (this.tracing) {
        console.log("Write bytes");
        this.trace(`Write ${outData.length} bytes: ${this.hexConvert(outData)}`);
      }
      await writer.write(outData);
      writer.releaseLock();
    }
  }
  /**
   * Append a buffer array after another buffer array
   * @param {Uint8Array} arr1 - First array buffer.
   * @param {Uint8Array} arr2 - magic hex number to select ROM.
   * @returns {Uint8Array} Return a 8 bit unsigned array.
   */
  appendArray(arr1, arr2) {
    const combined = new Uint8Array(arr1.length + arr2.length);
    combined.set(arr1);
    combined.set(arr2, arr1.length);
    return combined;
  }
  // Asynchronous generator to yield incoming data chunks
  async *readLoop(timeout) {
    if (!this.reader) return;
    try {
      while (true) {
        const timeoutPromise = new Promise((_, reject) => setTimeout(() => reject(new Error("Read timeout exceeded")), timeout));
        // Await the race between the timeout and the reader read
        const result = await Promise.race([this.reader.read(), timeoutPromise]);
        // If a timeout occurs, result will be null; otherwise, it will have { value, done }
        if (result === null) break;
        const {
          value,
          done
        } = result;
        if (done || !value) break;
        yield value; // Yield each data chunk
      }
    } catch (error) {
      console.error("Error reading from serial port:", error);
    } finally {
      this.buffer = new Uint8Array(0);
    }
  }
  // Read a specific number of bytes
  async newRead(numBytes, timeout) {
    if (this.buffer.length >= numBytes) {
      const output = this.buffer.slice(0, numBytes);
      this.buffer = this.buffer.slice(numBytes); // Remove the returned data from buffer
      return output;
    }
    while (this.buffer.length < numBytes) {
      const readLoop = this.readLoop(timeout);
      const {
        value,
        done
      } = await readLoop.next();
      if (done || !value) {
        break;
      }
      // Append the newly read data to the buffer
      this.buffer = this.appendArray(this.buffer, value);
    }
    // Return as much data as possible
    const output = this.buffer.slice(0, numBytes);
    this.buffer = this.buffer.slice(numBytes);
    return output;
  }
  async flushInput() {
    var _a;
    if (this.reader && !(await this.reader.closed)) {
      await this.reader.cancel();
      this.reader.releaseLock();
      this.reader = (_a = this.device.readable) === null || _a === void 0 ? void 0 : _a.getReader();
    }
  }
  async flushOutput() {
    var _a, _b;
    this.buffer = new Uint8Array(0);
    await ((_a = this.device.writable) === null || _a === void 0 ? void 0 : _a.getWriter().close());
    (_b = this.device.writable) === null || _b === void 0 ? void 0 : _b.getWriter().releaseLock();
  }
  // `inWaiting` returns the count of bytes in the buffer
  inWaiting() {
    return this.buffer.length;
  }
  /**
   * Detect if the data read from device is a Fatal or Guru meditation error.
   * @param {Uint8Array} input Data read from device
   */
  detectPanicHandler(input) {
    const guruMeditationRegex = /G?uru Meditation Error: (?:Core \d panic'ed \(([a-zA-Z ]*)\))?/;
    const fatalExceptionRegex = /F?atal exception \(\d+\): (?:([a-zA-Z ]*)?.*epc)?/;
    const inputString = new TextDecoder("utf-8").decode(input);
    const match = inputString.match(guruMeditationRegex) || inputString.match(fatalExceptionRegex);
    if (match) {
      const cause = match[1] || match[2];
      const msg = `Guru Meditation Error detected${cause ? ` (${cause})` : ""}`;
      throw new Error(msg);
    }
  }
  /**
   * Take a data array and return the first well formed packet after
   * replacing the escape sequence. Reads at least 8 bytes.
   * @param {number} timeout Timeout read data.
   * @yields {Uint8Array} Formatted packet using SLIP escape sequences.
   */
  async *read(timeout) {
    var _a;
    if (!this.reader) {
      this.reader = (_a = this.device.readable) === null || _a === void 0 ? void 0 : _a.getReader();
    }
    let partialPacket = null;
    let isEscaping = false;
    let successfulSlip = false;
    while (true) {
      const waitingBytes = this.inWaiting();
      const readBytes = await this.newRead(waitingBytes > 0 ? waitingBytes : 1, timeout);
      if (!readBytes || readBytes.length === 0) {
        const msg = partialPacket === null ? successfulSlip ? "Serial data stream stopped: Possible serial noise or corruption." : "No serial data received." : `Packet content transfer stopped`;
        this.trace(msg);
        throw new Error(msg);
      }
      this.trace(`Read ${readBytes.length} bytes: ${this.hexConvert(readBytes)}`);
      let i = 0; // Track position in readBytes
      while (i < readBytes.length) {
        const byte = readBytes[i++];
        if (partialPacket === null) {
          if (byte === this.SLIP_END) {
            partialPacket = new Uint8Array(0); // Start of a new packet
          } else {
            this.trace(`Read invalid data: ${this.hexConvert(readBytes)}`);
            const remainingData = await this.newRead(this.inWaiting(), timeout);
            this.trace(`Remaining data in serial buffer: ${this.hexConvert(remainingData)}`);
            this.detectPanicHandler(new Uint8Array([...readBytes, ...(remainingData || [])]));
            throw new Error(`Invalid head of packet (0x${byte.toString(16)}): Possible serial noise or corruption.`);
          }
        } else if (isEscaping) {
          isEscaping = false;
          if (byte === this.SLIP_ESC_END) {
            partialPacket = this.appendArray(partialPacket, new Uint8Array([this.SLIP_END]));
          } else if (byte === this.SLIP_ESC_ESC) {
            partialPacket = this.appendArray(partialPacket, new Uint8Array([this.SLIP_ESC]));
          } else {
            this.trace(`Read invalid data: ${this.hexConvert(readBytes)}`);
            const remainingData = await this.newRead(this.inWaiting(), timeout);
            this.trace(`Remaining data in serial buffer: ${this.hexConvert(remainingData)}`);
            this.detectPanicHandler(new Uint8Array([...readBytes, ...(remainingData || [])]));
            throw new Error(`Invalid SLIP escape (0xdb, 0x${byte.toString(16)})`);
          }
        } else if (byte === this.SLIP_ESC) {
          isEscaping = true;
        } else if (byte === this.SLIP_END) {
          this.trace(`Received full packet: ${this.hexConvert(partialPacket)}`);
          this.buffer = this.appendArray(this.buffer, readBytes.slice(i));
          yield partialPacket;
          partialPacket = null;
          successfulSlip = true;
        } else {
          partialPacket = this.appendArray(partialPacket, new Uint8Array([byte]));
        }
      }
    }
  }
  /**
   * Read from serial device without slip formatting.
   * @yields {Uint8Array} The next number in the Fibonacci sequence.
   */
  async *rawRead() {
    if (!this.reader) return;
    try {
      while (true) {
        const {
          value,
          done
        } = await this.reader.read();
        if (done || !value) break;
        if (this.tracing) {
          console.log("Raw Read bytes");
          this.trace(`Read ${value.length} bytes: ${this.hexConvert(value)}`);
        }
        yield value; // Yield each data chunk
      }
    } catch (error) {
      console.error("Error reading from serial port:", error);
    } finally {
      this.buffer = new Uint8Array(0);
    }
  }
  /**
   * Send the RequestToSend (RTS) signal to given state
   * # True for EN=LOW, chip in reset and False EN=HIGH, chip out of reset
   * @param {boolean} state Boolean state to set the signal
   */
  async setRTS(state) {
    await this.device.setSignals({
      requestToSend: state
    });
    // # Work-around for adapters on Windows using the usbser.sys driver:
    // # generate a dummy change to DTR so that the set-control-line-state
    // # request is sent with the updated RTS state and the same DTR state
    // Referenced to esptool.py
    await this.setDTR(this._DTR_state);
  }
  /**
   * Send the dataTerminalReady (DTS) signal to given state
   * # True for IO0=LOW, chip in reset and False IO0=HIGH
   * @param {boolean} state Boolean state to set the signal
   */
  async setDTR(state) {
    this._DTR_state = state;
    await this.device.setSignals({
      dataTerminalReady: state
    });
  }
  /**
   * Connect to serial device using the Webserial open method.
   * @param {number} baud Number baud rate for serial connection. Default is 115200.
   * @param {typeof import("w3c-web-serial").SerialOptions} serialOptions Serial Options for WebUSB SerialPort class.
   */
  async connect(baud = 115200, serialOptions = {}) {
    var _a;
    await this.device.open({
      baudRate: baud,
      dataBits: serialOptions === null || serialOptions === void 0 ? void 0 : serialOptions.dataBits,
      stopBits: serialOptions === null || serialOptions === void 0 ? void 0 : serialOptions.stopBits,
      bufferSize: serialOptions === null || serialOptions === void 0 ? void 0 : serialOptions.bufferSize,
      parity: serialOptions === null || serialOptions === void 0 ? void 0 : serialOptions.parity,
      flowControl: serialOptions === null || serialOptions === void 0 ? void 0 : serialOptions.flowControl
    });
    this.baudrate = baud;
    this.reader = (_a = this.device.readable) === null || _a === void 0 ? void 0 : _a.getReader();
  }
  async sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
  }
  /**
   * Wait for a given timeout ms for serial device unlock.
   * @param {number} timeout Timeout time in milliseconds (ms) to sleep
   */
  async waitForUnlock(timeout) {
    while (this.device.readable && this.device.readable.locked || this.device.writable && this.device.writable.locked) {
      await this.sleep(timeout);
    }
  }
  /**
   * Disconnect from serial device by running SerialPort.close() after streams unlock.
   */
  async disconnect() {
    var _a, _b;
    if ((_a = this.device.readable) === null || _a === void 0 ? void 0 : _a.locked) {
      await ((_b = this.reader) === null || _b === void 0 ? void 0 : _b.cancel());
    }
    await this.waitForUnlock(400);
    await this.device.close();
    this.reader = undefined;
  }
}

/**
 * Sleep for ms milliseconds
 * @param {number} ms Milliseconds to wait
 * @returns {Promise<void>}
 */
function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}
/**
 * Execute a classic set of commands that will reset the chip.
 *
 * Commands (e.g. R0) are defined by a code (R) and an argument (0).
 *
 * The commands are:
 *
 * D: setDTR - 1=True / 0=False
 *
 * R: setRTS - 1=True / 0=False
 *
 * W: Wait (time delay) - positive integer number (miliseconds)
 *
 * "D0|R1|W100|D1|R0|W50|D0" represents the classic reset strategy
 * @param {Transport} transport Transport class to perform serial communication.
 * @param {number} resetDelay Delay in milliseconds for reset.
 */
class ClassicReset {
  constructor(transport, resetDelay) {
    this.resetDelay = resetDelay;
    this.transport = transport;
  }
  async reset() {
    await this.transport.setDTR(false);
    await this.transport.setRTS(true);
    await sleep(100);
    await this.transport.setDTR(true);
    await this.transport.setRTS(false);
    await sleep(this.resetDelay);
    await this.transport.setDTR(false);
  }
}
/**
 * Execute a set of commands for USB JTAG serial reset.
 *
 * Commands (e.g. R0) are defined by a code (R) and an argument (0).
 *
 * The commands are:
 *
 * D: setDTR - 1=True / 0=False
 *
 * R: setRTS - 1=True / 0=False
 *
 * W: Wait (time delay) - positive integer number (miliseconds)
 * @param {Transport} transport Transport class to perform serial communication.
 */
class UsbJtagSerialReset {
  constructor(transport) {
    this.transport = transport;
  }
  async reset() {
    await this.transport.setRTS(false);
    await this.transport.setDTR(false);
    await sleep(100);
    await this.transport.setDTR(true);
    await this.transport.setRTS(false);
    await sleep(100);
    await this.transport.setRTS(true);
    await this.transport.setDTR(false);
    await this.transport.setRTS(true);
    await sleep(100);
    await this.transport.setRTS(false);
    await this.transport.setDTR(false);
  }
}
/**
 * Execute a set of commands that will hard reset the chip.
 *
 * Commands (e.g. R0) are defined by a code (R) and an argument (0).
 *
 * The commands are:
 *
 * D: setDTR - 1=True / 0=False
 *
 * R: setRTS - 1=True / 0=False
 *
 * W: Wait (time delay) - positive integer number (miliseconds)
 * @param {Transport} transport Transport class to perform serial communication.
 * @param {boolean} usingUsbOtg is it using USB-OTG ?
 */
class HardReset {
  constructor(transport, usingUsbOtg = false) {
    this.transport = transport;
    this.usingUsbOtg = usingUsbOtg;
    this.transport = transport;
  }
  async reset() {
    if (this.usingUsbOtg) {
      await sleep(200);
      await this.transport.setRTS(false);
      await sleep(200);
    } else {
      await sleep(100);
      await this.transport.setRTS(false);
    }
  }
}
/**
 * Validate a sequence string based on the following format:
 *
 * Commands (e.g. R0) are defined by a code (R) and an argument (0).
 *
 * The commands are:
 *
 * D: setDTR - 1=True / 0=False
 *
 * R: setRTS - 1=True / 0=False
 *
 * W: Wait (time delay) - positive integer number (miliseconds)
 * @param {string} seqStr Sequence string to validate
 * @returns {boolean} Is the sequence string valid ?
 */
function validateCustomResetStringSequence(seqStr) {
  const commands = ["D", "R", "W"];
  const commandsList = seqStr.split("|");
  for (const cmd of commandsList) {
    const code = cmd[0];
    const arg = cmd.slice(1);
    if (!commands.includes(code)) {
      return false; // Invalid command code
    }
    if (code === "D" || code === "R") {
      if (arg !== "0" && arg !== "1") {
        return false; // Invalid argument for D and R commands
      }
    } else if (code === "W") {
      const delay = parseInt(arg);
      if (isNaN(delay) || delay <= 0) {
        return false; // Invalid argument for W command
      }
    }
  }
  return true; // All commands are valid
}
/**
 * Custom reset strategy defined with a string.
 *
 * The sequenceString input string consists of individual commands divided by "|".
 *
 * Commands (e.g. R0) are defined by a code (R) and an argument (0).
 *
 * The commands are:
 *
 * D: setDTR - 1=True / 0=False
 *
 * R: setRTS - 1=True / 0=False
 *
 * W: Wait (time delay) - positive integer number (miliseconds)
 *
 * "D0|R1|W100|D1|R0|W50|D0" represents the classic reset strategy
 * @param {Transport} transport Transport class to perform serial communication.
 * @param {string} sequenceString Custom string sequence for reset strategy
 */
class CustomReset {
  constructor(transport, sequenceString) {
    this.transport = transport;
    this.sequenceString = sequenceString;
    this.transport = transport;
  }
  async reset() {
    const resetDictionary = {
      D: async arg => await this.transport.setDTR(arg),
      R: async arg => await this.transport.setRTS(arg),
      W: async delay => await sleep(delay)
    };
    try {
      const isValidSequence = validateCustomResetStringSequence(this.sequenceString);
      if (!isValidSequence) {
        return;
      }
      const cmds = this.sequenceString.split("|");
      for (const cmd of cmds) {
        const cmdKey = cmd[0];
        const cmdVal = cmd.slice(1);
        if (cmdKey === "W") {
          await resetDictionary["W"](Number(cmdVal));
        } else if (cmdKey === "D" || cmdKey === "R") {
          await resetDictionary[cmdKey](cmdVal === "1");
        }
      }
    } catch (error) {
      throw new Error("Invalid custom reset sequence");
    }
  }
}

function getDefaultExportFromCjs (x) {
	return x && x.__esModule && Object.prototype.hasOwnProperty.call(x, 'default') ? x['default'] : x;
}

var atobBrowser;
var hasRequiredAtobBrowser;
function requireAtobBrowser() {
  if (hasRequiredAtobBrowser) return atobBrowser;
  hasRequiredAtobBrowser = 1;
  atobBrowser = function _atob(str) {
    return atob(str);
  };
  return atobBrowser;
}

var atobBrowserExports = requireAtobBrowser();
var atob$1 = /*@__PURE__*/getDefaultExportFromCjs(atobBrowserExports);

/**
 * Import flash stub json for the given chip name.
 * @param {string} chipName Name of chip to obtain flasher stub
 * @returns {Stub} Stub information and decoded text and data
 */
async function getStubJsonByChipName(chipName) {
  let jsonStub;
  switch (chipName) {
    case "ESP32":
      jsonStub = await import('./stub_flasher_32-PHI0-Qmn.js');
      break;
    case "ESP32-C2":
      jsonStub = await import('./stub_flasher_32c2-CugjyRPP.js');
      break;
    case "ESP32-C3":
      jsonStub = await import('./stub_flasher_32c3-CrnyAK_i.js');
      break;
    case "ESP32-C5":
      jsonStub = await import('./stub_flasher_32c5-DhLKI-Lk.js');
      break;
    case "ESP32-C6":
      jsonStub = await import('./stub_flasher_32c6-C79H4Aa2.js');
      break;
    case "ESP32-C61":
      jsonStub = await import('./stub_flasher_32c61-zzVeF46D.js');
      break;
    case "ESP32-H2":
      jsonStub = await import('./stub_flasher_32h2--77h8sVG.js');
      break;
    case "ESP32-P4":
      jsonStub = await import('./stub_flasher_32p4-DrG7L-q0.js');
      break;
    case "ESP32-S2":
      jsonStub = await import('./stub_flasher_32s2-DEBPokHr.js');
      break;
    case "ESP32-S3":
      jsonStub = await import('./stub_flasher_32s3-XeJkL6UV.js');
      break;
    case "ESP8266":
      jsonStub = await import('./stub_flasher_8266-B5tGkJJF.js');
      break;
  }
  if (jsonStub) {
    return {
      bss_start: jsonStub.bss_start,
      data: jsonStub.data,
      data_start: jsonStub.data_start,
      entry: jsonStub.entry,
      text: jsonStub.text,
      text_start: jsonStub.text_start,
      decodedData: decodeBase64Data(jsonStub.data),
      decodedText: decodeBase64Data(jsonStub.text)
    };
  }
  return;
}
/**
 * Convert a base 64 string to Uint8Array.
 * @param {string} dataStr Base64 String to decode
 * @returns {Uint8Array} Decoded Uint8Array
 */
function decodeBase64Data(dataStr) {
  const decoded = atob$1(dataStr);
  const chardata = decoded.split("").map(function (x) {
    return x.charCodeAt(0);
  });
  return new Uint8Array(chardata);
}

/**
 * Pad to the next alignment boundary
 * @param {Uint8Array} data Uint8Array data to pad
 * @param {number} alignment Alignment boundary to fulfill
 * @param {number} padCharacter Character to fill with
 * @returns {Uint8Array} Padded UInt8Array image
 */
function padTo(data, alignment, padCharacter = 0xff) {
  const padMod = data.length % alignment;
  if (padMod !== 0) {
    const padding = new Uint8Array(alignment - padMod).fill(padCharacter);
    const paddedData = new Uint8Array(data.length + padding.length);
    paddedData.set(data);
    paddedData.set(padding, data.length);
    return paddedData;
  }
  return data;
}

/**
 * Return the chip ROM based on the given magic number
 * @param {number} magic - magic hex number to select ROM.
 * @returns {ROM} The chip ROM class related to given magic hex number.
 */
async function magic2Chip(magic) {
  switch (magic) {
    case 0x00f01d83:
      {
        const {
          ESP32ROM
        } = await import('./esp32-BbLhWH7B.js');
        return new ESP32ROM();
      }
    case 0x6f51306f:
    case 0x7c41a06f:
      {
        const {
          ESP32C2ROM
        } = await import('./esp32c2-CGMLgCZB.js');
        return new ESP32C2ROM();
      }
    case 0x6921506f:
    case 0x1b31506f:
    case 0x4881606f:
    case 0x4361606f:
      {
        const {
          ESP32C3ROM
        } = await import('./esp32c3-CfxJx9As.js');
        return new ESP32C3ROM();
      }
    case 0x2ce0806f:
      {
        const {
          ESP32C6ROM
        } = await import('./esp32c6-DsFTtj2a.js');
        return new ESP32C6ROM();
      }
    case 0x2421606f:
    case 0x33f0206f:
    case 0x4f81606f:
      {
        const {
          ESP32C61ROM
        } = await import('./esp32c61-DwLhvz5u.js');
        return new ESP32C61ROM();
      }
    case 0x1101406f:
    case 0x63e1406f:
      {
        const {
          ESP32C5ROM
        } = await import('./esp32c5-B62vWKc8.js');
        return new ESP32C5ROM();
      }
    case 0xd7b73e80:
      {
        const {
          ESP32H2ROM
        } = await import('./esp32h2-QfjbmgD7.js');
        return new ESP32H2ROM();
      }
    case 0x09:
      {
        const {
          ESP32S3ROM
        } = await import('./esp32s3-Denmzdqv.js');
        return new ESP32S3ROM();
      }
    case 0x000007c6:
      {
        const {
          ESP32S2ROM
        } = await import('./esp32s2-BSe2lc8-.js');
        return new ESP32S2ROM();
      }
    case 0xfff0c101:
      {
        const {
          ESP8266ROM
        } = await import('./esp8266-Ce8DKzPh.js');
        return new ESP8266ROM();
      }
    case 0x0:
    case 0x0addbad0:
    case 0x7039ad9:
      {
        const {
          ESP32P4ROM
        } = await import('./esp32p4-B0YpCtPf.js');
        return new ESP32P4ROM();
      }
    default:
      return null;
  }
}
class ESPLoader {
  /**
   * Create a new ESPLoader to perform serial communication
   * such as read/write flash memory and registers using a LoaderOptions object.
   * @param {LoaderOptions} options - LoaderOptions object argument for ESPLoader.
   * ```
   * const myLoader = new ESPLoader({ transport: Transport, baudrate: number, terminal?: IEspLoaderTerminal });
   * ```
   */
  constructor(options) {
    var _a, _b, _c, _d, _e, _f, _g, _h;
    this.ESP_RAM_BLOCK = 0x1800;
    this.ESP_FLASH_BEGIN = 0x02;
    this.ESP_FLASH_DATA = 0x03;
    this.ESP_FLASH_END = 0x04;
    this.ESP_MEM_BEGIN = 0x05;
    this.ESP_MEM_END = 0x06;
    this.ESP_MEM_DATA = 0x07;
    this.ESP_WRITE_REG = 0x09;
    this.ESP_READ_REG = 0x0a;
    this.ESP_SPI_ATTACH = 0x0d;
    this.ESP_CHANGE_BAUDRATE = 0x0f;
    this.ESP_FLASH_DEFL_BEGIN = 0x10;
    this.ESP_FLASH_DEFL_DATA = 0x11;
    this.ESP_FLASH_DEFL_END = 0x12;
    this.ESP_SPI_FLASH_MD5 = 0x13;
    // Only Stub supported commands
    this.ESP_ERASE_FLASH = 0xd0;
    this.ESP_ERASE_REGION = 0xd1;
    this.ESP_READ_FLASH = 0xd2;
    this.ESP_RUN_USER_CODE = 0xd3;
    this.ESP_IMAGE_MAGIC = 0xe9;
    this.ESP_CHECKSUM_MAGIC = 0xef;
    // Response code(s) sent by ROM
    this.ROM_INVALID_RECV_MSG = 0x05; // response if an invalid message is received
    this.DEFAULT_TIMEOUT = 3000;
    this.ERASE_REGION_TIMEOUT_PER_MB = 30000;
    this.ERASE_WRITE_TIMEOUT_PER_MB = 40000;
    this.MD5_TIMEOUT_PER_MB = 8000;
    this.CHIP_ERASE_TIMEOUT = 120000;
    this.FLASH_READ_TIMEOUT = 100000;
    this.MAX_TIMEOUT = this.CHIP_ERASE_TIMEOUT * 2;
    this.CHIP_DETECT_MAGIC_REG_ADDR = 0x40001000;
    this.DETECTED_FLASH_SIZES = {
      0x12: "256KB",
      0x13: "512KB",
      0x14: "1MB",
      0x15: "2MB",
      0x16: "4MB",
      0x17: "8MB",
      0x18: "16MB"
    };
    this.DETECTED_FLASH_SIZES_NUM = {
      0x12: 256,
      0x13: 512,
      0x14: 1024,
      0x15: 2048,
      0x16: 4096,
      0x17: 8192,
      0x18: 16384
    };
    this.USB_JTAG_SERIAL_PID = 0x1001;
    this.romBaudrate = 115200;
    this.debugLogging = false;
    this.syncStubDetected = false;
    /**
     * Get flash size bytes from flash size string.
     * @param {string} flashSize Flash Size string
     * @returns {number} Flash size bytes
     */
    this.flashSizeBytes = function (flashSize) {
      let flashSizeB = -1;
      if (flashSize.indexOf("KB") !== -1) {
        flashSizeB = parseInt(flashSize.slice(0, flashSize.indexOf("KB"))) * 1024;
      } else if (flashSize.indexOf("MB") !== -1) {
        flashSizeB = parseInt(flashSize.slice(0, flashSize.indexOf("MB"))) * 1024 * 1024;
      }
      return flashSizeB;
    };
    this.IS_STUB = false;
    this.FLASH_WRITE_SIZE = 0x4000;
    this.transport = options.transport;
    this.baudrate = options.baudrate;
    this.resetConstructors = {
      classicReset: (transport, resetDelay) => new ClassicReset(transport, resetDelay),
      customReset: (transport, sequenceString) => new CustomReset(transport, sequenceString),
      hardReset: (transport, usingUsbOtg) => new HardReset(transport, usingUsbOtg),
      usbJTAGSerialReset: transport => new UsbJtagSerialReset(transport)
    };
    if (options.serialOptions) {
      this.serialOptions = options.serialOptions;
    }
    if (options.romBaudrate) {
      this.romBaudrate = options.romBaudrate;
    }
    if (options.terminal) {
      this.terminal = options.terminal;
      this.terminal.clean();
    }
    if (typeof options.debugLogging !== "undefined") {
      this.debugLogging = options.debugLogging;
    }
    if (options.port) {
      this.transport = new Transport(options.port);
    }
    if (typeof options.enableTracing !== "undefined") {
      this.transport.tracing = options.enableTracing;
    }
    if ((_a = options.resetConstructors) === null || _a === void 0 ? void 0 : _a.classicReset) {
      this.resetConstructors.classicReset = (_b = options.resetConstructors) === null || _b === void 0 ? void 0 : _b.classicReset;
    }
    if ((_c = options.resetConstructors) === null || _c === void 0 ? void 0 : _c.customReset) {
      this.resetConstructors.customReset = (_d = options.resetConstructors) === null || _d === void 0 ? void 0 : _d.customReset;
    }
    if ((_e = options.resetConstructors) === null || _e === void 0 ? void 0 : _e.hardReset) {
      this.resetConstructors.hardReset = (_f = options.resetConstructors) === null || _f === void 0 ? void 0 : _f.hardReset;
    }
    if ((_g = options.resetConstructors) === null || _g === void 0 ? void 0 : _g.usbJTAGSerialReset) {
      this.resetConstructors.usbJTAGSerialReset = (_h = options.resetConstructors) === null || _h === void 0 ? void 0 : _h.usbJTAGSerialReset;
    }
    this.info("esptool.js");
    this.info("Serial port " + this.transport.getInfo());
  }
  _sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
  }
  /**
   * Write to ESP Loader constructor's terminal with or without new line.
   * @param {string} str - String to write.
   * @param {boolean} withNewline - Add new line at the end ?
   */
  write(str, withNewline = true) {
    if (this.terminal) {
      if (withNewline) {
        this.terminal.writeLine(str);
      } else {
        this.terminal.write(str);
      }
    } else {
      // eslint-disable-next-line no-console
      console.log(str);
    }
  }
  /**
   * Write error message to ESP Loader constructor's terminal with or without new line.
   * @param {string} str - String to write.
   * @param {boolean} withNewline - Add new line at the end ?
   */
  error(str, withNewline = true) {
    this.write(`Error: ${str}`, withNewline);
  }
  /**
   * Write information message to ESP Loader constructor's terminal with or without new line.
   * @param {string} str - String to write.
   * @param {boolean} withNewline - Add new line at the end ?
   */
  info(str, withNewline = true) {
    this.write(str, withNewline);
  }
  /**
   * Write debug message to ESP Loader constructor's terminal with or without new line.
   * @param {string} str - String to write.
   * @param {boolean} withNewline - Add new line at the end ?
   */
  debug(str, withNewline = true) {
    if (this.debugLogging) {
      this.write(`Debug: ${str}`, withNewline);
    }
  }
  /**
   * Convert short integer to byte array
   * @param {number} i - Number to convert.
   * @returns {Uint8Array} Byte array.
   */
  _shortToBytearray(i) {
    return new Uint8Array([i & 0xff, i >> 8 & 0xff]);
  }
  /**
   * Convert an integer to byte array
   * @param {number} i - Number to convert.
   * @returns {ROM} The chip ROM class related to given magic hex number.
   */
  _intToByteArray(i) {
    return new Uint8Array([i & 0xff, i >> 8 & 0xff, i >> 16 & 0xff, i >> 24 & 0xff]);
  }
  /**
   * Convert a byte array to short integer.
   * @param {number} i - Number to convert.
   * @param {number} j - Number to convert.
   * @returns {number} Return a short integer number.
   */
  _byteArrayToShort(i, j) {
    return i | j >> 8;
  }
  /**
   * Convert a byte array to integer.
   * @param {number} i - Number to convert.
   * @param {number} j - Number to convert.
   * @param {number} k - Number to convert.
   * @param {number} l - Number to convert.
   * @returns {number} Return a integer number.
   */
  _byteArrayToInt(i, j, k, l) {
    return i | j << 8 | k << 16 | l << 24;
  }
  /**
   * Append a buffer array after another buffer array
   * @param {ArrayBuffer} buffer1 - First array buffer.
   * @param {ArrayBuffer} buffer2 - magic hex number to select ROM.
   * @returns {ArrayBufferLike} Return an array buffer.
   */
  _appendBuffer(buffer1, buffer2) {
    const tmp = new Uint8Array(buffer1.byteLength + buffer2.byteLength);
    tmp.set(new Uint8Array(buffer1), 0);
    tmp.set(new Uint8Array(buffer2), buffer1.byteLength);
    return tmp.buffer;
  }
  /**
   * Append a buffer array after another buffer array
   * @param {Uint8Array} arr1 - First array buffer.
   * @param {Uint8Array} arr2 - magic hex number to select ROM.
   * @returns {Uint8Array} Return a 8 bit unsigned array.
   */
  _appendArray(arr1, arr2) {
    const c = new Uint8Array(arr1.length + arr2.length);
    c.set(arr1, 0);
    c.set(arr2, arr1.length);
    return c;
  }
  /**
   * Convert a unsigned 8 bit integer array to byte string.
   * @param {Uint8Array} u8Array - magic hex number to select ROM.
   * @returns {string} Return the equivalent string.
   */
  ui8ToBstr(u8Array) {
    let bStr = "";
    for (let i = 0; i < u8Array.length; i++) {
      bStr += String.fromCharCode(u8Array[i]);
    }
    return bStr;
  }
  /**
   * Convert a byte string to unsigned 8 bit integer array.
   * @param {string} bStr - binary string input
   * @returns {Uint8Array} Return a 8 bit unsigned integer array.
   */
  bstrToUi8(bStr) {
    const u8Array = new Uint8Array(bStr.length);
    for (let i = 0; i < bStr.length; i++) {
      u8Array[i] = bStr.charCodeAt(i);
    }
    return u8Array;
  }
  /**
   * Flush the serial input by raw read with 200 ms timeout.
   */
  async flushInput() {
    try {
      await this.transport.flushInput();
    } catch (e) {
      this.error(e.message);
    }
  }
  /**
   * Use the device serial port read function with given timeout to create a valid packet.
   * @param {number} op Operation number
   * @param {number} timeout timeout number in milliseconds
   * @returns {[number, Uint8Array]} valid response packet.
   */
  async readPacket(op = null, timeout = this.DEFAULT_TIMEOUT) {
    // Check up-to next 100 packets for valid response packet
    for (let i = 0; i < 100; i++) {
      const {
        value: p
      } = await this.transport.read(timeout).next();
      if (!p || p.length < 8) {
        continue;
      }
      const resp = p[0];
      if (resp !== 1) {
        continue;
      }
      const opRet = p[1];
      const val = this._byteArrayToInt(p[4], p[5], p[6], p[7]);
      const data = p.slice(8);
      if (resp == 1) {
        if (op == null || opRet == op) {
          return [val, data];
        } else if (data[0] != 0 && data[1] == this.ROM_INVALID_RECV_MSG) {
          await this.flushInput();
          throw new ESPError("unsupported command error");
        }
      }
    }
    throw new ESPError("invalid response");
  }
  /**
   * Write a serial command to the chip
   * @param {number} op - Operation number
   * @param {Uint8Array} data - Unsigned 8 bit array
   * @param {number} chk - channel number
   * @param {boolean} waitResponse - wait for response ?
   * @param {number} timeout - timeout number in milliseconds
   * @returns {Promise<[number, Uint8Array]>} Return a number and a 8 bit unsigned integer array.
   */
  async command(op = null, data = new Uint8Array(0), chk = 0, waitResponse = true, timeout = this.DEFAULT_TIMEOUT) {
    if (op != null) {
      if (this.transport.tracing) {
        this.transport.trace(`command op:0x${op.toString(16).padStart(2, "0")} data len=${data.length} wait_response=${waitResponse ? 1 : 0} timeout=${(timeout / 1000).toFixed(3)} data=${this.transport.hexConvert(data)}`);
      }
      const pkt = new Uint8Array(8 + data.length);
      pkt[0] = 0x00;
      pkt[1] = op;
      pkt[2] = this._shortToBytearray(data.length)[0];
      pkt[3] = this._shortToBytearray(data.length)[1];
      pkt[4] = this._intToByteArray(chk)[0];
      pkt[5] = this._intToByteArray(chk)[1];
      pkt[6] = this._intToByteArray(chk)[2];
      pkt[7] = this._intToByteArray(chk)[3];
      let i;
      for (i = 0; i < data.length; i++) {
        pkt[8 + i] = data[i];
      }
      await this.transport.write(pkt);
    }
    if (!waitResponse) {
      return [0, new Uint8Array(0)];
    }
    return this.readPacket(op, timeout);
  }
  /**
   * Read a register from chip.
   * @param {number} addr - Register address number
   * @param {number} timeout - Timeout in milliseconds (Default: 3000ms)
   * @returns {number} - Command number value
   */
  async readReg(addr, timeout = this.DEFAULT_TIMEOUT) {
    const pkt = this._intToByteArray(addr);
    const val = await this.command(this.ESP_READ_REG, pkt, undefined, undefined, timeout);
    return val[0];
  }
  /**
   * Write a number value to register address in chip.
   * @param {number} addr - Register address number
   * @param {number} value - Number value to write in register
   * @param {number} mask - Hex number for mask
   * @param {number} delayUs Delay number
   * @param {number} delayAfterUs Delay after previous delay
   */
  async writeReg(addr, value, mask = 0xffffffff, delayUs = 0, delayAfterUs = 0) {
    let pkt = this._appendArray(this._intToByteArray(addr), this._intToByteArray(value));
    pkt = this._appendArray(pkt, this._intToByteArray(mask));
    pkt = this._appendArray(pkt, this._intToByteArray(delayUs));
    if (delayAfterUs > 0) {
      pkt = this._appendArray(pkt, this._intToByteArray(this.chip.UART_DATE_REG_ADDR));
      pkt = this._appendArray(pkt, this._intToByteArray(0));
      pkt = this._appendArray(pkt, this._intToByteArray(0));
      pkt = this._appendArray(pkt, this._intToByteArray(delayAfterUs));
    }
    await this.checkCommand("write target memory", this.ESP_WRITE_REG, pkt);
  }
  /**
   * Sync chip by sending sync command.
   * @returns {[number, Uint8Array]} Command result
   */
  async sync() {
    this.debug("Sync");
    const cmd = new Uint8Array(36);
    let i;
    cmd[0] = 0x07;
    cmd[1] = 0x07;
    cmd[2] = 0x12;
    cmd[3] = 0x20;
    for (i = 0; i < 32; i++) {
      cmd[4 + i] = 0x55;
    }
    try {
      let resp = await this.command(0x08, cmd, undefined, undefined, 100);
      // ROM bootloaders send some non-zero "val" response. The flasher stub sends 0.
      // If we receive 0 then it probably indicates that the chip wasn't or couldn't be
      // reset properly and esptool is talking to the flasher stub.
      this.syncStubDetected = resp[0] === 0;
      for (let i = 0; i < 7; i++) {
        resp = await this.command();
        this.syncStubDetected = this.syncStubDetected && resp[0] === 0;
      }
      return resp;
    } catch (e) {
      this.debug("Sync err " + e);
      throw e;
    }
  }
  /**
   * Attempt to connect to the chip by sending a reset sequence and later a sync command.
   * @param {string} mode - Reset mode to use
   * @param {ResetStrategy} resetStrategy - Reset strategy class to use for connect
   * @returns {string} - Returns 'success' or 'error' message.
   */
  async _connectAttempt(mode = "default_reset", resetStrategy) {
    this.debug("_connect_attempt " + mode);
    if (resetStrategy) {
      await resetStrategy.reset();
    }
    const waitingBytes = this.transport.inWaiting();
    const readBytes = await this.transport.newRead(waitingBytes > 0 ? waitingBytes : 1, this.DEFAULT_TIMEOUT);
    const binaryString = Array.from(readBytes, byte => String.fromCharCode(byte)).join("");
    const regex = /boot:(0x[0-9a-fA-F]+)(.*waiting for download)?/;
    const match = binaryString.match(regex);
    let bootLogDetected = false,
      bootMode = "",
      downloadMode = false;
    if (match) {
      bootLogDetected = true;
      bootMode = match[1];
      downloadMode = !!match[2];
    }
    let lastError = "";
    for (let i = 0; i < 5; i++) {
      try {
        this.debug(`Sync connect attempt ${i}`);
        const resp = await this.sync();
        this.debug(resp[0].toString());
        return "success";
      } catch (error) {
        this.debug(`Error at sync ${error}`);
        if (error instanceof Error) {
          lastError = error.message;
        } else if (typeof error === "string") {
          lastError = error;
        } else {
          lastError = JSON.stringify(error);
        }
      }
    }
    if (bootLogDetected) {
      lastError = `Wrong boot mode detected (${bootMode}).
        This chip needs to be in download mode.`;
      if (downloadMode) {
        lastError = `Download mode successfully detected, but getting no sync reply:
           The serial TX path seems to be down.`;
      }
    }
    return lastError;
  }
  /**
   * Constructs a sequence of reset strategies based on the OS,
   * used ESP chip, external settings, and environment variables.
   * Returns a tuple of one or more reset strategies to be tried sequentially.
   * @param {string} mode - Reset mode to use
   * @returns {ResetStrategy[]} - Array of reset strategies
   */
  constructResetSequence(mode) {
    if (mode !== "no_reset") {
      if (mode === "usb_reset" || this.transport.getPid() === this.USB_JTAG_SERIAL_PID) {
        // Custom reset sequence, which is required when the device
        // is connecting via its USB-JTAG-Serial peripheral
        if (this.resetConstructors.usbJTAGSerialReset) {
          this.debug("using USB JTAG Serial Reset");
          return [this.resetConstructors.usbJTAGSerialReset(this.transport)];
        }
      } else {
        const DEFAULT_RESET_DELAY = 50;
        const EXTRA_DELAY = DEFAULT_RESET_DELAY + 500;
        if (this.resetConstructors.classicReset) {
          this.debug("using Classic Serial Reset");
          return [this.resetConstructors.classicReset(this.transport, DEFAULT_RESET_DELAY), this.resetConstructors.classicReset(this.transport, EXTRA_DELAY)];
        }
      }
    }
    return [];
  }
  /**
   * Perform a connection to chip.
   * @param {string} mode - Reset mode to use. Example: 'default_reset' | 'no_reset'
   * @param {number} attempts - Number of connection attempts
   * @param {boolean} detecting - Detect the connected chip
   */
  async connect(mode = "default_reset", attempts = 7, detecting = false) {
    let resp;
    this.info("Connecting...", false);
    await this.transport.connect(this.romBaudrate, this.serialOptions);
    const resetSequences = this.constructResetSequence(mode);
    for (let i = 0; i < attempts; i++) {
      const resetSequence = resetSequences.length > 0 ? resetSequences[i % resetSequences.length] : null;
      resp = await this._connectAttempt(mode, resetSequence);
      if (resp === "success") {
        break;
      }
    }
    if (resp !== "success") {
      throw new ESPError("Failed to connect with the device");
    }
    this.debug("Connect attempt successful.");
    this.info("\n\r", false);
    if (!detecting) {
      const chipMagicValue = (await this.readReg(this.CHIP_DETECT_MAGIC_REG_ADDR)) >>> 0;
      this.debug("Chip Magic " + chipMagicValue.toString(16));
      const chip = await magic2Chip(chipMagicValue);
      if (this.chip === null) {
        throw new ESPError(`Unexpected CHIP magic value ${chipMagicValue}. Failed to autodetect chip type.`);
      } else {
        this.chip = chip;
      }
    }
  }
  /**
   * Connect and detect the existing chip.
   * @param {string} mode Reset mode to use for connection.
   */
  async detectChip(mode = "default_reset") {
    await this.connect(mode, this.romBaudrate);
    this.info("Detecting chip type... ", false);
    if (this.chip != null) {
      this.info(this.chip.CHIP_NAME);
    } else {
      this.info("unknown!");
    }
  }
  /**
   * Execute the command and check the command response.
   * @param {string} opDescription Command operation description.
   * @param {number} op Command operation number
   * @param {Uint8Array} data Command value
   * @param {number} chk Checksum to use
   * @param {number} timeout TImeout number in milliseconds (ms)
   * @returns {number} Command result
   */
  async checkCommand(opDescription = "", op = null, data = new Uint8Array(0), chk = 0, timeout = this.DEFAULT_TIMEOUT) {
    this.debug("check_command " + opDescription);
    const resp = await this.command(op, data, chk, undefined, timeout);
    if (resp[1].length > 4) {
      return resp[1];
    } else {
      return resp[0];
    }
  }
  /**
   * Start downloading an application image to RAM
   * @param {number} size Image size number
   * @param {number} blocks Number of data blocks
   * @param {number} blocksize Size of each data block
   * @param {number} offset Image offset number
   */
  async memBegin(size, blocks, blocksize, offset) {
    /* XXX: Add check to ensure that STUB is not getting overwritten */
    if (this.IS_STUB) {
      const loadStart = offset;
      const loadEnd = offset + size;
      const stub = await getStubJsonByChipName(this.chip.CHIP_NAME);
      if (stub) {
        const areasToCheck = [[stub.bss_start || stub.data_start, stub.data_start + stub.decodedData.length], [stub.text_start, stub.text_start + stub.decodedText.length]];
        for (const [stubStart, stubEnd] of areasToCheck) {
          if (loadStart < stubEnd && loadEnd > stubStart) {
            throw new ESPError(`Software loader is resident at 0x${stubStart.toString(16).padStart(8, "0")}-0x${stubEnd.toString(16).padStart(8, "0")}.
            Can't load binary at overlapping address range 0x${loadStart.toString(16).padStart(8, "0")}-0x${loadEnd.toString(16).padStart(8, "0")}.
            Either change binary loading address, or use the no-stub option to disable the software loader.`);
          }
        }
      }
    }
    this.debug("mem_begin " + size + " " + blocks + " " + blocksize + " " + offset.toString(16));
    let pkt = this._appendArray(this._intToByteArray(size), this._intToByteArray(blocks));
    pkt = this._appendArray(pkt, this._intToByteArray(blocksize));
    pkt = this._appendArray(pkt, this._intToByteArray(offset));
    await this.checkCommand("enter RAM download mode", this.ESP_MEM_BEGIN, pkt);
  }
  /**
   * Get the checksum for given unsigned 8-bit array
   * @param {Uint8Array} data Unsigned 8-bit integer array
   * @param {number} state Initial checksum
   * @returns {number} - Array checksum
   */
  checksum(data, state = this.ESP_CHECKSUM_MAGIC) {
    for (let i = 0; i < data.length; i++) {
      state ^= data[i];
    }
    return state;
  }
  /**
   * Send a block of image to RAM
   * @param {Uint8Array} buffer Unsigned 8-bit array
   * @param {number} seq Sequence number
   */
  async memBlock(buffer, seq) {
    let pkt = this._appendArray(this._intToByteArray(buffer.length), this._intToByteArray(seq));
    pkt = this._appendArray(pkt, this._intToByteArray(0));
    pkt = this._appendArray(pkt, this._intToByteArray(0));
    pkt = this._appendArray(pkt, buffer);
    const checksum = this.checksum(buffer);
    await this.checkCommand("write to target RAM", this.ESP_MEM_DATA, pkt, checksum);
  }
  /**
   * Leave RAM download mode and run application
   * @param {number} entrypoint - Entrypoint number
   */
  async memFinish(entrypoint) {
    const isEntry = entrypoint === 0 ? 1 : 0;
    const pkt = this._appendArray(this._intToByteArray(isEntry), this._intToByteArray(entrypoint));
    await this.checkCommand("leave RAM download mode", this.ESP_MEM_END, pkt, undefined, 200); // XXX: handle non-stub with diff timeout
  }
  /**
   * Configure SPI flash pins
   * @param {number} hspiArg -  Argument for SPI attachment
   */
  async flashSpiAttach(hspiArg) {
    const pkt = this._intToByteArray(hspiArg);
    await this.checkCommand("configure SPI flash pins", this.ESP_SPI_ATTACH, pkt);
  }
  /**
   * Scale timeouts which are size-specific.
   * @param {number} secondsPerMb Seconds per megabytes as number
   * @param {number} sizeBytes Size bytes number
   * @returns {number} - Scaled timeout for specified size.
   */
  timeoutPerMb(secondsPerMb, sizeBytes) {
    const result = secondsPerMb * (sizeBytes / 1000000);
    if (result < 3000) {
      return 3000;
    } else {
      return result;
    }
  }
  /**
   * Start downloading to Flash (performs an erase)
   * @param {number} size Size to erase
   * @param {number} offset Offset to erase
   * @returns {number} Number of blocks (of size self.FLASH_WRITE_SIZE) to write.
   */
  async flashBegin(size, offset) {
    const numBlocks = Math.floor((size + this.FLASH_WRITE_SIZE - 1) / this.FLASH_WRITE_SIZE);
    const eraseSize = this.chip.getEraseSize(offset, size);
    const d = new Date();
    const t1 = d.getTime();
    let timeout = 3000;
    if (this.IS_STUB == false) {
      timeout = this.timeoutPerMb(this.ERASE_REGION_TIMEOUT_PER_MB, size);
    }
    this.debug("flash begin " + eraseSize + " " + numBlocks + " " + this.FLASH_WRITE_SIZE + " " + offset + " " + size);
    let pkt = this._appendArray(this._intToByteArray(eraseSize), this._intToByteArray(numBlocks));
    pkt = this._appendArray(pkt, this._intToByteArray(this.FLASH_WRITE_SIZE));
    pkt = this._appendArray(pkt, this._intToByteArray(offset));
    if (this.IS_STUB == false) {
      pkt = this._appendArray(pkt, this._intToByteArray(0)); // XXX: Support encrypted
    }
    await this.checkCommand("enter Flash download mode", this.ESP_FLASH_BEGIN, pkt, undefined, timeout);
    const t2 = d.getTime();
    if (size != 0 && this.IS_STUB == false) {
      this.info("Took " + (t2 - t1) / 1000 + "." + (t2 - t1) % 1000 + "s to erase flash block");
    }
    return numBlocks;
  }
  /**
   * Start downloading compressed data to Flash (performs an erase)
   * @param {number} size Write size
   * @param {number} compsize Compressed size
   * @param {number} offset Offset for write
   * @returns {number} Returns number of blocks (size self.FLASH_WRITE_SIZE) to write.
   */
  async flashDeflBegin(size, compsize, offset) {
    const numBlocks = Math.floor((compsize + this.FLASH_WRITE_SIZE - 1) / this.FLASH_WRITE_SIZE);
    const eraseBlocks = Math.floor((size + this.FLASH_WRITE_SIZE - 1) / this.FLASH_WRITE_SIZE);
    const d = new Date();
    const t1 = d.getTime();
    let writeSize, timeout;
    if (this.IS_STUB) {
      writeSize = size;
      timeout = this.DEFAULT_TIMEOUT;
    } else {
      writeSize = eraseBlocks * this.FLASH_WRITE_SIZE;
      timeout = this.timeoutPerMb(this.ERASE_REGION_TIMEOUT_PER_MB, writeSize);
    }
    this.info("Compressed " + size + " bytes to " + compsize + "...");
    let pkt = this._appendArray(this._intToByteArray(writeSize), this._intToByteArray(numBlocks));
    pkt = this._appendArray(pkt, this._intToByteArray(this.FLASH_WRITE_SIZE));
    pkt = this._appendArray(pkt, this._intToByteArray(offset));
    if ((this.chip.CHIP_NAME === "ESP32-S2" || this.chip.CHIP_NAME === "ESP32-S3" || this.chip.CHIP_NAME === "ESP32-C3" || this.chip.CHIP_NAME === "ESP32-C2") && this.IS_STUB === false) {
      pkt = this._appendArray(pkt, this._intToByteArray(0));
    }
    await this.checkCommand("enter compressed flash mode", this.ESP_FLASH_DEFL_BEGIN, pkt, undefined, timeout);
    const t2 = d.getTime();
    if (size != 0 && this.IS_STUB === false) {
      this.info("Took " + (t2 - t1) / 1000 + "." + (t2 - t1) % 1000 + "s to erase flash block");
    }
    return numBlocks;
  }
  /**
   * Write block to flash, retry if fail
   * @param {Uint8Array} data Unsigned 8-bit array data.
   * @param {number} seq Sequence number
   * @param {number} timeout Timeout in milliseconds (ms)
   */
  async flashBlock(data, seq, timeout) {
    let pkt = this._appendArray(this._intToByteArray(data.length), this._intToByteArray(seq));
    pkt = this._appendArray(pkt, this._intToByteArray(0));
    pkt = this._appendArray(pkt, this._intToByteArray(0));
    pkt = this._appendArray(pkt, data);
    const checksum = this.checksum(data);
    await this.checkCommand("write to target Flash after seq " + seq, this.ESP_FLASH_DATA, pkt, checksum, timeout);
  }
  /**
   * Write block to flash, send compressed, retry if fail
   * @param {Uint8Array} data Unsigned int 8-bit array data to write
   * @param {number} seq Sequence number
   * @param {number} timeout Timeout in milliseconds (ms)
   */
  async flashDeflBlock(data, seq, timeout) {
    let pkt = this._appendArray(this._intToByteArray(data.length), this._intToByteArray(seq));
    pkt = this._appendArray(pkt, this._intToByteArray(0));
    pkt = this._appendArray(pkt, this._intToByteArray(0));
    pkt = this._appendArray(pkt, data);
    const checksum = this.checksum(data);
    this.debug("flash_defl_block " + data[0].toString(16) + " " + data[1].toString(16));
    await this.checkCommand("write compressed data to flash after seq " + seq, this.ESP_FLASH_DEFL_DATA, pkt, checksum, timeout);
  }
  /**
   * Leave flash mode and run/reboot
   * @param {boolean} reboot Reboot after leaving flash mode ?
   */
  async flashFinish(reboot = false) {
    const val = reboot ? 0 : 1;
    const pkt = this._intToByteArray(val);
    await this.checkCommand("leave Flash mode", this.ESP_FLASH_END, pkt);
  }
  /**
   * Leave compressed flash mode and run/reboot
   * @param {boolean} reboot Reboot after leaving flash mode ?
   */
  async flashDeflFinish(reboot = false) {
    const val = reboot ? 0 : 1;
    const pkt = this._intToByteArray(val);
    await this.checkCommand("leave compressed flash mode", this.ESP_FLASH_DEFL_END, pkt);
  }
  /**
   * Run an arbitrary SPI flash command.
   *
   * This function uses the "USR_COMMAND" functionality in the ESP
   * SPI hardware, rather than the precanned commands supported by
   * hardware. So the value of spiflashCommand is an actual command
   * byte, sent over the wire.
   *
   * After writing command byte, writes 'data' to MOSI and then
   * reads back 'readBits' of reply on MISO. Result is a number.
   * @param {number} spiflashCommand Command to execute in SPI
   * @param {Uint8Array} data Data to send
   * @param {number} readBits Number of bits to read
   * @returns {number} Register SPI_W0_REG value
   */
  async runSpiflashCommand(spiflashCommand, data, readBits) {
    // SPI_USR register flags
    const SPI_USR_COMMAND = 1 << 31;
    const SPI_USR_MISO = 1 << 28;
    const SPI_USR_MOSI = 1 << 27;
    // SPI registers, base address differs ESP32* vs 8266
    const base = this.chip.SPI_REG_BASE;
    const SPI_CMD_REG = base + 0x00;
    const SPI_USR_REG = base + this.chip.SPI_USR_OFFS;
    const SPI_USR1_REG = base + this.chip.SPI_USR1_OFFS;
    const SPI_USR2_REG = base + this.chip.SPI_USR2_OFFS;
    const SPI_W0_REG = base + this.chip.SPI_W0_OFFS;
    let setDataLengths;
    if (this.chip.SPI_MOSI_DLEN_OFFS != null) {
      setDataLengths = async (mosiBits, misoBits) => {
        const SPI_MOSI_DLEN_REG = base + this.chip.SPI_MOSI_DLEN_OFFS;
        const SPI_MISO_DLEN_REG = base + this.chip.SPI_MISO_DLEN_OFFS;
        if (mosiBits > 0) {
          await this.writeReg(SPI_MOSI_DLEN_REG, mosiBits - 1);
        }
        if (misoBits > 0) {
          await this.writeReg(SPI_MISO_DLEN_REG, misoBits - 1);
        }
      };
    } else {
      setDataLengths = async (mosiBits, misoBits) => {
        const SPI_DATA_LEN_REG = SPI_USR1_REG;
        const SPI_MOSI_BITLEN_S = 17;
        const SPI_MISO_BITLEN_S = 8;
        const mosiMask = mosiBits === 0 ? 0 : mosiBits - 1;
        const misoMask = misoBits === 0 ? 0 : misoBits - 1;
        const val = misoMask << SPI_MISO_BITLEN_S | mosiMask << SPI_MOSI_BITLEN_S;
        await this.writeReg(SPI_DATA_LEN_REG, val);
      };
    }
    const SPI_CMD_USR = 1 << 18;
    const SPI_USR2_COMMAND_LEN_SHIFT = 28;
    if (readBits > 32) {
      throw new ESPError("Reading more than 32 bits back from a SPI flash operation is unsupported");
    }
    if (data.length > 64) {
      throw new ESPError("Writing more than 64 bytes of data with one SPI command is unsupported");
    }
    const dataBits = data.length * 8;
    const oldSpiUsr = await this.readReg(SPI_USR_REG);
    const oldSpiUsr2 = await this.readReg(SPI_USR2_REG);
    let flags = SPI_USR_COMMAND;
    let i;
    if (readBits > 0) {
      flags |= SPI_USR_MISO;
    }
    if (dataBits > 0) {
      flags |= SPI_USR_MOSI;
    }
    await setDataLengths(dataBits, readBits);
    await this.writeReg(SPI_USR_REG, flags);
    let val = 7 << SPI_USR2_COMMAND_LEN_SHIFT | spiflashCommand;
    await this.writeReg(SPI_USR2_REG, val);
    if (dataBits == 0) {
      await this.writeReg(SPI_W0_REG, 0);
    } else {
      if (data.length % 4 != 0) {
        const padding = new Uint8Array(data.length % 4);
        data = this._appendArray(data, padding);
      }
      let nextReg = SPI_W0_REG;
      for (i = 0; i < data.length - 4; i += 4) {
        val = this._byteArrayToInt(data[i], data[i + 1], data[i + 2], data[i + 3]);
        await this.writeReg(nextReg, val);
        nextReg += 4;
      }
    }
    await this.writeReg(SPI_CMD_REG, SPI_CMD_USR);
    for (i = 0; i < 10; i++) {
      val = (await this.readReg(SPI_CMD_REG)) & SPI_CMD_USR;
      if (val == 0) {
        break;
      }
    }
    if (i === 10) {
      throw new ESPError("SPI command did not complete in time");
    }
    const stat = await this.readReg(SPI_W0_REG);
    await this.writeReg(SPI_USR_REG, oldSpiUsr);
    await this.writeReg(SPI_USR2_REG, oldSpiUsr2);
    return stat;
  }
  /**
   * Read flash id by executing the SPIFLASH_RDID flash command.
   * @returns {Promise<number>} Register SPI_W0_REG value
   */
  async readFlashId() {
    const SPIFLASH_RDID = 0x9f;
    const pkt = new Uint8Array(0);
    return await this.runSpiflashCommand(SPIFLASH_RDID, pkt, 24);
  }
  /**
   * Execute the erase flash command
   * @returns {Promise<number | Uint8Array>} Erase flash command result
   */
  async eraseFlash() {
    this.info("Erasing flash (this may take a while)...");
    let d = new Date();
    const t1 = d.getTime();
    const ret = await this.checkCommand("erase flash", this.ESP_ERASE_FLASH, undefined, undefined, this.CHIP_ERASE_TIMEOUT);
    d = new Date();
    const t2 = d.getTime();
    this.info("Chip erase completed successfully in " + (t2 - t1) / 1000 + "s");
    return ret;
  }
  /**
   * Convert a number or unsigned 8-bit array to hex string
   * @param {number | Uint8Array } buffer Data to convert to hex string.
   * @returns {string} A hex string
   */
  toHex(buffer) {
    return Array.prototype.map.call(buffer, x => ("00" + x.toString(16)).slice(-2)).join("");
  }
  /**
   * Calculate the MD5 Checksum command
   * @param {number} addr Address number
   * @param {number} size Package size
   * @returns {string} MD5 Checksum string
   */
  async flashMd5sum(addr, size) {
    const timeout = this.timeoutPerMb(this.MD5_TIMEOUT_PER_MB, size);
    let pkt = this._appendArray(this._intToByteArray(addr), this._intToByteArray(size));
    pkt = this._appendArray(pkt, this._intToByteArray(0));
    pkt = this._appendArray(pkt, this._intToByteArray(0));
    let res = await this.checkCommand("calculate md5sum", this.ESP_SPI_FLASH_MD5, pkt, undefined, timeout);
    if (res instanceof Uint8Array && res.length > 16) {
      res = res.slice(0, 16);
    }
    const strmd5 = this.toHex(res);
    return strmd5;
  }
  async readFlash(addr, size, onPacketReceived = null) {
    let pkt = this._appendArray(this._intToByteArray(addr), this._intToByteArray(size));
    pkt = this._appendArray(pkt, this._intToByteArray(0x1000));
    pkt = this._appendArray(pkt, this._intToByteArray(1024));
    const res = await this.checkCommand("read flash", this.ESP_READ_FLASH, pkt);
    if (res != 0) {
      throw new ESPError("Failed to read memory: " + res);
    }
    let resp = new Uint8Array(0);
    while (resp.length < size) {
      const {
        value: packet
      } = await this.transport.read(this.FLASH_READ_TIMEOUT).next();
      if (packet instanceof Uint8Array) {
        if (packet.length > 0) {
          resp = this._appendArray(resp, packet);
          await this.transport.write(this._intToByteArray(resp.length));
          if (onPacketReceived) {
            onPacketReceived(packet, resp.length, size);
          }
        }
      } else {
        throw new ESPError("Failed to read memory: " + packet);
      }
    }
    return resp;
  }
  /**
   * Upload the flasher ROM bootloader (flasher stub) to the chip.
   * @returns {ROM} The Chip ROM
   */
  async runStub() {
    if (this.syncStubDetected) {
      this.info("Stub is already running. No upload is necessary.");
      return this.chip;
    }
    this.info("Uploading stub...");
    const stubFlasher = await getStubJsonByChipName(this.chip.CHIP_NAME);
    if (stubFlasher === undefined) {
      this.debug("Error loading Stub json");
      throw new Error("Error loading Stub json");
    }
    const stub = [stubFlasher.decodedText, stubFlasher.decodedData];
    for (let i = 0; i < stub.length; i++) {
      if (stub[i]) {
        const offs = i === 0 ? stubFlasher.text_start : stubFlasher.data_start;
        const length = stub[i].length;
        const blocks = Math.floor((length + this.ESP_RAM_BLOCK - 1) / this.ESP_RAM_BLOCK);
        await this.memBegin(length, blocks, this.ESP_RAM_BLOCK, offs);
        for (let seq = 0; seq < blocks; seq++) {
          const fromOffs = seq * this.ESP_RAM_BLOCK;
          const toOffs = fromOffs + this.ESP_RAM_BLOCK;
          await this.memBlock(stub[i].slice(fromOffs, toOffs), seq);
        }
      }
    }
    this.info("Running stub...");
    await this.memFinish(stubFlasher.entry);
    const {
      value: packetResult
    } = await this.transport.read(this.DEFAULT_TIMEOUT).next();
    const packetStr = String.fromCharCode(...packetResult);
    if (packetStr !== "OHAI") {
      throw new ESPError(`Failed to start stub. Unexpected response ${packetStr}`);
    }
    this.info("Stub running...");
    this.IS_STUB = true;
    return this.chip;
  }
  /**
   * Change the chip baudrate.
   */
  async changeBaud() {
    this.info("Changing baudrate to " + this.baudrate);
    const secondArg = this.IS_STUB ? this.romBaudrate : 0;
    const pkt = this._appendArray(this._intToByteArray(this.baudrate), this._intToByteArray(secondArg));
    await this.command(this.ESP_CHANGE_BAUDRATE, pkt);
    this.info("Changed");
    await this.transport.disconnect();
    await this._sleep(50);
    await this.transport.connect(this.baudrate, this.serialOptions);
  }
  /**
   * Execute the main function of ESPLoader.
   * @param {string} mode Reset mode to use
   * @returns {string} chip ROM
   */
  async main(mode = "default_reset") {
    await this.detectChip(mode);
    const chip = await this.chip.getChipDescription(this);
    this.info("Chip is " + chip);
    this.info("Features: " + (await this.chip.getChipFeatures(this)));
    this.info("Crystal is " + (await this.chip.getCrystalFreq(this)) + "MHz");
    this.info("MAC: " + (await this.chip.readMac(this)));
    await this.chip.readMac(this);
    if (typeof this.chip.postConnect != "undefined") {
      await this.chip.postConnect(this);
    }
    await this.runStub();
    if (this.romBaudrate !== this.baudrate) {
      await this.changeBaud();
    }
    return chip;
  }
  /**
   * Parse a given flash size string to a number
   * @param {string} flsz Flash size to request
   * @returns {number} Flash size number
   */
  parseFlashSizeArg(flsz) {
    if (typeof this.chip.FLASH_SIZES[flsz] === "undefined") {
      throw new ESPError("Flash size " + flsz + " is not supported by this chip type. Supported sizes: " + this.chip.FLASH_SIZES);
    }
    return this.chip.FLASH_SIZES[flsz];
  }
  /**
   * Update the image flash parameters with given arguments.
   * @param {string} image binary image as string
   * @param {number} address flash address number
   * @param {string} flashSize Flash size string
   * @param {string} flashMode Flash mode string
   * @param {string} flashFreq Flash frequency string
   * @returns {string} modified image string
   */
  _updateImageFlashParams(image, address, flashSize, flashMode, flashFreq) {
    this.debug("_update_image_flash_params " + flashSize + " " + flashMode + " " + flashFreq);
    if (image.length < 8) {
      return image;
    }
    if (address != this.chip.BOOTLOADER_FLASH_OFFSET) {
      return image;
    }
    if (flashSize === "keep" && flashMode === "keep" && flashFreq === "keep") {
      this.info("Not changing the image");
      return image;
    }
    const magic = parseInt(image[0]);
    let aFlashMode = parseInt(image[2]);
    const flashSizeFreq = parseInt(image[3]);
    if (magic !== this.ESP_IMAGE_MAGIC) {
      this.info("Warning: Image file at 0x" + address.toString(16) + " doesn't look like an image file, so not changing any flash settings.");
      return image;
    }
    /* XXX: Yet to implement actual image verification */
    if (flashMode !== "keep") {
      const flashModes = {
        qio: 0,
        qout: 1,
        dio: 2,
        dout: 3
      };
      aFlashMode = flashModes[flashMode];
    }
    let aFlashFreq = flashSizeFreq & 0x0f;
    if (flashFreq !== "keep") {
      const flashFreqs = {
        "40m": 0,
        "26m": 1,
        "20m": 2,
        "80m": 0xf
      };
      aFlashFreq = flashFreqs[flashFreq];
    }
    let aFlashSize = flashSizeFreq & 0xf0;
    if (flashSize !== "keep") {
      aFlashSize = this.parseFlashSizeArg(flashSize);
    }
    const flashParams = aFlashMode << 8 | aFlashFreq + aFlashSize;
    this.info("Flash params set to " + flashParams.toString(16));
    if (parseInt(image[2]) !== aFlashMode << 8) {
      image = image.substring(0, 2) + (aFlashMode << 8).toString() + image.substring(2 + 1);
    }
    if (parseInt(image[3]) !== aFlashFreq + aFlashSize) {
      image = image.substring(0, 3) + (aFlashFreq + aFlashSize).toString() + image.substring(3 + 1);
    }
    return image;
  }
  /**
   * Write set of file images into given address based on given FlashOptions object.
   * @param {FlashOptions} options FlashOptions to configure how and what to write into flash.
   */
  async writeFlash(options) {
    this.debug("EspLoader program");
    if (options.flashSize !== "keep") {
      const flashEnd = this.flashSizeBytes(options.flashSize);
      for (let i = 0; i < options.fileArray.length; i++) {
        if (options.fileArray[i].data.length + options.fileArray[i].address > flashEnd) {
          throw new ESPError(`File ${i + 1} doesn't fit in the available flash`);
        }
      }
    }
    if (this.IS_STUB === true && options.eraseAll === true) {
      await this.eraseFlash();
    }
    let image, address;
    for (let i = 0; i < options.fileArray.length; i++) {
      this.debug("Data Length " + options.fileArray[i].data.length);
      image = options.fileArray[i].data;
      this.debug("Image Length " + image.length);
      if (image.length === 0) {
        this.debug("Warning: File is empty");
        continue;
      }
      image = this.ui8ToBstr(padTo(this.bstrToUi8(image), 4));
      address = options.fileArray[i].address;
      image = this._updateImageFlashParams(image, address, options.flashSize, options.flashMode, options.flashFreq);
      let calcmd5 = null;
      if (options.calculateMD5Hash) {
        calcmd5 = options.calculateMD5Hash(image);
        this.debug("Image MD5 " + calcmd5);
      }
      const uncsize = image.length;
      let blocks;
      if (options.compress) {
        const uncimage = this.bstrToUi8(image);
        image = this.ui8ToBstr(deflate_1(uncimage, {
          level: 9
        }));
        blocks = await this.flashDeflBegin(uncsize, image.length, address);
      } else {
        blocks = await this.flashBegin(uncsize, address);
      }
      let seq = 0;
      let bytesSent = 0;
      const totalBytes = image.length;
      if (options.reportProgress) options.reportProgress(i, 0, totalBytes);
      let d = new Date();
      const t1 = d.getTime();
      let timeout = 5000;
      // Create a decompressor to keep track of the size of uncompressed data
      // to be written in each chunk.
      const inflate = new Inflate_1({
        chunkSize: 1
      });
      let totalLenUncompressed = 0;
      inflate.onData = function (chunk) {
        totalLenUncompressed += chunk.byteLength;
      };
      while (image.length > 0) {
        this.debug("Write loop " + address + " " + seq + " " + blocks);
        this.info("Writing at 0x" + (address + totalLenUncompressed).toString(16) + "... (" + Math.floor(100 * (seq + 1) / blocks) + "%)");
        const block = this.bstrToUi8(image.slice(0, this.FLASH_WRITE_SIZE));
        if (options.compress) {
          const lenUncompressedPrevious = totalLenUncompressed;
          inflate.push(block, false);
          const blockUncompressed = totalLenUncompressed - lenUncompressedPrevious;
          let blockTimeout = 3000;
          if (this.timeoutPerMb(this.ERASE_WRITE_TIMEOUT_PER_MB, blockUncompressed) > 3000) {
            blockTimeout = this.timeoutPerMb(this.ERASE_WRITE_TIMEOUT_PER_MB, blockUncompressed);
          }
          if (this.IS_STUB === false) {
            // ROM code writes block to flash before ACKing
            timeout = blockTimeout;
          }
          await this.flashDeflBlock(block, seq, timeout);
          if (this.IS_STUB) {
            // Stub ACKs when block is received, then writes to flash while receiving the block after it
            timeout = blockTimeout;
          }
        } else {
          throw new ESPError("Yet to handle Non Compressed writes");
        }
        bytesSent += block.length;
        image = image.slice(this.FLASH_WRITE_SIZE, image.length);
        seq++;
        if (options.reportProgress) options.reportProgress(i, bytesSent, totalBytes);
      }
      if (this.IS_STUB) {
        await this.readReg(this.CHIP_DETECT_MAGIC_REG_ADDR, timeout);
      }
      d = new Date();
      const t = d.getTime() - t1;
      if (options.compress) {
        this.info("Wrote " + uncsize + " bytes (" + bytesSent + " compressed) at 0x" + address.toString(16) + " in " + t / 1000 + " seconds.");
      }
      if (calcmd5) {
        const res = await this.flashMd5sum(address, uncsize);
        if (new String(res).valueOf() != new String(calcmd5).valueOf()) {
          this.info("File  md5: " + calcmd5);
          this.info("Flash md5: " + res);
          throw new ESPError("MD5 of file does not match data in flash!");
        } else {
          this.info("Hash of data verified.");
        }
      }
    }
    this.info("Leaving...");
    if (this.IS_STUB) {
      await this.flashBegin(0, 0);
      if (options.compress) {
        await this.flashDeflFinish();
      } else {
        await this.flashFinish();
      }
    }
  }
  /**
   * Read SPI flash manufacturer and device id.
   */
  async flashId() {
    this.debug("flash_id");
    const flashid = await this.readFlashId();
    this.info("Manufacturer: " + (flashid & 0xff).toString(16));
    const flidLowbyte = flashid >> 16 & 0xff;
    this.info("Device: " + (flashid >> 8 & 0xff).toString(16) + flidLowbyte.toString(16));
    this.info("Detected flash size: " + this.DETECTED_FLASH_SIZES[flidLowbyte]);
  }
  async getFlashSize() {
    this.debug("flash_id");
    const flashid = await this.readFlashId();
    const flidLowbyte = flashid >> 16 & 0xff;
    return this.DETECTED_FLASH_SIZES_NUM[flidLowbyte];
  }
  /**
   * Soft reset the device chip. Soft reset with run user code is the closest.
   * @param {boolean} stayInBootloader Flag to indicate if to stay in bootloader
   */
  async softReset(stayInBootloader) {
    if (!this.IS_STUB) {
      if (stayInBootloader) {
        return; // ROM bootloader is already in bootloader!
      }
      // "run user code" is as close to a soft reset as we can do
      await this.flashBegin(0, 0);
      await this.flashFinish(false);
    } else if (this.chip.CHIP_NAME != "ESP8266") {
      throw new ESPError("Soft resetting is currently only supported on ESP8266");
    } else {
      if (stayInBootloader) {
        // soft resetting from the stub loader
        // will re-load the ROM bootloader
        await this.flashBegin(0, 0);
        await this.flashFinish(true);
      } else {
        // running user code from stub loader requires some hacks
        // in the stub loader
        await this.command(this.ESP_RUN_USER_CODE, undefined, undefined, false);
      }
    }
  }
  /**
   * Execute this function to execute after operation reset functions.
   * @param {After} mode After operation mode. Default is 'hard_reset'.
   * @param { boolean } usingUsbOtg For 'hard_reset' to specify if using USB-OTG
   */
  async after(mode = "hard_reset", usingUsbOtg) {
    switch (mode) {
      case "hard_reset":
        if (this.resetConstructors.hardReset) {
          this.info("Hard resetting via RTS pin...");
          const hardReset = this.resetConstructors.hardReset(this.transport, usingUsbOtg);
          await hardReset.reset();
        }
        break;
      case "soft_reset":
        this.info("Soft resetting...");
        await this.softReset(false);
        break;
      case "no_reset_stub":
        this.info("Staying in flasher stub.");
        break;
      default:
        this.info("Staying in bootloader.");
        if (this.IS_STUB) {
          this.softReset(true);
        }
        break;
    }
  }
}

const hardReset = async transport => {
  console.log("Triggering reset");
  await transport.device.setSignals({
    dataTerminalReady: false,
    requestToSend: true
  });
  await sleep$2(250);
  await transport.device.setSignals({
    dataTerminalReady: false,
    requestToSend: false
  });
  await sleep$2(250);
  await new Promise(resolve => setTimeout(resolve, 1000));
};

const flash = async (onEvent, port, manifestPath, manifest, eraseFirst) => {
  let build;
  let chipFamily;
  const fireStateEvent = stateUpdate => onEvent({
    ...stateUpdate,
    manifest,
    build,
    chipFamily
  });
  const transport = new Transport(port);
  const esploader = new ESPLoader({
    transport,
    baudrate: 115200,
    romBaudrate: 115200,
    enableTracing: false
  });
  // For debugging
  window.esploader = esploader;
  fireStateEvent({
    state: "initializing" /* FlashStateType.INITIALIZING */,
    message: "Initializing...",
    details: {
      done: false
    }
  });
  try {
    await esploader.main();
    await esploader.flashId();
  } catch (err) {
    console.error(err);
    fireStateEvent({
      state: "error" /* FlashStateType.ERROR */,
      message: "Failed to initialize. Try resetting your device or holding the BOOT button while clicking INSTALL.",
      details: {
        error: "failed_initialize" /* FlashError.FAILED_INITIALIZING */,
        details: err
      }
    });
    await hardReset(transport);
    await transport.disconnect();
    return;
  }
  chipFamily = esploader.chip.CHIP_NAME;
  fireStateEvent({
    state: "initializing" /* FlashStateType.INITIALIZING */,
    message: `Initialized. Found ${chipFamily}`,
    details: {
      done: true
    }
  });
  build = manifest.builds.find(b => b.chipFamily === chipFamily);
  if (!build) {
    fireStateEvent({
      state: "error" /* FlashStateType.ERROR */,
      message: `Your ${chipFamily} board is not supported.`,
      details: {
        error: "not_supported" /* FlashError.NOT_SUPPORTED */,
        details: chipFamily
      }
    });
    await hardReset(transport);
    await transport.disconnect();
    return;
  }
  fireStateEvent({
    state: "preparing" /* FlashStateType.PREPARING */,
    message: "Preparing installation...",
    details: {
      done: false
    }
  });
  const manifestURL = new URL(manifestPath, location.toString()).toString();
  const filePromises = build.parts.map(async part => {
    const url = new URL(part.path, manifestURL).toString();
    const resp = await fetch(url);
    if (!resp.ok) {
      throw new Error(`Downlading firmware ${part.path} failed: ${resp.status}`);
    }
    const reader = new FileReader();
    const blob = await resp.blob();
    return new Promise(resolve => {
      reader.addEventListener("load", () => resolve(reader.result));
      reader.readAsBinaryString(blob);
    });
  });
  const fileArray = [];
  let totalSize = 0;
  for (let part = 0; part < filePromises.length; part++) {
    try {
      const data = await filePromises[part];
      fileArray.push({
        data,
        address: build.parts[part].offset
      });
      totalSize += data.length;
    } catch (err) {
      fireStateEvent({
        state: "error" /* FlashStateType.ERROR */,
        message: err.message,
        details: {
          error: "failed_firmware_download" /* FlashError.FAILED_FIRMWARE_DOWNLOAD */,
          details: err.message
        }
      });
      await hardReset(transport);
      await transport.disconnect();
      return;
    }
  }
  fireStateEvent({
    state: "preparing" /* FlashStateType.PREPARING */,
    message: "Installation prepared",
    details: {
      done: true
    }
  });
  if (eraseFirst) {
    fireStateEvent({
      state: "erasing" /* FlashStateType.ERASING */,
      message: "Erasing device...",
      details: {
        done: false
      }
    });
    await esploader.eraseFlash();
    fireStateEvent({
      state: "erasing" /* FlashStateType.ERASING */,
      message: "Device erased",
      details: {
        done: true
      }
    });
  }
  fireStateEvent({
    state: "writing" /* FlashStateType.WRITING */,
    message: `Writing progress: 0%`,
    details: {
      bytesTotal: totalSize,
      bytesWritten: 0,
      percentage: 0
    }
  });
  let totalWritten = 0;
  try {
    await esploader.writeFlash({
      fileArray,
      flashSize: "keep",
      flashMode: "keep",
      flashFreq: "keep",
      eraseAll: false,
      compress: true,
      // report progress
      reportProgress: (fileIndex, written, total) => {
        const uncompressedWritten = written / total * fileArray[fileIndex].data.length;
        const newPct = Math.floor((totalWritten + uncompressedWritten) / totalSize * 100);
        // we're done with this file
        if (written === total) {
          totalWritten += uncompressedWritten;
          return;
        }
        fireStateEvent({
          state: "writing" /* FlashStateType.WRITING */,
          message: `Writing progress: ${newPct}%`,
          details: {
            bytesTotal: totalSize,
            bytesWritten: totalWritten + written,
            percentage: newPct
          }
        });
      }
    });
  } catch (err) {
    fireStateEvent({
      state: "error" /* FlashStateType.ERROR */,
      message: err.message,
      details: {
        error: "write_failed" /* FlashError.WRITE_FAILED */,
        details: err
      }
    });
    await hardReset(transport);
    await transport.disconnect();
    return;
  }
  fireStateEvent({
    state: "writing" /* FlashStateType.WRITING */,
    message: "Writing complete",
    details: {
      bytesTotal: totalSize,
      bytesWritten: totalWritten,
      percentage: 100
    }
  });
  await hardReset(transport);
  console.log("DISCONNECT");
  await transport.disconnect();
  fireStateEvent({
    state: "finished" /* FlashStateType.FINISHED */,
    message: "All done!"
  });
};

const fileDownload = (href, filename = "") => {
  const a = document.createElement("a");
  a.target = "_blank";
  a.href = href;
  a.download = filename;
  document.body.appendChild(a);
  a.dispatchEvent(new MouseEvent("click"));
  document.body.removeChild(a);
};
const textDownload = (text, filename = "") => {
  const blob = new Blob([text], {
    type: "text/plain"
  });
  const url = URL.createObjectURL(blob);
  fileDownload(url, filename);
  setTimeout(() => URL.revokeObjectURL(url), 0);
};

const fireEvent = (eventTarget, type,
// @ts-ignore
detail, options) => {
  options = options || {};
  const event = new CustomEvent(type, {
    bubbles: options.bubbles === undefined ? true : options.bubbles,
    cancelable: Boolean(options.cancelable),
    composed: options.composed === undefined ? true : options.composed,
    detail
  });
  eventTarget.dispatchEvent(event);
};

const downloadManifest = async manifestPath => {
  const manifestURL = new URL(manifestPath, location.toString()).toString();
  const resp = await fetch(manifestURL);
  const manifest = await resp.json();
  if ("new_install_skip_erase" in manifest) {
    console.warn('Manifest option "new_install_skip_erase" is deprecated. Use "new_install_prompt_erase" instead.');
    if (manifest.new_install_skip_erase) {
      manifest.new_install_prompt_erase = true;
    }
  }
  return manifest;
};

const version = "dev";

console.log(`ESP Web Tools ${version} by Open Home Foundation; https://esphome.github.io/esp-web-tools/`);
const ERROR_ICON = "⚠️";
const OK_ICON = "🎉";
class EwtInstallDialog extends i$2 {
  constructor() {
    super(...arguments);
    this.logger = console;
    this._state = "DASHBOARD";
    this._installErase = false;
    this._installConfirmed = false;
    this._provisionForce = false;
    this._wasProvisioned = false;
    this._busy = false;
    // Name of Ssid. Null = other
    this._selectedSsid = null;
    this._bodyOverflow = null;
    this._handleDisconnect = () => {
      this._state = "ERROR";
      this._error = "Disconnected";
    };
  }
  render() {
    if (!this.port) {
      return x``;
    }
    let heading;
    let content;
    let allowClosing = false;
    // During installation phase we temporarily remove the client
    if (this._client === undefined && this._state !== "INSTALL" && this._state !== "LOGS") {
      if (this._error) {
        [heading, content] = this._renderError(this._error);
      } else {
        content = this._renderProgress("Connecting");
      }
    } else if (this._state === "INSTALL") {
      [heading, content, allowClosing] = this._renderInstall();
    } else if (this._state === "ASK_ERASE") {
      [heading, content] = this._renderAskErase();
    } else if (this._state === "ERROR") {
      [heading, content] = this._renderError(this._error);
    } else if (this._state === "DASHBOARD") {
      [heading, content, allowClosing] = this._client ? this._renderDashboard() : this._renderDashboardNoImprov();
    } else if (this._state === "PROVISION") {
      [heading, content] = this._renderProvision();
    } else if (this._state === "LOGS") {
      [heading, content] = this._renderLogs();
    }
    return x`
      <ew-dialog
        open
        .heading=${heading}
        @cancel=${this._preventDefault}
        @closed=${this._handleClose}
      >
        ${heading ? x`<div slot="headline">${heading}</div>` : ""}
        ${allowClosing ? x`
              <ew-icon-button slot="headline" @click=${this._closeDialog}>
                ${closeIcon}
              </ew-icon-button>
            ` : ""}
        ${content}
      </ew-dialog>
    `;
  }
  _renderProgress(label, progress) {
    return x`
      <ewt-page-progress
        slot="content"
        .label=${label}
        .progress=${progress}
      ></ewt-page-progress>
    `;
  }
  _renderError(label) {
    const heading = "Error";
    const content = x`
      <ewt-page-message
        slot="content"
        .icon=${ERROR_ICON}
        .label=${label}
      ></ewt-page-message>
      <div slot="actions">
        <ew-text-button @click=${this._closeDialog}>Close</ew-text-button>
      </div>
    `;
    return [heading, content];
  }
  _renderDashboard() {
    const heading = this._manifest.name;
    let content;
    let allowClosing = true;
    content = x`
      <div slot="content">
        <ew-list>
          <ew-list-item>
            <div slot="headline">Connected to ${this._info.name}</div>
            <div slot="supporting-text">
              ${this._info.firmware}&nbsp;${this._info.version}
              (${this._info.chipFamily})
            </div>
          </ew-list-item>
          ${!this._isSameVersion ? x`
                <ew-list-item
                  type="button"
                  @click=${() => {
      if (this._isSameFirmware) {
        this._startInstall(false);
      } else if (this._manifest.new_install_prompt_erase) {
        this._state = "ASK_ERASE";
      } else {
        this._startInstall(true);
      }
    }}
                >
                  ${listItemInstallIcon}
                  <div slot="headline">
                    ${!this._isSameFirmware ? `Install ${this._manifest.name}` : `Update ${this._manifest.name}`}
                  </div>
                </ew-list-item>
              ` : ""}
          ${this._client.nextUrl === undefined ? "" : x`
                <ew-list-item
                  type="link"
                  href=${this._client.nextUrl}
                  target="_blank"
                >
                  ${listItemVisitDevice}
                  <div slot="headline">Visit Device</div>
                </ew-list-item>
              `}
          ${!this._manifest.home_assistant_domain || this._client.state !== ImprovSerialCurrentState.PROVISIONED ? "" : x`
                <ew-list-item
                  type="link"
                  href=${`https://my.home-assistant.io/redirect/config_flow_start/?domain=${this._manifest.home_assistant_domain}`}
                  target="_blank"
                >
                  ${listItemHomeAssistant}
                  <div slot="headline">Add to Home Assistant</div>
                </ew-list-item>
              `}
          <ew-list-item
            type="button"
            @click=${() => {
      this._state = "PROVISION";
      if (this._client.state === ImprovSerialCurrentState.PROVISIONED) {
        this._provisionForce = true;
      }
    }}
          >
            ${listItemWifi}
            <div slot="headline">
              ${this._client.state === ImprovSerialCurrentState.READY ? "Connect to Wi-Fi" : "Change Wi-Fi"}
            </div>
          </ew-list-item>
          <ew-list-item
            type="button"
            @click=${async () => {
      const client = this._client;
      if (client) {
        await this._closeClientWithoutEvents(client);
        await sleep$2(100);
      }
      // Also set `null` back to undefined.
      this._client = undefined;
      this._state = "LOGS";
    }}
          >
            ${listItemConsole}
            <div slot="headline">Logs & Console</div>
          </ew-list-item>
          ${this._isSameFirmware && this._manifest.funding_url ? x`
                <ew-list-item
                  type="link"
                  href=${this._manifest.funding_url}
                  target="_blank"
                >
                  ${listItemFundDevelopment}
                  <div slot="headline">Fund Development</div>
                </ew-list-item>
              ` : ""}
          ${this._isSameVersion ? x`
                <ew-list-item
                  type="button"
                  class="danger"
                  @click=${() => this._startInstall(true)}
                >
                  ${listItemEraseUserData}
                  <div slot="headline">Erase User Data</div>
                </ew-list-item>
              ` : ""}
        </ew-list>
      </div>
    `;
    return [heading, content, allowClosing];
  }
  _renderDashboardNoImprov() {
    const heading = this._manifest.name;
    let content;
    let allowClosing = true;
    content = x`
      <div slot="content">
        <ew-list>
          <ew-list-item
            type="button"
            @click=${() => {
      if (this._manifest.new_install_prompt_erase) {
        this._state = "ASK_ERASE";
      } else {
        // Default is to erase a device that does not support Improv Serial
        this._startInstall(true);
      }
    }}
          >
            ${listItemInstallIcon}
            <div slot="headline">${`Install ${this._manifest.name}`}</div>
          </ew-list-item>
          <ew-list-item
            type="button"
            @click=${async () => {
      // Also set `null` back to undefined.
      this._client = undefined;
      this._state = "LOGS";
    }}
          >
            ${listItemConsole}
            <div slot="headline">Logs & Console</div>
          </ew-list-item>
        </ew-list>
      </div>
    `;
    return [heading, content, allowClosing];
  }
  _renderProvision() {
    var _a;
    let heading = "Configure Wi-Fi";
    let content;
    if (this._busy) {
      return [heading, this._renderProgress(this._ssids === undefined ? "Scanning for networks" : "Trying to connect")];
    }
    if (!this._provisionForce && this._client.state === ImprovSerialCurrentState.PROVISIONED) {
      heading = undefined;
      const showSetupLinks = !this._wasProvisioned && (this._client.nextUrl !== undefined || "home_assistant_domain" in this._manifest);
      content = x`
        <div slot="content">
          <ewt-page-message
            .icon=${OK_ICON}
            label="Device connected to the network!"
          ></ewt-page-message>
          ${showSetupLinks ? x`
                <ew-list>
                  ${this._client.nextUrl === undefined ? "" : x`
                        <ew-list-item
                          type="link"
                          href=${this._client.nextUrl}
                          target="_blank"
                          @click=${() => {
        this._state = "DASHBOARD";
      }}
                        >
                          ${listItemVisitDevice}
                          <div slot="headline">Visit Device</div>
                        </ew-list-item>
                      `}
                  ${!this._manifest.home_assistant_domain ? "" : x`
                        <ew-list-item
                          type="link"
                          href=${`https://my.home-assistant.io/redirect/config_flow_start/?domain=${this._manifest.home_assistant_domain}`}
                          target="_blank"
                          @click=${() => {
        this._state = "DASHBOARD";
      }}
                        >
                          ${listItemHomeAssistant}
                          <div slot="headline">Add to Home Assistant</div>
                        </ew-list-item>
                      `}
                  <ew-list-item
                    type="button"
                    @click=${() => {
        this._state = "DASHBOARD";
      }}
                  >
                    <div slot="start" class="fake-icon"></div>
                    <div slot="headline">Skip</div>
                  </ew-list-item>
                </ew-list>
              ` : ""}
        </div>

        ${!showSetupLinks ? x`
              <div slot="actions">
                <ew-text-button
                  @click=${() => {
        this._state = "DASHBOARD";
      }}
                >
                  Continue
                </ew-text-button>
              </div>
            ` : ""}
      `;
    } else {
      let error;
      switch (this._client.error) {
        case 3 /* ImprovSerialErrorState.UNABLE_TO_CONNECT */:
          error = "Unable to connect";
          break;
        case 254 /* ImprovSerialErrorState.TIMEOUT */:
          error = "Timeout";
          break;
        case 0 /* ImprovSerialErrorState.NO_ERROR */:
        // Happens when list SSIDs not supported.
        case 2 /* ImprovSerialErrorState.UNKNOWN_RPC_COMMAND */:
          break;
        default:
          error = `Unknown error (${this._client.error})`;
      }
      const selectedSsid = (_a = this._ssids) === null || _a === void 0 ? void 0 : _a.find(info => info.name === this._selectedSsid);
      content = x`
        <ew-icon-button slot="headline" @click=${this._updateSsids}>
          ${refreshIcon}
        </ew-icon-button>
        <div slot="content">
          <div>Connect your device to the network to start using it.</div>
          ${error ? x`<p class="error">${error}</p>` : ""}
          ${this._ssids !== null ? x`
                <ew-filled-select
                  menu-positioning="fixed"
                  label="Network"
                  @change=${ev => {
        const index = ev.target.selectedIndex;
        // The "Join Other" item is always the last item.
        this._selectedSsid = index === this._ssids.length ? null : this._ssids[index].name;
      }}
                >
                  ${this._ssids.map(info => x`
                      <ew-select-option
                        .selected=${selectedSsid === info}
                        .value=${info.name}
                      >
                        ${info.name}
                      </ew-select-option>
                    `)}
                  <ew-divider></ew-divider>
                  <ew-select-option .selected=${!selectedSsid}>
                    Join other…
                  </ew-select-option>
                </ew-filled-select>
              ` : ""}
          ${
      // Show input box if command not supported or "Join Other" selected
      !selectedSsid ? x`
                  <ew-filled-text-field
                    label="Network Name"
                    name="ssid"
                  ></ew-filled-text-field>
                ` : ""}
          ${!selectedSsid || selectedSsid.secured ? x`
                <ew-filled-text-field
                  label="Password"
                  name="password"
                  type="password"
                ></ew-filled-text-field>
              ` : ""}
        </div>
        <div slot="actions">
          <ew-text-button
            @click=${() => {
        this._state = "DASHBOARD";
      }}
          >
            ${this._installState && this._installErase ? "Skip" : "Back"}
          </ew-text-button>
          <ew-text-button @click=${this._doProvision}>Connect</ew-text-button>
        </div>
      `;
    }
    return [heading, content];
  }
  _renderAskErase() {
    const heading = "Erase device";
    const content = x`
      <div slot="content">
        <div>
          Do you want to erase the device before installing
          ${this._manifest.name}? All data on the device will be lost.
        </div>
        <label class="formfield">
          <ew-checkbox touch-target="wrapper" class="danger"></ew-checkbox>
          Erase device
        </label>
      </div>
      <div slot="actions">
        <ew-text-button
          @click=${() => {
      this._state = "DASHBOARD";
    }}
        >
          Back
        </ew-text-button>
        <ew-text-button
          @click=${() => {
      const checkbox = this.shadowRoot.querySelector("ew-checkbox");
      this._startInstall(checkbox.checked);
    }}
        >
          Next
        </ew-text-button>
      </div>
    `;
    return [heading, content];
  }
  _renderInstall() {
    let heading;
    let content;
    const allowClosing = false;
    const isUpdate = !this._installErase && this._isSameFirmware;
    if (!this._installConfirmed && this._isSameVersion) {
      heading = "Erase User Data";
      content = x`
        <div slot="content">
          Do you want to reset your device and erase all user data from your
          device?
        </div>
        <div slot="actions">
          <ew-text-button class="danger" @click=${this._confirmInstall}>
            Erase User Data
          </ew-text-button>
        </div>
      `;
    } else if (!this._installConfirmed) {
      heading = "Confirm Installation";
      const action = isUpdate ? "update to" : "install";
      content = x`
        <div slot="content">
          ${isUpdate ? x`Your device is running
                ${this._info.firmware}&nbsp;${this._info.version}.<br /><br />` : ""}
          Do you want to ${action}
          ${this._manifest.name}&nbsp;${this._manifest.version}?
          ${this._installErase ? x`<br /><br />All data on the device will be erased.` : ""}
        </div>
        <div slot="actions">
          <ew-text-button
            @click=${() => {
        this._state = "DASHBOARD";
      }}
          >
            Back
          </ew-text-button>
          <ew-text-button @click=${this._confirmInstall}>
            Install
          </ew-text-button>
        </div>
      `;
    } else if (!this._installState || this._installState.state === "initializing" /* FlashStateType.INITIALIZING */ || this._installState.state === "preparing" /* FlashStateType.PREPARING */) {
      heading = "Installing";
      content = this._renderProgress("Preparing installation");
    } else if (this._installState.state === "erasing" /* FlashStateType.ERASING */) {
      heading = "Installing";
      content = this._renderProgress("Erasing");
    } else if (this._installState.state === "writing" /* FlashStateType.WRITING */ ||
    // When we're finished, keep showing this screen with 100% written
    // until Improv is initialized / not detected.
    this._installState.state === "finished" /* FlashStateType.FINISHED */ && this._client === undefined) {
      heading = "Installing";
      let percentage;
      let undeterminateLabel;
      if (this._installState.state === "finished" /* FlashStateType.FINISHED */) {
        // We're done writing and detecting improv, show spinner
        undeterminateLabel = "Wrapping up";
      } else if (this._installState.details.percentage < 4) {
        // We're writing the firmware under 4%, show spinner or else we don't show any pixels
        undeterminateLabel = "Installing";
      } else {
        // We're writing the firmware over 4%, show progress bar
        percentage = this._installState.details.percentage;
      }
      content = this._renderProgress(x`
          ${undeterminateLabel ? x`${undeterminateLabel}<br />` : ""}
          <br />
          This will take
          ${this._installState.chipFamily === "ESP8266" ? "a minute" : "2 minutes"}.<br />
          Keep this page visible to prevent slow down
        `, percentage);
    } else if (this._installState.state === "finished" /* FlashStateType.FINISHED */) {
      heading = undefined;
      const supportsImprov = this._client !== null;
      content = x`
        <ewt-page-message
          slot="content"
          .icon=${OK_ICON}
          label="Installation complete!"
        ></ewt-page-message>

        <div slot="actions">
          <ew-text-button
            @click=${() => {
        this._state = supportsImprov && this._installErase ? "PROVISION" : "DASHBOARD";
      }}
          >
            Next
          </ew-text-button>
        </div>
      `;
    } else if (this._installState.state === "error" /* FlashStateType.ERROR */) {
      heading = "Installation failed";
      content = x`
        <ewt-page-message
          slot="content"
          .icon=${ERROR_ICON}
          .label=${this._installState.message}
        ></ewt-page-message>
        <div slot="actions">
          <ew-text-button
            @click=${async () => {
        this._initialize();
        this._state = "DASHBOARD";
      }}
          >
            Back
          </ew-text-button>
        </div>
      `;
    }
    return [heading, content, allowClosing];
  }
  _renderLogs() {
    let heading = `Logs`;
    let content;
    content = x`
      <div slot="content">
        <ewt-console .port=${this.port} .logger=${this.logger}></ewt-console>
      </div>
      <div slot="actions">
        <ew-text-button
          @click=${async () => {
      await this.shadowRoot.querySelector("ewt-console").reset();
    }}
        >
          Reset Device
        </ew-text-button>
        <ew-text-button
          @click=${() => {
      textDownload(this.shadowRoot.querySelector("ewt-console").logs(), `esp-web-tools-logs.txt`);
      this.shadowRoot.querySelector("ewt-console").reset();
    }}
        >
          Download Logs
        </ew-text-button>
        <ew-text-button
          @click=${async () => {
      await this.shadowRoot.querySelector("ewt-console").disconnect();
      this._state = "DASHBOARD";
      this._initialize();
    }}
        >
          Back
        </ew-text-button>
      </div>
    `;
    return [heading, content];
  }
  willUpdate(changedProps) {
    if (!changedProps.has("_state")) {
      return;
    }
    // Clear errors when changing between pages unless we change
    // to the error page.
    if (this._state !== "ERROR") {
      this._error = undefined;
    }
    // Scan for SSIDs on provision
    if (this._state === "PROVISION") {
      this._updateSsids();
    } else {
      // Reset this value if we leave provisioning.
      this._provisionForce = false;
    }
    if (this._state === "INSTALL") {
      this._installConfirmed = false;
      this._installState = undefined;
    }
  }
  async _updateSsids(tries = 0) {
    const oldSsids = this._ssids;
    this._ssids = undefined;
    this._busy = true;
    let ssids;
    try {
      ssids = await this._client.scan();
    } catch (err) {
      // When we fail while loading, pick "Join other"
      if (this._ssids === undefined) {
        this._ssids = null;
        this._selectedSsid = null;
      }
      this._busy = false;
      return;
    }
    // We will retry a few times if we don't get any results
    if (ssids.length === 0 && tries < 3) {
      console.log("SCHEDULE RETRY", tries);
      setTimeout(() => this._updateSsids(tries + 1), 1000);
      return;
    }
    if (oldSsids) {
      // If we had a previous list, ensure the selection is still valid
      if (this._selectedSsid && !ssids.find(s => s.name === this._selectedSsid)) {
        this._selectedSsid = ssids[0].name;
      }
    } else {
      this._selectedSsid = ssids.length ? ssids[0].name : null;
    }
    this._ssids = ssids;
    this._busy = false;
  }
  firstUpdated(changedProps) {
    super.firstUpdated(changedProps);
    this._bodyOverflow = document.body.style.overflow;
    document.body.style.overflow = "hidden";
    this._initialize();
  }
  updated(changedProps) {
    super.updated(changedProps);
    if (changedProps.has("_state")) {
      this.setAttribute("state", this._state);
    }
    if (this._state !== "PROVISION") {
      return;
    }
    if (changedProps.has("_selectedSsid") && this._selectedSsid === null) {
      // If we pick "Join other", select SSID input.
      this._focusFormElement("ew-filled-text-field[name=ssid]");
    } else if (changedProps.has("_ssids")) {
      // Form is shown when SSIDs are loaded/marked not supported
      this._focusFormElement();
    }
  }
  _focusFormElement(selector = "ew-filled-text-field, ew-filled-select") {
    const formEl = this.shadowRoot.querySelector(selector);
    if (formEl) {
      formEl.updateComplete.then(() => setTimeout(() => formEl.focus(), 100));
    }
  }
  async _initialize(justInstalled = false) {
    if (this.port.readable === null || this.port.writable === null) {
      this._state = "ERROR";
      this._error = "Serial port is not readable/writable. Close any other application using it and try again.";
      return;
    }
    try {
      this._manifest = await downloadManifest(this.manifestPath);
    } catch (err) {
      this._state = "ERROR";
      this._error = "Failed to download manifest";
      return;
    }
    if (this._manifest.new_install_improv_wait_time === 0) {
      this._client = null;
      return;
    }
    const client = new ImprovSerial(this.port, this.logger);
    client.addEventListener("state-changed", () => {
      this.requestUpdate();
    });
    client.addEventListener("error-changed", () => this.requestUpdate());
    try {
      // If a device was just installed, give new firmware 10 seconds (overridable) to
      // format the rest of the flash and do other stuff.
      const timeout = !justInstalled ? 1000 : this._manifest.new_install_improv_wait_time !== undefined ? this._manifest.new_install_improv_wait_time * 1000 : 10000;
      this._info = await client.initialize(timeout);
      this._client = client;
      client.addEventListener("disconnect", this._handleDisconnect);
    } catch (err) {
      // Clear old value
      this._info = undefined;
      if (err instanceof PortNotReady) {
        this._state = "ERROR";
        this._error = "Serial port is not ready. Close any other application using it and try again.";
      } else {
        this._client = null; // not supported
        this.logger.error("Improv initialization failed.", err);
      }
    }
  }
  _startInstall(erase) {
    this._state = "INSTALL";
    this._installErase = erase;
    this._installConfirmed = false;
  }
  async _confirmInstall() {
    this._installConfirmed = true;
    this._installState = undefined;
    if (this._client) {
      await this._closeClientWithoutEvents(this._client);
    }
    this._client = undefined;
    // Close port. ESPLoader likes opening it.
    await this.port.close();
    flash(state => {
      this._installState = state;
      if (state.state === "finished" /* FlashStateType.FINISHED */) {
        sleep$2(100)
        // Flashing closes the port
        .then(() => this.port.open({
          baudRate: 115200,
          bufferSize: 8192
        })).then(() => this._initialize(true)).then(() => this.requestUpdate());
      } else if (state.state === "error" /* FlashStateType.ERROR */) {
        sleep$2(100)
        // Flashing closes the port
        .then(() => this.port.open({
          baudRate: 115200,
          bufferSize: 8192
        }));
      }
    }, this.port, this.manifestPath, this._manifest, this._installErase);
  }
  async _doProvision() {
    var _a;
    this._busy = true;
    this._wasProvisioned = this._client.state === ImprovSerialCurrentState.PROVISIONED;
    const ssid = this._selectedSsid === null ? this.shadowRoot.querySelector("ew-filled-text-field[name=ssid]").value : this._selectedSsid;
    const password = ((_a = this.shadowRoot.querySelector("ew-filled-text-field[name=password]")) === null || _a === void 0 ? void 0 : _a.value) || "";
    try {
      await this._client.provision(ssid, password, 30000);
    } catch (err) {
      return;
    } finally {
      this._busy = false;
      this._provisionForce = false;
    }
  }
  _closeDialog() {
    this.shadowRoot.querySelector("ew-dialog").close();
  }
  async _handleClose() {
    if (this._client) {
      await this._closeClientWithoutEvents(this._client);
    }
    fireEvent(this, "closed");
    document.body.style.overflow = this._bodyOverflow;
    this.parentNode.removeChild(this);
  }
  /**
   * Return if the device runs same firmware as manifest.
   */
  get _isSameFirmware() {
    var _a;
    return !this._info ? false : ((_a = this.overrides) === null || _a === void 0 ? void 0 : _a.checkSameFirmware) ? this.overrides.checkSameFirmware(this._manifest, this._info) : this._info.firmware === this._manifest.name;
  }
  /**
   * Return if the device runs same firmware and version as manifest.
   */
  get _isSameVersion() {
    return this._isSameFirmware && this._info.version === this._manifest.version;
  }
  async _closeClientWithoutEvents(client) {
    client.removeEventListener("disconnect", this._handleDisconnect);
    await client.close();
  }
  _preventDefault(ev) {
    ev.preventDefault();
  }
}
EwtInstallDialog.styles = [dialogStyles, i$3`
      :host {
        --mdc-dialog-max-width: 390px;
      }
      div[slot="headline"] {
        padding-right: 48px;
      }
      ew-icon-button[slot="headline"] {
        position: absolute;
        right: 4px;
        top: 8px;
      }
      ew-icon-button[slot="headline"] svg {
        padding: 8px;
        color: var(--text-color);
      }
      .dialog-nav svg {
        color: var(--text-color);
      }
      .table-row {
        display: flex;
      }
      .table-row.last {
        margin-bottom: 16px;
      }
      .table-row svg {
        width: 20px;
        margin-right: 8px;
      }
      ew-filled-text-field,
      ew-filled-select {
        display: block;
        margin-top: 16px;
      }
      label.formfield {
        display: inline-flex;
        align-items: center;
        padding-right: 8px;
      }
      ew-list {
        margin: 0 -24px;
        padding: 0;
      }
      ew-list-item svg {
        height: 24px;
      }
      ewt-page-message + ew-list {
        padding-top: 16px;
      }
      .fake-icon {
        width: 24px;
      }
      .error {
        color: var(--danger-color);
      }
      .danger {
        --mdc-theme-primary: var(--danger-color);
        --mdc-theme-secondary: var(--danger-color);
        --md-sys-color-primary: var(--danger-color);
        --md-sys-color-on-surface: var(--danger-color);
      }
      button.link {
        background: none;
        color: inherit;
        border: none;
        padding: 0;
        font: inherit;
        text-align: left;
        text-decoration: underline;
        cursor: pointer;
      }
      :host([state="LOGS"]) ew-dialog {
        max-width: 90vw;
        max-height: 90vh;
      }
      ewt-console {
        width: calc(80vw - 48px);
        height: calc(90vh - 168px);
      }
    `];
__decorate([r$1()], EwtInstallDialog.prototype, "_client", void 0);
__decorate([r$1()], EwtInstallDialog.prototype, "_state", void 0);
__decorate([r$1()], EwtInstallDialog.prototype, "_installErase", void 0);
__decorate([r$1()], EwtInstallDialog.prototype, "_installConfirmed", void 0);
__decorate([r$1()], EwtInstallDialog.prototype, "_installState", void 0);
__decorate([r$1()], EwtInstallDialog.prototype, "_provisionForce", void 0);
__decorate([r$1()], EwtInstallDialog.prototype, "_error", void 0);
__decorate([r$1()], EwtInstallDialog.prototype, "_busy", void 0);
__decorate([r$1()], EwtInstallDialog.prototype, "_ssids", void 0);
__decorate([r$1()], EwtInstallDialog.prototype, "_selectedSsid", void 0);
customElements.define("ewt-install-dialog", EwtInstallDialog);

export { EwtInstallDialog };
