/*
  " HELLO DRUM LIBRARY" Ver.0.7.2
  
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
static int showVelocity;
static int nameIndex;
static int nameIndexMax;
static int showValue = 0;
static int padIndex = 0;
static int muxIndex = -1;
static int HHCnum = -1;
static int HHnum = -1;
static bool edit;
static bool editCheck;
static bool editdone;
static bool change;
static int itemNumber;
static bool buttonState;
static bool buttonState_set;
static int button_set;
static int button_up;
static int button_down;
static int button_next;
static int button_back;
static int UP[7] = {1, 1, 1, 1, 1, 1, 1}; //{sensitivity, threshold1, scantime, masktime, note}

#ifdef __AVR_ATmega328P__
static int rawValue[16];                  //2 * 8chanel Mux 
#else
static int rawValue[64];                  //8 * 8chanel Mux
#endif

//static int UP_ADVANCE[4] = {1, 50, 1, 1};  //{scantime, rim / head, pedal velocity ,masktime}

class HelloDrum
{
public:
  HelloDrum(int pin1, int pin2);
  HelloDrum(int pin1);

  void singlePiezoMUX(int sens, int thre1, int scan, int mask);
  void singlePiezoMUX();
  void dualPiezoMUX(int sens, int thre1, int scan, int mask);
  void dualPiezoMUX();
  void HHMUX(int sens, int thre1, int scan, int mask);
  void HHMUX();
  void HH2zoneMUX(int sens, int thre1, int scan, int mask);
  void HH2zoneMUX();
  void cymbal3zoneMUX(int sens, int thre1, int scan, int mask);
  void cymbal3zoneMUX();
  void cymbal2zoneMUX(int sens, int thre1, int scan, int mask);
  void cymbal2zoneMUX();
  void TCRT5000MUX(int sens, int thre1, int thre2, int scan);
  void TCRT5000MUX();
  void FSRMUX(int sens, int thre1, int thre2, int scan);
  void FSRMUX();

  void singlePiezo(int sens, int thre1, int scan, int mask);
  void singlePiezo();
  void dualPiezo(int sens, int thre1, int scan, int mask);
  void dualPiezo();
  void HH(int sens, int thre1, int scan, int mask);
  void HH();
  void HH2zone(int sens, int thre1, int scan, int mask);
  void HH2zone();
  void cymbal3zone(int sens, int thre1, int scan, int mask);
  void cymbal3zone();
  void cymbal2zone(int sens, int thre1, int scan, int mask);
  void cymbal2zone();
  void TCRT5000(int sens, int thre1, int thre2, int scan);
  void TCRT5000();
  void FSR(int sens, int thre1, int thre2, int scan);
  void FSR();

  void settingName(char *instrumentName);
  void settingEnable();

  void loadMemory();
  void initMemory();

  int velocity;
  int velocityRim;
  int velocityCup;
  int pedalCC;

  int exValue;
  int exTCRT = 0;
  int exFSR = 0;
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

  char *GetItem(int i);

  int value;
  int padNum;

  int note;
  int noteRim;
  int noteCup;
  int noteOpen;
  int noteClose;
  int noteOpenEdge;
  int noteCloseEdge;
  int threshold1;
  int threshold2;
  int scantime;
  int masktime;
  int sensitivity;

  //int initialValue[5];

private:
  int pin_1;
  int pin_2;
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
  HelloDrumMUX_4051(int pin1, int pin2, int pin3, int pinA);
  void scan();
  int selectPins[3];
  int muxNum;

private:
  int pin_1;
  int pin_2;
  int pin_3;
  int pin_A;
};

class HelloDrumMUX_4067
{
public:
  HelloDrumMUX_4067(int pin1, int pin2, int pin3, int pin4, int pinA);
  void scan();
  int selectPins[4];
  int muxNum;

private:
  int pin_1;
  int pin_2;
  int pin_3;
  int pin_4;
  int pin_A;
};

class HelloDrumButton
{
public:
  HelloDrumButton(int pin1, int pin2, int pin3, int pin4, int pin5);

  void readButtonState();

  int GetSettingValue();
  int GetVelocity();
  bool GetEditState();
  int GetEditdoneState();
  bool GetPushState();
  bool GetDisplayState();
  char *GetPadName();
  char *GetSettingItem();
  char *GetHitPad();

  int UPDOWN;
  int NEXTBACK;

private:
  int pin_1;
  int pin_2;
  int pin_3;
  int pin_4;
  int pin_5;
};

class HelloDrumKnob
{
public:
  HelloDrumKnob(int pin1);
  void read();
  void readMUX();
  int knobValue;

private:
  int pin_1;
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