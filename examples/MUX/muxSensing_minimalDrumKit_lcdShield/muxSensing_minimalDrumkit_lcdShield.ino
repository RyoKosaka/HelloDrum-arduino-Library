/*
  EXAMPLE - Minimal Drum Kit (kick Snare Hihat Ride Tom)

  With this sample code, you can make minimal drum kit with MUX (74HC4051) and LCD Shield.
  https://open-e-drums.tumblr.com/

  MUX Circuit :
    Mux Breakout ----------- Arduino
    S0 ------------------- D2
    S1 ------------------- D3
    S2 ------------------- D4
    Z -------------------- A1

  Pads & Controller Circuit:
    Piezo for kick to MUX 0 pin
    Piezo for snare head to MUX 1 pin
    Piezo for snare rim to MUX 2 pin
    Piezo for hihat to MUX 3 pin
    Sensor for hihat Pedal to MUX 4 pin
    Piezo for ride to MUX 5 pin
    Sensor for ride edge to MUX 6 pin
    Piezo for tom to MUX 7 pin

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

/*
Mux Breakout ----------- Arduino
S0 ------------------- 2
S1 ------------------- 3
S2 ------------------- 4
Z -------------------- A1
 */
//Define MUX Pins
HelloDrumMUX_4051 mux(2, 3, 4, 1);

//Please name your pad and controller.
HelloDrum kick(0);
HelloDrum snare(1, 2);
HelloDrum hihat(3);
HelloDrum hihatPedal(4);
HelloDrum ride(5, 6);
HelloDrum tom(7);

//Set the ANALOG pin number to which the buttons are connected.
HelloDrumButtonLcdShield button(0); //A0

void setup()
{
  //if you use ESP32, you have to uncomment the next line.
  //EEPROM_ESP.begin(512);

  //If you use Hairless MIDI, you have to comment out the next line.
  //MIDI.begin(10);
  //And uncomment the next two lines. Please set the baud rate of Hairless to 38400.
  MIDI.begin();
  Serial.begin(38400);

  //Give each pad a name to be displayed on the LCD.
  //It is necessary to make the order in exactly the same order as you named the pad first.
  kick.settingName("KICK");
  snare.settingName("SNARE");
  hihat.settingName("HIHAT");
  hihatPedal.settingName("HIHAT PEDAL");
  ride.settingName("RIDE");
  tom.settingName("TOM");

  //Load settings from EEPROM.
  //It is necessary to make the order in exactly the same order as you named the pad first.
  kick.loadMemory();
  snare.loadMemory();
  hihat.loadMemory();
  hihatPedal.loadMemory();
  ride.loadMemory();
  tom.loadMemory();

  //boot message
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Hello world!");
  lcd.setCursor(0, 1);
  lcd.print("Hello drum!");
}

void loop()
{

  /////////// 1. LCD and SETTING MODE /////////////

  bool buttonPush = button.GetPushState();
  bool editStart = button.GetEditState();
  bool editDone = button.GetEditdoneState();
  bool display = button.GetDisplayState();

  const char *padName = button.GetPadName();
  const char *item = button.GetSettingItem();
  int settingValue = button.GetSettingValue();
  int velocity = button.GetVelocity();
  const char *hitPad = button.GetHitPad();

  button.readButtonState();

  kick.settingEnable();
  snare.settingEnable();
  hihat.settingEnable();
  hihatPedal.settingEnable();
  ride.settingEnable();
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

  if (display == true)
  {
    int velocity = button.GetVelocity();
    char *hitPad = button.GetHitPad();

    lcd.clear();
    lcd.print(hitPad);
    lcd.setCursor(0, 1);
    lcd.print(velocity);

    //snare
    if (snare.hit == true)
    {
      if (snare.hitRim == true)
      {
        lcd.setCursor(15, 0);
        lcd.print("O");
      }
    }

    //hihat
    if (hihat.hit == true)
    {
      //open
      if (hihatPedal.openHH == true)
      {
        lcd.setCursor(15, 0);
        lcd.print("O");
      }
      //close
      else if (hihatPedal.closeHH == true)
      {
        lcd.setCursor(15, 0);
        lcd.print("C");
      }
    }

    //ride
    if (ride.hit == true)
    {
      //edge
      if (ride.hitRim == true)
      {
        lcd.setCursor(15, 0);
        lcd.print("E");
      }

      //cup
      else if (ride.hitCup == true)
      {
        lcd.setCursor(15, 0);
        lcd.print("C");
      }
    }
  }

  ////////// 2. SENSING & SENDING MIDI////////////

  //Sensing each pad.
  mux.scan();

  kick.singlePiezoMUX();
  snare.dualPiezoMUX();
  hihat.HHMUX();
  hihatPedal.hihatControlMUX();
  ride.cymbal3zoneMUX();
  tom.singlePiezoMUX();

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
  else if (snare.hitRim == true)
  {
    if (snare.velocity > 60) //Rim shot
    {
      MIDI.sendNoteOn(snare.noteRim, snare.velocity, 10); //(note, velocity, channel)
      MIDI.sendNoteOff(snare.noteRim, 0, 10);
    }
    else //Side Stick
    {
      MIDI.sendNoteOn(snare.noteCup, snare.velocity * 2, 10); //(note, velocity, channel)
      MIDI.sendNoteOff(snare.noteCup, 0, 10);
    }
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

  //TOM//
  if (tom.hit == true)
  {
    MIDI.sendNoteOn(tom.note, tom.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(tom.note, 0, 10);
  }
}
