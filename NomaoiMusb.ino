// -*- mode: c++ -*-
#include <Usb.h>
#include "CollinMidi.h"

void* gCollin;

void setup() {
    Serial.begin(115200);
    gCollin = new CollinMidi();
    ((CollinMidi*)gCollin)->setup();
    delay(200);
}

void loop() {
    ((CollinMidi*)gCollin)->loop();
}

