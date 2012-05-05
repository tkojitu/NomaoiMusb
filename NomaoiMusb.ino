// -*- mode: c++ -*-
#include <Usb.h> // include here for CollinMidi.h
#include <SoftwareSerial.h> // include here for MisPort.h
#include "CollinMidiReceiver.h"
#include "CollinMidi.h"
#include "MisPort.h"

class NomaoiMusb : public CollinMidiReceiver {
public:
    CollinMidi* collin;
    MisPort* misport;

public:
    NomaoiMusb() : collin(NULL), misport(NULL) {
        collin = new CollinMidi(this);
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

    virtual void receive(byte cmd, byte data1, byte data2) {
        misport->send(cmd, data1, data2);
        dumpMidi(cmd, data1, data2);
    }

    void dumpMidi(byte cmd, byte data1, byte data2) {
        Serial.print(cmd, HEX);
        Serial.print(" ");
        Serial.print(data1, HEX);
        Serial.print(" ");
        Serial.print(data2, HEX);
        Serial.print("\n");
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
