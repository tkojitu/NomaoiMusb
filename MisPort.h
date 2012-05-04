// -*- mode: c++ -*-
#pragma once

#include <Arduino.h>
#include <SoftwareSerial.h>

class MisPort {
private:
    SoftwareSerial* output;

public:
    MisPort();
    ~MisPort();
    void setup();
    void setBankDefault();
    void setChannelVolume(byte channel, byte volume);
    void noteOn(byte channel, byte note, byte attack_velocity);
    void noteOff(byte channel, byte note, byte release_velocity);
    void sendShort(byte channel, byte command, byte data1, byte data2);
    void send(byte cmd, byte data1, byte data2);

private:
    void setRealtimeMidiMode();
};
