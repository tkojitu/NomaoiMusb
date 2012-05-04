#include "MisPort.h"

MisPort::MisPort() : output(NULL) {
    output = new SoftwareSerial(2, 3);
}

MisPort::~MisPort() {
    delete output;
}

void MisPort::setup() {
    output->begin(31250);
    setRealtimeMidiMode();
    setChannelVolume(0, 120);
}

void MisPort::setRealtimeMidiMode() {
    byte RESET_MIDI = 4;
    ::pinMode(RESET_MIDI, OUTPUT);
    ::digitalWrite(RESET_MIDI, LOW);
    ::delay(100);
    ::digitalWrite(RESET_MIDI, HIGH);
    ::delay(100);
}

void MisPort::setBankDefault() {
    send(0xB0, 0, 0x00);
}

void MisPort::setChannelVolume(byte channel, byte volume) {
    sendShort(0, 0xB0, 0x07, volume);
}

void MisPort::noteOn(byte channel, byte note, byte attack_velocity) {
    sendShort(channel, 0x90, note, attack_velocity);
}

void MisPort::noteOff(byte channel, byte note, byte release_velocity) {
    sendShort(channel, 0x80, note, release_velocity);
}

void MisPort::sendShort(byte channel, byte command, byte data1, byte data2) {
    send(channel | command, data1, data2);
}

void MisPort::send(byte cmd, byte data1, byte data2) {
    output->write(cmd);
    output->write(data1);
    if ((cmd & 0xF0) <= 0xB0) {
        output->write(data2);
    }
}
