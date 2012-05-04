//////////////////////////////////////////////////////
// LPK25 USB-MIDI to Serial MIDI converter
// for use with USB Host Shield from Circuitsathome.com
// by Collin Cunningham - makezine.com, narbotic.com
//
// modified for NomaoiMusb by tkojitu
//////////////////////////////////////////////////////

#include "CollinMidi.h"

// For general USB-MIDI compliance, we need to determine which
// endpoint has a direction of IN and likely also need to get the poll
// duration(EP_POLL), not sure if necessary though
//

/*
  USB ERROR CODES
   hrSUCCESS   0x00
   hrBUSY      0x01
   hrBADREQ    0x02
   hrUNDEF     0x03
   hrNAK       0x04
   hrSTALL     0x05
   hrTOGERR    0x06
   hrWRONGPID  0x07
   hrBADBC     0x08
   hrPIDERR    0x09
   hrPKTERR    0x0A
   hrCRCERR    0x0B
   hrKERR      0x0C
   hrJERR      0x0D
   hrTIMEOUT   0x0E
   hrBABBLE    0x0F
*/

//////////////////////////
// MIDI MESAGES
// midi.org/techspecs/
//////////////////////////
// STATUS BYTES
// 0x80 == noteOff
// 0x90 == noteOn
// 0xA0 == afterTouch
// 0xB0 == controlChange
//////////////////////////
// DATA BYTE 1
// note# == (0-127)
// or
// control# == (0-119)
//////////////////////////
// DATA BYTE 2
// velocity == (0-127)
// or
// controlVal == (0-127)
//////////////////////////

CollinMidi::CollinMidi() : max3421e(NULL), usb(NULL) {
    max3421e = new MAX3421E();
    usb = new USB();
    memset(descrBuf, 0, sizeof(descrBuf));
    memset(buf, 0, sizeof(buf));
    memset(oldBuf, 0, sizeof(buf));
    // avr libc does not support new/delete [].
    endpoints = (EP_RECORD*)calloc(USB_NUM_EP, sizeof(EP_RECORD));
}

CollinMidi::~CollinMidi() {
    delete max3421e;
    delete usb;
    free(endpoints);
}

void CollinMidi::setup() {
    max3421e->powerOn();
}

void CollinMidi::loop() {
    doTasks();
    if (usb->getUsbTaskState() == USB_STATE_CONFIGURING) {
        initUsb();
        usb->setUsbTaskState(USB_STATE_RUNNING);
    }
    if (usb->getUsbTaskState() == USB_STATE_RUNNING) {
        pollUsb();
    }
}

void CollinMidi::doTasks() {
    max3421e->Task();
    usb->Task();
}

void CollinMidi::initUsb() {
    byte rcode = 0;
    initEndpointControl();
    initEndpointInput();
    initEndpointOutput();
    usb->setDevTableEntry(USB_ADDR, endpoints);
    readDeviceDesc();
    configureDevice();
    printGreeting();
    delay(200);
}

void CollinMidi::initEndpointControl() {
    endpoints[CONTROL_EP] = *(usb->getDevTableEntry(0, 0));
}

void CollinMidi::initEndpointInput() {
    // Input endpoint, 0x01 for Keystation mini 32
    endpoints[INPUT_EP].epAddr = 0x01;
    endpoints[INPUT_EP].Attr = EP_BULK;
    endpoints[INPUT_EP].MaxPktSize = EP_MAXPKTSIZE;
    endpoints[INPUT_EP].Interval = EP_POLL;
    endpoints[INPUT_EP].sndToggle = bmSNDTOG0;
    endpoints[INPUT_EP].rcvToggle = bmRCVTOG0;
}

void CollinMidi::initEndpointOutput() {
    // Output endpoint, 0x02 for Keystation mini 32
    endpoints[OUTPUT_EP].epAddr = 0x02;
    endpoints[OUTPUT_EP].Attr = EP_BULK;
    endpoints[OUTPUT_EP].MaxPktSize = EP_MAXPKTSIZE;
    endpoints[OUTPUT_EP].Interval = EP_POLL;
    endpoints[OUTPUT_EP].sndToggle = bmSNDTOG0;
    endpoints[OUTPUT_EP].rcvToggle = bmRCVTOG0;
}

void CollinMidi::readDeviceDesc() {
    byte rcode = usb->getDevDescr(USB_ADDR, endpoints[CONTROL_EP].epAddr,
                                  USB_DESCR_LEN, descrBuf);
    if (rcode) {
        Serial.print("Error attempting read device descriptor. Return code: ");
        Serial.println(rcode, HEX);
        while (1);
    }
}

void CollinMidi::configureDevice() {
    byte rcode = usb->setConf(USB_ADDR, endpoints[CONTROL_EP].epAddr,
                              USB_CONFIG);
    if (rcode) {
        Serial.print("Error attempting to configure USB. Return code: ");
        Serial.println(rcode, HEX);
        while (1);
    }
}

void CollinMidi::printGreeting() {
    Serial.println("USB initialized");
    Serial.println("");
    Serial.println("");
    Serial.println("");
}

void CollinMidi::pollUsb() {
    byte rcode = usb->inTransfer(USB_ADDR, endpoints[INPUT_EP].epAddr,
                                 USB_01_REPORT_LEN, buf);
    if (rcode != 0) {
        return;
    }
    reportProcess();
}

void CollinMidi::reportProcess() {
    Serial.print(byte(buf[1]), HEX);
    Serial.print(" ");
    Serial.print(byte(buf[2]), HEX);
    Serial.print(" ");
    Serial.print(byte(buf[3]), HEX);
    Serial.print("\n");
}
