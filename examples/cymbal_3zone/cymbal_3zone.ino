/*
  EXAMPLE - 3 Zone Cymbal Sensing

  This sample code tesed with YAMAHA PCY135/PCY155.
  https://open-e-drums.tumblr.com/
*/

/*
  NOTICE

  You have to install some library.
  1.MIDI : https://playground.arduino.cc/Main/MIDILibrary
  2.LCD Bar Graph : https://playground.arduino.cc/Code/LcdBarGraph (Extra)
*/

//////////////////////////////////////////   INITIAL VALUE   ////////////////////////////////////////////

//Determine the initial value.
//By changing the number in this array you can set sensitivity, threshold and so on.

//{sensitivity, threshold1, threshold2, retrigger, note of bow, note of edge, note of cup}
int RIDE[7] = {500, 50, 50, 10, 51, 59, 53};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <MIDI.h>
#include <hellodrum.h>
MIDI_CREATE_DEFAULT_INSTANCE();

//Please name your piezo.
//The piezo named ride is connected to the A5 pin and A15 pin.
HelloDrum ride(5, 15);


void setup() {
  //If you use Hairless MIDI, you have to comment out the next line.
  //MIDI.begin(10);

  //And uncomment the next two lines.
  MIDI.begin();
  Serial.begin(38400);
}

void loop() {

  //Piezo sensing is done in this line. And it is returned as a velocity of 127 stages.
  //For each pad, one line is required.
  //So, you need the same number of lines as the number of pads or controller.
  ride.cymbal3zone(RIDE[0], RIDE[1], RIDE[2], RIDE[3]);


  //MIDI signals are transmitted with this IF statement.
  //bow
  if (ride.hit == true) {
    MIDI.sendNoteOn(RIDE[4], ride.velocity, 10);  //(note, velocity, channel)
    MIDI.sendNoteOff(RIDE[4], 0, 10);
  }

  //edge
  else if (ride.hit_edge == true) {
    MIDI.sendNoteOn(RIDE[5], ride.velocity, 10);  //(note, velocity, channel)
    MIDI.sendNoteOff(RIDE[5], 0, 10);
  }

  //cup
  else if (ride.hit_cup == true) {
    MIDI.sendNoteOn(RIDE[6], ride.velocity, 10);  //(note, velocity, channel)
    MIDI.sendNoteOff(RIDE[6], 0, 10);
  }

  //choke
  if (ride.choke == true) {
    MIDI.sendPolyPressure(RIDE[4], 127, 10);
    MIDI.sendPolyPressure(RIDE[5], 127, 10);
    MIDI.sendPolyPressure(RIDE[6], 127, 10);
    MIDI.sendPolyPressure(RIDE[4], 0, 10);
    MIDI.sendPolyPressure(RIDE[5], 0, 10);
    MIDI.sendPolyPressure(RIDE[6], 0, 10);
  }
}

