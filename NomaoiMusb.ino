// -*- mode: c++ -*-
#include <Usb.h> // include here for CollinMidi.h
#include <SoftwareSerial.h> // include here for MisPort.h
#include "CollinMidi.h"
#include "MisPort.h"

class NomaoiMusb {
public:
    CollinMidi* collin;
    MisPort* misport;

public:
    NomaoiMusb() : collin(NULL), misport(NULL) {
        collin = new CollinMidi();
        misport = new MisPort();
    }

    ~NomaoiMusb() {
        delete collin;
        delete misport;
    }

    void setup() {
        Serial.begin(115200);
        collin->setup();
        delay(200);
        misport->setup();
        misport->setBankDefault();
    }

    void loop() {
        collin->loop();
    }
};

void* gApp;

void setup() {
    NomaoiMusb* app = new NomaoiMusb();
    gApp = app;
    app->setup();
}

void loop() {
    ((NomaoiMusb*)gApp)->loop();
}
