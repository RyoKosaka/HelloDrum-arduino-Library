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

/* EXAMPLE 1 - Simple Piezo Sensing

 With this sample code, you will make snare and tom using two piezo.

*/


///////////////////////////////   INITIAL VALUE   ///////////////////////////////////

//Determine the initial value.
//By changing the number in this array you can set sensitivity, threshold and so on.

//{sensitivity, threshold1, threshold2, retrigger, note}
int SNARE[6] = {800, 20, 50, 10, 38};
int TOM[6] = {800, 20, 50, 10, 50};

/////////////////////////////////////////////////////////////////////////////////////

#include <MIDI.h>
#include <hellodrum.h>
MIDI_CREATE_DEFAULT_INSTANCE();

//Please name your piezo.
//The piezo named snare is connected to the A0 pin and the piezo named tom is connected to the A1 pin.

HelloDrum snare(0); 
HelloDrum tom(1);

void setup() {
  //If you use Hairless MIDI, you have to comment out the next line.
  //MIDI.begin(10);

  //And uncomment the next two lines.
  MIDI.begin();
  Serial.begin(38400);
}

void loop() {

  //Piezo sensing is done in this line. And it is returned as a velocity of 127 stages.
  //For each piezo, one line is required.
  snare.singlePiezo(SNARE[0], SNARE[1], SNARE[2], SNARE[3]);
  tom.singlePiezo(TOM[0], TOM[1], TOM[2], TOM[3]);
  
  
  //MIDI signals are transmitted with this IF statement.
  //For each piezo, one IF statement is required
  if (snare.hit == true) {
    MIDI.sendNoteOn(SNARE[4], snare.velocity, 10);  //(note, velocity, channel)
    MIDI.sendNoteOff(SNARE[4], 0, 10);
  }
  
  if (tom.hit == true) {
    MIDI.sendNoteOn(TOM[4], tom.velocity, 10);  //(note, velocity, channel)
    MIDI.sendNoteOff(TOM[4], 0, 10);
  }

}

