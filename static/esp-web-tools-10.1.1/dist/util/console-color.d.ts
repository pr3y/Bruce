interface ConsoleState {
    bold: boolean;
    italic: boolean;
    underline: boolean;
    strikethrough: boolean;
    foregroundColor: string | null;
    backgroundColor: string | null;
    carriageReturn: boolean;
    lines: string[];
    secret: boolean;
}
export declare class ColoredConsole {
    targetElement: HTMLElement;
    state: ConsoleState;
    constructor(targetElement: HTMLElement);
    logs(): string;
    processLine(line: string): Element;
    processLines(): void;
    addLine(line: string): void;
}
export declare const coloredConsoleStyles = "\n  .log {\n    flex: 1;\n    background-color: #1c1c1c;\n    font-family: \"SFMono-Regular\", Consolas, \"Liberation Mono\", Menlo, Courier,\n      monospace;\n    font-size: 12px;\n    padding: 16px;\n    overflow: auto;\n    line-height: 1.45;\n    border-radius: 3px;\n    white-space: pre-wrap;\n    overflow-wrap: break-word;\n    color: #ddd;\n  }\n\n  .log-bold {\n    font-weight: bold;\n  }\n  .log-italic {\n    font-style: italic;\n  }\n  .log-underline {\n    text-decoration: underline;\n  }\n  .log-strikethrough {\n    text-decoration: line-through;\n  }\n  .log-underline.log-strikethrough {\n    text-decoration: underline line-through;\n  }\n  .log-secret {\n    -webkit-user-select: none;\n    -moz-user-select: none;\n    -ms-user-select: none;\n    user-select: none;\n  }\n  .log-secret-redacted {\n    opacity: 0;\n    width: 1px;\n    font-size: 1px;\n  }\n  .log-fg-black {\n    color: rgb(128, 128, 128);\n  }\n  .log-fg-red {\n    color: rgb(255, 0, 0);\n  }\n  .log-fg-green {\n    color: rgb(0, 255, 0);\n  }\n  .log-fg-yellow {\n    color: rgb(255, 255, 0);\n  }\n  .log-fg-blue {\n    color: rgb(0, 0, 255);\n  }\n  .log-fg-magenta {\n    color: rgb(255, 0, 255);\n  }\n  .log-fg-cyan {\n    color: rgb(0, 255, 255);\n  }\n  .log-fg-white {\n    color: rgb(187, 187, 187);\n  }\n  .log-bg-black {\n    background-color: rgb(0, 0, 0);\n  }\n  .log-bg-red {\n    background-color: rgb(255, 0, 0);\n  }\n  .log-bg-green {\n    background-color: rgb(0, 255, 0);\n  }\n  .log-bg-yellow {\n    background-color: rgb(255, 255, 0);\n  }\n  .log-bg-blue {\n    background-color: rgb(0, 0, 255);\n  }\n  .log-bg-magenta {\n    background-color: rgb(255, 0, 255);\n  }\n  .log-bg-cyan {\n    background-color: rgb(0, 255, 255);\n  }\n  .log-bg-white {\n    background-color: rgb(255, 255, 255);\n  }\n";
export {};
