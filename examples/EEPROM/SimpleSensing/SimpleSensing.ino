/*
  EXAMPLE - Simple Piezo Sensing

  With this sample code, you will make snare and tom using two piezo.
  https://open-e-drums.tumblr.com/
*/

/* NOTICE

  You have to install the MIDI library.
  MIDI : https://playground.arduino.cc/Main/MIDILibrary

*/

/////////////////////////////////////////////////////////////////////////////////////

#include <MIDI.h>
#include <hellodrum.h>
#include <LiquidCrystal.h>
MIDI_CREATE_DEFAULT_INSTANCE();

//LCD pin define
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //(rs, en, d4, d5, d6, d7)

//Please name your piezo.
//The piezo named snare is connected to the A0 pin and the piezo named tom is connected to the A1 pin.
HelloDrum snare(0);
HelloDrum tom(1);

//Set the DIGITAL pin number to which the buttons are connected.
HelloDrumButton button(6, 7, 8, 9, 10); //(EDIT,UP,DOWN,NEXT,BACK)

void setup()
{
  //If you use Hairless MIDI, you have to comment out the next line.
  MIDI.begin(10);

  //And uncomment the next two lines.
  //MIDI.begin();
  //Serial.begin(38400);

  //Give each pad a name to be displayed on the LCD.
  //It is necessary to make the order in exactly the same order as you named the pad first.
  snare.settingName("SNARE");
  tom.settingName("TOM");

  //Load settings from EEPROM.
  //It is necessary to make the order in exactly the same order as you named the pad first.
  snare.loadMemory();
  tom.loadMemory();

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

  snare.settingEnable();
  tom.settingEnable();

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
  }

  ////////// 2. SENSING & SENDING MIDI////////////

  //Piezo sensing is done in this line. And it is returned as a velocity of 127 stages.
  //For each piezo, one line is required.
  snare.singlePiezo();
  tom.singlePiezo();

  //MIDI signals are transmitted with this IF statement.
  //For each piezo, one IF statement is required
  if (snare.hit == true)
  {
    MIDI.sendNoteOn(snare.note, snare.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(snare.note, 0, 10);
  }

  if (tom.hit == true)
  {
    MIDI.sendNoteOn(tom.note, tom.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(tom.note, 0, 10);
  }
}