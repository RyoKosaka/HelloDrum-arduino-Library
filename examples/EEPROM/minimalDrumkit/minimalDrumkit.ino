/*
  EXAMPLE - Minimal Drum Kit (kick Snare Hihat Ride)

  With this sample code, you can make minimal drum kit.

  Pads & Controller Circuit:
    Piezo for kick to A0.
    Piezo for snare to A1.
    Piezo for hihat to A2.
    TCRT5000 for hihat controller to A3
    Yamaha PCY135/155 to A4,A5

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
  Arduino MIDI Library : https://playground.arduino.cc/Main/MIDILibrary

*/

#include <hellodrum.h>
#include <MIDI.h>
#include <LiquidCrystal.h>

MIDI_CREATE_DEFAULT_INSTANCE();

//LCD pin define
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //(rs, en, d4, d5, d6, d7)

//Please name your pad and controller.
HelloDrum kick(0);
HelloDrum snare(1);
HelloDrum hihat(2);
HelloDrum hihatPedal(3);
HelloDrum ride(4, 5);

//Set the DIGITAL pin number to which the buttons are connected.
HelloDrumButton button(6, 7, 8, 9, 10); //(EDIT,UP,DOWN,NEXT,BACK)

void setup()
{
  //if you use ESP32, you have to uncomment the next line.
  //EEPROM_ESP.begin(512);

  //If you use Hairless MIDI, you have to comment out the next line.
  MIDI.begin(10);
  //And uncomment the next two lines. Please set the baud rate of Hairless to 38400.
  //MIDI.begin();
  //Serial.begin(38400);

  //Give each pad a name to be displayed on the LCD.
  //It is necessary to make the order in exactly the same order as you named the pad first.
  kick.settingName("KICK");
  snare.settingName("SNARE");
  hihat.settingName("HIHAT");
  hihatPedal.settingName("HIHAT PEDAL");
  ride.settingName("RIDE");

  //Load settings from EEPROM.
  //It is necessary to make the order in exactly the same order as you named the pad first.
  kick.loadMemory();
  snare.loadMemory();
  hihat.loadMemory();
  hihatPedal.loadMemory();
  ride.loadMemory();

  //boot message
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("hello, world!");
  lcd.setCursor(0, 1);
  lcd.print("hello, drum!");
}

void loop()
{

  /////////// 1. LCD & SETTING MODE /////////////

  bool buttonPush = button.GetPushState();
  bool editStart = button.GetEditState();
  bool editDone = button.GetEditdoneState();
  bool display = button.GetDisplayState();

  char *padName = button.GetPadName();
  char *item = button.GetSettingItem();
  int settingValue = button.GetSettingValue();

  button.readButtonState();

  kick.settingEnable();
  snare.settingEnable();
  hihat.settingEnable();
  hihatPedal.settingEnable();
  ride.settingEnable();

  if (buttonPush == true)
  {
    lcd.clear();
    lcd.print(padName);
    lcd.setCursor(0, 1);
    lcd.print(item);
    lcd.setCursor(13, 1);
    lcd.print(settingValue);
  }

  if (editStart == true)
  {
    lcd.clear();
    lcd.print("EDIT START");
    delay(500);
    lcd.clear();
    lcd.print(padName);
    lcd.setCursor(0, 1);
    lcd.print(item);
    lcd.setCursor(13, 1);
    lcd.print(settingValue);
  }

  if (editDone == true)
  {
    lcd.clear();
    lcd.print("EDIT DONE");
    delay(500);
    lcd.clear();
    lcd.print(padName);
    lcd.setCursor(0, 1);
    lcd.print(item);
    lcd.setCursor(13, 1);
    lcd.print(settingValue);
  }

  //show hitted pad name and velocity to LCD
  if (display == true)
  {
    int velocity = button.GetVelocity();
    char *hitPad = button.GetHitPad();

    lcd.clear();
    lcd.print(hitPad);
    lcd.setCursor(0, 1);
    lcd.print(velocity);

    if (hihat.hit == true)
    {
      //check open or close
      //1.open
      if (hihatPedal.openHH == true)
      {
        lcd.setCursor(15, 0);
        lcd.print("O");
      }
      //2.close
      else if (hihatPedal.closeHH == true)
      {
        lcd.setCursor(15, 0);
        lcd.print("C");
      }
    }
  }

  ////////// 2. SENSING & SENDING MIDI////////////

  //Sensing each pad.
  kick.singlePiezo();
  snare.singlePiezo();
  hihat.HH();
  hihatPedal.hihatControl();
  ride.cymbal3zone();

  //Sending MIDI signals.
  //KICK//
  if (kick.hit == true)
  {
    MIDI.sendNoteOn(kick.note, kick.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(kick.note, 0, 10);
  }

  //SNARE//
  if (snare.hit == true)
  {
    MIDI.sendNoteOn(snare.note, snare.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(snare.note, 0, 10);
  }

  //HIHAT//
  if (hihat.hit == true)
  {
    //check open or close
    //1.open
    if (hihatPedal.openHH == true)
    {
      MIDI.sendNoteOn(hihat.noteOpen, hihat.velocity, 10); //(note of open, velocity, channel)
      MIDI.sendNoteOff(hihat.noteOpen, 0, 10);
    }
    //2.close
    else if (hihatPedal.closeHH == true)
    {
      MIDI.sendNoteOn(hihat.noteClose, hihat.velocity, 10); //(note of close, velocity, channel)
      MIDI.sendNoteOff(hihat.noteClose, 0, 10);
    }
  }

  //HIHAT CONTROLLER//
  //when hihat is closed
  if (hihatPedal.hit == true)
  {
    MIDI.sendNoteOn(hihatPedal.note, hihatPedal.velocity, 10); //(note of pedal, velocity, channel)
    MIDI.sendNoteOff(hihatPedal.note, 0, 10);
  }

  //sending state of pedal with controll change
  if (hihatPedal.moving == true)
  {
    MIDI.sendControlChange(4, hihatPedal.pedalCC, 10);
  }

  //RIDE//
  //1.bow
  if (ride.hit == true)
  {
    MIDI.sendNoteOn(ride.note, ride.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(ride.note, 0, 10);
  }

  //2.edge
  else if (ride.hitRim == true)
  {
    MIDI.sendNoteOn(ride.noteRim, ride.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(ride.noteRim, 0, 10);
  }

  //3.cup
  else if (ride.hitCup == true)
  {
    MIDI.sendNoteOn(ride.noteCup, ride.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(ride.noteCup, 0, 10);
  }

  //4.choke
  if (ride.choke == true)
  {
    MIDI.sendPolyPressure(ride.note, 127, 10);
    MIDI.sendPolyPressure(ride.noteRim, 127, 10);
    MIDI.sendPolyPressure(ride.noteCup, 127, 10);
    MIDI.sendPolyPressure(ride.note, 0, 10);
    MIDI.sendPolyPressure(ride.noteRim, 0, 10);
    MIDI.sendPolyPressure(ride.noteCup, 0, 10);
  }
}