/*
  EXAMPLE - MUX Piezo Sensing With ESP32(BLE MIDI)

  With this sample code, you can make the octapad with ESP32 and 74HC4051.
  And send MIDI signals with bluetooth.
  https://github.com/RyoKosaka/HelloDrum-arduino-Library
*/

///////////////////////////////   SETTING VALUE   ///////////////////////////////////

//Determine the setting value.
//By changing the number in this array you can set sensitivity, threshold and so on.

byte PAD1[5] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    38   //note
};

byte PAD2[5] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    36   //note
};

byte PAD3[5] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    41   //note
};

byte PAD4[5] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    46   //note
};

byte PAD5[5] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    47   //note
};

byte PAD6[5] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    48   //note
};

byte PAD7[5] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    55   //note
};

byte PAD8[5] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    84   //note
};

/////////////////////////////////////////////////////////////////////////////////////

#include <hellodrum.h>
#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32.h>
BLEMIDI_CREATE_DEFAULT_INSTANCE();

/*
Mux Breakout ----------- ESP32
S0 ------------------- 4
S1 ------------------- 16
S2 ------------------- 17
Z -------------------- 33
 */
//Define MUX Pins
HelloDrumMUX_4051 mux(4, 16, 17, 33);

//Please name your piezo.
HelloDrum pad1(0); //mux pin
HelloDrum pad2(1); //mux pin
HelloDrum pad3(2); //mux pin
HelloDrum pad4(3); //mux pin
HelloDrum pad5(4); //mux pin
HelloDrum pad6(5); //mux pin
HelloDrum pad7(6); //mux pin
HelloDrum pad8(7); //mux pin

void setup()
{
  MIDI.begin();
}

void loop()
{
  //scanning 8 piezos.
  mux.scan();

  //Piezo sensing is done in this line. And it is returned as a velocity of 127 stages.
  //For each piezo, one line is required.
  pad1.singlePiezoMUX(PAD1[0], PAD1[1], PAD1[2], PAD1[3]);
  pad2.singlePiezoMUX(PAD2[0], PAD2[1], PAD2[2], PAD2[3]);
  pad3.singlePiezoMUX(PAD3[0], PAD3[1], PAD3[2], PAD3[3]);
  pad4.singlePiezoMUX(PAD4[0], PAD4[1], PAD4[2], PAD4[3]);
  pad5.singlePiezoMUX(PAD5[0], PAD5[1], PAD5[2], PAD5[3]);
  pad6.singlePiezoMUX(PAD6[0], PAD6[1], PAD6[2], PAD6[3]);
  pad7.singlePiezoMUX(PAD7[0], PAD7[1], PAD7[2], PAD7[3]);
  pad8.singlePiezoMUX(PAD8[0], PAD8[1], PAD8[2], PAD8[3]);

  if (pad1.hit == true)
  {
    MIDI.sendNoteOn(PAD1[4], pad1.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(PAD1[4], 0, 10);
  }

  if (pad2.hit == true)
  {
    MIDI.sendNoteOn(PAD2[4], pad2.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(PAD2[4], 0, 10);
  }

  if (pad3.hit == true)
  {
    MIDI.sendNoteOn(PAD3[4], pad3.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(PAD3[4], 0, 10);
  }

  if (pad4.hit == true)
  {
    MIDI.sendNoteOn(PAD4[4], pad4.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(PAD4[4], 0, 10);
  }

  if (pad5.hit == true)
  {
    MIDI.sendNoteOn(PAD5[4], pad5.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(PAD5[4], 0, 10);
  }

  if (pad6.hit == true)
  {
    MIDI.sendNoteOn(PAD6[4], pad6.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(PAD6[4], 0, 10);
  }

  if (pad7.hit == true)
  {
    MIDI.sendNoteOn(PAD7[4], pad7.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(PAD7[4], 0, 10);
  }

  if (pad8.hit == true)
  {
    MIDI.sendNoteOn(PAD8[4], pad8.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(PAD8[4], 0, 10);
  }
}