/*
  EXAMPLE - 2 Zone Cymbal Sensing

  This sample code is tesed with YAMAHA PCY135/PCY155.
  https://open-e-drums.tumblr.com/
*/

/* NOTICE

  You have to install the MIDI library.
  MIDI : https://playground.arduino.cc/Main/MIDILibrary

*/

//////////////////////////////////////////   SETTING VALUE   ////////////////////////////////////////////

//Determine the setting value.
//By changing the number in this array you can set sensitivity, threshold and so on.

byte CRASH[8] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    5,   //edge Threshold
    1,   // curve type(0-4)
    49,  //note of bow
    55   //note of edge
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <MIDI.h>
#include <hellodrum.h>
MIDI_CREATE_DEFAULT_INSTANCE();

//Please name your cymbal.
//The cymbal named "crash" is connected to the A0 pin (piezo) and A1 pin (edge sensor).
HelloDrum crash(0, 1);

void setup()
{
  //If you use Hairless MIDI, you have to comment out the next line.
  //MIDI.begin(10);

  //And uncomment the next two lines.
  MIDI.begin();
  Serial.begin(38400);

  //Set Curve Type
  crash.setCurve(CRASH[5]);
}

void loop()
{
  //Piezo sensing is done in this line. And it is returned as a velocity of 127 stages.
  //For each pad, one line is required.
  //So, you need the same number of lines as the number of pads or controller.
  crash.cymbal2zone(CRASH[0], CRASH[1], CRASH[2], CRASH[3], CRASH[4]); //cymbal2zone(byte sens, byte thre, byte scan, byte mask, byte edgeThre);

  //MIDI signals are transmitted with this IF statement.
  //bow
  if (crash.hit == true)
  {
    MIDI.sendNoteOn(CRASH[6], crash.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(CRASH[6], 0, 10);
  }

  //edge
  else if (crash.hitRim == true)
  {
    MIDI.sendNoteOn(CRASH[7], crash.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(CRASH[7], 0, 10);
  }

  //choke
  if (crash.choke == true)
  {
    MIDI.sendPolyPressure(CRASH[6], 127, 10);
    MIDI.sendPolyPressure(CRASH[7], 127, 10);
    MIDI.sendPolyPressure(CRASH[6], 0, 10);
    MIDI.sendPolyPressure(CRASH[7], 0, 10);
  }
}
