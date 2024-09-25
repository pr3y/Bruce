# flipperducky-badUSB-payload-generator
GUI (Graphic user interface) in HTML, CSS, JavaScript, to make easyer and faster to create payload (.txt) for you Flipper Zero's bad-USB function.
---
## From this gist
https://gist.github.com/methanoliver/efebfe8f4008e167417d4ab96e5e3cac

# DuckyScript commands supported by Flipper's BadUSB

Explanations are only given for commands not present in the original DuckyScript, for everything else refer to [DuckyScript documentation](https://docs.hak5.org/hak5-usb-rubber-ducky/).

## Keys

These mean exactly what one would expect and should need no further explanation.

+ Modifiers: `CTRL`, `CONTROL`, `SHIFT`, `ALT`, `GUI`, `WINDOWS`
+ Combos: `CTRL-ALT`, `CTRL-SHIFT`, `ALT-SHIFT`, `ALT-GUI`, `GUI-SHIFT`
+ Cursor: `DOWNARROW`, `DOWN`, `LEFTARROW`, `LEFT`, `RIGHTARROW`, `RIGHT`, `UPARROW`, `UP`
+ Control and navigation: `ENTER`, `BREAK`, `PAUSE`, `CAPSLOCK`, `DELETE`, `BACKSPACE`, `END`, `ESC`, `ESCAPE`, `HOME`, `INSERT`, `NUMLOCK`, `PAGEUP`, `PAGEDOWN`, `PRINTSCREEN`, `SCROLLOCK`, `SPACE`, `TAB`, `MENU`, `APP`, `SYSRQ`
+ Function: `F1`, `F2`, `F3`, `F4`, `F5`, `F6`, `F7`, `F8`, `F9`, `F10`, `F11`, `F12`

## Input

+ `STRING`
+ `ALTSTRING <string>`, `ALTCODE <string>` -- Not present in DuckyScript, these are the equivalent of `STRING`, as if the string was typed in by holding Alt and entering the unicode code of every character, [as described in Windows documentation](https://support.microsoft.com/en-us/office/insert-ascii-or-unicode-latin-based-symbols-and-characters-d13f58d3-7bcb-44a7-a4d5-972ee12e50e0). This is the way to get around the fact that there are no standard ways to switch input keyboard language and important in parts of the world where default keyboard has no Latin characters on it at all. This only works on Windows.
+ `ALTCHAR <code>` -- Altstring is internally a sequence of `ALTCHAR` invocations.

There is *no* support for `STRINGLN`.

## Structural

+ `REM`
+ `ID <vendor:id>` - Equivalent to certain features of DuckyScript's `ATTACKMODE`. executed in preload phase, sets USB id of the keyboard device. For example, `ID 04d9:1702` is an AJAZZ keyboard.
+ `DELAY`
+ `DEFAULT_DELAY`, `DEFAULTDELAY`
+ `REPEAT`
+ `DUCKY_LANG` - ignored, recognized as a command purely for compatibility with existing scripts.

-----

# Official documentation
https://github.com/DarkFlippers/unleashed-firmware/blob/dev/documentation/file_formats/BadUsbScriptFormat.md

# Command syntax
BadUsb app uses extended Duckyscript syntax. It is compatible with classic USB Rubber Ducky 1.0 scripts, but provides some additional commands and features, such as custom USB ID, ALT+Numpad input method, SYSRQ command and more functional keys.
# Script file format
BadUsb app can execute only text scrips from .txt files, no compilation is required. Both `\n` and `\r\n` line endings are supported. Empty lines are allowed. You can use spaces ore tabs for line indentation.
# Command set
## Comment line
Just a single comment line. All text after REM command will be ignored by interpreter
|Command|Parameters|Notes|
|-|-|-|
|REM|Comment text||

## Delay
Pause script execution by defined time
|Command|Parameters|Notes|
|-|-|-|
|DELAY|Delay value in ms|Single delay|
|DEFAULT_DELAY|Delay value in ms|Add delay before every next command|
|DEFAULTDELAY|Delay value in ms|Same as DEFAULT_DELAY|

## Special keys
|Command|Notes|
|-|-|
|DOWNARROW / DOWN||
|LEFTARROW / LEFT||
|RIGHTARROW / RIGHT||
|UPARROW / UP||
|ENTER||
|DELETE||
|BACKSPACE||
|END||
|HOME||
|ESCAPE / ESC||
|INSERT||
|PAGEUP||
|PAGEDOWN||
|CAPSLOCK||
|NUMLOCK||
|SCROLLLOCK||
|PRINTSCREEN||
|BREAK|Pause/Break key|
|PAUSE|Pause/Break key|
|SPACE||
|TAB||
|MENU|Context menu key|
|APP|Same as MENU|
|Fx|F1-F12 keys|

## Modifier keys
Can be combined with special key command or single character
|Command|Notes|
|-|-|
|CONTROL / CTRL||
|SHIFT||
|ALT||
|WINDOWS / GUI||
|CTRL-ALT|CTRL+ALT|
|CTRL-SHIFT|CTRL+SHIFT|
|ALT-SHIFT|ALT+SHIFT|
|ALT-GUI|ALT+WIN|
|GUI-SHIFT|WIN+SHIFT|
## String
|Command|Parameters|Notes|
|-|-|-|
|STRING|Text string|Print text string|
## Repeat
|Command|Parameters|Notes|
|-|-|-|
|REPEAT|Number of additional repeats|Repeat previous command|
## ALT+Numpad input
On Windows and some Linux systems you can print character by pressing ALT key and entering its code on numpad
|Command|Parameters|Notes|
|-|-|-|
|ALTCHAR|Character code|Print single character|
|ALTSTRING|Text string|Print text string using ALT+Numpad method|
|ALTCODE|Text string|Same as ALTSTRING, presents in some Duckyscript implementations|
## SysRq
Send [SysRq command](https://en.wikipedia.org/wiki/Magic_SysRq_key)
|Command|Parameters|Notes|
|-|-|-|
|SYSRQ|Single character||
## USB device ID
You can set custom ID of Flipper USB HID device. ID command should be in the **first line** of script, it is executed before script run.

|Command|Parameters|Notes|
|-|-|-|
|ID|VID:PID Manufacturer:Product||

Example:
`ID 1234:abcd Flipper Devices:Flipper Zero`

VID and PID are hex codes and are mandatory, Manufacturer and Product are text strings and are optional.

-----

## DTK user script downloader
https://github.com/cribb-it/DTKDownloader
The DTK(ducktoolkit user script https://ducktoolkit.com/userscripts) downloader doesn't work for me. So if you get it to work send in a PR and I will add the resulting dump here.

-----

# Helpful hints

Here are a bunch of cheatsheets for use in building your scripts [here](https://github.com/FalsePhilosopher/BadUSB-Playground/tree/main/Misc/Cheat_Sheets)

Shorten your payloads as much as possible by stringing multiple commands together with variables like `&&` and `;;`, read more into each OS/shell you are targeting and how to achieve this.

RTFM for CMD
https://learn.microsoft.com/en-us/windows-server/administration/windows-commands/cmd

RTFM for powershell
https://learn.microsoft.com/en-us/powershell/scripting/learn/ps101/04-pipelines?view=powershell-7.3

Other ducky scripts can be found here
http://www.theatomheart.net/post/rubber-ducky-payloads/
