/*
  EXAMPLE - Single Piezo Hi-Hat with hihat Controller (FSR or TCRT5000)

  https://open-e-drums.tumblr.com/
*/

/* NOTICE

  You have to install the MIDI library.
  MIDI : https://playground.arduino.cc/Main/MIDILibrary

*/

//////////////////////////////////////////   SETTING VALUE   ////////////////////////////////////////////

//Determine the setting value.
//By changing the number in this array you can set sensitivity, threshold and so on.

byte HIHAT[6] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan start
    30,  //scan end
    46,  //note of open
    42,  //note of close
};

byte HIHAT_PEDAL[6] = {
    90, //sensitivity
    30, //threshold
    60, //scan start
    80, //scan end
    10, //pedal sensitivity
    44  //note of pedal
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <MIDI.h>
#include <hellodrum.h>
MIDI_CREATE_DEFAULT_INSTANCE();

//Please name your piezo and sensor.
//So, you need the same number of lines as the number of pads.
//The piezo named "hihat" is connected to the A0 pin.
//The FSR or TCRT5000 named "hihatPedal" is connected to the A1 pin.
HelloDrum hihat(0);
HelloDrum hihatPedal(1);

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
  hihat.HH(HIHAT[0], HIHAT[1], HIHAT[2], HIHAT[3]);
  hihatPedal.hihatControl(HIHAT_PEDAL[0], HIHAT_PEDAL[1], HIHAT_PEDAL[2], HIHAT_PEDAL[3], HIHAT_PEDAL[4]); //hihatControl(byte sens, byte thre, byte scanStart, byte scanEnd, byte pedalSens);

  //MIDI signals are transmitted with this IF statement.
  if (hihat.hit == true)
  {
    //check open or close
    //1.open
    if (hihatPedal.openHH == true)
    {
      MIDI.sendNoteOn(HIHAT[4], hihat.velocity, 10); //(note, velocity, channel)
      MIDI.sendNoteOff(HIHAT[4], 0, 10);
    }
    //2.close
    else
    {
      MIDI.sendNoteOn(HIHAT[5], hihat.velocity, 10); //(note, velocity, channel)
      MIDI.sendNoteOff(HIHAT[5], 0, 10);
    }
  }

  //when pedal is closed
  if (hihatPedal.closeHH == true)
  {
    MIDI.sendNoteOn(HIHAT_PEDAL[5], hihatPedal.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(HIHAT_PEDAL[5], 0, 10);
  }

  //sending state of pedal with controll change
  if (hihatPedal.moving == true)
  {
    MIDI.sendControlChange(4, hihatPedal.pedalCC, 10);
  }
}
