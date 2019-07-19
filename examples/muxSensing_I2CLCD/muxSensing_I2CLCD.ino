/*
  EXAMPLE - MUX Sensing with I2CLCD and EEPROM

  With this sample code, you can make the octapad with MUX(74HC4051).
  And you can set the value of sensitivity etc.. with LCD I2C.
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
Mux Breakout ----------- ESP32
S0 ------------------- 4
S1 ------------------- 16
S2 ------------------- 17
Z -------------------- 33
 */
//Define MUX Pins
HelloDrumMUX mux(4, 16, 2, 36);

//Please name your piezo.
HelloDrum pad1(0); //mux pin
HelloDrum pad2(1); //mux pin
HelloDrum pad3(2); //mux pin
HelloDrum pad4(3); //mux pin
HelloDrum pad5(4); //mux pin
HelloDrum pad6(5); //mux pin
HelloDrum pad7(6); //mux pin
HelloDrum pad8(7); //mux pin

//Define control buttons
HelloDrumButton button(12, 14, 27, 26, 25); //(EDIT,UP,DOWN,NEXT,BACK)

void setup()
{
    lcd.init(); // initialize the lcd
    lcd.backlight();

    //if you use ESP32, you have to uncomment the next line.
    //EEPROM_ESP.begin(512);

    //If you use Hairless MIDI, you have to comment out the next line.
    //MIDI.begin(10);

    //And uncomment the next two lines.
    MIDI.begin();
    Serial.begin(38400);

    pad1.settingName("PAD 1");
    pad2.settingName("PAD 2");
    pad3.settingName("PAD 3");
    pad4.settingName("PAD 4");
    pad5.settingName("PAD 5");
    pad6.settingName("PAD 6");
    pad7.settingName("PAD 7");
    pad8.settingName("PAD 8");

    pad1.loadMemory();
    pad2.loadMemory();
    pad3.loadMemory();
    pad4.loadMemory();
    pad5.loadMemory();
    pad6.loadMemory();
    pad7.loadMemory();
    pad8.loadMemory();

    lcd.clear();
    lcd.print("Hello world!");
    lcd.setCursor(0, 1);
    lcd.print("Hello drum!");
}

void loop()
{
    bool buttonPush = button.GetPushState();
    bool editStart = button.GetEditState();
    bool editDone = button.GetEditdoneState();
    char *padName = button.GetPadName();
    char *item = button.GetSettingItem();
    int settingValue = button.GetSettingValue();

    button.readButtonState();

    pad1.settingEnable();
    pad2.settingEnable();
    pad3.settingEnable();
    pad4.settingEnable();
    pad5.settingEnable();
    pad6.settingEnable();
    pad7.settingEnable();
    pad8.settingEnable();

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

    //scanning 8 piezos.
    mux.scan();

    //Piezo sensing is done in this line. And it is returned as a velocity of 127 stages.
    //For each piezo, one line is required.
    pad1.singlePiezoMUX();
    pad2.singlePiezoMUX();
    pad3.singlePiezoMUX();
    pad4.singlePiezoMUX();
    pad5.singlePiezoMUX();
    pad6.singlePiezoMUX();
    pad7.singlePiezoMUX();
    pad8.singlePiezoMUX();

    if (pad1.hit == true)
    {
        MIDI.sendNoteOn(pad1.note, pad1.velocity, 10); //(note, velocity, channel)
        MIDI.sendNoteOff(pad1.note, 0, 10);

        int velocity = button.GetVelocity();
        char *hitPad = button.GetHitPad();

        lcd.clear();
        lcd.print(hitPad);
        lcd.setCursor(0, 1);
        lcd.print(velocity);
    }

    if (pad2.hit == true)
    {
        MIDI.sendNoteOn(pad2.note, pad2.velocity, 10); //(note, velocity, channel)
        MIDI.sendNoteOff(pad2.note, 0, 10);

        int velocity = button.GetVelocity();
        char *hitPad = button.GetHitPad();

        lcd.clear();
        lcd.print(hitPad);
        lcd.setCursor(0, 1);
        lcd.print(velocity);
    }

    if (pad3.hit == true)
    {
        MIDI.sendNoteOn(pad3.note, pad3.velocity, 10); //(note, velocity, channel)
        MIDI.sendNoteOff(pad3.note, 0, 10);

        int velocity = button.GetVelocity();
        char *hitPad = button.GetHitPad();

        lcd.clear();
        lcd.print(hitPad);
        lcd.setCursor(0, 1);
        lcd.print(velocity);
    }

    if (pad4.hit == true)
    {
        MIDI.sendNoteOn(pad4.note, pad4.velocity, 10); //(note, velocity, channel)
        MIDI.sendNoteOff(pad4.note, 0, 10);

        int velocity = button.GetVelocity();
        char *hitPad = button.GetHitPad();

        lcd.clear();
        lcd.print(hitPad);
        lcd.setCursor(0, 1);
        lcd.print(velocity);
    }

    if (pad5.hit == true)
    {
        MIDI.sendNoteOn(pad5.note, pad5.velocity, 10); //(note, velocity, channel)
        MIDI.sendNoteOff(pad5.note, 0, 10);

        int velocity = button.GetVelocity();
        char *hitPad = button.GetHitPad();

        lcd.clear();
        lcd.print(hitPad);
        lcd.setCursor(0, 1);
        lcd.print(velocity);
    }

    if (pad6.hit == true)
    {
        MIDI.sendNoteOn(pad6.note, pad6.velocity, 10); //(note, velocity, channel)
        MIDI.sendNoteOff(pad6.note, 0, 10);

        int velocity = button.GetVelocity();
        char *hitPad = button.GetHitPad();

        lcd.clear();
        lcd.print(hitPad);
        lcd.setCursor(0, 1);
        lcd.print(velocity);
    }

    if (pad7.hit == true)
    {
        MIDI.sendNoteOn(pad7.note, pad7.velocity, 10); //(note, velocity, channel)
        MIDI.sendNoteOff(pad7.note, 0, 10);

        int velocity = button.GetVelocity();
        char *hitPad = button.GetHitPad();

        lcd.clear();
        lcd.print(hitPad);
        lcd.setCursor(0, 1);
        lcd.print(velocity);
    }

    if (pad8.hit == true)
    {
        MIDI.sendNoteOn(pad8.note, pad8.velocity, 10); //(note, velocity, channel)
        MIDI.sendNoteOff(pad8.note, 0, 10);

        int velocity = button.GetVelocity();
        char *hitPad = button.GetHitPad();

        lcd.clear();
        lcd.print(hitPad);
        lcd.setCursor(0, 1);
        lcd.print(velocity);
    }
}