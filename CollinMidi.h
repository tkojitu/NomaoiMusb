// -*- mode: c++ -*-
//////////////////////////////////////////////////////
// LPK25 USB-MIDI to Serial MIDI converter
// for use with USB Host Shield from Circuitsathome.com
// by Collin Cunningham - makezine.com, narbotic.com
//
// modified for NomaoiMusb by tkojitu
//////////////////////////////////////////////////////

#pragma once

#include <Usb.h>

class CollinMidiReceiver;

class CollinMidi {
private:
    enum {
        USB_ADDR = 1,
        USB_CONFIG = 1,
        EP_MAXPKTSIZE = 64,
        EP_BULK = 0x02,
        EP_POLL = 0x00, // 0x0B for Korg nanoKey
        USB_01_REPORT_LEN = 0x09,
        USB_DESCR_LEN = 0x0C
    };

    enum {
        USB_NUM_EP = 3,
        CONTROL_EP = 0,
        OUTPUT_EP = 1,
        INPUT_EP  = 2,
    };

    MAX3421E* max3421e;
    USB* usb;
    char descrBuf[12]; // buffer for device description data
    char buf[4]; // buffer for USB-MIDI data
    EP_RECORD* endpoints;
    CollinMidiReceiver* receiver;

public:
    CollinMidi(CollinMidiReceiver* receiver);
    ~CollinMidi();
    void setup();
    void loop();

private:
    void doTasks();
    void initUsb();
    void initEndpointControl();
    void initEndpointInput();
    void initEndpointOutput();
    void readDeviceDesc();
    void configureDevice();
    void printGreeting();
    void pollUsb();
    void reportProcess();
};
