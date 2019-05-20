//THIS CODE IS FOR DEBUG AND DEVELOP

#include <hellodrum.h>
#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

//Please name your pad and controller.
HelloDrum snare(0);
HelloDrum hihat(2);
HelloDrum hihatControl(5);
HelloDrum kick(4);

//Set the DIGITAL pin number to which the button and the LCD are connected.
HelloDrumButton button(6, 7, 8, 9, 10); //(EDIT,UP,DOWN,NEXT,BACK)
HelloDrumLCD lcd(12, 11, 5, 4, 3, 2); //(rs, en, d4, d5, d6, d7)


void setup() {

  //If you use Hairless MIDI, you have to comment out the next line.
  MIDI.begin(10);
  //And uncomment the next two lines. Please set the baud rate of Hairless to 38400.
  //MIDI.begin();
  //Serial.begin(38400);

  //Give each pad a name to be displayed on the LCD.
  //It is necessary to make the order in exactly the same order as you named the pad first.
  snare.settingName("SNARE");
  hihat.settingName("HIHAT");
  hihatControl.settingName("HIHAT PEDAL");
  kick.settingName("KICK");


  //Load settings from EEPROM.
  //It is necessary to make the order in exactly the same order as you named the pad first.
  snare.loadMemory();
  hihat.loadMemory();
  hihatControl.loadMemory();
  kick.loadMemory();
}

void loop() {

  /////////// 1. LCD & SETTING MODE /////////////

  button.readButtonState();
  lcd.show();

  snare.settingEnable();
  hihat.settingEnable();
  hihatControl.settingEnable();
  kick.settingEnable();


  ////////// 2. SENSING & SENDING MIDI////////////

  //Sensing each pad.
  snare.singlePiezo();
  hihat.HH();
  hihatControl.FSR();
  kick.singlePiezo();

  //Sending MIDI signals.
  //KICK//
  if (kick.hit == true) {
    MIDI.sendNoteOn(kick.note, kick.velocity, 10);  //(note, velocity, channel)
    MIDI.sendNoteOff(kick.note, 0, 10);
  }

  //SNARE//
  if (snare.hit == true) {
    MIDI.sendNoteOn(snare.note, snare.velocity, 10);  //(note, velocity, channel)
    MIDI.sendNoteOff(snare.note, 0, 10);
  }

  //HIHAT//
  if (hihat.hit == true) {
    //check open or close
    //1.open
    if (hihatControl.openHH == true) {
      MIDI.sendNoteOn(hihat.noteOpen, hihat.velocity, 10);  //(note of open, velocity, channel)
      MIDI.sendNoteOff(hihat.noteOpen, 0, 10);
    }
    //2.close
    else {
      MIDI.sendNoteOn(hihat.noteClose, hihat.velocity, 10);  //(note of close, velocity, channel)
      MIDI.sendNoteOff(hihat.noteClose, 0, 10);
    }
  }

  //HIHAT CONTROLLER//
  //when hihat is closed
  if (hihatControl.closeHH == true) {
    MIDI.sendNoteOn(hihatControl.note, hihatControl.velocity, 10);  //(note of pedal, velocity, channel)
    MIDI.sendNoteOff(hihatControl.note, 0, 10);
  }

  //sending state of pedal with controll change
  if (hihatControl.moving == true) {
    MIDI.sendControlChange(4, hihatControl.pedalCC, 10);
  }
}
