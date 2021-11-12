# BCF2000 Midi Host

Since the M1 chips introduced by Apple, the good'ol Behringer BCF2000 does not work proper anymore. It does send data to the Mac, but does not receive any messages.

This project is about making the BCF2000 communicate well with any Mac. The code in this project is for a Teensy 4.0 which has the ability to act as a USB host. The teensy will be a man-in-the-middle, it communicates to the Mac as a class-compient MIDI device and passes the data as-is to the BCF. 

It is possible to build the Teensy inside the BCF2000. There is plenty of spcae in there.