#ifndef __HID_RECORDER_H
#define __HID_RECORDER_H

#include "modules/badusb_ble/ducky_typer.h"

// Declaração da função externa do ducky_typer.cpp
extern void ducky_chooseKb(HIDInterface *&hid, bool ble);

// Função principal para menu do Bruce
void hid_recorder_setup();

// Função de gravação (baseada no ducky_keyboard)
void ducky_recorder(HIDInterface *&hid, bool ble);

#endif // __HID_RECORDER_H
