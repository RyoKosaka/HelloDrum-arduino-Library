/*
  EXAMPLE - Minimal Drum Kit (kick Snare Hihat Ride)

  With this sample code, you can make minimal drum kit with MUX (74HC4051) and I2CLCD.
  NOT BLE MIDI.
  https://open-e-drums.tumblr.com/
*/

/* NOTICE

  You have to install the MIDI library and LiquidCrystal_I2C library.
  MIDI : https://playground.arduino.cc/Main/MIDILibrary
  LiquidCrystal_I2C : https://github.com/johnrickman/LiquidCrystal_I2C

*/

#include <hellodrum.h>
#include <MIDI.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

MIDI_CREATE_DEFAULT_INSTANCE();

/*
Mux Breakout ----------- Arduino
S0 ------------------- 2
S1 ------------------- 3
S2 ------------------- 4
Z -------------------- A0
 */
//Define MUX Pins
HelloDrumMUX_4051 mux(2, 3, 4, 0);

//Please name your pad and controller.
HelloDrum kick(0);
HelloDrum snare(1, 2);
HelloDrum hihat(3);
HelloDrum hihatControl(4);
HelloDrum ride(5, 6);
HelloDrum tom(7);

//Set the DIGITAL pin number to which the buttons are connected.
HelloDrumButton button(6, 7, 8, 9, 10); //(EDIT,UP,DOWN,NEXT,BACK)

void setup()
{
  lcd.init(); // initialize the lcd
  lcd.backlight();

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
  hihatControl.settingName("HIHAT PEDAL");
  ride.settingName("RIDE");
  ride.settingName("TOM");

  //Load settings from EEPROM.
  //It is necessary to make the order in exactly the same order as you named the pad first.
  kick.loadMemory();
  snare.loadMemory();
  hihat.loadMemory();
  hihatControl.loadMemory();
  ride.loadMemory();
  tom.loadMemory();

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

  char *padName = button.GetPadName();
  char *item = button.GetSettingItem();
  int settingValue = button.GetSettingValue();
  int velocity = button.GetVelocity();
  char *hitPad = button.GetHitPad();

  button.readButtonState();

  kick.settingEnable();
  snare.settingEnable();
  hihat.settingEnable();
  hihatControl.settingEnable();
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

  //show hitted pad name and velocity to LCD
  //Immediately after I2C communication, scanning of piezo is stop.
  //So if you change the display right after you hit pad, it affects the sensing.
  //I recommend leaving it as a comment.
  /*
    if (display == true)
    {
        lcd.clear();
        lcd.print(hitPad);
        lcd.setCursor(0, 1);
        lcd.print(velocity);
    }
    */

  ////////// 2. SENSING & SENDING MIDI////////////

  //Sensing each pad.
  mux.scan();

  kick.singlePiezoMUX();
  snare.dualPiezoMUX();
  hihat.HHMUX();
  hihatControl.TCRT5000MUX();
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
    if (hihatControl.openHH == true)
    {
      MIDI.sendNoteOn(hihat.noteOpen, hihat.velocity, 10); //(note of open, velocity, channel)
      MIDI.sendNoteOff(hihat.noteOpen, 0, 10);
    }
    //2.close
    else
    {
      MIDI.sendNoteOn(hihat.noteClose, hihat.velocity, 10); //(note of close, velocity, channel)
      MIDI.sendNoteOff(hihat.noteClose, 0, 10);
    }
  }

  //HIHAT CONTROLLER//
  //when hihat is closed
  if (hihatControl.closeHH == true)
  {
    MIDI.sendNoteOn(hihatControl.note, hihatControl.velocity, 10); //(note of pedal, velocity, channel)
    MIDI.sendNoteOff(hihatControl.note, 0, 10);
  }

  //sending state of pedal with controll change
  if (hihatControl.moving == true)
  {
    MIDI.sendControlChange(4, hihatControl.pedalCC, 10);
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
