from pynput.keyboard import Key, Controller
import time

keyboard_controller = Controller()

# Mapeamento DuckScript -> Pynput Keys
duck_to_pynput = {
    "ENTER": Key.enter,
    "SPACE": Key.space,
    "TAB": Key.tab,
    "ESCAPE": Key.esc,
    "UPARROW": Key.up,
    "DOWNARROW": Key.down,
    "LEFTARROW": Key.left,
    "RIGHTARROW": Key.right,
    "BACKSPACE": Key.backspace,
    "DELETE": Key.delete,
    "CTRL": Key.ctrl,
    "ALT": Key.alt,
    "SHIFT": Key.shift,
    "GUI": Key.cmd  # Command Key on macOS
}

# Lê o arquivo de script
with open("recorded_payload.txt", "r") as f:
    lines = f.readlines()

# Estados
modifiers_pressed = set()

def press_modifiers(mods):
    for mod in mods:
        if mod in duck_to_pynput:
            keyboard_controller.press(duck_to_pynput[mod])
            modifiers_pressed.add(mod)

def release_modifiers():
    for mod in modifiers_pressed:
        keyboard_controller.release(duck_to_pynput[mod])
    modifiers_pressed.clear()

print("Reproduzindo Payload...")

for line in lines:
    line = line.strip()
    if not line or line.startswith("#"):
        continue

    if line.startswith("DELAY"):
        _, delay_ms = line.split()
        time.sleep(int(delay_ms) / 1000)
    elif line.startswith("STRING"):
        _, text = line.split(" ", 1)
        keyboard_controller.type(text)
    else:
        # Pode ser uma combinação de modifiers + key
        parts = line.split()
        key_action = parts[-1]
        mods = parts[:-1]

        press_modifiers(mods)

        # Pressiona e solta a tecla
        if key_action in duck_to_pynput:
            keyboard_controller.press(duck_to_pynput[key_action])
            keyboard_controller.release(duck_to_pynput[key_action])
        else:
            keyboard_controller.press(key_action)
            keyboard_controller.release(key_action)

        release_modifiers()

print("Execução Finalizada.")
