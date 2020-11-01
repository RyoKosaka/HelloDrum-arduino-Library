/*
  EXAMPLE - Simple Sensing (with DFRobot LCD keypad shield)

  Pads & Controller Circuit:
    Piezo for kick to A1.
    Piezo for snare to A2.
    Piezo for hihat to A3.

  LCD Circuit:https://wiki.dfrobot.com/LCD_KeyPad_Shield_For_Arduino_SKU__DFR0009
    LCD RS pin to digital pin 8
    LCD Enable pin to digital pin 9
    LCD D4 pin to digital pin 4
    LCD D5 pin to digital pin 5
    LCD D6 pin to digital pin 6
    LCD D7 pin to digital pin 7

  Buttons Circuit:
    Buttons of LCD keypad shield to analog pin A0

  *****  When the button behavior is incorrect  *****
  The value of the button has individual differences depending on the shield.
  You may need to modify the source code of this library.
  Please change the value from line 2710 to line 2747 in hellodrum.cpp.

  https://github.com/RyoKosaka/HelloDrum-arduino-Library
*/

#include <hellodrum.h>
#include <LiquidCrystal.h>

//Using MIDI Library. If you want to use USB-MIDI, comment out the next two lines.
#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

//Uncomment the next two lines for using USB-MIDI with atmega32u4 or Teensy
//#include <USB-MIDI.h>
//USBMIDI_CREATE_DEFAULT_INSTANCE();

//LCD pin define
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); //(rs, en, d4, d5, d6, d7)

//Please name your pad and controller.
HelloDrum kick(1); //the pad named "kick" is connected to A1 pin.
HelloDrum snare(2);
HelloDrum hihat(3);

//Set the ANALOG pin number to which the buttons are connected.
HelloDrumButtonLcdShield button(0); //A0

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

  //Load settings from EEPROM.
  //It is necessary to make the order in exactly the same order as you named the pad first.
  kick.loadMemory();
  snare.loadMemory();
  hihat.loadMemory();

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

  const char *padName = button.GetPadName();
  const char *item = button.GetSettingItem();
  int settingValue = button.GetSettingValue();

  button.readButtonState();

  kick.settingEnable();
  snare.settingEnable();
  hihat.settingEnable();

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
    const char *hitPad = button.GetHitPad();

    lcd.clear();
    lcd.print(hitPad);
    lcd.setCursor(0, 1);
    lcd.print(velocity);
  }

  ////////// 2. SENSING & SENDING MIDI////////////

  //Sensing each pad.
  kick.singlePiezo();
  snare.singlePiezo();
  hihat.singlePiezo();

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
    MIDI.sendNoteOn(hihat.note, hihat.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(hihat.note, 0, 10);
  }
}