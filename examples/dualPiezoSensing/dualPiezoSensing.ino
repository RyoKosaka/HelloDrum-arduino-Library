/* NOTICE

  Under development.
  You can't use this code.

*/

/*
  EXAMPLE - Dual Piezo Sensing

  With this sample code, you can make 2 zone snare drum.

  Pads & Controller Circuit:
    Piezo for head to A0.
    Piezo for rim to A1.

  Buttons Circuit:
    Button for EDIT to digital pin 6
    Button for UP to digital pin 7
    Button for DOWN to digital pin 8
    Button for NEXT to digital pin 9
    Button for BACK to digital pin 10

  LCD Circuit:https://www.arduino.cc/en/Tutorial/HelloWorld
   LCD RS pin to digital pin 12
   LCD Enable pin to digital pin 11
   LCD D4 pin to digital pin 5
   LCD D5 pin to digital pin 4
   LCD D6 pin to digital pin 3
   LCD D7 pin to digital pin 2


  https://open-e-drums.tumblr.com/
*/

/* NOTICE

  You have to install the MIDI library.
  MIDI : https://playground.arduino.cc/Main/MIDILibrary

*/


#include <hellodrum.h>
#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

//Please name your pad and controller.
HelloDrum snare(0, 1);


//Set the DIGITAL pin number to which the button and the LCD are connected.
HelloDrumButton button(6, 7, 8, 9, 10); //(EDIT,UP,DOWN,NEXT,BACK)
HelloDrumLCD lcd(12, 11, 5, 4, 3, 2); //(rs, en, d4, d5, d6, d7)


void setup() {

  //If you use Hairless MIDI, you have to comment out the next line.
  //MIDI.begin(10);
  //And uncomment the next two lines. Please set the baud rate of Hairless to 38400.
  MIDI.begin();
  Serial.begin(38400);

  //Give each pad a name to be displayed on the LCD.
  //It is necessary to make the order in exactly the same order as you named the pad first.
  snare.settingName("SNARE");


  //Load settings from EEPROM.
  //It is necessary to make the order in exactly the same order as you named the pad first.
  snare.loadMemory();
}

void loop() {

  /////////// 1. LCD & SETTING MODE /////////////

  button.readButtonState();
  lcd.show();

  snare.settingEnable();


  ////////// 2. SENSING & SENDING MIDI////////////

  //Sensing each pad.
  snare.dualPiezo();


  //Sending MIDI signals.
  //SNARE//
  if (snare.hit == true) {
    MIDI.sendNoteOn(snare.note, snare.velocity, 10);  //(note, velocity, channel)
    MIDI.sendNoteOff(snare.note, 0, 10);
  }

  else if (snare.hitRim == true) {
    MIDI.sendNoteOn(snare.noteRim, snare.velocity, 10);  //(note, velocity, channel)
    MIDI.sendNoteOff(snare.noteRim, 0, 10);
  }


}
