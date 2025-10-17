/******************************************************************************
Copyright (c) Microsoft Corporation.

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.
***************************************************************************** */
/* global Reflect, Promise, SuppressedError, Symbol, Iterator */

function __decorate(decorators, target, key, desc) {
  var c = arguments.length,
    r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc,
    d;
  if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
  return c > 3 && r && Object.defineProperty(target, key, r), r;
}
typeof SuppressedError === "function" ? SuppressedError : function (error, suppressed, message) {
  var e = new Error(message);
  return e.name = "SuppressedError", e.error = error, e.suppressed = suppressed, e;
};

/**
 * @license
 * Copyright 2019 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */
const t$3 = globalThis,
  e$6 = t$3.ShadowRoot && (void 0 === t$3.ShadyCSS || t$3.ShadyCSS.nativeShadow) && "adoptedStyleSheets" in Document.prototype && "replace" in CSSStyleSheet.prototype,
  s$2 = Symbol(),
  o$5 = new WeakMap();
let n$3 = class n {
  constructor(t, e, o) {
    if (this._$cssResult$ = true, o !== s$2) throw Error("CSSResult is not constructable. Use `unsafeCSS` or `css` instead.");
    this.cssText = t, this.t = e;
  }
  get styleSheet() {
    let t = this.o;
    const s = this.t;
    if (e$6 && void 0 === t) {
      const e = void 0 !== s && 1 === s.length;
      e && (t = o$5.get(s)), void 0 === t && ((this.o = t = new CSSStyleSheet()).replaceSync(this.cssText), e && o$5.set(s, t));
    }
    return t;
  }
  toString() {
    return this.cssText;
  }
};
const r$4 = t => new n$3("string" == typeof t ? t : t + "", void 0, s$2),
  i$4 = (t, ...e) => {
    const o = 1 === t.length ? t[0] : e.reduce((e, s, o) => e + (t => {
      if (true === t._$cssResult$) return t.cssText;
      if ("number" == typeof t) return t;
      throw Error("Value passed to 'css' function must be a 'css' function result: " + t + ". Use 'unsafeCSS' to pass non-literal values, but take care to ensure page security.");
    })(s) + t[o + 1], t[0]);
    return new n$3(o, t, s$2);
  },
  S$1 = (s, o) => {
    if (e$6) s.adoptedStyleSheets = o.map(t => t instanceof CSSStyleSheet ? t : t.styleSheet);else for (const e of o) {
      const o = document.createElement("style"),
        n = t$3.litNonce;
      void 0 !== n && o.setAttribute("nonce", n), o.textContent = e.cssText, s.appendChild(o);
    }
  },
  c$2 = e$6 ? t => t : t => t instanceof CSSStyleSheet ? (t => {
    let e = "";
    for (const s of t.cssRules) e += s.cssText;
    return r$4(e);
  })(t) : t;

/**
 * @license
 * Copyright 2017 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */
const {
    is: i$3,
    defineProperty: e$5,
    getOwnPropertyDescriptor: h$1,
    getOwnPropertyNames: r$3,
    getOwnPropertySymbols: o$4,
    getPrototypeOf: n$2
  } = Object,
  a$1 = globalThis,
  c$1 = a$1.trustedTypes,
  l$1 = c$1 ? c$1.emptyScript : "",
  p$1 = a$1.reactiveElementPolyfillSupport,
  d$1 = (t, s) => t,
  u$1 = {
    toAttribute(t, s) {
      switch (s) {
        case Boolean:
          t = t ? l$1 : null;
          break;
        case Object:
        case Array:
          t = null == t ? t : JSON.stringify(t);
      }
      return t;
    },
    fromAttribute(t, s) {
      let i = t;
      switch (s) {
        case Boolean:
          i = null !== t;
          break;
        case Number:
          i = null === t ? null : Number(t);
          break;
        case Object:
        case Array:
          try {
            i = JSON.parse(t);
          } catch (t) {
            i = null;
          }
      }
      return i;
    }
  },
  f$1 = (t, s) => !i$3(t, s),
  b$1 = {
    attribute: true,
    type: String,
    converter: u$1,
    reflect: false,
    useDefault: false,
    hasChanged: f$1
  };
Symbol.metadata ?? (Symbol.metadata = Symbol("metadata")), a$1.litPropertyMetadata ?? (a$1.litPropertyMetadata = new WeakMap());
let y$1 = class y extends HTMLElement {
  static addInitializer(t) {
    this._$Ei(), (this.l ?? (this.l = [])).push(t);
  }
  static get observedAttributes() {
    return this.finalize(), this._$Eh && [...this._$Eh.keys()];
  }
  static createProperty(t, s = b$1) {
    if (s.state && (s.attribute = false), this._$Ei(), this.prototype.hasOwnProperty(t) && ((s = Object.create(s)).wrapped = true), this.elementProperties.set(t, s), !s.noAccessor) {
      const i = Symbol(),
        h = this.getPropertyDescriptor(t, i, s);
      void 0 !== h && e$5(this.prototype, t, h);
    }
  }
  static getPropertyDescriptor(t, s, i) {
    const {
      get: e,
      set: r
    } = h$1(this.prototype, t) ?? {
      get() {
        return this[s];
      },
      set(t) {
        this[s] = t;
      }
    };
    return {
      get: e,
      set(s) {
        const h = e === null || e === void 0 ? void 0 : e.call(this);
        r !== null && r !== void 0 && r.call(this, s), this.requestUpdate(t, h, i);
      },
      configurable: true,
      enumerable: true
    };
  }
  static getPropertyOptions(t) {
    return this.elementProperties.get(t) ?? b$1;
  }
  static _$Ei() {
    if (this.hasOwnProperty(d$1("elementProperties"))) return;
    const t = n$2(this);
    t.finalize(), void 0 !== t.l && (this.l = [...t.l]), this.elementProperties = new Map(t.elementProperties);
  }
  static finalize() {
    if (this.hasOwnProperty(d$1("finalized"))) return;
    if (this.finalized = true, this._$Ei(), this.hasOwnProperty(d$1("properties"))) {
      const t = this.properties,
        s = [...r$3(t), ...o$4(t)];
      for (const i of s) this.createProperty(i, t[i]);
    }
    const t = this[Symbol.metadata];
    if (null !== t) {
      const s = litPropertyMetadata.get(t);
      if (void 0 !== s) for (const [t, i] of s) this.elementProperties.set(t, i);
    }
    this._$Eh = new Map();
    for (const [t, s] of this.elementProperties) {
      const i = this._$Eu(t, s);
      void 0 !== i && this._$Eh.set(i, t);
    }
    this.elementStyles = this.finalizeStyles(this.styles);
  }
  static finalizeStyles(s) {
    const i = [];
    if (Array.isArray(s)) {
      const e = new Set(s.flat(1 / 0).reverse());
      for (const s of e) i.unshift(c$2(s));
    } else void 0 !== s && i.push(c$2(s));
    return i;
  }
  static _$Eu(t, s) {
    const i = s.attribute;
    return false === i ? void 0 : "string" == typeof i ? i : "string" == typeof t ? t.toLowerCase() : void 0;
  }
  constructor() {
    super(), this._$Ep = void 0, this.isUpdatePending = false, this.hasUpdated = false, this._$Em = null, this._$Ev();
  }
  _$Ev() {
    var _this$constructor$l;
    this._$ES = new Promise(t => this.enableUpdating = t), this._$AL = new Map(), this._$E_(), this.requestUpdate(), (_this$constructor$l = this.constructor.l) === null || _this$constructor$l === void 0 ? void 0 : _this$constructor$l.forEach(t => t(this));
  }
  addController(t) {
    var _t$hostConnected;
    (this._$EO ?? (this._$EO = new Set())).add(t), void 0 !== this.renderRoot && this.isConnected && ((_t$hostConnected = t.hostConnected) === null || _t$hostConnected === void 0 ? void 0 : _t$hostConnected.call(t));
  }
  removeController(t) {
    var _this$_$EO;
    (_this$_$EO = this._$EO) === null || _this$_$EO === void 0 || _this$_$EO.delete(t);
  }
  _$E_() {
    const t = new Map(),
      s = this.constructor.elementProperties;
    for (const i of s.keys()) this.hasOwnProperty(i) && (t.set(i, this[i]), delete this[i]);
    t.size > 0 && (this._$Ep = t);
  }
  createRenderRoot() {
    const t = this.shadowRoot ?? this.attachShadow(this.constructor.shadowRootOptions);
    return S$1(t, this.constructor.elementStyles), t;
  }
  connectedCallback() {
    var _this$_$EO2;
    this.renderRoot ?? (this.renderRoot = this.createRenderRoot()), this.enableUpdating(true), (_this$_$EO2 = this._$EO) === null || _this$_$EO2 === void 0 ? void 0 : _this$_$EO2.forEach(t => {
      var _t$hostConnected2;
      return (_t$hostConnected2 = t.hostConnected) === null || _t$hostConnected2 === void 0 ? void 0 : _t$hostConnected2.call(t);
    });
  }
  enableUpdating(t) {}
  disconnectedCallback() {
    var _this$_$EO3;
    (_this$_$EO3 = this._$EO) === null || _this$_$EO3 === void 0 || _this$_$EO3.forEach(t => {
      var _t$hostDisconnected;
      return (_t$hostDisconnected = t.hostDisconnected) === null || _t$hostDisconnected === void 0 ? void 0 : _t$hostDisconnected.call(t);
    });
  }
  attributeChangedCallback(t, s, i) {
    this._$AK(t, i);
  }
  _$ET(t, s) {
    const i = this.constructor.elementProperties.get(t),
      e = this.constructor._$Eu(t, i);
    if (void 0 !== e && true === i.reflect) {
      var _i$converter;
      const h = (void 0 !== ((_i$converter = i.converter) === null || _i$converter === void 0 ? void 0 : _i$converter.toAttribute) ? i.converter : u$1).toAttribute(s, i.type);
      this._$Em = t, null == h ? this.removeAttribute(e) : this.setAttribute(e, h), this._$Em = null;
    }
  }
  _$AK(t, s) {
    const i = this.constructor,
      e = i._$Eh.get(t);
    if (void 0 !== e && this._$Em !== e) {
      var _t$converter, _this$_$Ej;
      const t = i.getPropertyOptions(e),
        h = "function" == typeof t.converter ? {
          fromAttribute: t.converter
        } : void 0 !== ((_t$converter = t.converter) === null || _t$converter === void 0 ? void 0 : _t$converter.fromAttribute) ? t.converter : u$1;
      this._$Em = e, this[e] = h.fromAttribute(s, t.type) ?? ((_this$_$Ej = this._$Ej) === null || _this$_$Ej === void 0 ? void 0 : _this$_$Ej.get(e)) ?? null, this._$Em = null;
    }
  }
  requestUpdate(t, s, i) {
    if (void 0 !== t) {
      var _this$_$Ej2;
      const e = this.constructor,
        h = this[t];
      if (i ?? (i = e.getPropertyOptions(t)), !((i.hasChanged ?? f$1)(h, s) || i.useDefault && i.reflect && h === ((_this$_$Ej2 = this._$Ej) === null || _this$_$Ej2 === void 0 ? void 0 : _this$_$Ej2.get(t)) && !this.hasAttribute(e._$Eu(t, i)))) return;
      this.C(t, s, i);
    }
    false === this.isUpdatePending && (this._$ES = this._$EP());
  }
  C(t, s, {
    useDefault: i,
    reflect: e,
    wrapped: h
  }, r) {
    i && !(this._$Ej ?? (this._$Ej = new Map())).has(t) && (this._$Ej.set(t, r ?? s ?? this[t]), true !== h || void 0 !== r) || (this._$AL.has(t) || (this.hasUpdated || i || (s = void 0), this._$AL.set(t, s)), true === e && this._$Em !== t && (this._$Eq ?? (this._$Eq = new Set())).add(t));
  }
  async _$EP() {
    this.isUpdatePending = true;
    try {
      await this._$ES;
    } catch (t) {
      Promise.reject(t);
    }
    const t = this.scheduleUpdate();
    return null != t && (await t), !this.isUpdatePending;
  }
  scheduleUpdate() {
    return this.performUpdate();
  }
  performUpdate() {
    if (!this.isUpdatePending) return;
    if (!this.hasUpdated) {
      if (this.renderRoot ?? (this.renderRoot = this.createRenderRoot()), this._$Ep) {
        for (const [t, s] of this._$Ep) this[t] = s;
        this._$Ep = void 0;
      }
      const t = this.constructor.elementProperties;
      if (t.size > 0) for (const [s, i] of t) {
        const {
            wrapped: t
          } = i,
          e = this[s];
        true !== t || this._$AL.has(s) || void 0 === e || this.C(s, void 0, i, e);
      }
    }
    let t = false;
    const s = this._$AL;
    try {
      var _this$_$EO4;
      t = this.shouldUpdate(s), t ? (this.willUpdate(s), (_this$_$EO4 = this._$EO) !== null && _this$_$EO4 !== void 0 && _this$_$EO4.forEach(t => {
        var _t$hostUpdate;
        return (_t$hostUpdate = t.hostUpdate) === null || _t$hostUpdate === void 0 ? void 0 : _t$hostUpdate.call(t);
      }), this.update(s)) : this._$EM();
    } catch (s) {
      throw t = false, this._$EM(), s;
    }
    t && this._$AE(s);
  }
  willUpdate(t) {}
  _$AE(t) {
    var _this$_$EO5;
    (_this$_$EO5 = this._$EO) !== null && _this$_$EO5 !== void 0 && _this$_$EO5.forEach(t => {
      var _t$hostUpdated;
      return (_t$hostUpdated = t.hostUpdated) === null || _t$hostUpdated === void 0 ? void 0 : _t$hostUpdated.call(t);
    }), this.hasUpdated || (this.hasUpdated = true, this.firstUpdated(t)), this.updated(t);
  }
  _$EM() {
    this._$AL = new Map(), this.isUpdatePending = false;
  }
  get updateComplete() {
    return this.getUpdateComplete();
  }
  getUpdateComplete() {
    return this._$ES;
  }
  shouldUpdate(t) {
    return true;
  }
  update(t) {
    this._$Eq && (this._$Eq = this._$Eq.forEach(t => this._$ET(t, this[t]))), this._$EM();
  }
  updated(t) {}
  firstUpdated(t) {}
};
y$1.elementStyles = [], y$1.shadowRootOptions = {
  mode: "open"
}, y$1[d$1("elementProperties")] = new Map(), y$1[d$1("finalized")] = new Map(), p$1 !== null && p$1 !== void 0 && p$1({
  ReactiveElement: y$1
}), (a$1.reactiveElementVersions ?? (a$1.reactiveElementVersions = [])).push("2.1.0");

/**
 * @license
 * Copyright 2017 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */
const t$2 = globalThis,
  i$2 = t$2.trustedTypes,
  s$1 = i$2 ? i$2.createPolicy("lit-html", {
    createHTML: t => t
  }) : void 0,
  e$4 = "$lit$",
  h = `lit$${Math.random().toFixed(9).slice(2)}$`,
  o$3 = "?" + h,
  n$1 = `<${o$3}>`,
  r$2 = document,
  l = () => r$2.createComment(""),
  c = t => null === t || "object" != typeof t && "function" != typeof t,
  a = Array.isArray,
  u = t => a(t) || "function" == typeof (t === null || t === void 0 ? void 0 : t[Symbol.iterator]),
  d = "[ \t\n\f\r]",
  f = /<(?:(!--|\/[^a-zA-Z])|(\/?[a-zA-Z][^>\s]*)|(\/?$))/g,
  v = /-->/g,
  _ = />/g,
  m = RegExp(`>|${d}(?:([^\\s"'>=/]+)(${d}*=${d}*(?:[^ \t\n\f\r"'\`<>=]|("|')|))|$)`, "g"),
  p = /'/g,
  g = /"/g,
  $ = /^(?:script|style|textarea|title)$/i,
  y = t => (i, ...s) => ({
    _$litType$: t,
    strings: i,
    values: s
  }),
  x = y(1),
  b = y(2),
  T = Symbol.for("lit-noChange"),
  E = Symbol.for("lit-nothing"),
  A = new WeakMap(),
  C = r$2.createTreeWalker(r$2, 129);
function P(t, i) {
  if (!a(t) || !t.hasOwnProperty("raw")) throw Error("invalid template strings array");
  return void 0 !== s$1 ? s$1.createHTML(i) : i;
}
const V = (t, i) => {
  const s = t.length - 1,
    o = [];
  let r,
    l = 2 === i ? "<svg>" : 3 === i ? "<math>" : "",
    c = f;
  for (let i = 0; i < s; i++) {
    const s = t[i];
    let a,
      u,
      d = -1,
      y = 0;
    for (; y < s.length && (c.lastIndex = y, u = c.exec(s), null !== u);) y = c.lastIndex, c === f ? "!--" === u[1] ? c = v : void 0 !== u[1] ? c = _ : void 0 !== u[2] ? ($.test(u[2]) && (r = RegExp("</" + u[2], "g")), c = m) : void 0 !== u[3] && (c = m) : c === m ? ">" === u[0] ? (c = r ?? f, d = -1) : void 0 === u[1] ? d = -2 : (d = c.lastIndex - u[2].length, a = u[1], c = void 0 === u[3] ? m : '"' === u[3] ? g : p) : c === g || c === p ? c = m : c === v || c === _ ? c = f : (c = m, r = void 0);
    const x = c === m && t[i + 1].startsWith("/>") ? " " : "";
    l += c === f ? s + n$1 : d >= 0 ? (o.push(a), s.slice(0, d) + e$4 + s.slice(d) + h + x) : s + h + (-2 === d ? i : x);
  }
  return [P(t, l + (t[s] || "<?>") + (2 === i ? "</svg>" : 3 === i ? "</math>" : "")), o];
};
class N {
  constructor({
    strings: t,
    _$litType$: s
  }, n) {
    let r;
    this.parts = [];
    let c = 0,
      a = 0;
    const u = t.length - 1,
      d = this.parts,
      [f, v] = V(t, s);
    if (this.el = N.createElement(f, n), C.currentNode = this.el.content, 2 === s || 3 === s) {
      const t = this.el.content.firstChild;
      t.replaceWith(...t.childNodes);
    }
    for (; null !== (r = C.nextNode()) && d.length < u;) {
      if (1 === r.nodeType) {
        if (r.hasAttributes()) for (const t of r.getAttributeNames()) if (t.endsWith(e$4)) {
          const i = v[a++],
            s = r.getAttribute(t).split(h),
            e = /([.?@])?(.*)/.exec(i);
          d.push({
            type: 1,
            index: c,
            name: e[2],
            strings: s,
            ctor: "." === e[1] ? H : "?" === e[1] ? I : "@" === e[1] ? L : k
          }), r.removeAttribute(t);
        } else t.startsWith(h) && (d.push({
          type: 6,
          index: c
        }), r.removeAttribute(t));
        if ($.test(r.tagName)) {
          const t = r.textContent.split(h),
            s = t.length - 1;
          if (s > 0) {
            r.textContent = i$2 ? i$2.emptyScript : "";
            for (let i = 0; i < s; i++) r.append(t[i], l()), C.nextNode(), d.push({
              type: 2,
              index: ++c
            });
            r.append(t[s], l());
          }
        }
      } else if (8 === r.nodeType) if (r.data === o$3) d.push({
        type: 2,
        index: c
      });else {
        let t = -1;
        for (; -1 !== (t = r.data.indexOf(h, t + 1));) d.push({
          type: 7,
          index: c
        }), t += h.length - 1;
      }
      c++;
    }
  }
  static createElement(t, i) {
    const s = r$2.createElement("template");
    return s.innerHTML = t, s;
  }
}
function S(t, i, s = t, e) {
  var _s$_$Co, _h, _h2, _h2$_$AO;
  if (i === T) return i;
  let h = void 0 !== e ? (_s$_$Co = s._$Co) === null || _s$_$Co === void 0 ? void 0 : _s$_$Co[e] : s._$Cl;
  const o = c(i) ? void 0 : i._$litDirective$;
  return ((_h = h) === null || _h === void 0 ? void 0 : _h.constructor) !== o && ((_h2 = h) !== null && _h2 !== void 0 && (_h2$_$AO = _h2._$AO) !== null && _h2$_$AO !== void 0 && _h2$_$AO.call(_h2, false), void 0 === o ? h = void 0 : (h = new o(t), h._$AT(t, s, e)), void 0 !== e ? (s._$Co ?? (s._$Co = []))[e] = h : s._$Cl = h), void 0 !== h && (i = S(t, h._$AS(t, i.values), h, e)), i;
}
class M {
  constructor(t, i) {
    this._$AV = [], this._$AN = void 0, this._$AD = t, this._$AM = i;
  }
  get parentNode() {
    return this._$AM.parentNode;
  }
  get _$AU() {
    return this._$AM._$AU;
  }
  u(t) {
    const {
        el: {
          content: i
        },
        parts: s
      } = this._$AD,
      e = ((t === null || t === void 0 ? void 0 : t.creationScope) ?? r$2).importNode(i, true);
    C.currentNode = e;
    let h = C.nextNode(),
      o = 0,
      n = 0,
      l = s[0];
    for (; void 0 !== l;) {
      var _l;
      if (o === l.index) {
        let i;
        2 === l.type ? i = new R(h, h.nextSibling, this, t) : 1 === l.type ? i = new l.ctor(h, l.name, l.strings, this, t) : 6 === l.type && (i = new z(h, this, t)), this._$AV.push(i), l = s[++n];
      }
      o !== ((_l = l) === null || _l === void 0 ? void 0 : _l.index) && (h = C.nextNode(), o++);
    }
    return C.currentNode = r$2, e;
  }
  p(t) {
    let i = 0;
    for (const s of this._$AV) void 0 !== s && (void 0 !== s.strings ? (s._$AI(t, s, i), i += s.strings.length - 2) : s._$AI(t[i])), i++;
  }
}
class R {
  get _$AU() {
    var _this$_$AM;
    return ((_this$_$AM = this._$AM) === null || _this$_$AM === void 0 ? void 0 : _this$_$AM._$AU) ?? this._$Cv;
  }
  constructor(t, i, s, e) {
    this.type = 2, this._$AH = E, this._$AN = void 0, this._$AA = t, this._$AB = i, this._$AM = s, this.options = e, this._$Cv = (e === null || e === void 0 ? void 0 : e.isConnected) ?? true;
  }
  get parentNode() {
    var _t;
    let t = this._$AA.parentNode;
    const i = this._$AM;
    return void 0 !== i && 11 === ((_t = t) === null || _t === void 0 ? void 0 : _t.nodeType) && (t = i.parentNode), t;
  }
  get startNode() {
    return this._$AA;
  }
  get endNode() {
    return this._$AB;
  }
  _$AI(t, i = this) {
    t = S(this, t, i), c(t) ? t === E || null == t || "" === t ? (this._$AH !== E && this._$AR(), this._$AH = E) : t !== this._$AH && t !== T && this._(t) : void 0 !== t._$litType$ ? this.$(t) : void 0 !== t.nodeType ? this.T(t) : u(t) ? this.k(t) : this._(t);
  }
  O(t) {
    return this._$AA.parentNode.insertBefore(t, this._$AB);
  }
  T(t) {
    this._$AH !== t && (this._$AR(), this._$AH = this.O(t));
  }
  _(t) {
    this._$AH !== E && c(this._$AH) ? this._$AA.nextSibling.data = t : this.T(r$2.createTextNode(t)), this._$AH = t;
  }
  $(t) {
    var _this$_$AH;
    const {
        values: i,
        _$litType$: s
      } = t,
      e = "number" == typeof s ? this._$AC(t) : (void 0 === s.el && (s.el = N.createElement(P(s.h, s.h[0]), this.options)), s);
    if (((_this$_$AH = this._$AH) === null || _this$_$AH === void 0 ? void 0 : _this$_$AH._$AD) === e) this._$AH.p(i);else {
      const t = new M(e, this),
        s = t.u(this.options);
      t.p(i), this.T(s), this._$AH = t;
    }
  }
  _$AC(t) {
    let i = A.get(t.strings);
    return void 0 === i && A.set(t.strings, i = new N(t)), i;
  }
  k(t) {
    a(this._$AH) || (this._$AH = [], this._$AR());
    const i = this._$AH;
    let s,
      e = 0;
    for (const h of t) e === i.length ? i.push(s = new R(this.O(l()), this.O(l()), this, this.options)) : s = i[e], s._$AI(h), e++;
    e < i.length && (this._$AR(s && s._$AB.nextSibling, e), i.length = e);
  }
  _$AR(t = this._$AA.nextSibling, i) {
    for ((_this$_$AP = this._$AP) === null || _this$_$AP === void 0 ? void 0 : _this$_$AP.call(this, false, true, i); t && t !== this._$AB;) {
      var _this$_$AP;
      const i = t.nextSibling;
      t.remove(), t = i;
    }
  }
  setConnected(t) {
    var _this$_$AP2;
    void 0 === this._$AM && (this._$Cv = t, (_this$_$AP2 = this._$AP) === null || _this$_$AP2 === void 0 ? void 0 : _this$_$AP2.call(this, t));
  }
}
class k {
  get tagName() {
    return this.element.tagName;
  }
  get _$AU() {
    return this._$AM._$AU;
  }
  constructor(t, i, s, e, h) {
    this.type = 1, this._$AH = E, this._$AN = void 0, this.element = t, this.name = i, this._$AM = e, this.options = h, s.length > 2 || "" !== s[0] || "" !== s[1] ? (this._$AH = Array(s.length - 1).fill(new String()), this.strings = s) : this._$AH = E;
  }
  _$AI(t, i = this, s, e) {
    const h = this.strings;
    let o = false;
    if (void 0 === h) t = S(this, t, i, 0), o = !c(t) || t !== this._$AH && t !== T, o && (this._$AH = t);else {
      const e = t;
      let n, r;
      for (t = h[0], n = 0; n < h.length - 1; n++) r = S(this, e[s + n], i, n), r === T && (r = this._$AH[n]), o || (o = !c(r) || r !== this._$AH[n]), r === E ? t = E : t !== E && (t += (r ?? "") + h[n + 1]), this._$AH[n] = r;
    }
    o && !e && this.j(t);
  }
  j(t) {
    t === E ? this.element.removeAttribute(this.name) : this.element.setAttribute(this.name, t ?? "");
  }
}
class H extends k {
  constructor() {
    super(...arguments), this.type = 3;
  }
  j(t) {
    this.element[this.name] = t === E ? void 0 : t;
  }
}
class I extends k {
  constructor() {
    super(...arguments), this.type = 4;
  }
  j(t) {
    this.element.toggleAttribute(this.name, !!t && t !== E);
  }
}
class L extends k {
  constructor(t, i, s, e, h) {
    super(t, i, s, e, h), this.type = 5;
  }
  _$AI(t, i = this) {
    if ((t = S(this, t, i, 0) ?? E) === T) return;
    const s = this._$AH,
      e = t === E && s !== E || t.capture !== s.capture || t.once !== s.once || t.passive !== s.passive,
      h = t !== E && (s === E || e);
    e && this.element.removeEventListener(this.name, this, s), h && this.element.addEventListener(this.name, this, t), this._$AH = t;
  }
  handleEvent(t) {
    var _this$options;
    "function" == typeof this._$AH ? this._$AH.call(((_this$options = this.options) === null || _this$options === void 0 ? void 0 : _this$options.host) ?? this.element, t) : this._$AH.handleEvent(t);
  }
}
class z {
  constructor(t, i, s) {
    this.element = t, this.type = 6, this._$AN = void 0, this._$AM = i, this.options = s;
  }
  get _$AU() {
    return this._$AM._$AU;
  }
  _$AI(t) {
    S(this, t);
  }
}
const j = t$2.litHtmlPolyfillSupport;
j !== null && j !== void 0 && j(N, R), (t$2.litHtmlVersions ?? (t$2.litHtmlVersions = [])).push("3.3.0");
const B = (t, i, s) => {
  const e = (s === null || s === void 0 ? void 0 : s.renderBefore) ?? i;
  let h = e._$litPart$;
  if (void 0 === h) {
    const t = (s === null || s === void 0 ? void 0 : s.renderBefore) ?? null;
    e._$litPart$ = h = new R(i.insertBefore(l(), t), t, void 0, s ?? {});
  }
  return h._$AI(t), h;
};

var _s$litElementHydrateS;
/**
 * @license
 * Copyright 2017 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */
const s = globalThis;
let i$1 = class i extends y$1 {
  constructor() {
    super(...arguments), this.renderOptions = {
      host: this
    }, this._$Do = void 0;
  }
  createRenderRoot() {
    var _this$renderOptions;
    const t = super.createRenderRoot();
    return (_this$renderOptions = this.renderOptions).renderBefore ?? (_this$renderOptions.renderBefore = t.firstChild), t;
  }
  update(t) {
    const r = this.render();
    this.hasUpdated || (this.renderOptions.isConnected = this.isConnected), super.update(t), this._$Do = B(r, this.renderRoot, this.renderOptions);
  }
  connectedCallback() {
    var _this$_$Do;
    super.connectedCallback(), (_this$_$Do = this._$Do) === null || _this$_$Do === void 0 ? void 0 : _this$_$Do.setConnected(true);
  }
  disconnectedCallback() {
    var _this$_$Do2;
    super.disconnectedCallback(), (_this$_$Do2 = this._$Do) === null || _this$_$Do2 === void 0 ? void 0 : _this$_$Do2.setConnected(false);
  }
  render() {
    return T;
  }
};
i$1._$litElement$ = true, i$1["finalized"] = true, (_s$litElementHydrateS = s.litElementHydrateSupport) === null || _s$litElementHydrateS === void 0 ? void 0 : _s$litElementHydrateS.call(s, {
  LitElement: i$1
});
const o$2 = s.litElementPolyfillSupport;
o$2 === null || o$2 === void 0 || o$2({
  LitElement: i$1
});
(s.litElementVersions ?? (s.litElementVersions = [])).push("4.2.0");

/**
 * @license
 * Copyright 2017 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */
const t$1 = t => (e, o) => {
  void 0 !== o ? o.addInitializer(() => {
    customElements.define(t, e);
  }) : customElements.define(t, e);
};

/**
 * @license
 * Copyright 2017 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */
const o$1 = {
    attribute: true,
    type: String,
    converter: u$1,
    reflect: false,
    hasChanged: f$1
  },
  r$1 = (t = o$1, e, r) => {
    const {
      kind: n,
      metadata: i
    } = r;
    let s = globalThis.litPropertyMetadata.get(i);
    if (void 0 === s && globalThis.litPropertyMetadata.set(i, s = new Map()), "setter" === n && ((t = Object.create(t)).wrapped = true), s.set(r.name, t), "accessor" === n) {
      const {
        name: o
      } = r;
      return {
        set(r) {
          const n = e.get.call(this);
          e.set.call(this, r), this.requestUpdate(o, n, t);
        },
        init(e) {
          return void 0 !== e && this.C(o, void 0, t, e), e;
        }
      };
    }
    if ("setter" === n) {
      const {
        name: o
      } = r;
      return function (r) {
        const n = this[o];
        e.call(this, r), this.requestUpdate(o, n, t);
      };
    }
    throw Error("Unsupported decorator location: " + n);
  };
function n(t) {
  return (e, o) => "object" == typeof o ? r$1(t, e, o) : ((t, e, o) => {
    const r = e.hasOwnProperty(o);
    return e.constructor.createProperty(o, t), r ? Object.getOwnPropertyDescriptor(e, o) : void 0;
  })(t, e, o);
}

/**
 * @license
 * Copyright 2017 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */
function r(r) {
  return n({
    ...r,
    state: true,
    attribute: false
  });
}

/**
 * @license
 * Copyright 2017 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */
const e$3 = (e, t, c) => (c.configurable = true, c.enumerable = true, Reflect.decorate && "object" != typeof t && Object.defineProperty(e, t, c), c);

/**
 * @license
 * Copyright 2017 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */
function e$2(e, r) {
  return (n, s, i) => {
    const o = t => {
      var _t$renderRoot;
      return ((_t$renderRoot = t.renderRoot) === null || _t$renderRoot === void 0 ? void 0 : _t$renderRoot.querySelector(e)) ?? null;
    };
    return e$3(n, s, {
      get() {
        return o(this);
      }
    });
  };
}

/**
 * @license
 * Copyright 2021 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */
function o(o) {
  return (e, n) => {
    const {
        slot: r,
        selector: s
      } = o ?? {},
      c = "slot" + (r ? `[name=${r}]` : ":not([name])");
    return e$3(e, n, {
      get() {
        var _this$renderRoot;
        const t = (_this$renderRoot = this.renderRoot) === null || _this$renderRoot === void 0 ? void 0 : _this$renderRoot.querySelector(c),
          e = (t === null || t === void 0 ? void 0 : t.assignedElements(o)) ?? [];
        return void 0 === s ? e : e.filter(t => t.matches(s));
      }
    });
  };
}

/**
 * @license
 * Copyright 2024 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Generated stylesheet for ./button/internal/shared-styles.css.
const styles$5 = i$4`:host{border-start-start-radius:var(--_container-shape-start-start);border-start-end-radius:var(--_container-shape-start-end);border-end-start-radius:var(--_container-shape-end-start);border-end-end-radius:var(--_container-shape-end-end);box-sizing:border-box;cursor:pointer;display:inline-flex;gap:8px;min-height:var(--_container-height);outline:none;padding-block:calc((var(--_container-height) - max(var(--_label-text-line-height),var(--_icon-size)))/2);padding-inline-start:var(--_leading-space);padding-inline-end:var(--_trailing-space);place-content:center;place-items:center;position:relative;font-family:var(--_label-text-font);font-size:var(--_label-text-size);line-height:var(--_label-text-line-height);font-weight:var(--_label-text-weight);text-overflow:ellipsis;text-wrap:nowrap;user-select:none;-webkit-tap-highlight-color:rgba(0,0,0,0);vertical-align:top;--md-ripple-hover-color: var(--_hover-state-layer-color);--md-ripple-pressed-color: var(--_pressed-state-layer-color);--md-ripple-hover-opacity: var(--_hover-state-layer-opacity);--md-ripple-pressed-opacity: var(--_pressed-state-layer-opacity)}md-focus-ring{--md-focus-ring-shape-start-start: var(--_container-shape-start-start);--md-focus-ring-shape-start-end: var(--_container-shape-start-end);--md-focus-ring-shape-end-end: var(--_container-shape-end-end);--md-focus-ring-shape-end-start: var(--_container-shape-end-start)}:host(:is([disabled],[soft-disabled])){cursor:default;pointer-events:none}.button{border-radius:inherit;cursor:inherit;display:inline-flex;align-items:center;justify-content:center;border:none;outline:none;-webkit-appearance:none;vertical-align:middle;background:rgba(0,0,0,0);text-decoration:none;min-width:calc(64px - var(--_leading-space) - var(--_trailing-space));width:100%;z-index:0;height:100%;font:inherit;color:var(--_label-text-color);padding:0;gap:inherit;text-transform:inherit}.button::-moz-focus-inner{padding:0;border:0}:host(:hover) .button{color:var(--_hover-label-text-color)}:host(:focus-within) .button{color:var(--_focus-label-text-color)}:host(:active) .button{color:var(--_pressed-label-text-color)}.background{background-color:var(--_container-color);border-radius:inherit;inset:0;position:absolute}.label{overflow:hidden}:is(.button,.label,.label slot),.label ::slotted(*){text-overflow:inherit}:host(:is([disabled],[soft-disabled])) .label{color:var(--_disabled-label-text-color);opacity:var(--_disabled-label-text-opacity)}:host(:is([disabled],[soft-disabled])) .background{background-color:var(--_disabled-container-color);opacity:var(--_disabled-container-opacity)}@media(forced-colors: active){.background{border:1px solid CanvasText}:host(:is([disabled],[soft-disabled])){--_disabled-icon-color: GrayText;--_disabled-icon-opacity: 1;--_disabled-container-opacity: 1;--_disabled-label-text-color: GrayText;--_disabled-label-text-opacity: 1}}:host([has-icon]:not([trailing-icon])){padding-inline-start:var(--_with-leading-icon-leading-space);padding-inline-end:var(--_with-leading-icon-trailing-space)}:host([has-icon][trailing-icon]){padding-inline-start:var(--_with-trailing-icon-leading-space);padding-inline-end:var(--_with-trailing-icon-trailing-space)}::slotted([slot=icon]){display:inline-flex;position:relative;writing-mode:horizontal-tb;fill:currentColor;flex-shrink:0;color:var(--_icon-color);font-size:var(--_icon-size);inline-size:var(--_icon-size);block-size:var(--_icon-size)}:host(:hover) ::slotted([slot=icon]){color:var(--_hover-icon-color)}:host(:focus-within) ::slotted([slot=icon]){color:var(--_focus-icon-color)}:host(:active) ::slotted([slot=icon]){color:var(--_pressed-icon-color)}:host(:is([disabled],[soft-disabled])) ::slotted([slot=icon]){color:var(--_disabled-icon-color);opacity:var(--_disabled-icon-opacity)}.touch{position:absolute;top:50%;height:48px;left:0;right:0;transform:translateY(-50%)}:host([touch-target=wrapper]){margin:max(0px,(48px - var(--_container-height))/2) 0}:host([touch-target=none]) .touch{display:none}
`;

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * A key to retrieve an `Attachable` element's `AttachableController` from a
 * global `MutationObserver`.
 */
const ATTACHABLE_CONTROLLER = Symbol('attachableController');
let FOR_ATTRIBUTE_OBSERVER;
{
  /**
   * A global `MutationObserver` that reacts to `for` attribute changes on
   * `Attachable` elements. If the `for` attribute changes, the controller will
   * re-attach to the new referenced element.
   */
  FOR_ATTRIBUTE_OBSERVER = new MutationObserver(records => {
    for (const record of records) {
      var _record$target$ATTACH;
      // When a control's `for` attribute changes, inform its
      // `AttachableController` to update to a new control.
      (_record$target$ATTACH = record.target[ATTACHABLE_CONTROLLER]) === null || _record$target$ATTACH === void 0 || _record$target$ATTACH.hostConnected();
    }
  });
}
/**
 * A controller that provides an implementation for `Attachable` elements.
 *
 * @example
 * ```ts
 * class MyElement extends LitElement implements Attachable {
 *   get control() { return this.attachableController.control; }
 *
 *   private readonly attachableController = new AttachableController(
 *     this,
 *     (previousControl, newControl) => {
 *       previousControl?.removeEventListener('click', this.handleClick);
 *       newControl?.addEventListener('click', this.handleClick);
 *     }
 *   );
 *
 *   // Implement remaining `Attachable` properties/methods that call the
 *   // controller's properties/methods.
 * }
 * ```
 */
class AttachableController {
  get htmlFor() {
    return this.host.getAttribute('for');
  }
  set htmlFor(htmlFor) {
    if (htmlFor === null) {
      this.host.removeAttribute('for');
    } else {
      this.host.setAttribute('for', htmlFor);
    }
  }
  get control() {
    if (this.host.hasAttribute('for')) {
      if (!this.htmlFor || !this.host.isConnected) {
        return null;
      }
      return this.host.getRootNode().querySelector(`#${this.htmlFor}`);
    }
    return this.currentControl || this.host.parentElement;
  }
  set control(control) {
    if (control) {
      this.attach(control);
    } else {
      this.detach();
    }
  }
  /**
   * Creates a new controller for an `Attachable` element.
   *
   * @param host The `Attachable` element.
   * @param onControlChange A callback with two parameters for the previous and
   *     next control. An `Attachable` element may perform setup or teardown
   *     logic whenever the control changes.
   */
  constructor(host, onControlChange) {
    var _FOR_ATTRIBUTE_OBSERV;
    this.host = host;
    this.onControlChange = onControlChange;
    this.currentControl = null;
    host.addController(this);
    host[ATTACHABLE_CONTROLLER] = this;
    (_FOR_ATTRIBUTE_OBSERV = FOR_ATTRIBUTE_OBSERVER) === null || _FOR_ATTRIBUTE_OBSERV === void 0 || _FOR_ATTRIBUTE_OBSERV.observe(host, {
      attributeFilter: ['for']
    });
  }
  attach(control) {
    if (control === this.currentControl) {
      return;
    }
    this.setCurrentControl(control);
    // When imperatively attaching, remove the `for` attribute so
    // that the attached control is used instead of a referenced one.
    this.host.removeAttribute('for');
  }
  detach() {
    this.setCurrentControl(null);
    // When imperatively detaching, add an empty `for=""` attribute. This will
    // ensure the control is `null` rather than the `parentElement`.
    this.host.setAttribute('for', '');
  }
  /** @private */
  hostConnected() {
    this.setCurrentControl(this.control);
  }
  /** @private */
  hostDisconnected() {
    this.setCurrentControl(null);
  }
  setCurrentControl(control) {
    this.onControlChange(this.currentControl, control);
    this.currentControl = control;
  }
}

/**
 * @license
 * Copyright 2021 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * Events that the focus ring listens to.
 */
const EVENTS$1 = ['focusin', 'focusout', 'pointerdown'];
/**
 * A focus ring component.
 *
 * @fires visibility-changed {Event} Fired whenever `visible` changes.
 */
class FocusRing extends i$1 {
  constructor() {
    super(...arguments);
    /**
     * Makes the focus ring visible.
     */
    this.visible = false;
    /**
     * Makes the focus ring animate inwards instead of outwards.
     */
    this.inward = false;
    this.attachableController = new AttachableController(this, this.onControlChange.bind(this));
  }
  get htmlFor() {
    return this.attachableController.htmlFor;
  }
  set htmlFor(htmlFor) {
    this.attachableController.htmlFor = htmlFor;
  }
  get control() {
    return this.attachableController.control;
  }
  set control(control) {
    this.attachableController.control = control;
  }
  attach(control) {
    this.attachableController.attach(control);
  }
  detach() {
    this.attachableController.detach();
  }
  connectedCallback() {
    super.connectedCallback();
    // Needed for VoiceOver, which will create a "group" if the element is a
    // sibling to other content.
    this.setAttribute('aria-hidden', 'true');
  }
  /** @private */
  handleEvent(event) {
    var _this$control;
    if (event[HANDLED_BY_FOCUS_RING]) {
      // This ensures the focus ring does not activate when multiple focus rings
      // are used within a single component.
      return;
    }
    switch (event.type) {
      default:
        return;
      case 'focusin':
        this.visible = ((_this$control = this.control) === null || _this$control === void 0 ? void 0 : _this$control.matches(':focus-visible')) ?? false;
        break;
      case 'focusout':
      case 'pointerdown':
        this.visible = false;
        break;
    }
    event[HANDLED_BY_FOCUS_RING] = true;
  }
  onControlChange(prev, next) {
    for (const event of EVENTS$1) {
      prev === null || prev === void 0 || prev.removeEventListener(event, this);
      next === null || next === void 0 || next.addEventListener(event, this);
    }
  }
  update(changed) {
    if (changed.has('visible')) {
      // This logic can be removed once the `:has` selector has been introduced
      // to Firefox. This is necessary to allow correct submenu styles.
      this.dispatchEvent(new Event('visibility-changed'));
    }
    super.update(changed);
  }
}
__decorate([n({
  type: Boolean,
  reflect: true
})], FocusRing.prototype, "visible", void 0);
__decorate([n({
  type: Boolean,
  reflect: true
})], FocusRing.prototype, "inward", void 0);
const HANDLED_BY_FOCUS_RING = Symbol('handledByFocusRing');

/**
 * @license
 * Copyright 2024 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Generated stylesheet for ./focus/internal/focus-ring-styles.css.
const styles$4 = i$4`:host{animation-delay:0s,calc(var(--md-focus-ring-duration, 600ms)*.25);animation-duration:calc(var(--md-focus-ring-duration, 600ms)*.25),calc(var(--md-focus-ring-duration, 600ms)*.75);animation-timing-function:cubic-bezier(0.2, 0, 0, 1);box-sizing:border-box;color:var(--md-focus-ring-color, var(--md-sys-color-secondary, #625b71));display:none;pointer-events:none;position:absolute}:host([visible]){display:flex}:host(:not([inward])){animation-name:outward-grow,outward-shrink;border-end-end-radius:calc(var(--md-focus-ring-shape-end-end, var(--md-focus-ring-shape, var(--md-sys-shape-corner-full, 9999px))) + var(--md-focus-ring-outward-offset, 2px));border-end-start-radius:calc(var(--md-focus-ring-shape-end-start, var(--md-focus-ring-shape, var(--md-sys-shape-corner-full, 9999px))) + var(--md-focus-ring-outward-offset, 2px));border-start-end-radius:calc(var(--md-focus-ring-shape-start-end, var(--md-focus-ring-shape, var(--md-sys-shape-corner-full, 9999px))) + var(--md-focus-ring-outward-offset, 2px));border-start-start-radius:calc(var(--md-focus-ring-shape-start-start, var(--md-focus-ring-shape, var(--md-sys-shape-corner-full, 9999px))) + var(--md-focus-ring-outward-offset, 2px));inset:calc(-1*var(--md-focus-ring-outward-offset, 2px));outline:var(--md-focus-ring-width, 3px) solid currentColor}:host([inward]){animation-name:inward-grow,inward-shrink;border-end-end-radius:calc(var(--md-focus-ring-shape-end-end, var(--md-focus-ring-shape, var(--md-sys-shape-corner-full, 9999px))) - var(--md-focus-ring-inward-offset, 0px));border-end-start-radius:calc(var(--md-focus-ring-shape-end-start, var(--md-focus-ring-shape, var(--md-sys-shape-corner-full, 9999px))) - var(--md-focus-ring-inward-offset, 0px));border-start-end-radius:calc(var(--md-focus-ring-shape-start-end, var(--md-focus-ring-shape, var(--md-sys-shape-corner-full, 9999px))) - var(--md-focus-ring-inward-offset, 0px));border-start-start-radius:calc(var(--md-focus-ring-shape-start-start, var(--md-focus-ring-shape, var(--md-sys-shape-corner-full, 9999px))) - var(--md-focus-ring-inward-offset, 0px));border:var(--md-focus-ring-width, 3px) solid currentColor;inset:var(--md-focus-ring-inward-offset, 0px)}@keyframes outward-grow{from{outline-width:0}to{outline-width:var(--md-focus-ring-active-width, 8px)}}@keyframes outward-shrink{from{outline-width:var(--md-focus-ring-active-width, 8px)}}@keyframes inward-grow{from{border-width:0}to{border-width:var(--md-focus-ring-active-width, 8px)}}@keyframes inward-shrink{from{border-width:var(--md-focus-ring-active-width, 8px)}}@media(prefers-reduced-motion){:host{animation:none}}
`;

/**
 * @license
 * Copyright 2021 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * TODO(b/267336424): add docs
 *
 * @final
 * @suppress {visibility}
 */
let MdFocusRing = class MdFocusRing extends FocusRing {};
MdFocusRing.styles = [styles$4];
MdFocusRing = __decorate([t$1('md-focus-ring')], MdFocusRing);

/**
 * @license
 * Copyright 2017 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */
const t = {
    ATTRIBUTE: 1,
    PROPERTY: 3,
    BOOLEAN_ATTRIBUTE: 4},
  e$1 = t => (...e) => ({
    _$litDirective$: t,
    values: e
  });
class i {
  constructor(t) {}
  get _$AU() {
    return this._$AM._$AU;
  }
  _$AT(t, e, i) {
    this._$Ct = t, this._$AM = e, this._$Ci = i;
  }
  _$AS(t, e) {
    return this.update(t, e);
  }
  update(t, e) {
    return this.render(...e);
  }
}

/**
 * @license
 * Copyright 2018 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */
const e = e$1(class extends i {
  constructor(t$1) {
    var _t$strings;
    if (super(t$1), t$1.type !== t.ATTRIBUTE || "class" !== t$1.name || ((_t$strings = t$1.strings) === null || _t$strings === void 0 ? void 0 : _t$strings.length) > 2) throw Error("`classMap()` can only be used in the `class` attribute and must be the only part in the attribute.");
  }
  render(t) {
    return " " + Object.keys(t).filter(s => t[s]).join(" ") + " ";
  }
  update(s, [i]) {
    if (void 0 === this.st) {
      this.st = new Set(), void 0 !== s.strings && (this.nt = new Set(s.strings.join(" ").split(/\s/).filter(t => "" !== t)));
      for (const t in i) {
        var _this$nt;
        i[t] && !((_this$nt = this.nt) !== null && _this$nt !== void 0 && _this$nt.has(t)) && this.st.add(t);
      }
      return this.render(i);
    }
    const r = s.element.classList;
    for (const t of this.st) t in i || (r.remove(t), this.st.delete(t));
    for (const t in i) {
      var _this$nt2;
      const s = !!i[t];
      s === this.st.has(t) || ((_this$nt2 = this.nt) === null || _this$nt2 === void 0 ? void 0 : _this$nt2.has(t)) || (s ? (r.add(t), this.st.add(t)) : (r.remove(t), this.st.delete(t)));
    }
    return T;
  }
});

/**
 * @license
 * Copyright 2021 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * Easing functions to use for web animations.
 *
 * **NOTE:** `EASING.EMPHASIZED` is approximated with unknown accuracy.
 *
 * TODO(b/241113345): replace with tokens
 */
const EASING = {
  STANDARD: 'cubic-bezier(0.2, 0, 0, 1)',
  EMPHASIZED: 'cubic-bezier(.3,0,0,1)',
  EMPHASIZED_ACCELERATE: 'cubic-bezier(.3,0,.8,.15)'};
/**
 * Creates an `AnimationSignal` that can be used to cancel a previous task.
 *
 * @example
 * class MyClass {
 *   private labelAnimationSignal = createAnimationSignal();
 *
 *   private async animateLabel() {
 *     // Start of the task. Previous tasks will be canceled.
 *     const signal = this.labelAnimationSignal.start();
 *
 *     // Do async work...
 *     if (signal.aborted) {
 *       // Use AbortSignal to check if a request was made to abort after some
 *       // asynchronous work.
 *       return;
 *     }
 *
 *     const animation = this.animate(...);
 *     // Add event listeners to be notified when the task should be canceled.
 *     signal.addEventListener('abort', () => {
 *       animation.cancel();
 *     });
 *
 *     animation.addEventListener('finish', () => {
 *       // Tell the signal that the current task is finished.
 *       this.labelAnimationSignal.finish();
 *     });
 *   }
 * }
 *
 * @return An `AnimationSignal`.
 */
function createAnimationSignal() {
  // The current animation's AbortController
  let animationAbortController = null;
  return {
    start() {
      var _animationAbortContro;
      // Tell the previous animation to cancel.
      (_animationAbortContro = animationAbortController) === null || _animationAbortContro === void 0 || _animationAbortContro.abort();
      // Set up a new AbortController for the current animation.
      animationAbortController = new AbortController();
      // Provide the AbortSignal so that the caller can check aborted status
      // and add listeners.
      return animationAbortController.signal;
    },
    finish() {
      animationAbortController = null;
    }
  };
}

/**
 * @license
 * Copyright 2022 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
const PRESS_GROW_MS = 450;
const MINIMUM_PRESS_MS = 225;
const INITIAL_ORIGIN_SCALE = 0.2;
const PADDING = 10;
const SOFT_EDGE_MINIMUM_SIZE = 75;
const SOFT_EDGE_CONTAINER_RATIO = 0.35;
const PRESS_PSEUDO = '::after';
const ANIMATION_FILL = 'forwards';
/**
 * Interaction states for the ripple.
 *
 * On Touch:
 *  - `INACTIVE -> TOUCH_DELAY -> WAITING_FOR_CLICK -> INACTIVE`
 *  - `INACTIVE -> TOUCH_DELAY -> HOLDING -> WAITING_FOR_CLICK -> INACTIVE`
 *
 * On Mouse or Pen:
 *   - `INACTIVE -> WAITING_FOR_CLICK -> INACTIVE`
 */
var State;
(function (State) {
  /**
   * Initial state of the control, no touch in progress.
   *
   * Transitions:
   *   - on touch down: transition to `TOUCH_DELAY`.
   *   - on mouse down: transition to `WAITING_FOR_CLICK`.
   */
  State[State["INACTIVE"] = 0] = "INACTIVE";
  /**
   * Touch down has been received, waiting to determine if it's a swipe or
   * scroll.
   *
   * Transitions:
   *   - on touch up: begin press; transition to `WAITING_FOR_CLICK`.
   *   - on cancel: transition to `INACTIVE`.
   *   - after `TOUCH_DELAY_MS`: begin press; transition to `HOLDING`.
   */
  State[State["TOUCH_DELAY"] = 1] = "TOUCH_DELAY";
  /**
   * A touch has been deemed to be a press
   *
   * Transitions:
   *  - on up: transition to `WAITING_FOR_CLICK`.
   */
  State[State["HOLDING"] = 2] = "HOLDING";
  /**
   * The user touch has finished, transition into rest state.
   *
   * Transitions:
   *   - on click end press; transition to `INACTIVE`.
   */
  State[State["WAITING_FOR_CLICK"] = 3] = "WAITING_FOR_CLICK";
})(State || (State = {}));
/**
 * Events that the ripple listens to.
 */
const EVENTS = ['click', 'contextmenu', 'pointercancel', 'pointerdown', 'pointerenter', 'pointerleave', 'pointerup'];
/**
 * Delay reacting to touch so that we do not show the ripple for a swipe or
 * scroll interaction.
 */
const TOUCH_DELAY_MS = 150;
/**
 * Used to detect if HCM is active. Events do not process during HCM when the
 * ripple is not displayed.
 */
const FORCED_COLORS = window.matchMedia('(forced-colors: active)');
/**
 * A ripple component.
 */
class Ripple extends i$1 {
  constructor() {
    super(...arguments);
    /**
     * Disables the ripple.
     */
    this.disabled = false;
    this.hovered = false;
    this.pressed = false;
    this.rippleSize = '';
    this.rippleScale = '';
    this.initialSize = 0;
    this.state = State.INACTIVE;
    this.checkBoundsAfterContextMenu = false;
    this.attachableController = new AttachableController(this, this.onControlChange.bind(this));
  }
  get htmlFor() {
    return this.attachableController.htmlFor;
  }
  set htmlFor(htmlFor) {
    this.attachableController.htmlFor = htmlFor;
  }
  get control() {
    return this.attachableController.control;
  }
  set control(control) {
    this.attachableController.control = control;
  }
  attach(control) {
    this.attachableController.attach(control);
  }
  detach() {
    this.attachableController.detach();
  }
  connectedCallback() {
    super.connectedCallback();
    // Needed for VoiceOver, which will create a "group" if the element is a
    // sibling to other content.
    this.setAttribute('aria-hidden', 'true');
  }
  render() {
    const classes = {
      'hovered': this.hovered,
      'pressed': this.pressed
    };
    return x`<div class="surface ${e(classes)}"></div>`;
  }
  update(changedProps) {
    if (changedProps.has('disabled') && this.disabled) {
      this.hovered = false;
      this.pressed = false;
    }
    super.update(changedProps);
  }
  /**
   * TODO(b/269799771): make private
   * @private only public for slider
   */
  handlePointerenter(event) {
    if (!this.shouldReactToEvent(event)) {
      return;
    }
    this.hovered = true;
  }
  /**
   * TODO(b/269799771): make private
   * @private only public for slider
   */
  handlePointerleave(event) {
    if (!this.shouldReactToEvent(event)) {
      return;
    }
    this.hovered = false;
    // release a held mouse or pen press that moves outside the element
    if (this.state !== State.INACTIVE) {
      this.endPressAnimation();
    }
  }
  handlePointerup(event) {
    if (!this.shouldReactToEvent(event)) {
      return;
    }
    if (this.state === State.HOLDING) {
      this.state = State.WAITING_FOR_CLICK;
      return;
    }
    if (this.state === State.TOUCH_DELAY) {
      this.state = State.WAITING_FOR_CLICK;
      this.startPressAnimation(this.rippleStartEvent);
      return;
    }
  }
  async handlePointerdown(event) {
    if (!this.shouldReactToEvent(event)) {
      return;
    }
    this.rippleStartEvent = event;
    if (!this.isTouch(event)) {
      this.state = State.WAITING_FOR_CLICK;
      this.startPressAnimation(event);
      return;
    }
    // after a longpress contextmenu event, an extra `pointerdown` can be
    // dispatched to the pressed element. Check that the down is within
    // bounds of the element in this case.
    if (this.checkBoundsAfterContextMenu && !this.inBounds(event)) {
      return;
    }
    this.checkBoundsAfterContextMenu = false;
    // Wait for a hold after touch delay
    this.state = State.TOUCH_DELAY;
    await new Promise(resolve => {
      setTimeout(resolve, TOUCH_DELAY_MS);
    });
    if (this.state !== State.TOUCH_DELAY) {
      return;
    }
    this.state = State.HOLDING;
    this.startPressAnimation(event);
  }
  handleClick() {
    // Click is a MouseEvent in Firefox and Safari, so we cannot use
    // `shouldReactToEvent`
    if (this.disabled) {
      return;
    }
    if (this.state === State.WAITING_FOR_CLICK) {
      this.endPressAnimation();
      return;
    }
    if (this.state === State.INACTIVE) {
      // keyboard synthesized click event
      this.startPressAnimation();
      this.endPressAnimation();
    }
  }
  handlePointercancel(event) {
    if (!this.shouldReactToEvent(event)) {
      return;
    }
    this.endPressAnimation();
  }
  handleContextmenu() {
    if (this.disabled) {
      return;
    }
    this.checkBoundsAfterContextMenu = true;
    this.endPressAnimation();
  }
  determineRippleSize() {
    const {
      height,
      width
    } = this.getBoundingClientRect();
    const maxDim = Math.max(height, width);
    const softEdgeSize = Math.max(SOFT_EDGE_CONTAINER_RATIO * maxDim, SOFT_EDGE_MINIMUM_SIZE);
    const initialSize = Math.floor(maxDim * INITIAL_ORIGIN_SCALE);
    const hypotenuse = Math.sqrt(width ** 2 + height ** 2);
    const maxRadius = hypotenuse + PADDING;
    this.initialSize = initialSize;
    this.rippleScale = `${(maxRadius + softEdgeSize) / initialSize}`;
    this.rippleSize = `${initialSize}px`;
  }
  getNormalizedPointerEventCoords(pointerEvent) {
    const {
      scrollX,
      scrollY
    } = window;
    const {
      left,
      top
    } = this.getBoundingClientRect();
    const documentX = scrollX + left;
    const documentY = scrollY + top;
    const {
      pageX,
      pageY
    } = pointerEvent;
    return {
      x: pageX - documentX,
      y: pageY - documentY
    };
  }
  getTranslationCoordinates(positionEvent) {
    const {
      height,
      width
    } = this.getBoundingClientRect();
    // end in the center
    const endPoint = {
      x: (width - this.initialSize) / 2,
      y: (height - this.initialSize) / 2
    };
    let startPoint;
    if (positionEvent instanceof PointerEvent) {
      startPoint = this.getNormalizedPointerEventCoords(positionEvent);
    } else {
      startPoint = {
        x: width / 2,
        y: height / 2
      };
    }
    // center around start point
    startPoint = {
      x: startPoint.x - this.initialSize / 2,
      y: startPoint.y - this.initialSize / 2
    };
    return {
      startPoint,
      endPoint
    };
  }
  startPressAnimation(positionEvent) {
    var _this$growAnimation;
    if (!this.mdRoot) {
      return;
    }
    this.pressed = true;
    (_this$growAnimation = this.growAnimation) === null || _this$growAnimation === void 0 || _this$growAnimation.cancel();
    this.determineRippleSize();
    const {
      startPoint,
      endPoint
    } = this.getTranslationCoordinates(positionEvent);
    const translateStart = `${startPoint.x}px, ${startPoint.y}px`;
    const translateEnd = `${endPoint.x}px, ${endPoint.y}px`;
    this.growAnimation = this.mdRoot.animate({
      top: [0, 0],
      left: [0, 0],
      height: [this.rippleSize, this.rippleSize],
      width: [this.rippleSize, this.rippleSize],
      transform: [`translate(${translateStart}) scale(1)`, `translate(${translateEnd}) scale(${this.rippleScale})`]
    }, {
      pseudoElement: PRESS_PSEUDO,
      duration: PRESS_GROW_MS,
      easing: EASING.STANDARD,
      fill: ANIMATION_FILL
    });
  }
  async endPressAnimation() {
    this.rippleStartEvent = undefined;
    this.state = State.INACTIVE;
    const animation = this.growAnimation;
    let pressAnimationPlayState = Infinity;
    if (typeof (animation === null || animation === void 0 ? void 0 : animation.currentTime) === 'number') {
      pressAnimationPlayState = animation.currentTime;
    } else if (animation !== null && animation !== void 0 && animation.currentTime) {
      pressAnimationPlayState = animation.currentTime.to('ms').value;
    }
    if (pressAnimationPlayState >= MINIMUM_PRESS_MS) {
      this.pressed = false;
      return;
    }
    await new Promise(resolve => {
      setTimeout(resolve, MINIMUM_PRESS_MS - pressAnimationPlayState);
    });
    if (this.growAnimation !== animation) {
      // A new press animation was started. The old animation was canceled and
      // should not finish the pressed state.
      return;
    }
    this.pressed = false;
  }
  /**
   * Returns `true` if
   *  - the ripple element is enabled
   *  - the pointer is primary for the input type
   *  - the pointer is the pointer that started the interaction, or will start
   * the interaction
   *  - the pointer is a touch, or the pointer state has the primary button
   * held, or the pointer is hovering
   */
  shouldReactToEvent(event) {
    if (this.disabled || !event.isPrimary) {
      return false;
    }
    if (this.rippleStartEvent && this.rippleStartEvent.pointerId !== event.pointerId) {
      return false;
    }
    if (event.type === 'pointerenter' || event.type === 'pointerleave') {
      return !this.isTouch(event);
    }
    const isPrimaryButton = event.buttons === 1;
    return this.isTouch(event) || isPrimaryButton;
  }
  /**
   * Check if the event is within the bounds of the element.
   *
   * This is only needed for the "stuck" contextmenu longpress on Chrome.
   */
  inBounds({
    x,
    y
  }) {
    const {
      top,
      left,
      bottom,
      right
    } = this.getBoundingClientRect();
    return x >= left && x <= right && y >= top && y <= bottom;
  }
  isTouch({
    pointerType
  }) {
    return pointerType === 'touch';
  }
  /** @private */
  async handleEvent(event) {
    if (FORCED_COLORS !== null && FORCED_COLORS !== void 0 && FORCED_COLORS.matches) {
      // Skip event logic since the ripple is `display: none`.
      return;
    }
    switch (event.type) {
      case 'click':
        this.handleClick();
        break;
      case 'contextmenu':
        this.handleContextmenu();
        break;
      case 'pointercancel':
        this.handlePointercancel(event);
        break;
      case 'pointerdown':
        await this.handlePointerdown(event);
        break;
      case 'pointerenter':
        this.handlePointerenter(event);
        break;
      case 'pointerleave':
        this.handlePointerleave(event);
        break;
      case 'pointerup':
        this.handlePointerup(event);
        break;
    }
  }
  onControlChange(prev, next) {
    for (const event of EVENTS) {
      prev === null || prev === void 0 || prev.removeEventListener(event, this);
      next === null || next === void 0 || next.addEventListener(event, this);
    }
  }
}
__decorate([n({
  type: Boolean,
  reflect: true
})], Ripple.prototype, "disabled", void 0);
__decorate([r()], Ripple.prototype, "hovered", void 0);
__decorate([r()], Ripple.prototype, "pressed", void 0);
__decorate([e$2('.surface')], Ripple.prototype, "mdRoot", void 0);

/**
 * @license
 * Copyright 2024 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Generated stylesheet for ./ripple/internal/ripple-styles.css.
const styles$3 = i$4`:host{display:flex;margin:auto;pointer-events:none}:host([disabled]){display:none}@media(forced-colors: active){:host{display:none}}:host,.surface{border-radius:inherit;position:absolute;inset:0;overflow:hidden}.surface{-webkit-tap-highlight-color:rgba(0,0,0,0)}.surface::before,.surface::after{content:"";opacity:0;position:absolute}.surface::before{background-color:var(--md-ripple-hover-color, var(--md-sys-color-on-surface, #1d1b20));inset:0;transition:opacity 15ms linear,background-color 15ms linear}.surface::after{background:radial-gradient(closest-side, var(--md-ripple-pressed-color, var(--md-sys-color-on-surface, #1d1b20)) max(100% - 70px, 65%), transparent 100%);transform-origin:center center;transition:opacity 375ms linear}.hovered::before{background-color:var(--md-ripple-hover-color, var(--md-sys-color-on-surface, #1d1b20));opacity:var(--md-ripple-hover-opacity, 0.08)}.pressed::after{opacity:var(--md-ripple-pressed-opacity, 0.12);transition-duration:105ms}
`;

/**
 * @license
 * Copyright 2022 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * @summary Ripples, also known as state layers, are visual indicators used to
 * communicate the status of a component or interactive element.
 *
 * @description A state layer is a semi-transparent covering on an element that
 * indicates its state. State layers provide a systematic approach to
 * visualizing states by using opacity. A layer can be applied to an entire
 * element or in a circular shape and only one state layer can be applied at a
 * given time.
 *
 * @final
 * @suppress {visibility}
 */
let MdRipple = class MdRipple extends Ripple {};
MdRipple.styles = [styles$3];
MdRipple = __decorate([t$1('md-ripple')], MdRipple);

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * Accessibility Object Model reflective aria properties.
 */
const ARIA_PROPERTIES = ['role', 'ariaAtomic', 'ariaAutoComplete', 'ariaBusy', 'ariaChecked', 'ariaColCount', 'ariaColIndex', 'ariaColSpan', 'ariaCurrent', 'ariaDisabled', 'ariaExpanded', 'ariaHasPopup', 'ariaHidden', 'ariaInvalid', 'ariaKeyShortcuts', 'ariaLabel', 'ariaLevel', 'ariaLive', 'ariaModal', 'ariaMultiLine', 'ariaMultiSelectable', 'ariaOrientation', 'ariaPlaceholder', 'ariaPosInSet', 'ariaPressed', 'ariaReadOnly', 'ariaRequired', 'ariaRoleDescription', 'ariaRowCount', 'ariaRowIndex', 'ariaRowSpan', 'ariaSelected', 'ariaSetSize', 'ariaSort', 'ariaValueMax', 'ariaValueMin', 'ariaValueNow', 'ariaValueText'];
/**
 * Accessibility Object Model aria attributes.
 */
const ARIA_ATTRIBUTES = ARIA_PROPERTIES.map(ariaPropertyToAttribute);
/**
 * Checks if an attribute is one of the AOM aria attributes.
 *
 * @example
 * isAriaAttribute('aria-label'); // true
 *
 * @param attribute The attribute to check.
 * @return True if the attribute is an aria attribute, or false if not.
 */
function isAriaAttribute(attribute) {
  return ARIA_ATTRIBUTES.includes(attribute);
}
/**
 * Converts an AOM aria property into its corresponding attribute.
 *
 * @example
 * ariaPropertyToAttribute('ariaLabel'); // 'aria-label'
 *
 * @param property The aria property.
 * @return The aria attribute.
 */
function ariaPropertyToAttribute(property) {
  return property.replace('aria', 'aria-')
  // IDREF attributes also include an "Element" or "Elements" suffix
  .replace(/Elements?/g, '').toLowerCase();
}

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Private symbols
const privateIgnoreAttributeChangesFor = Symbol('privateIgnoreAttributeChangesFor');
/**
 * Mixes in aria delegation for elements that delegate focus and aria to inner
 * shadow root elements.
 *
 * This mixin fixes invalid aria announcements with shadow roots, caused by
 * duplicate aria attributes on both the host and the inner shadow root element.
 *
 * Note: this mixin **does not yet support** ID reference attributes, such as
 * `aria-labelledby` or `aria-controls`.
 *
 * @example
 * ```ts
 * class MyButton extends mixinDelegatesAria(LitElement) {
 *   static shadowRootOptions = {mode: 'open', delegatesFocus: true};
 *
 *   render() {
 *     return html`
 *       <button aria-label=${this.ariaLabel || nothing}>
 *         <slot></slot>
 *       </button>
 *     `;
 *   }
 * }
 * ```
 * ```html
 * <my-button aria-label="Plus one">+1</my-button>
 * ```
 *
 * Use `ARIAMixinStrict` for lit analyzer strict types, such as the "role"
 * attribute.
 *
 * @example
 * ```ts
 * return html`
 *   <button role=${(this as ARIAMixinStrict).role || nothing}>
 *     <slot></slot>
 *   </button>
 * `;
 * ```
 *
 * In the future, updates to the Accessibility Object Model (AOM) will provide
 * built-in aria delegation features that will replace this mixin.
 *
 * @param base The class to mix functionality into.
 * @return The provided class with aria delegation mixed in.
 */
function mixinDelegatesAria(base) {
  var _a;
  class WithDelegatesAriaElement extends base {
    constructor() {
      super(...arguments);
      this[_a] = new Set();
    }
    attributeChangedCallback(name, oldValue, newValue) {
      if (!isAriaAttribute(name)) {
        super.attributeChangedCallback(name, oldValue, newValue);
        return;
      }
      if (this[privateIgnoreAttributeChangesFor].has(name)) {
        return;
      }
      // Don't trigger another `attributeChangedCallback` once we remove the
      // aria attribute from the host. We check the explicit name of the
      // attribute to ignore since `attributeChangedCallback` can be called
      // multiple times out of an expected order when hydrating an element with
      // multiple attributes.
      this[privateIgnoreAttributeChangesFor].add(name);
      this.removeAttribute(name);
      this[privateIgnoreAttributeChangesFor].delete(name);
      const dataProperty = ariaAttributeToDataProperty(name);
      if (newValue === null) {
        delete this.dataset[dataProperty];
      } else {
        this.dataset[dataProperty] = newValue;
      }
      this.requestUpdate(ariaAttributeToDataProperty(name), oldValue);
    }
    getAttribute(name) {
      if (isAriaAttribute(name)) {
        return super.getAttribute(ariaAttributeToDataAttribute(name));
      }
      return super.getAttribute(name);
    }
    removeAttribute(name) {
      super.removeAttribute(name);
      if (isAriaAttribute(name)) {
        super.removeAttribute(ariaAttributeToDataAttribute(name));
        // Since `aria-*` attributes are already removed`, we need to request
        // an update because `attributeChangedCallback` will not be called.
        this.requestUpdate();
      }
    }
  }
  _a = privateIgnoreAttributeChangesFor;
  setupDelegatesAriaProperties(WithDelegatesAriaElement);
  return WithDelegatesAriaElement;
}
/**
 * Overrides the constructor's native `ARIAMixin` properties to ensure that
 * aria properties reflect the values that were shifted to a data attribute.
 *
 * @param ctor The `ReactiveElement` constructor to patch.
 */
function setupDelegatesAriaProperties(ctor) {
  for (const ariaProperty of ARIA_PROPERTIES) {
    // The casing between ariaProperty and the dataProperty may be different.
    // ex: aria-haspopup -> ariaHasPopup
    const ariaAttribute = ariaPropertyToAttribute(ariaProperty);
    // ex: aria-haspopup -> data-aria-haspopup
    const dataAttribute = ariaAttributeToDataAttribute(ariaAttribute);
    // ex: aria-haspopup -> dataset.ariaHaspopup
    const dataProperty = ariaAttributeToDataProperty(ariaAttribute);
    // Call `ReactiveElement.createProperty()` so that the `aria-*` and `data-*`
    // attributes are added to the `static observedAttributes` array. This
    // triggers `attributeChangedCallback` for the delegates aria mixin to
    // handle.
    ctor.createProperty(ariaProperty, {
      attribute: ariaAttribute,
      noAccessor: true
    });
    ctor.createProperty(Symbol(dataAttribute), {
      attribute: dataAttribute,
      noAccessor: true
    });
    // Re-define the `ARIAMixin` properties to handle data attribute shifting.
    // It is safe to use `Object.defineProperty` here because the properties
    // are native and not renamed.
    // tslint:disable-next-line:ban-unsafe-reflection
    Object.defineProperty(ctor.prototype, ariaProperty, {
      configurable: true,
      enumerable: true,
      get() {
        return this.dataset[dataProperty] ?? null;
      },
      set(value) {
        const prevValue = this.dataset[dataProperty] ?? null;
        if (value === prevValue) {
          return;
        }
        if (value === null) {
          delete this.dataset[dataProperty];
        } else {
          this.dataset[dataProperty] = value;
        }
        this.requestUpdate(ariaProperty, prevValue);
      }
    });
  }
}
function ariaAttributeToDataAttribute(ariaAttribute) {
  // aria-haspopup -> data-aria-haspopup
  return `data-${ariaAttribute}`;
}
function ariaAttributeToDataProperty(ariaAttribute) {
  // aria-haspopup -> dataset.ariaHaspopup
  return ariaAttribute.replace(/-\w/, dashLetter => dashLetter[1].toUpperCase());
}

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * A unique symbol used for protected access to an instance's
 * `ElementInternals`.
 *
 * @example
 * ```ts
 * class MyElement extends mixinElementInternals(LitElement) {
 *   constructor() {
 *     super();
 *     this[internals].role = 'button';
 *   }
 * }
 * ```
 */
const internals = Symbol('internals');
// Private symbols
const privateInternals = Symbol('privateInternals');
/**
 * Mixes in an attached `ElementInternals` instance.
 *
 * This mixin is only needed when other shared code needs access to a
 * component's `ElementInternals`, such as form-associated mixins.
 *
 * @param base The class to mix functionality into.
 * @return The provided class with `WithElementInternals` mixed in.
 */
function mixinElementInternals(base) {
  class WithElementInternalsElement extends base {
    get [internals]() {
      // Create internals in getter so that it can be used in methods called on
      // construction in `ReactiveElement`, such as `requestUpdate()`.
      if (!this[privateInternals]) {
        // Cast needed for closure
        this[privateInternals] = this.attachInternals();
      }
      return this[privateInternals];
    }
  }
  return WithElementInternalsElement;
}

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * Sets up an element's constructor to enable form submission. The element
 * instance should be form associated and have a `type` property.
 *
 * A click listener is added to each element instance. If the click is not
 * default prevented, it will submit the element's form, if any.
 *
 * @example
 * ```ts
 * class MyElement extends mixinElementInternals(LitElement) {
 *   static {
 *     setupFormSubmitter(MyElement);
 *   }
 *
 *   static formAssociated = true;
 *
 *   type: FormSubmitterType = 'submit';
 * }
 * ```
 *
 * @param ctor The form submitter element's constructor.
 */
function setupFormSubmitter(ctor) {
  ctor.addInitializer(instance => {
    const submitter = instance;
    submitter.addEventListener('click', async event => {
      const {
        type,
        [internals]: elementInternals
      } = submitter;
      const {
        form
      } = elementInternals;
      if (!form || type === 'button') {
        return;
      }
      // Wait a full task for event bubbling to complete.
      await new Promise(resolve => {
        setTimeout(resolve);
      });
      if (event.defaultPrevented) {
        return;
      }
      if (type === 'reset') {
        form.reset();
        return;
      }
      // form.requestSubmit(submitter) does not work with form associated custom
      // elements. This patches the dispatched submit event to add the correct
      // `submitter`.
      // See https://github.com/WICG/webcomponents/issues/814
      form.addEventListener('submit', submitEvent => {
        Object.defineProperty(submitEvent, 'submitter', {
          configurable: true,
          enumerable: true,
          get: () => submitter
        });
      }, {
        capture: true,
        once: true
      });
      elementInternals.setFormValue(submitter.value);
      form.requestSubmit();
    });
  });
}

/**
 * @license
 * Copyright 2021 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * Dispatches a click event to the given element that triggers a native action,
 * but is not composed and therefore is not seen outside the element.
 *
 * This is useful for responding to an external click event on the host element
 * that should trigger an internal action like a button click.
 *
 * Note, a helper is provided because setting this up correctly is a bit tricky.
 * In particular, calling `click` on an element creates a composed event, which
 * is not desirable, and a manually dispatched event must specifically be a
 * `MouseEvent` to trigger a native action.
 *
 * @example
 * hostClickListener = (event: MouseEvent) {
 *   if (isActivationClick(event)) {
 *     this.dispatchActivationClick(this.buttonElement);
 *   }
 * }
 *
 */
function dispatchActivationClick(element) {
  const event = new MouseEvent('click', {
    bubbles: true
  });
  element.dispatchEvent(event);
  return event;
}
/**
 * Returns true if the click event should trigger an activation behavior. The
 * behavior is defined by the element and is whatever it should do when
 * clicked.
 *
 * Typically when an element needs to handle a click, the click is generated
 * from within the element and an event listener within the element implements
 * the needed behavior; however, it's possible to fire a click directly
 * at the element that the element should handle. This method helps
 * distinguish these "external" clicks.
 *
 * An "external" click can be triggered in a number of ways: via a click
 * on an associated label for a form  associated element, calling
 * `element.click()`, or calling
 * `element.dispatchEvent(new MouseEvent('click', ...))`.
 *
 * Also works around Firefox issue
 * https://bugzilla.mozilla.org/show_bug.cgi?id=1804576 by squelching
 * events for a microtask after called.
 *
 * @example
 * hostClickListener = (event: MouseEvent) {
 *   if (isActivationClick(event)) {
 *     this.dispatchActivationClick(this.buttonElement);
 *   }
 * }
 *
 */
function isActivationClick(event) {
  // Event must start at the event target.
  if (event.currentTarget !== event.target) {
    return false;
  }
  // Event must not be retargeted from shadowRoot.
  if (event.composedPath()[0] !== event.target) {
    return false;
  }
  // Target must not be disabled; this should only occur for a synthetically
  // dispatched click.
  if (event.target.disabled) {
    return false;
  }
  // This is an activation if the event should not be squelched.
  return !squelchEvent(event);
}
// TODO(https://bugzilla.mozilla.org/show_bug.cgi?id=1804576)
//  Remove when Firefox bug is addressed.
function squelchEvent(event) {
  const squelched = isSquelchingEvents;
  if (squelched) {
    event.preventDefault();
    event.stopImmediatePropagation();
  }
  squelchEventsForMicrotask();
  return squelched;
}
// Ignore events for one microtask only.
let isSquelchingEvents = false;
async function squelchEventsForMicrotask() {
  isSquelchingEvents = true;
  // Need to pause for just one microtask.
  // tslint:disable-next-line
  await null;
  isSquelchingEvents = false;
}

/**
 * @license
 * Copyright 2019 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Separate variable needed for closure.
const buttonBaseClass = mixinDelegatesAria(mixinElementInternals(i$1));
/**
 * A button component.
 */
class Button extends buttonBaseClass {
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
  constructor() {
    super();
    /**
     * Whether or not the button is disabled.
     */
    this.disabled = false;
    /**
     * Whether or not the button is "soft-disabled" (disabled but still
     * focusable).
     *
     * Use this when a button needs increased visibility when disabled. See
     * https://www.w3.org/WAI/ARIA/apg/practices/keyboard-interface/#kbd_disabled_controls
     * for more guidance on when this is needed.
     */
    this.softDisabled = false;
    /**
     * The URL that the link button points to.
     */
    this.href = '';
    /**
     * The filename to use when downloading the linked resource.
     * If not specified, the browser will determine a filename.
     * This is only applicable when the button is used as a link (`href` is set).
     */
    this.download = '';
    /**
     * Where to display the linked `href` URL for a link button. Common options
     * include `_blank` to open in a new tab.
     */
    this.target = '';
    /**
     * Whether to render the icon at the inline end of the label rather than the
     * inline start.
     *
     * _Note:_ Link buttons cannot have trailing icons.
     */
    this.trailingIcon = false;
    /**
     * Whether to display the icon or not.
     */
    this.hasIcon = false;
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
    {
      this.addEventListener('click', this.handleClick.bind(this));
    }
  }
  focus() {
    var _this$buttonElement;
    (_this$buttonElement = this.buttonElement) === null || _this$buttonElement === void 0 || _this$buttonElement.focus();
  }
  blur() {
    var _this$buttonElement2;
    (_this$buttonElement2 = this.buttonElement) === null || _this$buttonElement2 === void 0 || _this$buttonElement2.blur();
  }
  render() {
    var _this$renderElevation;
    // Link buttons may not be disabled
    const isRippleDisabled = !this.href && (this.disabled || this.softDisabled);
    const buttonOrLink = this.href ? this.renderLink() : this.renderButton();
    // TODO(b/310046938): due to a limitation in focus ring/ripple, we can't use
    // the same ID for different elements, so we change the ID instead.
    const buttonId = this.href ? 'link' : 'button';
    return x`
      ${(_this$renderElevation = this.renderElevationOrOutline) === null || _this$renderElevation === void 0 ? void 0 : _this$renderElevation.call(this)}
      <div class="background"></div>
      <md-focus-ring part="focus-ring" for=${buttonId}></md-focus-ring>
      <md-ripple
        part="ripple"
        for=${buttonId}
        ?disabled="${isRippleDisabled}"></md-ripple>
      ${buttonOrLink}
    `;
  }
  renderButton() {
    // Needed for closure conformance
    const {
      ariaLabel,
      ariaHasPopup,
      ariaExpanded
    } = this;
    return x`<button
      id="button"
      class="button"
      ?disabled=${this.disabled}
      aria-disabled=${this.softDisabled || E}
      aria-label="${ariaLabel || E}"
      aria-haspopup="${ariaHasPopup || E}"
      aria-expanded="${ariaExpanded || E}">
      ${this.renderContent()}
    </button>`;
  }
  renderLink() {
    // Needed for closure conformance
    const {
      ariaLabel,
      ariaHasPopup,
      ariaExpanded
    } = this;
    return x`<a
      id="link"
      class="button"
      aria-label="${ariaLabel || E}"
      aria-haspopup="${ariaHasPopup || E}"
      aria-expanded="${ariaExpanded || E}"
      href=${this.href}
      download=${this.download || E}
      target=${this.target || E}
      >${this.renderContent()}
    </a>`;
  }
  renderContent() {
    const icon = x`<slot
      name="icon"
      @slotchange="${this.handleSlotChange}"></slot>`;
    return x`
      <span class="touch"></span>
      ${this.trailingIcon ? E : icon}
      <span class="label"><slot></slot></span>
      ${this.trailingIcon ? icon : E}
    `;
  }
  handleClick(event) {
    // If the button is soft-disabled, we need to explicitly prevent the click
    // from propagating to other event listeners as well as prevent the default
    // action.
    if (!this.href && this.softDisabled) {
      event.stopImmediatePropagation();
      event.preventDefault();
      return;
    }
    if (!isActivationClick(event) || !this.buttonElement) {
      return;
    }
    this.focus();
    dispatchActivationClick(this.buttonElement);
  }
  handleSlotChange() {
    this.hasIcon = this.assignedIcons.length > 0;
  }
}
(() => {
  setupFormSubmitter(Button);
})();
/** @nocollapse */
Button.formAssociated = true;
/** @nocollapse */
Button.shadowRootOptions = {
  mode: 'open',
  delegatesFocus: true
};
__decorate([n({
  type: Boolean,
  reflect: true
})], Button.prototype, "disabled", void 0);
__decorate([n({
  type: Boolean,
  attribute: 'soft-disabled',
  reflect: true
})], Button.prototype, "softDisabled", void 0);
__decorate([n()], Button.prototype, "href", void 0);
__decorate([n()], Button.prototype, "download", void 0);
__decorate([n()], Button.prototype, "target", void 0);
__decorate([n({
  type: Boolean,
  attribute: 'trailing-icon',
  reflect: true
})], Button.prototype, "trailingIcon", void 0);
__decorate([n({
  type: Boolean,
  attribute: 'has-icon',
  reflect: true
})], Button.prototype, "hasIcon", void 0);
__decorate([n()], Button.prototype, "type", void 0);
__decorate([n({
  reflect: true
})], Button.prototype, "value", void 0);
__decorate([e$2('.button')], Button.prototype, "buttonElement", void 0);
__decorate([o({
  slot: 'icon',
  flatten: true
})], Button.prototype, "assignedIcons", void 0);

/**
 * @license
 * Copyright 2021 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * A text button component.
 */
class TextButton extends Button {}

/**
 * @license
 * Copyright 2024 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Generated stylesheet for ./button/internal/text-styles.css.
const styles$2 = i$4`:host{--_container-height: var(--md-text-button-container-height, 40px);--_disabled-label-text-color: var(--md-text-button-disabled-label-text-color, var(--md-sys-color-on-surface, #1d1b20));--_disabled-label-text-opacity: var(--md-text-button-disabled-label-text-opacity, 0.38);--_focus-label-text-color: var(--md-text-button-focus-label-text-color, var(--md-sys-color-primary, #6750a4));--_hover-label-text-color: var(--md-text-button-hover-label-text-color, var(--md-sys-color-primary, #6750a4));--_hover-state-layer-color: var(--md-text-button-hover-state-layer-color, var(--md-sys-color-primary, #6750a4));--_hover-state-layer-opacity: var(--md-text-button-hover-state-layer-opacity, 0.08);--_label-text-color: var(--md-text-button-label-text-color, var(--md-sys-color-primary, #6750a4));--_label-text-font: var(--md-text-button-label-text-font, var(--md-sys-typescale-label-large-font, var(--md-ref-typeface-plain, Roboto)));--_label-text-line-height: var(--md-text-button-label-text-line-height, var(--md-sys-typescale-label-large-line-height, 1.25rem));--_label-text-size: var(--md-text-button-label-text-size, var(--md-sys-typescale-label-large-size, 0.875rem));--_label-text-weight: var(--md-text-button-label-text-weight, var(--md-sys-typescale-label-large-weight, var(--md-ref-typeface-weight-medium, 500)));--_pressed-label-text-color: var(--md-text-button-pressed-label-text-color, var(--md-sys-color-primary, #6750a4));--_pressed-state-layer-color: var(--md-text-button-pressed-state-layer-color, var(--md-sys-color-primary, #6750a4));--_pressed-state-layer-opacity: var(--md-text-button-pressed-state-layer-opacity, 0.12);--_disabled-icon-color: var(--md-text-button-disabled-icon-color, var(--md-sys-color-on-surface, #1d1b20));--_disabled-icon-opacity: var(--md-text-button-disabled-icon-opacity, 0.38);--_focus-icon-color: var(--md-text-button-focus-icon-color, var(--md-sys-color-primary, #6750a4));--_hover-icon-color: var(--md-text-button-hover-icon-color, var(--md-sys-color-primary, #6750a4));--_icon-color: var(--md-text-button-icon-color, var(--md-sys-color-primary, #6750a4));--_icon-size: var(--md-text-button-icon-size, 18px);--_pressed-icon-color: var(--md-text-button-pressed-icon-color, var(--md-sys-color-primary, #6750a4));--_container-shape-start-start: var(--md-text-button-container-shape-start-start, var(--md-text-button-container-shape, var(--md-sys-shape-corner-full, 9999px)));--_container-shape-start-end: var(--md-text-button-container-shape-start-end, var(--md-text-button-container-shape, var(--md-sys-shape-corner-full, 9999px)));--_container-shape-end-end: var(--md-text-button-container-shape-end-end, var(--md-text-button-container-shape, var(--md-sys-shape-corner-full, 9999px)));--_container-shape-end-start: var(--md-text-button-container-shape-end-start, var(--md-text-button-container-shape, var(--md-sys-shape-corner-full, 9999px)));--_leading-space: var(--md-text-button-leading-space, 12px);--_trailing-space: var(--md-text-button-trailing-space, 12px);--_with-leading-icon-leading-space: var(--md-text-button-with-leading-icon-leading-space, 12px);--_with-leading-icon-trailing-space: var(--md-text-button-with-leading-icon-trailing-space, 16px);--_with-trailing-icon-leading-space: var(--md-text-button-with-trailing-icon-leading-space, 16px);--_with-trailing-icon-trailing-space: var(--md-text-button-with-trailing-icon-trailing-space, 12px);--_container-color: none;--_disabled-container-color: none;--_disabled-container-opacity: 0}
`;

class EwTextButton extends TextButton {}
EwTextButton.styles = [styles$5, styles$2];
customElements.define("ew-text-button", EwTextButton);

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * A divider component.
 */
class Divider extends i$1 {
  constructor() {
    super(...arguments);
    /**
     * Indents the divider with equal padding on both sides.
     */
    this.inset = false;
    /**
     * Indents the divider with padding on the leading side.
     */
    this.insetStart = false;
    /**
     * Indents the divider with padding on the trailing side.
     */
    this.insetEnd = false;
  }
}
__decorate([n({
  type: Boolean,
  reflect: true
})], Divider.prototype, "inset", void 0);
__decorate([n({
  type: Boolean,
  reflect: true,
  attribute: 'inset-start'
})], Divider.prototype, "insetStart", void 0);
__decorate([n({
  type: Boolean,
  reflect: true,
  attribute: 'inset-end'
})], Divider.prototype, "insetEnd", void 0);

/**
 * @license
 * Copyright 2024 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Generated stylesheet for ./divider/internal/divider-styles.css.
const styles$1 = i$4`:host{box-sizing:border-box;color:var(--md-divider-color, var(--md-sys-color-outline-variant, #cac4d0));display:flex;height:var(--md-divider-thickness, 1px);width:100%}:host([inset]),:host([inset-start]){padding-inline-start:16px}:host([inset]),:host([inset-end]){padding-inline-end:16px}:host::before{background:currentColor;content:"";height:100%;width:100%}@media(forced-colors: active){:host::before{background:CanvasText}}
`;

/**
 * @license
 * Copyright 2021 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * Re-dispatches an event from the provided element.
 *
 * This function is useful for forwarding non-composed events, such as `change`
 * events.
 *
 * @example
 * class MyInput extends LitElement {
 *   render() {
 *     return html`<input @change=${this.redispatchEvent}>`;
 *   }
 *
 *   protected redispatchEvent(event: Event) {
 *     redispatchEvent(this, event);
 *   }
 * }
 *
 * @param element The element to dispatch the event from.
 * @param event The event to re-dispatch.
 * @return Whether or not the event was dispatched (if cancelable).
 */
function redispatchEvent(element, event) {
  // For bubbling events in SSR light DOM (or composed), stop their propagation
  // and dispatch the copy.
  if (event.bubbles && (!element.shadowRoot || event.composed)) {
    event.stopPropagation();
  }
  const copy = Reflect.construct(event.constructor, [event.type, event]);
  const dispatched = element.dispatchEvent(copy);
  if (!dispatched) {
    event.preventDefault();
  }
  return dispatched;
}

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * @summary A divider is a thin line that groups content in lists and
 * containers.
 *
 * @description Dividers can reinforce tapability, such as when used to separate
 * list items or define tappable regions in an accordion.
 *
 * @final
 * @suppress {visibility}
 */
let MdDivider = class MdDivider extends Divider {};
MdDivider.styles = [styles$1];
MdDivider = __decorate([t$1('md-divider')], MdDivider);

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * The default dialog open animation.
 */
const DIALOG_DEFAULT_OPEN_ANIMATION = {
  dialog: [[
  // Dialog slide down
  [{
    'transform': 'translateY(-50px)'
  }, {
    'transform': 'translateY(0)'
  }], {
    duration: 500,
    easing: EASING.EMPHASIZED
  }]],
  scrim: [[
  // Scrim fade in
  [{
    'opacity': 0
  }, {
    'opacity': 0.32
  }], {
    duration: 500,
    easing: 'linear'
  }]],
  container: [[
  // Container fade in
  [{
    'opacity': 0
  }, {
    'opacity': 1
  }], {
    duration: 50,
    easing: 'linear',
    pseudoElement: '::before'
  }], [
  // Container grow
  // Note: current spec says to grow from 0dp->100% and shrink from
  // 100%->35%. We change this to 35%->100% to simplify the animation that
  // is supposed to clip content as it grows. From 0dp it's possible to see
  // text/actions appear before the container has fully grown.
  [{
    'height': '35%'
  }, {
    'height': '100%'
  }], {
    duration: 500,
    easing: EASING.EMPHASIZED,
    pseudoElement: '::before'
  }]],
  headline: [[
  // Headline fade in
  [{
    'opacity': 0
  }, {
    'opacity': 0,
    offset: 0.2
  }, {
    'opacity': 1
  }], {
    duration: 250,
    easing: 'linear',
    fill: 'forwards'
  }]],
  content: [[
  // Content fade in
  [{
    'opacity': 0
  }, {
    'opacity': 0,
    offset: 0.2
  }, {
    'opacity': 1
  }], {
    duration: 250,
    easing: 'linear',
    fill: 'forwards'
  }]],
  actions: [[
  // Actions fade in
  [{
    'opacity': 0
  }, {
    'opacity': 0,
    offset: 0.5
  }, {
    'opacity': 1
  }], {
    duration: 300,
    easing: 'linear',
    fill: 'forwards'
  }]]
};
/**
 * The default dialog close animation.
 */
const DIALOG_DEFAULT_CLOSE_ANIMATION = {
  dialog: [[
  // Dialog slide up
  [{
    'transform': 'translateY(0)'
  }, {
    'transform': 'translateY(-50px)'
  }], {
    duration: 150,
    easing: EASING.EMPHASIZED_ACCELERATE
  }]],
  scrim: [[
  // Scrim fade out
  [{
    'opacity': 0.32
  }, {
    'opacity': 0
  }], {
    duration: 150,
    easing: 'linear'
  }]],
  container: [[
  // Container shrink
  [{
    'height': '100%'
  }, {
    'height': '35%'
  }], {
    duration: 150,
    easing: EASING.EMPHASIZED_ACCELERATE,
    pseudoElement: '::before'
  }], [
  // Container fade out
  [{
    'opacity': '1'
  }, {
    'opacity': '0'
  }], {
    delay: 100,
    duration: 50,
    easing: 'linear',
    pseudoElement: '::before'
  }]],
  headline: [[
  // Headline fade out
  [{
    'opacity': 1
  }, {
    'opacity': 0
  }], {
    duration: 100,
    easing: 'linear',
    fill: 'forwards'
  }]],
  content: [[
  // Content fade out
  [{
    'opacity': 1
  }, {
    'opacity': 0
  }], {
    duration: 100,
    easing: 'linear',
    fill: 'forwards'
  }]],
  actions: [[
  // Actions fade out
  [{
    'opacity': 1
  }, {
    'opacity': 0
  }], {
    duration: 100,
    easing: 'linear',
    fill: 'forwards'
  }]]
};

/**
 * @license
 * Copyright 2023 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Separate variable needed for closure.
const dialogBaseClass = mixinDelegatesAria(i$1);
/**
 * A dialog component.
 *
 * @fires open {Event} Dispatched when the dialog is opening before any animations.
 * @fires opened {Event} Dispatched when the dialog has opened after any animations.
 * @fires close {Event} Dispatched when the dialog is closing before any animations.
 * @fires closed {Event} Dispatched when the dialog has closed after any animations.
 * @fires cancel {Event} Dispatched when the dialog has been canceled by clicking
 * on the scrim or pressing Escape.
 */
class Dialog extends dialogBaseClass {
  // We do not use `delegatesFocus: true` due to a Chromium bug with
  // selecting text.
  // See https://bugs.chromium.org/p/chromium/issues/detail?id=950357
  /**
   * Opens the dialog when set to `true` and closes it when set to `false`.
   */
  get open() {
    return this.isOpen;
  }
  set open(open) {
    if (open === this.isOpen) {
      return;
    }
    this.isOpen = open;
    if (open) {
      this.setAttribute('open', '');
      this.show();
    } else {
      this.removeAttribute('open');
      this.close();
    }
  }
  constructor() {
    super();
    /**
     * Skips the opening and closing animations.
     */
    this.quick = false;
    /**
     * Gets or sets the dialog's return value, usually to indicate which button
     * a user pressed to close it.
     *
     * https://developer.mozilla.org/en-US/docs/Web/API/HTMLDialogElement/returnValue
     */
    this.returnValue = '';
    /**
     * Disables focus trapping, which by default keeps keyboard Tab navigation
     * within the dialog.
     *
     * When disabled, after focusing the last element of a dialog, pressing Tab
     * again will release focus from the window back to the browser (such as the
     * URL bar).
     *
     * Focus trapping is recommended for accessibility, and should not typically
     * be disabled. Only turn this off if the use case of a dialog is more
     * accessible without focus trapping.
     */
    this.noFocusTrap = false;
    /**
     * Gets the opening animation for a dialog. Set to a new function to customize
     * the animation.
     */
    this.getOpenAnimation = () => DIALOG_DEFAULT_OPEN_ANIMATION;
    /**
     * Gets the closing animation for a dialog. Set to a new function to customize
     * the animation.
     */
    this.getCloseAnimation = () => DIALOG_DEFAULT_CLOSE_ANIMATION;
    this.isOpen = false;
    this.isOpening = false;
    this.isConnectedPromise = this.getIsConnectedPromise();
    this.isAtScrollTop = false;
    this.isAtScrollBottom = false;
    this.nextClickIsFromContent = false;
    // Dialogs should not be SSR'd while open, so we can just use runtime checks.
    this.hasHeadline = false;
    this.hasActions = false;
    this.hasIcon = false;
    // See https://bugs.chromium.org/p/chromium/issues/detail?id=1512224
    // Chrome v120 has a bug where escape keys do not trigger cancels. If we get
    // a dialog "close" event that is triggered without a "cancel" after an escape
    // keydown, then we need to manually trigger our closing logic.
    //
    // This bug occurs when pressing escape to close a dialog without first
    // interacting with the dialog's content.
    //
    // Cleanup tracking:
    // https://github.com/material-components/material-web/issues/5330
    // This can be removed when full CloseWatcher support added and the above bug
    // in Chromium is fixed to fire 'cancel' with one escape press and close with
    // multiple.
    this.escapePressedWithoutCancel = false;
    // This TreeWalker is used to walk through a dialog's children to find
    // focusable elements. TreeWalker is faster than `querySelectorAll('*')`.
    // We check for isServer because there isn't a "document" during an SSR
    // run.
    this.treewalker = document.createTreeWalker(this, NodeFilter.SHOW_ELEMENT);
    {
      this.addEventListener('submit', this.handleSubmit);
    }
  }
  /**
   * Opens the dialog and fires a cancelable `open` event. After a dialog's
   * animation, an `opened` event is fired.
   *
   * Add an `autofocus` attribute to a child of the dialog that should
   * receive focus after opening.
   *
   * @return A Promise that resolves after the animation is finished and the
   *     `opened` event was fired.
   */
  async show() {
    var _this$querySelector;
    this.isOpening = true;
    // Dialogs can be opened before being attached to the DOM, so we need to
    // wait until we're connected before calling `showModal()`.
    await this.isConnectedPromise;
    await this.updateComplete;
    const dialog = this.dialog;
    // Check if already opened or if `dialog.close()` was called while awaiting.
    if (dialog.open || !this.isOpening) {
      this.isOpening = false;
      return;
    }
    const preventOpen = !this.dispatchEvent(new Event('open', {
      cancelable: true
    }));
    if (preventOpen) {
      this.open = false;
      this.isOpening = false;
      return;
    }
    // All Material dialogs are modal.
    dialog.showModal();
    this.open = true;
    // Reset scroll position if re-opening a dialog with the same content.
    if (this.scroller) {
      this.scroller.scrollTop = 0;
    }
    // Native modal dialogs ignore autofocus and instead force focus to the
    // first focusable child. Override this behavior if there is a child with
    // an autofocus attribute.
    (_this$querySelector = this.querySelector('[autofocus]')) === null || _this$querySelector === void 0 || _this$querySelector.focus();
    await this.animateDialog(this.getOpenAnimation());
    this.dispatchEvent(new Event('opened'));
    this.isOpening = false;
  }
  /**
   * Closes the dialog and fires a cancelable `close` event. After a dialog's
   * animation, a `closed` event is fired.
   *
   * @param returnValue A return value usually indicating which button was used
   *     to close a dialog. If a dialog is canceled by clicking the scrim or
   *     pressing Escape, it will not change the return value after closing.
   * @return A Promise that resolves after the animation is finished and the
   *     `closed` event was fired.
   */
  async close(returnValue = this.returnValue) {
    this.isOpening = false;
    if (!this.isConnected) {
      // Disconnected dialogs do not fire close events or animate.
      this.open = false;
      return;
    }
    await this.updateComplete;
    const dialog = this.dialog;
    // Check if already closed or if `dialog.show()` was called while awaiting.
    if (!dialog.open || this.isOpening) {
      this.open = false;
      return;
    }
    const prevReturnValue = this.returnValue;
    this.returnValue = returnValue;
    const preventClose = !this.dispatchEvent(new Event('close', {
      cancelable: true
    }));
    if (preventClose) {
      this.returnValue = prevReturnValue;
      return;
    }
    await this.animateDialog(this.getCloseAnimation());
    dialog.close(returnValue);
    this.open = false;
    this.dispatchEvent(new Event('closed'));
  }
  connectedCallback() {
    super.connectedCallback();
    this.isConnectedPromiseResolve();
  }
  disconnectedCallback() {
    super.disconnectedCallback();
    this.isConnectedPromise = this.getIsConnectedPromise();
  }
  render() {
    const scrollable = this.open && !(this.isAtScrollTop && this.isAtScrollBottom);
    const classes = {
      'has-headline': this.hasHeadline,
      'has-actions': this.hasActions,
      'has-icon': this.hasIcon,
      'scrollable': scrollable,
      'show-top-divider': scrollable && !this.isAtScrollTop,
      'show-bottom-divider': scrollable && !this.isAtScrollBottom
    };
    // The focus trap sentinels are only added after the dialog opens, since
    // dialog.showModal() will try to autofocus them, even with tabindex="-1".
    const showFocusTrap = this.open && !this.noFocusTrap;
    const focusTrap = x`
      <div
        class="focus-trap"
        tabindex="0"
        aria-hidden="true"
        @focus=${this.handleFocusTrapFocus}></div>
    `;
    const {
      ariaLabel
    } = this;
    return x`
      <div class="scrim"></div>
      <dialog
        class=${e(classes)}
        aria-label=${ariaLabel || E}
        aria-labelledby=${this.hasHeadline ? 'headline' : E}
        role=${this.type === 'alert' ? 'alertdialog' : E}
        @cancel=${this.handleCancel}
        @click=${this.handleDialogClick}
        @close=${this.handleClose}
        @keydown=${this.handleKeydown}
        .returnValue=${this.returnValue || E}>
        ${showFocusTrap ? focusTrap : E}
        <div class="container" @click=${this.handleContentClick}>
          <div class="headline">
            <div class="icon" aria-hidden="true">
              <slot name="icon" @slotchange=${this.handleIconChange}></slot>
            </div>
            <h2 id="headline" aria-hidden=${!this.hasHeadline || E}>
              <slot
                name="headline"
                @slotchange=${this.handleHeadlineChange}></slot>
            </h2>
            <md-divider></md-divider>
          </div>
          <div class="scroller">
            <div class="content">
              <div class="top anchor"></div>
              <slot name="content"></slot>
              <div class="bottom anchor"></div>
            </div>
          </div>
          <div class="actions">
            <md-divider></md-divider>
            <slot name="actions" @slotchange=${this.handleActionsChange}></slot>
          </div>
        </div>
        ${showFocusTrap ? focusTrap : E}
      </dialog>
    `;
  }
  firstUpdated() {
    this.intersectionObserver = new IntersectionObserver(entries => {
      for (const entry of entries) {
        this.handleAnchorIntersection(entry);
      }
    }, {
      root: this.scroller
    });
    this.intersectionObserver.observe(this.topAnchor);
    this.intersectionObserver.observe(this.bottomAnchor);
  }
  handleDialogClick() {
    if (this.nextClickIsFromContent) {
      // Avoid doing a layout calculation below if we know the click came from
      // content.
      this.nextClickIsFromContent = false;
      return;
    }
    // Click originated on the backdrop. Native `<dialog>`s will not cancel,
    // but Material dialogs do.
    const preventDefault = !this.dispatchEvent(new Event('cancel', {
      cancelable: true
    }));
    if (preventDefault) {
      return;
    }
    this.close();
  }
  handleContentClick() {
    this.nextClickIsFromContent = true;
  }
  handleSubmit(event) {
    const form = event.target;
    const {
      submitter
    } = event;
    if (form.getAttribute('method') !== 'dialog' || !submitter) {
      return;
    }
    // Close reason is the submitter's value attribute, or the dialog's
    // `returnValue` if there is no attribute.
    this.close(submitter.getAttribute('value') ?? this.returnValue);
  }
  handleCancel(event) {
    if (event.target !== this.dialog) {
      // Ignore any cancel events dispatched by content.
      return;
    }
    this.escapePressedWithoutCancel = false;
    const preventDefault = !redispatchEvent(this, event);
    // We always prevent default on the original dialog event since we'll
    // animate closing it before it actually closes.
    event.preventDefault();
    if (preventDefault) {
      return;
    }
    this.close();
  }
  handleClose() {
    var _this$dialog;
    if (!this.escapePressedWithoutCancel) {
      return;
    }
    this.escapePressedWithoutCancel = false;
    (_this$dialog = this.dialog) === null || _this$dialog === void 0 || _this$dialog.dispatchEvent(new Event('cancel', {
      cancelable: true
    }));
  }
  handleKeydown(event) {
    if (event.key !== 'Escape') {
      return;
    }
    // An escape key was pressed. If a "close" event fires next without a
    // "cancel" event first, then we know we're in the Chrome v120 bug.
    this.escapePressedWithoutCancel = true;
    // Wait a full task for the cancel/close event listeners to fire, then
    // reset the flag.
    setTimeout(() => {
      this.escapePressedWithoutCancel = false;
    });
  }
  async animateDialog(animation) {
    var _this$cancelAnimation;
    // Always cancel the previous animations. Animations can include `fill`
    // modes that need to be cleared when `quick` is toggled. If not, content
    // that faded out will remain hidden when a `quick` dialog re-opens after
    // previously opening and closing without `quick`.
    (_this$cancelAnimation = this.cancelAnimations) === null || _this$cancelAnimation === void 0 || _this$cancelAnimation.abort();
    this.cancelAnimations = new AbortController();
    if (this.quick) {
      return;
    }
    const {
      dialog,
      scrim,
      container,
      headline,
      content,
      actions
    } = this;
    if (!dialog || !scrim || !container || !headline || !content || !actions) {
      return;
    }
    const {
      container: containerAnimate,
      dialog: dialogAnimate,
      scrim: scrimAnimate,
      headline: headlineAnimate,
      content: contentAnimate,
      actions: actionsAnimate
    } = animation;
    const elementAndAnimation = [[dialog, dialogAnimate ?? []], [scrim, scrimAnimate ?? []], [container, containerAnimate ?? []], [headline, headlineAnimate ?? []], [content, contentAnimate ?? []], [actions, actionsAnimate ?? []]];
    const animations = [];
    for (const [element, animation] of elementAndAnimation) {
      for (const animateArgs of animation) {
        const animation = element.animate(...animateArgs);
        this.cancelAnimations.signal.addEventListener('abort', () => {
          animation.cancel();
        });
        animations.push(animation);
      }
    }
    await Promise.all(animations.map(animation => animation.finished.catch(() => {
      // Ignore intentional AbortErrors when calling `animation.cancel()`.
    })));
  }
  handleHeadlineChange(event) {
    const slot = event.target;
    this.hasHeadline = slot.assignedElements().length > 0;
  }
  handleActionsChange(event) {
    const slot = event.target;
    this.hasActions = slot.assignedElements().length > 0;
  }
  handleIconChange(event) {
    const slot = event.target;
    this.hasIcon = slot.assignedElements().length > 0;
  }
  handleAnchorIntersection(entry) {
    const {
      target,
      isIntersecting
    } = entry;
    if (target === this.topAnchor) {
      this.isAtScrollTop = isIntersecting;
    }
    if (target === this.bottomAnchor) {
      this.isAtScrollBottom = isIntersecting;
    }
  }
  getIsConnectedPromise() {
    return new Promise(resolve => {
      this.isConnectedPromiseResolve = resolve;
    });
  }
  handleFocusTrapFocus(event) {
    const [firstFocusableChild, lastFocusableChild] = this.getFirstAndLastFocusableChildren();
    if (!firstFocusableChild || !lastFocusableChild) {
      var _this$dialog2;
      // When a dialog does not have focusable children, the dialog itself
      // receives focus.
      (_this$dialog2 = this.dialog) === null || _this$dialog2 === void 0 || _this$dialog2.focus();
      return;
    }
    // To determine which child to focus, we need to know which focus trap
    // received focus...
    const isFirstFocusTrap = event.target === this.firstFocusTrap;
    const isLastFocusTrap = !isFirstFocusTrap;
    // ...and where the focus came from (what was previously focused).
    const focusCameFromFirstChild = event.relatedTarget === firstFocusableChild;
    const focusCameFromLastChild = event.relatedTarget === lastFocusableChild;
    // Although this is a focus trap, focus can come from outside the trap.
    // This can happen when elements are programmatically `focus()`'d. It also
    // happens when focus leaves and returns to the window, such as clicking on
    // the browser's URL bar and pressing Tab, or switching focus between
    // iframes.
    const focusCameFromOutsideDialog = !focusCameFromFirstChild && !focusCameFromLastChild;
    // Focus the dialog's first child when we reach the end of the dialog and
    // focus is moving forward. Or, when focus is moving forwards into the
    // dialog from outside of the window.
    const shouldFocusFirstChild = isLastFocusTrap && focusCameFromLastChild || isFirstFocusTrap && focusCameFromOutsideDialog;
    if (shouldFocusFirstChild) {
      firstFocusableChild.focus();
      return;
    }
    // Focus the dialog's last child when we reach the beginning of the dialog
    // and focus is moving backward. Or, when focus is moving backwards into the
    // dialog from outside of the window.
    const shouldFocusLastChild = isFirstFocusTrap && focusCameFromFirstChild || isLastFocusTrap && focusCameFromOutsideDialog;
    if (shouldFocusLastChild) {
      lastFocusableChild.focus();
      return;
    }
    // The booleans above are verbose for readability, but code executation
    // won't actually reach here.
  }
  getFirstAndLastFocusableChildren() {
    if (!this.treewalker) {
      return [null, null];
    }
    let firstFocusableChild = null;
    let lastFocusableChild = null;
    // Reset the current node back to the root host element.
    this.treewalker.currentNode = this.treewalker.root;
    while (this.treewalker.nextNode()) {
      // Cast as Element since the TreeWalker filter only accepts Elements.
      const nextChild = this.treewalker.currentNode;
      if (!isFocusable(nextChild)) {
        continue;
      }
      if (!firstFocusableChild) {
        firstFocusableChild = nextChild;
      }
      lastFocusableChild = nextChild;
    }
    // We set lastFocusableChild immediately after finding a
    // firstFocusableChild, which means the pair is either both null or both
    // non-null. Cast since TypeScript does not recognize this.
    return [firstFocusableChild, lastFocusableChild];
  }
}
__decorate([n({
  type: Boolean
})], Dialog.prototype, "open", null);
__decorate([n({
  type: Boolean
})], Dialog.prototype, "quick", void 0);
__decorate([n({
  attribute: false
})], Dialog.prototype, "returnValue", void 0);
__decorate([n()], Dialog.prototype, "type", void 0);
__decorate([n({
  type: Boolean,
  attribute: 'no-focus-trap'
})], Dialog.prototype, "noFocusTrap", void 0);
__decorate([e$2('dialog')], Dialog.prototype, "dialog", void 0);
__decorate([e$2('.scrim')], Dialog.prototype, "scrim", void 0);
__decorate([e$2('.container')], Dialog.prototype, "container", void 0);
__decorate([e$2('.headline')], Dialog.prototype, "headline", void 0);
__decorate([e$2('.content')], Dialog.prototype, "content", void 0);
__decorate([e$2('.actions')], Dialog.prototype, "actions", void 0);
__decorate([r()], Dialog.prototype, "isAtScrollTop", void 0);
__decorate([r()], Dialog.prototype, "isAtScrollBottom", void 0);
__decorate([e$2('.scroller')], Dialog.prototype, "scroller", void 0);
__decorate([e$2('.top.anchor')], Dialog.prototype, "topAnchor", void 0);
__decorate([e$2('.bottom.anchor')], Dialog.prototype, "bottomAnchor", void 0);
__decorate([e$2('.focus-trap')], Dialog.prototype, "firstFocusTrap", void 0);
__decorate([r()], Dialog.prototype, "hasHeadline", void 0);
__decorate([r()], Dialog.prototype, "hasActions", void 0);
__decorate([r()], Dialog.prototype, "hasIcon", void 0);
function isFocusable(element) {
  var _element$shadowRoot;
  // Check if the element is a known built-in focusable element:
  // - <a> and <area> with `href` attributes.
  // - Form controls that are not disabled.
  // - `contenteditable` elements.
  // - Anything with a non-negative `tabindex`.
  const knownFocusableElements = ':is(button,input,select,textarea,object,:is(a,area)[href],[tabindex],[contenteditable=true])';
  const notDisabled = ':not(:disabled,[disabled])';
  const notNegativeTabIndex = ':not([tabindex^="-"])';
  if (element.matches(knownFocusableElements + notDisabled + notNegativeTabIndex)) {
    return true;
  }
  const isCustomElement = element.localName.includes('-');
  if (!isCustomElement) {
    return false;
  }
  // If a custom element does not have a tabindex, it may still be focusable
  // if it delegates focus with a shadow root. We also need to check again if
  // the custom element is a disabled form control.
  if (!element.matches(notDisabled)) {
    return false;
  }
  return ((_element$shadowRoot = element.shadowRoot) === null || _element$shadowRoot === void 0 ? void 0 : _element$shadowRoot.delegatesFocus) ?? false;
}

/**
 * @license
 * Copyright 2024 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 */
// Generated stylesheet for ./dialog/internal/dialog-styles.css.
const styles = i$4`:host{border-start-start-radius:var(--md-dialog-container-shape-start-start, var(--md-dialog-container-shape, var(--md-sys-shape-corner-extra-large, 28px)));border-start-end-radius:var(--md-dialog-container-shape-start-end, var(--md-dialog-container-shape, var(--md-sys-shape-corner-extra-large, 28px)));border-end-end-radius:var(--md-dialog-container-shape-end-end, var(--md-dialog-container-shape, var(--md-sys-shape-corner-extra-large, 28px)));border-end-start-radius:var(--md-dialog-container-shape-end-start, var(--md-dialog-container-shape, var(--md-sys-shape-corner-extra-large, 28px)));display:contents;margin:auto;max-height:min(560px,100% - 48px);max-width:min(560px,100% - 48px);min-height:140px;min-width:280px;position:fixed;height:fit-content;width:fit-content}dialog{background:rgba(0,0,0,0);border:none;border-radius:inherit;flex-direction:column;height:inherit;margin:inherit;max-height:inherit;max-width:inherit;min-height:inherit;min-width:inherit;outline:none;overflow:visible;padding:0;width:inherit}dialog[open]{display:flex}::backdrop{background:none}.scrim{background:var(--md-sys-color-scrim, #000);display:none;inset:0;opacity:32%;pointer-events:none;position:fixed;z-index:1}:host([open]) .scrim{display:flex}h2{all:unset;align-self:stretch}.headline{align-items:center;color:var(--md-dialog-headline-color, var(--md-sys-color-on-surface, #1d1b20));display:flex;flex-direction:column;font-family:var(--md-dialog-headline-font, var(--md-sys-typescale-headline-small-font, var(--md-ref-typeface-brand, Roboto)));font-size:var(--md-dialog-headline-size, var(--md-sys-typescale-headline-small-size, 1.5rem));line-height:var(--md-dialog-headline-line-height, var(--md-sys-typescale-headline-small-line-height, 2rem));font-weight:var(--md-dialog-headline-weight, var(--md-sys-typescale-headline-small-weight, var(--md-ref-typeface-weight-regular, 400)));position:relative}slot[name=headline]::slotted(*){align-items:center;align-self:stretch;box-sizing:border-box;display:flex;gap:8px;padding:24px 24px 0}.icon{display:flex}slot[name=icon]::slotted(*){color:var(--md-dialog-icon-color, var(--md-sys-color-secondary, #625b71));fill:currentColor;font-size:var(--md-dialog-icon-size, 24px);margin-top:24px;height:var(--md-dialog-icon-size, 24px);width:var(--md-dialog-icon-size, 24px)}.has-icon slot[name=headline]::slotted(*){justify-content:center;padding-top:16px}.scrollable slot[name=headline]::slotted(*){padding-bottom:16px}.scrollable.has-headline slot[name=content]::slotted(*){padding-top:8px}.container{border-radius:inherit;display:flex;flex-direction:column;flex-grow:1;overflow:hidden;position:relative;transform-origin:top}.container::before{background:var(--md-dialog-container-color, var(--md-sys-color-surface-container-high, #ece6f0));border-radius:inherit;content:"";inset:0;position:absolute}.scroller{display:flex;flex:1;flex-direction:column;overflow:hidden;z-index:1}.scrollable .scroller{overflow-y:scroll}.content{color:var(--md-dialog-supporting-text-color, var(--md-sys-color-on-surface-variant, #49454f));font-family:var(--md-dialog-supporting-text-font, var(--md-sys-typescale-body-medium-font, var(--md-ref-typeface-plain, Roboto)));font-size:var(--md-dialog-supporting-text-size, var(--md-sys-typescale-body-medium-size, 0.875rem));line-height:var(--md-dialog-supporting-text-line-height, var(--md-sys-typescale-body-medium-line-height, 1.25rem));flex:1;font-weight:var(--md-dialog-supporting-text-weight, var(--md-sys-typescale-body-medium-weight, var(--md-ref-typeface-weight-regular, 400)));height:min-content;position:relative}slot[name=content]::slotted(*){box-sizing:border-box;padding:24px}.anchor{position:absolute}.top.anchor{top:0}.bottom.anchor{bottom:0}.actions{position:relative}slot[name=actions]::slotted(*){box-sizing:border-box;display:flex;gap:8px;justify-content:flex-end;padding:16px 24px 24px}.has-actions slot[name=content]::slotted(*){padding-bottom:8px}md-divider{display:none;position:absolute}.has-headline.show-top-divider .headline md-divider,.has-actions.show-bottom-divider .actions md-divider{display:flex}.headline md-divider{bottom:0}.actions md-divider{top:0}@media(forced-colors: active){dialog{outline:2px solid WindowText}}
`;

class EwDialog extends Dialog {}
EwDialog.styles = [styles];
customElements.define("ew-dialog", EwDialog);

// We set font-size to 16px and all the mdc typography styles
// because it defaults to rem, which means that the font-size
// of the host website would influence the ESP Web Tools dialog.
const dialogStyles = i$4`
  :host {
    --roboto-font: Roboto, system-ui;
    --text-color: rgba(0, 0, 0, 0.6);
    --danger-color: #db4437;

    --md-sys-color-primary: #03a9f4;
    --md-sys-color-on-primary: #fff;
    --md-ref-typeface-brand: var(--roboto-font);
    --md-ref-typeface-plain: var(--roboto-font);

    --md-sys-color-surface: #fff;
    --md-sys-color-surface-container: #fff;
    --md-sys-color-surface-container-high: #fff;
    --md-sys-color-surface-container-highest: #f5f5f5;
    --md-sys-color-secondary-container: #e0e0e0;

    --md-sys-typescale-headline-font: var(--roboto-font);
    --md-sys-typescale-title-font: var(--roboto-font);
  }

  a {
    color: var(--md-sys-color-primary);
  }
`;

export { B, Divider as D, E, T, __decorate as _, i$4 as a, e$2 as b, e as c, internals as d, e$3 as e, mixinElementInternals as f, isActivationClick as g, dispatchActivationClick as h, i$1 as i, redispatchEvent as j, setupFormSubmitter as k, EASING as l, mixinDelegatesAria as m, n, o, e$1 as p, i as q, r, styles$1 as s, t$1 as t, t as u, createAnimationSignal as v, b as w, x, dialogStyles as y };
