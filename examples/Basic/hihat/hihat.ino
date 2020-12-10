/*
  EXAMPLE - Single Piezo Hi-Hat with hihat Controller (FSR or TCRT5000)

  https://github.com/RyoKosaka/HelloDrum-arduino-Library
*/

//////////////////////////////////////////   SETTING VALUE   ////////////////////////////////////////////

//Determine the setting value.
//By changing the number in this array you can set sensitivity, threshold and so on.

byte HIHAT[7] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    0,   //curve type
    46,  //note of open
    42   //note of close
};

byte HIHAT_PEDAL[7] = {
    90, //sensitivity (Hold the pedal at the position where you want to send 127(full open) in the MIDI CC, and read the Analog Read value in the TCRT5000. Divide the value by 10 and enter the value.)
    30, //threshold (Hold the pedal at the position where you want to send 0(close) in the MIDI CC, and read the Analog Read value in the TCRT5000. Divide the value by 10 and enter the value.)
    70, //scan start (When the analogRead value reaches this value * 10, the scan will start and the time will be recorded.)
    90, //scan end (When the analogRead value reaches this value * 10, the scan is finished and the time taken for the scan is referenced to determine the velocity.Then noteOn will be sent.The default value is 90, the same as Sensitivity. This means that the pedal is closed at the point where MIDI CC is 127.)
    10, //pedal sensitivity (This value multiplied by 100 is the time used to determine the velocity of the pedal. If the time taken from the scan start position to the scan end position is more than 1000 milliseconds, the velocity will be set to 1. So, if you feel that the velocity is low, either increase pedalSensitivity or increase the value of scan start.)
    0,  //curve type
    44  //note of pedal
};

//about setting, see also https://github.com/RyoKosaka/HelloDrum-arduino-Library/blob/master/docs/sensing.md

/////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <hellodrum.h>

//Using MIDI Library. If you want to use USB-MIDI, comment out the next two lines.
#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

//Uncomment the next two lines for using USB-MIDI with atmega32u4 or Teensy
//#include <USB-MIDI.h>
//USBMIDI_CREATE_DEFAULT_INSTANCE();

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

  //set curve type
  hihat.setCurve(HIHAT[4]);
  hihatPedal.setCurve(HIHAT_PEDAL[5])
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
      MIDI.sendNoteOn(HIHAT[5], hihat.velocity, 10); //(note, velocity, channel)
      MIDI.sendNoteOff(HIHAT[5], 0, 10);
    }
    //2.close
    else if (hihatPedal.closeHH == true)
    {
      MIDI.sendNoteOn(HIHAT[6], hihat.velocity, 10); //(note, velocity, channel)
      MIDI.sendNoteOff(HIHAT[6], 0, 10);
    }
  }

  //when pedal is closed
  if (hihatPedal.hit == true)
  {
    MIDI.sendNoteOn(HIHAT_PEDAL[6], hihatPedal.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(HIHAT_PEDAL[6], 0, 10);
  }

  //sending state of pedal with controll change
  if (hihatPedal.moving == true)
  {
    MIDI.sendControlChange(4, hihatPedal.pedalCC, 10);
  }
}
