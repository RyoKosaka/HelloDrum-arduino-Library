/*
  " HELLO DRUM LIBRARY" Ver.0.7.5
  
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
    "SENSITIVITY", //0 0
    "THRESHOLD",   //1 1
    "SCAN TIME",   //2 2
    "MASK TIME",   //3 3
    "CURVE TYPE",  //6 4
    "NOTE",        //7 5
};

static char *itemD[] = {
    "SENSITIVITY", //0
    "THRESHOLD",   //1
    "SCAN TIME",   //2
    "MASK TIME",   //3
    "RIM SENS",    //4
    "RIM THRE",    //5
    "CURVE TYPE",  //6
    "NOTE HEAD",   //7
    "NOTE RIM",    //8
    "NOTE CROSS",  //9
};

static char *itemCY2[] = {
    "SENSITIVITY", //0 0
    "THRESHOLD",   //1 1
    "SCAN TIME",   //2 2
    "MASK TIME",   //3 3
    "EDGE THRE",   //4 4
    "CURVE TYPE",  //6 5
    "NOTE BOW",    //7 6
    "NOTE EDGE",   //8 7
};

static char *itemCY3[] = {
    "SENSITIVITY", //0
    "THRESHOLD",   //1
    "SCAN TIME",   //2
    "MASK TIME",   //3
    "EDGE THRE",   //4
    "CUP THRE",    //5
    "CURVE TYPE",  //6
    "NOTE BOW",    //7
    "NOTE EDGE",   //8
    "NOTE CUP",    //9
};

static char *itemHH[] = {
    "SENSITIVITY", //0 0
    "THRESHOLD",   //1 1
    "SCAN TIME",   //2 2
    "MASK TIME",   //3 3
    "CURVE TYPE",  //6 4
    "NOTE OPEN",   //7 5
    "NOTE CLOSE",  //8 6
};

static char *itemHH2[] = {
    "SENSITIVITY", //0 0
    "THRESHOLD",   //1 1
    "SCAN TIME",   //2 2
    "MASK TIME",   //3 3
    "EDGE THRE",   //4 4
    "CURVE TYPE",  //6 5
    "NOTE OPEN",   //7 6
    "NOTE CLOSE",  //8 7
};

static char *itemHHC[] = {
    "SENSITIVITY",  //0 0
    "THRESHOLD",    //1 1
    "SCAN START",   //2 2
    "SCAN END",     //3 3
    "PEDAL SENS",   //4 4
    "CURVE TYPE",   //6 5
    "NOTE PEDAL",   //7 6
    "NOTE OPEN E",  //8 7
    "NOTE CLOSE E", //9 8
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
static byte HH2num = 255;
static byte CY2num = 255;
static byte CY3num = 255;
static byte Dnum = 255;
static byte Snum = 255;

static bool edit;
static bool editCheck;
static bool editdone;
static bool change;
static byte itemNumber;
static byte itemNumberShow;
static bool buttonState;
static bool buttonState_set;
static bool button_set;
static bool button_up;
static bool button_down;
static bool button_next;
static bool button_back;
static byte UP[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

#ifdef __AVR_ATmega328P__
static int rawValue[16]; //2 * 8chanel Mux
#else
static int rawValue[64]; //8 * 8chanel Mux
#endif

class HelloDrum
{
public:
  HelloDrum(byte pin1, byte pin2);
  HelloDrum(byte pin1);

  void singlePiezoMUX(byte sens, byte thre, byte scan, byte mask);
  void singlePiezoMUX();
  void dualPiezoMUX(byte sens, byte thre, byte scan, byte mask, byte rimSens, byte rimThre);
  void dualPiezoMUX();
  void HHMUX(byte sens, byte thre, byte scan, byte mask);
  void HHMUX();
  void HH2zoneMUX(byte sens, byte thre, byte scan, byte mask, byte edgeThre);
  void HH2zoneMUX();
  void cymbal3zoneMUX(byte sens, byte thre, byte scan, byte mask, byte edgeThre, byte cupThre);
  void cymbal3zoneMUX();
  void cymbal2zoneMUX(byte sens, byte thre, byte scan, byte mask, byte edgeThre);
  void cymbal2zoneMUX();
  void TCRT5000MUX(byte sens, byte thre1, byte thre2, byte scan);
  void TCRT5000MUX();
  void FSRMUX(byte sens, byte thre, byte scanStart, byte scanEnd, byte pedalSens);
  void FSRMUX();
  void hihatControlMUX(byte sens, byte thre, byte scanStart, byte scanEnd, byte pedalSens);
  void hihatControlMUX();

  void singlePiezo(byte sens, byte thre, byte scan, byte mask);
  void singlePiezo();
  void dualPiezo(byte sens, byte thre, byte scan, byte mask, byte rimSens, byte rimThre);
  void dualPiezo();
  void HH(byte sens, byte thre, byte scan, byte mask);
  void HH();
  void HH2zone(byte sens, byte thre, byte scan, byte mask, byte edgeThre);
  void HH2zone();
  void cymbal3zone(byte sens, byte thre, byte scan, byte mask, byte edgeThre, byte cupThre);
  void cymbal3zone();
  void cymbal2zone(byte sens, byte thre, byte scan, byte mask, byte edgeThre);
  void cymbal2zone();
  void TCRT5000(byte sens, byte thre1, byte thre2, byte scan);
  void TCRT5000();
  void FSR(byte sens, byte thre, byte scanStart, byte scanEnd, byte pedalSens);
  void FSR();
  void hihatControl(byte sens, byte thre, byte scanStart, byte scanEnd, byte pedalSens);
  void hihatControl();

  void setCurve(byte curveType);

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
  bool pedalFlag = true;
  bool settingHHC = false;
  bool chokeFlag;

  char *GetItem(byte i);

  byte value;
  byte padNum;

  byte note;
  byte noteRim;
  byte noteCup;
  byte noteEdge;
  byte noteOpen;
  byte noteClose;
  byte noteOpenEdge;
  byte noteCloseEdge;
  byte noteCross;
  byte threshold1;
  byte threshold2;
  byte scantime;
  byte masktime;
  byte sensitivity;
  byte curvetype;
  byte rimThreshold;
  byte rimSensitivity;

private:
  byte pin_1;
  byte pin_2;
  int piezoValue;
  int RimPiezoValue;
  int sensorValue;
  int TCRT;
  int fsr;
  int firstSensorValue;
  int lastSensorValue;
  int loopTimes = 0;
  unsigned long time_hit;
  unsigned long time_end;
  unsigned long time_choke;
  unsigned long time_hit_pedal_1;
  unsigned long time_hit_pedal_2;

  void singlePiezoSensing(byte sens, byte thre, byte scanTime, byte maskTime);
  void dualPiezoSensing(byte sens, byte thre, byte scanTime, byte maskTime, byte rimSens, byte rimThre);
  void cymbal2zoneSensing(byte sens, byte thre, byte scanTime, byte maskTime, byte edgeThre);
  void cymbal3zoneSensing(byte sens, byte thre, byte scanTime, byte maskTime, byte edgeThre, byte cupThre);
  void TCRT5000Sensing(byte sens, byte thre1, byte thre2, byte scanTime);
  void FSRSensing(byte sens, byte thre, byte scanStart, byte scanEnd, byte pedalSens);
  int curve(int velocityRaw, int threshold, int sensRaw, byte curveType);
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
  void readButton(bool button_set, bool button_up, bool button_down, bool button_next, bool button_back);

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
  void readButton(bool button_set, bool button_up, bool button_down, bool button_next, bool button_back);

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

#endif
