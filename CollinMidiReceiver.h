// -*- mode: c++ -*-
#pragma once

class CollinMidiReceiver {
public:
    virtual void receive(byte cmd, byte data1, byte data2) = 0;
};
