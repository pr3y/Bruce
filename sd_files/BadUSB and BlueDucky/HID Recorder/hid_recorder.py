from pynput import keyboard
import time

output_file = open("recorded_payload.txt", "w")

pressed_modifiers = set()
last_time = time.time()
string_buffer = ""


special_keys = {
    keyboard.Key.enter: "ENTER",
    keyboard.Key.space: "SPACE",
    keyboard.Key.backspace: "BACKSPACE",
    keyboard.Key.tab: "TAB",
    keyboard.Key.esc: "ESCAPE",
    keyboard.Key.cmd: "GUI",
    keyboard.Key.ctrl: "CTRL",
    keyboard.Key.alt: "ALT",
    keyboard.Key.shift: "SHIFT",
    keyboard.Key.delete: "DELETE",
    keyboard.Key.up: "UPARROW",
    keyboard.Key.down: "DOWNARROW",
    keyboard.Key.left: "LEFTARROW",
    keyboard.Key.right: "RIGHTARROW",
}


def flush_string_buffer():
    """Flush do buffer de string se não estiver vazio"""
    global string_buffer
    if string_buffer.strip():
        print(f"STRING {string_buffer}")
        output_file.write(f"STRING {string_buffer}\n")
        string_buffer = ""


def write_delay():
    global last_time
    now = time.time()
    delta = now - last_time
    if delta > 0.5:

        flush_string_buffer()
        delay_ms = int(delta * 1000)
        print(f"DELAY {delay_ms}")
        output_file.write(f"DELAY {delay_ms}\n")
    last_time = now


def on_press(key):
    global pressed_modifiers, string_buffer

    write_delay()

    if key in special_keys:
        key_name = special_keys[key]
        if key_name in ["SHIFT", "CTRL", "ALT", "GUI"]:
            pressed_modifiers.add(key_name)
        else:

            flush_string_buffer()

            if pressed_modifiers:
                combo = " ".join(pressed_modifiers) + " " + key_name
                print(combo)
                output_file.write(combo + "\n")
            else:
                print(key_name)
                output_file.write(key_name + "\n")
    else:
        try:
            char = key.char
            if pressed_modifiers:

                flush_string_buffer()
                combo = " ".join(pressed_modifiers) + " " + char.upper()
                print(combo)
                output_file.write(combo + "\n")
            else:

                string_buffer += char
        except AttributeError:
            pass


def on_release(key):
    global pressed_modifiers

    if key in special_keys:
        key_name = special_keys[key]
        if key_name in pressed_modifiers:
            pressed_modifiers.remove(key_name)

    if key == keyboard.Key.esc:
        # Flush buffer antes de finalizar
        flush_string_buffer()
        print("Gravação finalizada.")
        output_file.close()
        return False  # Stop listener


# Listener do teclado
with keyboard.Listener(on_press=on_press, on_release=on_release) as listener:
    print("Gravando... Pressione ESC para finalizar.\n")
    listener.join()
