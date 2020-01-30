/*
  " HELLO DRUM LIBRARY" Ver.0.7.3
  
  by Ryo Kosaka

  GitHUb : https://github.com/RyoKosaka/HelloDrum-arduino-Library
  Blog : https://open-e-drums.tumblr.com/
*/

#ifndef HelloDrum_h
#define HelloDrum_h

#include "Arduino.h"

#ifdef ESP32
#include "EEPROM_ESP.h"
#else
#include "EEPROM.h"
#endif

static char *item[] = {
    "SENSITIVITY",
    "THRESHOLD",
    "SCAN TIME",
    "MASK TIME",
    "NOTE",
    "NOTE RIM",
    "NOTE CUP",
};

static char *itemHH[] = {
    "SENSITIVITY",
    "THRESHOLD",
    "SCAN TIME",
    "MASK TIME",
    "NOTE OPEN",
    "NOTE CLOSE",
    "NOTE CUP",
};

static char *itemHHC[] = {
    "SENSITIVITY",
    "THRESHOLD1",
    "THRESHOLD2",
    "SCAN TIME",
    "NOTE PEDAL",
    "NOTE OPEN E",
    "NOTE CLOSE E",
};

static char *showInstrument[] = {
    "Pad 1",
    "Pad 2",
    "Pad 3",
    "Pad 4",
    "Pad 5",
    "Pad 6",
    "Pad 7",
    "Pad 8",
    "Pad 9",
    "Pad 10",
    "Pad 11",
    "Pad 12",
    "Pad 13",
    "Pad 14",
    "Pad 15",
    "Pad 16",
};

static bool push;
static bool showLCD;
static bool showFlag;
static byte showVelocity;
static byte nameIndex;
static byte nameIndexMax;
static byte showValue = 0;
static byte padIndex = 0;
static byte muxIndex = 0;
static byte HHCnum = 255;
static byte HHnum = 255;
static bool edit;
static bool editCheck;
static bool editdone;
static bool change;
static byte itemNumber;
static bool buttonState;
static bool buttonState_set;
static bool button_set;
static bool button_up;
static bool button_down;
static bool button_next;
static bool button_back;
static byte UP[7] = {1, 1, 1, 1, 1, 1, 1}; //{sensitivity, threshold1, scantime, masktime, note}

#ifdef __AVR_ATmega328P__
static int rawValue[16];                  //2 * 8chanel Mux 
#else
static int rawValue[64];                  //8 * 8chanel Mux
#endif

//static byte UP_ADVANCE[4] = {1, 50, 1, 1};  //{scantime, rim / head, pedal velocity ,masktime}

class HelloDrum
{
public:
  HelloDrum(byte pin1, byte pin2);
  HelloDrum(byte pin1);

  void singlePiezoMUX(byte sens, byte thre1, byte scan, byte mask);
  void singlePiezoMUX();
  void dualPiezoMUX(byte sens, byte thre1, byte scan, byte mask);
  void dualPiezoMUX();
  void HHMUX(byte sens, byte thre1, byte scan, byte mask);
  void HHMUX();
  void HH2zoneMUX(byte sens, byte thre1, byte scan, byte mask);
  void HH2zoneMUX();
  void cymbal3zoneMUX(byte sens, byte thre1, byte scan, byte mask);
  void cymbal3zoneMUX();
  void cymbal2zoneMUX(byte sens, byte thre1, byte scan, byte mask);
  void cymbal2zoneMUX();
  void TCRT5000MUX(byte sens, byte thre1, byte thre2, byte scan);
  void TCRT5000MUX();
  void FSRMUX(byte sens, byte thre1, byte thre2, byte scan);
  void FSRMUX();

  void singlePiezo(byte sens, byte thre1, byte scan, byte mask);
  void singlePiezo();
  void dualPiezo(byte sens, byte thre1, byte scan, byte mask);
  void dualPiezo();
  void HH(byte sens, byte thre1, byte scan, byte mask);
  void HH();
  void HH2zone(byte sens, byte thre1, byte scan, byte mask);
  void HH2zone();
  void cymbal3zone(byte sens, byte thre1, byte scan, byte mask);
  void cymbal3zone();
  void cymbal2zone(byte sens, byte thre1, byte scan, byte mask);
  void cymbal2zone();
  void TCRT5000(byte sens, byte thre1, byte thre2, byte scan);
  void TCRT5000();
  void FSR(byte sens, byte thre1, byte thre2, byte scan);
  void FSR();

  void settingName(char *instrumentName);
  void settingEnable();

  void loadMemory();
  void initMemory();

  int velocity;
  int velocityRim;
  int velocityCup;
  byte pedalCC;

//  int exValue;
  byte exTCRT = 0;
  byte exFSR = 0;
  bool hit;
  bool openHH = false;
  bool closeHH = false;
  bool hitRim;
  bool hitCup;
  bool choke;
  bool sensorFlag;
  bool moving;
  bool pedalVelocityFlag = false;
  bool pedalFlag = false;
  bool settingHHC = false;
  bool chokeFlag;

  char *GetItem(byte i);

  byte value;
  byte padNum;

  byte note;
  byte noteRim;
  byte noteCup;
  byte noteOpen;
  byte noteClose;
  byte noteOpenEdge;
  byte noteCloseEdge;
  byte threshold1;
  byte threshold2;
  byte scantime;
  byte masktime;
  byte sensitivity;

  //int initialValue[5];

private:
  byte pin_1;
  byte pin_2;
  int piezoValue;
  int RimPiezoValue;
  int sensorValue;
  int firstSensorValue;
  int lastSensorValue;
//  int piezoValueSUM;
//  int RimPiezoValueSUM;
  int loopTimes = 0;
  bool flag;
  unsigned long time_hit;
  unsigned long time_end;
  unsigned long time_choke;
  unsigned long time_hit_pedal_1;
  unsigned long time_hit_pedal_2;
};

class HelloDrumMUX_4051
{
public:
  HelloDrumMUX_4051(byte pin1, byte pin2, byte pin3, byte pinA);
  void scan();
  byte selectPins[3];
  byte muxNum;

private:
  byte pin_1;
  byte pin_2;
  byte pin_3;
  byte pin_A;
};

class HelloDrumMUX_4067
{
public:
  HelloDrumMUX_4067(byte pin1, byte pin2, byte pin3, byte pin4, byte pinA);
  void scan();
  byte selectPins[4];
  byte muxNum;

private:
  byte pin_1;
  byte pin_2;
  byte pin_3;
  byte pin_4;
  byte pin_A;
};

class HelloDrumButton
{
public:
  HelloDrumButton(byte pin1, byte pin2, byte pin3, byte pin4, byte pin5);

  void readButtonState();

  byte GetSettingValue();
  byte GetVelocity();
  bool GetEditState();
  bool GetEditdoneState();
  bool GetPushState();
  bool GetDisplayState();
  char *GetPadName();
  char *GetSettingItem();
  char *GetHitPad();

  int UPDOWN;
  int NEXTBACK;

private:
  byte pin_1;
  byte pin_2;
  byte pin_3;
  byte pin_4;
  byte pin_5;
};

class HelloDrumButtonLcdShield
{
public:
  HelloDrumButtonLcdShield(byte pin1);

  void readButtonState();

  byte GetSettingValue();
  byte GetVelocity();
  bool GetEditState();
  bool GetEditdoneState();
  bool GetPushState();
  bool GetDisplayState();
  char *GetPadName();
  char *GetSettingItem();
  char *GetHitPad();

  int UPDOWN;
  int NEXTBACK;

private:
  int buttonValue;
  byte pin_1;
};

class HelloDrumKnob
{
public:
  HelloDrumKnob(byte pin1);
  void read();
  void readMUX();
  int knobValue;

private:
  byte pin_1;
};

/* 
class HelloDrumLCD
{
public:
  HelloDrumLCD(int pin1, int pin2, int pin3, int pin4, int pin5, int pin6);

  void show();

  LiquidCrystal lcd;

private:
  int pin_1;
  int pin_2;
  int pin_3;
  int pin_4;
  int pin_5;
  int pin_6;
};
*/

#endif
