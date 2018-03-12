/*
  " HELLO DRUM LIBRARY" Ver.0.1

  Created March 1, 2018
  by Ryo Kosaka

  https://open-e-drums.tumblr.com/
*/

/* NOTICE

  You have to install some library.
  1.MIDI : https://playground.arduino.cc/Main/MIDILibrary

  If you use LCD,
  2.LCD Bar Graph : https://playground.arduino.cc/Code/LcdBarGraph

*/

/* EXAMPLE 1 - Simple Single Piezo Sensing
 

*/


///////////////////////////////   INITIAL VALUE   ///////////////////////////////////

//{sensitivity, threshold1, threshold2, retrigger, note}

int SNARE[6] = {800, 20, 50, 10, 38};

/////////////////////////////////////////////////////////////////////////////////////

#include <MIDI.h>
#include <hellodrum.h>
MIDI_CREATE_DEFAULT_INSTANCE();

HelloDrum snare(0);

void setup() {
  MIDI.begin();
  Serial.begin(38400);
}

void loop() {

  snare.singlePiezo(SNARE[0], SNARE[1], SNARE[2], SNARE[3]);

  if (snare.hit == true) {
    MIDI.sendNoteOn(SNARE[4], snare.velocity, 10);  //(note, velocity, channel)
    MIDI.sendNoteOff(SNARE[4], 0, 10);
  }

}

