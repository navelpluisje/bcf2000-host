#include <Arduino.h>
#include <USBHost_t36.h> 

// Create the ports for USB devices plugged into Teensy's 2nd USB port (via hubs)
USBHost myusb;
USBHub hub1(myusb);
MIDIDevice midi01(myusb);

// A variable to know how long the LED has been turned on
elapsedMillis ledOnMillis;

void sendToComputer(byte type, byte data1, byte data2, byte channel, const uint8_t *sysexarray, byte cable)
{
  if (type != usbMIDI.SystemExclusive) {
    usbMIDI.send(type, data1, data2, channel, cable);
  } else {
    unsigned int SysExLength = data1 + data2 * 256;
    usbMIDI.sendSysEx(SysExLength, sysexarray, true, cable);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(13, OUTPUT); // LED pin
  digitalWrite(13, LOW);
  // Wait 1.5 seconds before turning on USB Host.  If connected USB devices
  // use too much power, Teensy at least completes USB enumeration, which
  // makes isolating the power issue easier.
  delay(1500);
  Serial.println("BCF2000 Host");
  delay(10);
  myusb.begin();
}


void loop() {
  bool activity = false;

  // Next read messages arriving from the BCF2000
  if (midi01.read()) {
    uint8_t type =       midi01.getType();
    uint8_t data1 =      midi01.getData1();
    uint8_t data2 =      midi01.getData2();
    uint8_t channel =    midi01.getChannel();
    const uint8_t *sys = midi01.getSysExArray();
    sendToComputer(type, data1, data2, channel, sys, 1);
    activity = true;
  }

  // Finally, read any messages the PC sends to Teensy, and forward them
  // to the BCF2000.
  if (usbMIDI.read()) {
    // get the USB MIDI message, defined by these 5 numbers (except SysEX)
    byte type = usbMIDI.getType();
    byte channel = usbMIDI.getChannel();
    byte data1 = usbMIDI.getData1();
    byte data2 = usbMIDI.getData2();
    byte cable = usbMIDI.getCable();

    // forward this message to 1 of the 3 Serial MIDI OUT ports
    if (type != usbMIDI.SystemExclusive) {
      // Then simply give the data to the MIDI library send()
      midi01.send(type, data1, data2, channel);

    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      midi01.sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
    }
    activity = true;
  }

  // blink the LED when any activity has happened
  if (activity) {
    digitalWriteFast(13, HIGH); // LED on
    ledOnMillis = 0;
  }
  if (ledOnMillis > 15) {
    digitalWriteFast(13, LOW);  // LED off
  }
}
