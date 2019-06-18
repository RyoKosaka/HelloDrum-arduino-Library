/*
  EXAMPLE - MUX Sensing

  With this sample code, you can make octapad with a 74HC4051. 
  https://open-e-drums.tumblr.com/
*/

///////////////////////////////   INITIAL VALUE   ///////////////////////////////////

//Determine the initial value.
//By changing the number in this array you can set sensitivity, threshold and so on.

int PAD1[5] = {
  800, //sensitivity
  150, //threshold
  5,   //scan time
  10,  //mask time
  38   //note
};

int PAD2[5] = {
  800, //sensitivity
  150, //threshold
  5,   //scan time
  10,  //mask time
  36   //note
};

int PAD3[5] = {
  800, //sensitivity
  150, //threshold
  5,   //scan time
  10,  //mask time
  41   //note
};

int PAD4[5] = {
  800, //sensitivity
  150, //threshold
  5,   //scan time
  10,  //mask time
  46   //note
};

int PAD5[5] = {
  800, //sensitivity
  150, //threshold
  5,   //scan time
  10,  //mask time
  47   //note
};

int PAD6[5] = {
  800, //sensitivity
  150, //threshold
  5,   //scan time
  10,  //mask time
  48   //note
};

int PAD7[5] = {
  800, //sensitivity
  150, //threshold
  5,   //scan time
  10,  //mask time
  55   //note
};

int PAD8[5] = {
  800, //sensitivity
  150, //threshold
  5,   //scan time
  10,  //mask time
  84   //note
};

/////////////////////////////////////////////////////////////////////////////////////

#include <MIDI.h>
#include <hellodrum.h>
MIDI_CREATE_DEFAULT_INSTANCE();

/*
Mux Breakout ----------- Arduino
S0 ------------------- 2
S1 ------------------- 3
S2 ------------------- 4
Z -------------------- A0
 */
//Define MUX Pins
HelloDrumMUX mux(2,3,4,0);//D2, D3, D4, A0

HelloDrum pad1(0); //mux pin
HelloDrum pad2(1); //mux pin
HelloDrum pad3(2); //mux pin
HelloDrum pad4(3); //mux pin
HelloDrum pad5(4); //mux pin
HelloDrum pad6(5); //mux pin
HelloDrum pad7(6); //mux pin
HelloDrum pad8(7); //mux pin

void setup() {
  //If you use Hairless MIDI, you have to comment out the next line.
  //MIDI.begin(10);

  //And uncomment the next two lines.
  MIDI.begin();
  Serial.begin(38400);
}

void loop() {
  mux.scan();
  pad1.singlePiezoMUX(PAD1[0], PAD1[1], PAD1[2], PAD1[3]);
  pad2.singlePiezoMUX(PAD2[0], PAD2[1], PAD2[2], PAD2[3]);
  pad3.singlePiezoMUX(PAD3[0], PAD3[1], PAD3[2], PAD3[3]);
  pad4.singlePiezoMUX(PAD4[0], PAD4[1], PAD4[2], PAD4[3]);
  pad5.singlePiezoMUX(PAD5[0], PAD5[1], PAD5[2], PAD5[3]);
  pad6.singlePiezoMUX(PAD6[0], PAD6[1], PAD6[2], PAD6[3]);
  pad7.singlePiezoMUX(PAD7[0], PAD7[1], PAD7[2], PAD7[3]);
  pad8.singlePiezoMUX(PAD8[0], PAD8[1], PAD8[2], PAD8[3]);

  if (pad1.hit == true) {
    MIDI.sendNoteOn(PAD1[4], pad1.velocity, 10);  //(note, velocity, channel)
    MIDI.sendNoteOff(PAD1[4], 0, 10);
  }

  if (pad2.hit == true) {
    MIDI.sendNoteOn(PAD2[4], pad2.velocity, 10);  //(note, velocity, channel)
    MIDI.sendNoteOff(PAD2[4], 0, 10);
  }

  if (pad3.hit == true) {
    MIDI.sendNoteOn(PAD3[4], pad3.velocity, 10);  //(note, velocity, channel)
    MIDI.sendNoteOff(PAD3[4], 0, 10);
  }

  if (pad4.hit == true) {
    MIDI.sendNoteOn(PAD4[4], pad4.velocity, 10);  //(note, velocity, channel)
    MIDI.sendNoteOff(PAD4[4], 0, 10);
  }

  if (pad5.hit == true) {
    MIDI.sendNoteOn(PAD5[4], pad5.velocity, 10);  //(note, velocity, channel)
    MIDI.sendNoteOff(PAD5[4], 0, 10);
  }

  if (pad6.hit == true) {
    MIDI.sendNoteOn(PAD6[4], pad6.velocity, 10);  //(note, velocity, channel)
    MIDI.sendNoteOff(PAD6[4], 0, 10);
  }

  if (pad7.hit == true) {
    MIDI.sendNoteOn(PAD7[4], pad7.velocity, 10);  //(note, velocity, channel)
    MIDI.sendNoteOff(PAD7[4], 0, 10);
  }

  if (pad8.hit == true) {
    MIDI.sendNoteOn(PAD8[4], pad8.velocity, 10);  //(note, velocity, channel)
    MIDI.sendNoteOff(PAD8[4], 0, 10);
  }
}