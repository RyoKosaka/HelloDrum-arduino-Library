/*
  EXAMPLE - MUX Sensing with OLED(SSD1306) and EEPROM

  With this sample code, you can make the octapad with MUX(74HC4051).
  And you can set the value of sensitivity etc.. with OLED (SSD1306).
  NOT BLE MIDI.
  https://open-e-drums.tumblr.com/
*/

/* NOTICE

  1. You have to install the MIDI library and u8g2 library.
  MIDI : https://playground.arduino.cc/Main/MIDILibrary
  u8g2 : https://github.com/olikraus/u8g2

  2. You CAN NOT use this code with Arduino UNO because of u8g2.
     You have to use arudino MEGA or ESP32.

*/

#include <hellodrum.h>
#include <MIDI.h>
#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

MIDI_CREATE_DEFAULT_INSTANCE();

U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);

/*
Mux Breakout ----------- ESP32
S0 ------------------- 4
S1 ------------------- 16
S2 ------------------- 17
Z -------------------- 33
 */
//Define MUX Pins
HelloDrumMUX_4051 mux(4, 16, 17, 33);
//HelloDrumMUX_4067 mux(2, 4, 16, 17, 36);

//Please name your piezo.
//The piezo named snare is connected to the A0 pin and the piezo named tom is connected to the A1 pin.
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
    //if you use ESP32, you have to uncomment the next line.
    //EEPROM_ESP.begin(512);

    //If you use Hairless MIDI, you have to comment out the next line.
    //MIDI.begin(10);

    //And uncomment the next two lines.
    MIDI.begin();
    Serial.begin(38400);

    u8g2.begin();
    u8g2.clearBuffer();                  // clear the internal memory
    u8g2.setFont(u8g2_font_ncenB08_tr);  // choose a suitable font
    u8g2.drawStr(0, 10, "Hello World!"); // write something to the internal memory
    u8g2.sendBuffer();                   // transfer internal memory to the display

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
}

void loop()
{
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
        u8g2.clearBuffer();
        u8g2.drawStr(0, 10, padName);
        u8g2.drawStr(0, 20, item);
        u8g2.setCursor(0, 30);
        u8g2.print(settingValue);
        u8g2.sendBuffer();
    }

    if (editStart == true)
    {
        u8g2.clearBuffer();
        u8g2.setCursor(0, 10);
        u8g2.print("EDIT START");
        u8g2.sendBuffer();
        delay(500);
        u8g2.clearBuffer();
        u8g2.drawStr(0, 10, padName);
        u8g2.drawStr(0, 20, item);
        u8g2.setCursor(0, 30);
        u8g2.print(settingValue);
        u8g2.sendBuffer();
    }

    if (editDone == true)
    {
        u8g2.clearBuffer();
        u8g2.setCursor(0, 10);
        u8g2.print("EDIT DONE");
        u8g2.sendBuffer();
        delay(500);
        u8g2.clearBuffer();
        u8g2.drawStr(0, 10, padName);
        u8g2.drawStr(0, 20, item);
        u8g2.setCursor(0, 30);
        u8g2.print(settingValue);
        u8g2.sendBuffer();
    }

    //show hitted pad name and velocity to LCD
    //Immediately after I2C communication, scanning of piezo is stop.
    //So if you change the display right after you hit pad, it affects the sensing.
    //I recommend leaving it as a comment.
    /*
    if (display == true)
    {
        u8g2.clearBuffer();
        u8g2.setCursor(0, 10);
        u8g2.print(hitPad);
        u8g2.setCursor(0, 30);
        u8g2.print(velocity);
        u8g2.sendBuffer();
    }
    */

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
    }

    if (pad2.hit == true)
    {
        MIDI.sendNoteOn(pad2.note, pad2.velocity, 10); //(note, velocity, channel)
        MIDI.sendNoteOff(pad2.note, 0, 10);
    }

    if (pad3.hit == true)
    {
        MIDI.sendNoteOn(pad3.note, pad3.velocity, 10); //(note, velocity, channel)
        MIDI.sendNoteOff(pad3.note, 0, 10);
    }

    if (pad4.hit == true)
    {
        MIDI.sendNoteOn(pad4.note, pad4.velocity, 10); //(note, velocity, channel)
        MIDI.sendNoteOff(pad4.note, 0, 10);
    }

    if (pad5.hit == true)
    {
        MIDI.sendNoteOn(pad5.note, pad5.velocity, 10); //(note, velocity, channel)
        MIDI.sendNoteOff(pad5.note, 0, 10);
    }

    if (pad6.hit == true)
    {
        MIDI.sendNoteOn(pad6.note, pad6.velocity, 10); //(note, velocity, channel)
        MIDI.sendNoteOff(pad6.note, 0, 10);
    }

    if (pad7.hit == true)
    {
        MIDI.sendNoteOn(pad7.note, pad7.velocity, 10); //(note, velocity, channel)
        MIDI.sendNoteOff(pad7.note, 0, 10);
    }

    if (pad8.hit == true)
    {
        MIDI.sendNoteOn(pad8.note, pad8.velocity, 10); //(note, velocity, channel)
        MIDI.sendNoteOff(pad8.note, 0, 10);
    }
}