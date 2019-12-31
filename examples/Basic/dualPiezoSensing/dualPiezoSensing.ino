/*
  EXAMPLE - Dual Piezo Sensing

  With this sample code, you can make 2 zone snare drum.

  Pads & Controller Circuit:
    Piezo for head to A0.
    Piezo for rim to A1.

  https://open-e-drums.tumblr.com/
*/

/* NOTICE

  You have to install the MIDI library.
  MIDI : https://playground.arduino.cc/Main/MIDILibrary

*/

///////////////////////////////   SETTING VALUE   ///////////////////////////////////

//Determine the setting value.
//By changing the number in this array you can set sensitivity, threshold and so on.

int SNARE[7] = {
    100, //sensitivity (1-100)
    10,  //threshold (1-100)
    30,  //scan time (1-)
    10,  //mask time (1-)
    38,  //note (0-127)
    37,  //note of side stick (0-127)
    40   //note of rim shot (0-127)
};

/////////////////////////////////////////////////////////////////////////////////////

#include <hellodrum.h>
#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

//Please name your pad and controller.
HelloDrum snare(0, 1);

void setup()
{
  //If you use Hairless MIDI, you have to comment out the next line.
  //MIDI.begin(10);

  //And uncomment the next two lines. Please set the baud rate of Hairless to 38400.
  MIDI.begin();
  Serial.begin(38400);
}

void loop()
{
  //Sensing pad.
  snare.dualPiezo(SNARE[0], SNARE[1], SNARE[2], SNARE[3]);

  //Sending MIDI signals.
  //HEAD
  if (snare.hit == true)
  {
    MIDI.sendNoteOn(SNARE[4], snare.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(SNARE[4], 0, 10);
  }
  //RIM
  else if (snare.hitRim == true)
  {
    if (snare.velocity > 60)
    {
      MIDI.sendNoteOn(SNARE[6], snare.velocity, 10); //(note, velocity, channel)
      MIDI.sendNoteOff(SNARE[6], 0, 10);
    }
    else
    {
      MIDI.sendNoteOn(SNARE[5], snare.velocity * 2, 10); //(note, velocity, channel)
      MIDI.sendNoteOff(SNARE[5], 0, 10);
    }
  }
}
