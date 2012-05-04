//////////////////////////////////////////////////////
// LPK25 USB-MIDI to Serial MIDI converter
// for use with USB Host Shield from Circuitsathome.com
// by Collin Cunningham - makezine.com, narbotic.com
//
// modified for NomaoiMusb by tkojitu
//////////////////////////////////////////////////////

#include <Max3421e.h>
#include <Usb.h>
#include <avr/pgmspace.h>

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

/* LPK25 data taken from descriptors */
// descriptors retrieved using descriptor_parser.pde (Usb library
// example sketch)
#define USB_ADDR            1
#define USB_CONFIG          1
#define USB_NUM_EP          3
#define EP_MAXPKTSIZE       64
#define EP_BULK             0x02
#define EP_POLL             0x00 // 0x0B for Korg nanoKey
#define CONTROL_EP          0
#define OUTPUT_EP           1
#define INPUT_EP            2
#define USB_01_REPORT_LEN   0x09
#define USB_DESCR_LEN       0x0C

char descrBuf[12] = {0}; // buffer for device description data
char buf[4] = {0}; // buffer for USB-MIDI data
char oldBuf[4] = {0}; // buffer for old USB-MIDI data
byte outBuf[3] = {0}; // buffer for outgoing MIDI data

class CollinMidi {
public:
    MAX3421E max;
    USB usb;
    EP_RECORD endpoints[USB_NUM_EP];

public:
    void powerOnMax() {
        max.powerOn();
    }

    void loop() {
        doTasks();
        if (usb.getUsbTaskState() == USB_STATE_CONFIGURING) {
            initUsb();
            usb.setUsbTaskState(USB_STATE_RUNNING);
        }
        if (usb.getUsbTaskState() == USB_STATE_RUNNING) {
            pollUsb();
        }
    }

    void doTasks() {
        max.Task();
        usb.Task();
    }

    void initUsb() {
        byte rcode = 0;
        byte i;

        /* Initialize data structures for endpoints of device 1*/
        // copy endpoint 0 parameters
        endpoints[CONTROL_EP] = *(usb.getDevTableEntry(0, 0));

        // Output endpoint, 0x02 for Keystation mini 32
        endpoints[OUTPUT_EP].epAddr = 0x02;
        endpoints[OUTPUT_EP].Attr = EP_BULK;
        endpoints[OUTPUT_EP].MaxPktSize = EP_MAXPKTSIZE;
        endpoints[OUTPUT_EP].Interval = EP_POLL;
        endpoints[OUTPUT_EP].sndToggle = bmSNDTOG0;
        endpoints[OUTPUT_EP].rcvToggle = bmRCVTOG0;

        // Input endpoint, 0x01 for Keystation mini 32
        endpoints[INPUT_EP].epAddr = 0x01;
        endpoints[INPUT_EP].Attr = EP_BULK;
        endpoints[INPUT_EP].MaxPktSize = EP_MAXPKTSIZE;
        endpoints[INPUT_EP].Interval = EP_POLL;
        endpoints[INPUT_EP].sndToggle = bmSNDTOG0;
        endpoints[INPUT_EP].rcvToggle = bmRCVTOG0;

        // plug kbd.endpoint parameters to devtable
        usb.setDevTableEntry(USB_ADDR, endpoints);

        // read the device descriptor and check VID and PID
        rcode = usb.getDevDescr(USB_ADDR, endpoints[CONTROL_EP].epAddr,
                                USB_DESCR_LEN, descrBuf);
        if (rcode) {
            Serial.print("Error attempting read device descriptor. Return code: ");
            Serial.println(rcode, HEX);
            while (1);
        }

        /* Configure device */
        rcode = usb.setConf(USB_ADDR, endpoints[CONTROL_EP].epAddr, USB_CONFIG);
        if (rcode) {
            Serial.print("Error attempting to configure USB. Return code: ");
            Serial.println(rcode, HEX);
            while (1);
        }
        Serial.println("USB initialized");
        Serial.println("");
        Serial.println("");
        Serial.println("");
        delay(200);
    }

    void pollUsb() {
        byte rcode = usb.inTransfer(USB_ADDR, endpoints[INPUT_EP].epAddr,
                                    USB_01_REPORT_LEN, buf);
        if (rcode != 0) {
            return;
        }
        reportProcess();
    }

    void reportProcess() {
        byte i, codeIndexNumber;
        for (i = 0; i < 4; i++) {
            if (buf[i] != oldBuf[i]) {
                break;
            }
        }
        if (i == 4) {
            return;
        }
        outBuf[0] = byte(buf[1]);
        outBuf[1] = byte(buf[2]);
        outBuf[2] = byte(buf[3]);
        Serial.print(outBuf[0], HEX);
        Serial.print(" ");
        Serial.print(outBuf[1], HEX);
        Serial.print(" ");
        Serial.print(outBuf[2], HEX);
        Serial.print("\n");
    }
};

void* gCollin;

void setup()
{
    Serial.begin(115200);
    gCollin = new CollinMidi();
    ((CollinMidi*)gCollin)->powerOnMax();
    delay(200);
}

void loop()
{
    ((CollinMidi*)gCollin)->loop();
}
