/*
  EXAMPLE - MUX Sensing

  With this sample code, you can make 16 single piezo pads with a 4067. 
  https://open-e-drums.tumblr.com/
*/

///////////////////////////////   SETTING VALUE   ///////////////////////////////////

//Determine the setting value.
//By changing the number in this array you can set sensitivity, threshold and so on.

#define Drum_map 0  //0-ADD2, 1-EZD2
 
byte Note[2][16] = {
/*   _ Hihat tip
    |     _ Kick
    |    |    _ Snare Open hit
    |    |   |    _ Snare SideStick
    |    |   |   |    _ Tom1
    |    |   |   |   |    _ Tom2
    |    |   |   |   |   |    _ Tom3
    |    |   |   |   |   |   |    _ Tom4
    |    |   |   |   |   |   |   |    _ Extra1
    |    |   |   |   |   |   |   |   |    _ Extra2
    |    |   |   |   |   |   |   |   |   |    _ Cym1  
    |    |   |   |   |   |   |   |   |   |   |    _ Cym2
    |    |   |   |   |   |   |   |   |   |   |   |    _ Cym3
    |    |   |   |   |   |   |   |   |   |   |   |   |    _ Cym4
    |    |   |   |   |   |   |   |   |   |   |   |   |   |   _ Ride tip
    |    |   |   |   |   |   |   |   |   |   |   |   |   |  |    _ Ride bell 
    |    |   |   |   |   |   |   |   |   |   |   |   |   |  |   |       BANK */ 
  { 8,  36, 38, 42, 71, 69, 67, 65, 47, 96, 77, 79, 81, 89, 60, 61}, // 0 - ADD2 MAP 
  { 20, 36, 38, 37, 47, 48, 41, 43, 45,  1, 55, 49, 57, 52, 51, 53}  // 1 - EZD MAP  
};//1    2   3   4   5   6   7   8   9  10  11  12  13  14  15  16

int PAD1[5] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    Note[Drum_map][0]  //note
};

int PAD2[5] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    Note[Drum_map][1]  //note
};

int PAD3[5] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    Note[Drum_map][2]  //note
};

int PAD4[5] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    Note[Drum_map][3]  //note
};

int PAD5[5] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    Note[Drum_map][4] //note
};

int PAD6[5] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    Note[Drum_map][5]
};

int PAD7[5] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    Note[Drum_map][6]  //note
};

int PAD8[5] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    Note[Drum_map][7]
};

int PAD9[5] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    Note[Drum_map][8]  //note
};

int PAD10[5] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    Note[Drum_map][9]
};

int PAD11[5] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    Note[Drum_map][10]
};

int PAD12[5] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    Note[Drum_map][11]
};

int PAD13[5] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    Note[Drum_map][12]
};

int PAD14[5] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    Note[Drum_map][13]
};

int PAD15[5] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    Note[Drum_map][14]
};

int PAD16[5] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan time
    30,  //mask time
    Note[Drum_map][15]
};

/////////////////////////////////////////////////////////////////////////////////////


#if defined(__AVR_ATmega32U4__)
 #include <USB-MIDI.h>//https://github.com/lathoub/Arduino-USBMIDI
 USBMIDI_CREATE_DEFAULT_INSTANCE();
#else
 #include <MIDI.h>
 MIDI_CREATE_DEFAULT_INSTANCE();
#endif
#include <hellodrum.h>
/*
Mux Breakout ----------- Arduino
S0 ------------------- 2
S1 ------------------- 3
S2 ------------------- 4
S2 ------------------- 5
Z -------------------- A0
 */
//Define MUX Pins
HelloDrumMUX_4067 mux(2, 3, 4, 5, 0); //D2, D3, D4, D5, A0

//or use two 4501s
//HelloDrumMUX_4051 mux1(2,3,4,0); //D2, D3, D4, A0
//HelloDrumMUX_4051 mux2(5,6,7,1); //D5, D6, D7, A1

//name your pad and define mux pin
HelloDrum pad1(0);
HelloDrum pad2(1);
HelloDrum pad3(2);
HelloDrum pad4(3);
HelloDrum pad5(4);
HelloDrum pad6(5);
HelloDrum pad7(6);
HelloDrum pad8(7);
HelloDrum pad9(8);
HelloDrum pad10(9);
HelloDrum pad11(10);
HelloDrum pad12(11);
HelloDrum pad13(12);
HelloDrum pad14(13);
HelloDrum pad15(14);
HelloDrum pad16(15);

void setup()
{
  //If you use Hairless MIDI, you have to comment out the next line.
  MIDI.begin(10);

  //And uncomment the next two lines.
  //MIDI.begin();
  //Serial.begin(38400);
}

void loop()
{
  mux.scan();
  //or use two 4501s
  //mux1.scan();
  //mux2.scan();

  pad1.singlePiezoMUX(PAD1[0], PAD1[1], PAD1[2], PAD1[3]);
  pad2.singlePiezoMUX(PAD2[0], PAD2[1], PAD2[2], PAD2[3]);
  pad3.singlePiezoMUX(PAD3[0], PAD3[1], PAD3[2], PAD3[3]);
  pad4.singlePiezoMUX(PAD4[0], PAD4[1], PAD4[2], PAD4[3]);
  pad5.singlePiezoMUX(PAD5[0], PAD5[1], PAD5[2], PAD5[3]);
  pad6.singlePiezoMUX(PAD6[0], PAD6[1], PAD6[2], PAD6[3]);
  pad7.singlePiezoMUX(PAD7[0], PAD7[1], PAD7[2], PAD7[3]);
  pad8.singlePiezoMUX(PAD8[0], PAD8[1], PAD8[2], PAD8[3]);
  pad9.singlePiezoMUX(PAD9[0], PAD9[1], PAD9[2], PAD9[3]);
  pad10.singlePiezoMUX(PAD10[0], PAD10[1], PAD10[2], PAD10[3]);
  pad11.singlePiezoMUX(PAD11[0], PAD11[1], PAD11[2], PAD11[3]);
  pad12.singlePiezoMUX(PAD12[0], PAD12[1], PAD12[2], PAD12[3]);
  pad13.singlePiezoMUX(PAD13[0], PAD13[1], PAD13[2], PAD13[3]);
  pad14.singlePiezoMUX(PAD14[0], PAD14[1], PAD14[2], PAD14[3]);
  pad15.singlePiezoMUX(PAD15[0], PAD15[1], PAD15[2], PAD15[3]);
  pad16.singlePiezoMUX(PAD16[0], PAD16[1], PAD16[2], PAD16[3]);

  if (pad1.hit == true)
  {
    MIDI.sendNoteOn(PAD1[4], pad1.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(PAD1[4], 0, 10);
  }

  if (pad2.hit == true)
  {
    MIDI.sendNoteOn(PAD2[4], pad2.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(PAD2[4], 0, 10);
  }

  if (pad3.hit == true)
  {
    MIDI.sendNoteOn(PAD3[4], pad3.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(PAD3[4], 0, 10);
  }

  if (pad4.hit == true)
  {
    MIDI.sendNoteOn(PAD4[4], pad4.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(PAD4[4], 0, 10);
  }

  if (pad5.hit == true)
  {
    MIDI.sendNoteOn(PAD5[4], pad5.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(PAD5[4], 0, 10);
  }

  if (pad6.hit == true)
  {
    MIDI.sendNoteOn(PAD6[4], pad6.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(PAD6[4], 0, 10);
  }

  if (pad7.hit == true)
  {
    MIDI.sendNoteOn(PAD7[4], pad7.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(PAD7[4], 0, 10);
  }

  if (pad8.hit == true)
  {
    MIDI.sendNoteOn(PAD8[4], pad8.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(PAD8[4], 0, 10);
  }

  if (pad9.hit == true)
  {
    MIDI.sendNoteOn(PAD9[4], pad9.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(PAD9[4], 0, 10);
  }

  if (pad10.hit == true)
  {
    MIDI.sendNoteOn(PAD10[4], pad10.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(PAD10[4], 0, 10);
  }

  if (pad11.hit == true)
  {
    MIDI.sendNoteOn(PAD11[4], pad11.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(PAD11[4], 0, 10);
  }

  if (pad12.hit == true)
  {
    MIDI.sendNoteOn(PAD12[4], pad12.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(PAD12[4], 0, 10);
  }

  if (pad13.hit == true)
  {
    MIDI.sendNoteOn(PAD13[4], pad13.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(PAD13[4], 0, 10);
  }

  if (pad14.hit == true)
  {
    MIDI.sendNoteOn(PAD14[4], pad14.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(PAD14[4], 0, 10);
  }

  if (pad15.hit == true)
  {
    MIDI.sendNoteOn(PAD15[4], pad15.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(PAD15[4], 0, 10);
  }

  if (pad16.hit == true)
  {
    MIDI.sendNoteOn(PAD16[4], pad16.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(PAD16[4], 0, 10);
  }
}
