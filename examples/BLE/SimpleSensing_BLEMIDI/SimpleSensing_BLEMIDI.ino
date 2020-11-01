/*
  EXAMPLE - Simple Piezo Sensing With ESP32(BLE MIDI)

  With this sample code, you will make snare and tom using two piezo.
  And send MIDI signals with bluetooth. 
  https://github.com/RyoKosaka/HelloDrum-arduino-Library
*/

///////////////////////////////   SETTING VALUE   ///////////////////////////////////

//Determine the setting value.
//By changing the number in this array you can set sensitivity, threshold and so on.

byte SNARE[6] = {
    100, //sensitivity (1-100)
    10,  //threshold (1-100)
    10,  //scan time (1-)
    30,  //mask time (1-)
    38,  //note (0-127)
    1    //curve type (0-4)
};

byte TOM[6] = {
    100, //sensitivity (1-100)
    10,  //threshold (1-100)
    10,  //scan time (1-)
    30,  //mask time(1-)
    50,  //note (0-127)
    1    //curve type(0-4)
};

/////////////////////////////////////////////////////////////////////////////////////

#include <hellodrum.h>

//The next only three lines are about BLE-MIDI.
//Don't forget to install the BLE-MIDI library.
//https://github.com/lathoub/Arduino-BLE-MIDI
#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32.h>
BLEMIDI_CREATE_DEFAULT_INSTANCE();

//Please name your piezo.
//The piezo named snare is connected to the IO33 pin and the piezo named tom is connected to the IO36 pin.
HelloDrum snare(33);
HelloDrum tom(36);

void setup()
{
  MIDI.begin();

  //Set Curve Type
  snare.setCurve(SNARE[5]);
  tom.setCurve(TOM[5]);
}

void loop()
{
  //Piezo sensing is done in this line. And it is returned as a velocity of 127 stages.
  //For each piezo, one line is required.
  snare.singlePiezo(SNARE[0], SNARE[1], SNARE[2], SNARE[3]);
  tom.singlePiezo(TOM[0], TOM[1], TOM[2], TOM[3]);

  //MIDI signals are transmitted with this IF statement.
  //For each piezo, one IF statement is required
  if (snare.hit == true)
  {
    MIDI.sendNoteOn(SNARE[4], snare.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(SNARE[4], 0, 10);
  }

  if (tom.hit == true)
  {
    MIDI.sendNoteOn(TOM[4], tom.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(TOM[4], 0, 10);
  }
}