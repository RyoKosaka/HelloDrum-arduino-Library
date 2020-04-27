/*
  EXAMPLE - Dual Piezo Sensing

  With this sample code, you can make 2 zone snare drum.
  Tested with Roland PD-85

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

byte SNARE[10] = {
    100, //sensitivity (1-100)
    10,  //threshold (1-100)
    30,  //scan time (1-100)
    10,  //mask time (1-100)
    10,  //rim sensitivity (1-100)
    3,   //rim threshold (1-100)
    1,   //curve type (0-4)
    38,  //note (0-127)
    40,  //note of rim shot (0-127)
    37   //note of cross stick (0-127)
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

  //Set Curve Type
  snare.setCurve(SNARE[6]);
}

void loop()
{

  //Sensing pad.
  snare.dualPiezo(SNARE[0], SNARE[1], SNARE[2], SNARE[3], SNARE[4], SNARE[5]); //dualPiezo(byte sens, byte thre, byte scan, byte mask, byte rimSens, byte rimThre);

  //Sending MIDI signals.
  //HEAD
  if (snare.hit == true)
  {
    MIDI.sendNoteOn(SNARE[7], snare.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(SNARE[7], 0, 10);
  }
  //RIM
  else if (snare.hitRim == true)
  {
    if (snare.velocity > 60)
    {
      MIDI.sendNoteOn(SNARE[8], snare.velocity, 10); //(note, velocity, channel)
      MIDI.sendNoteOff(SNARE[8], 0, 10);
    }
    else
    {
      MIDI.sendNoteOn(SNARE[9], snare.velocity * 2, 10); //(note, velocity, channel)
      MIDI.sendNoteOff(SNARE[9], 0, 10);
    }
  }
}
