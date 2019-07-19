/*
  EXAMPLE - 2 Zone Cymbal Sensing

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

int CRASH[6] = {
    80, //sensitivity
    3,  //threshold
    20, //scan time
    30, //mask time
    49, //note of bow
    55  //note of edge
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <MIDI.h>
#include <hellodrum.h>
MIDI_CREATE_DEFAULT_INSTANCE();

//Please name your piezo.
//The piezo named "crash" is connected to the A0 pin and A1 pin.
HelloDrum crash(0, 1);

void setup()
{
  //If you use Hairless MIDI, you have to comment out the next line.
  //MIDI.begin(10);

  //And uncomment the next two lines.
  MIDI.begin();
  Serial.begin(38400);
}

void loop()
{
  //Piezo sensing is done in this line. And it is returned as a velocity of 127 stages.
  //For each pad, one line is required.
  //So, you need the same number of lines as the number of pads or controller.
  crash.cymbal2zone(CRASH[0], CRASH[1], CRASH[2], CRASH[3]);

  //MIDI signals are transmitted with this IF statement.
  //bow
  if (crash.hit == true)
  {
    MIDI.sendNoteOn(CRASH[4], crash.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(CRASH[4], 0, 10);
  }

  //edge
  else if (crash.hitRim == true)
  {
    MIDI.sendNoteOn(CRASH[5], crash.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(CRASH[5], 0, 10);
  }

  //choke
  if (crash.choke == true)
  {
    MIDI.sendPolyPressure(CRASH[4], 127, 10);
    MIDI.sendPolyPressure(CRASH[5], 127, 10);
    MIDI.sendPolyPressure(CRASH[4], 0, 10);
    MIDI.sendPolyPressure(CRASH[5], 0, 10);
  }
}
