/*
  EXAMPLE - 3 Zone Cymbal Sensing

  This sample code tesed with YAMAHA PCY135/PCY155.
  https://open-e-drums.tumblr.com/
*/

/* NOTICE

  You have to install the MIDI library.
  MIDI : https://playground.arduino.cc/Main/MIDILibrary

*/

//////////////////////////////////////////   SETTING VALUE   ////////////////////////////////////////////

//Determine the setting value.
//By changing the number in this array you can set sensitivity, threshold and so on.

byte RIDE[10] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    4,   //edge threshold
    9,   //cup threshold
    1,   //curve type(0-4)
    51,  //note of bow
    59,  //note of edge
    53   //note of cup
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <MIDI.h>
#include <hellodrum.h>
MIDI_CREATE_DEFAULT_INSTANCE();

//Please name your piezo.
//The piezo named ride is connected to the A0 pin and A1 pin.
HelloDrum ride(0, 1);

void setup()
{
  //If you use Hairless MIDI, you have to comment out the next line.
  //MIDI.begin(10);

  //And uncomment the next two lines.
  MIDI.begin();
  Serial.begin(38400);

  //Set Curve Type
  ride.setCurve(RIDE[6]);
}

void loop()
{
  //Piezo sensing is done in this line. And it is returned as a velocity of 127 stages.
  //For each pad, one line is required.
  //So, you need the same number of lines as the number of pads or controller.
  ride.cymbal3zone(RIDE[0], RIDE[1], RIDE[2], RIDE[3], RIDE[4], RIDE[5]); //cymbal3zone(byte sens, byte thre, byte scan, byte mask, byte edgeThre, byte cupThre);

  //MIDI signals are transmitted with this IF statement.
  //bow
  if (ride.hit == true)
  {
    MIDI.sendNoteOn(RIDE[7], ride.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(RIDE[7], 0, 10);
  }

  //edge
  else if (ride.hitRim == true)
  {
    MIDI.sendNoteOn(RIDE[8], ride.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(RIDE[8], 0, 10);
  }

  //cup
  else if (ride.hitCup == true)
  {
    MIDI.sendNoteOn(RIDE[9], ride.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(RIDE[9], 0, 10);
  }

  //choke
  if (ride.choke == true)
  {
    MIDI.sendPolyPressure(RIDE[7], 127, 10);
    MIDI.sendPolyPressure(RIDE[8], 127, 10);
    MIDI.sendPolyPressure(RIDE[9], 127, 10);
    MIDI.sendPolyPressure(RIDE[7], 0, 10);
    MIDI.sendPolyPressure(RIDE[8], 0, 10);
    MIDI.sendPolyPressure(RIDE[9], 0, 10);
  }
}
