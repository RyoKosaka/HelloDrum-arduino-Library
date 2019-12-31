/*
  " HELLO DRUM LIBRARY" Ver.0.7.2
  
  by Ryo Kosaka

  GitHUb : https://github.com/RyoKosaka/HelloDrum-arduino-Library
  Blog : https://open-e-drums.tumblr.com/
*/

#include "hellodrum.h"
#include "Arduino.h"

#ifdef ESP32
#include "EEPROM_ESP.h"
#else
#include "EEPROM.h"
#endif

//Pad with 2 sensors.
HelloDrum::HelloDrum(int pin1, int pin2)
{
  pin_1 = pin1;
  pin_2 = pin2;

  //initial value
  note = 38;
  noteRim = 39;
  noteCup = 40;
  threshold1 = 10;
  scantime = 30;
  masktime = 10;
  sensitivity = 100;

  //Give the instance a pad number.
  padNum = padIndex;
  padIndex = ++padIndex;
}

//Pad with a sensor.
HelloDrum::HelloDrum(int pin1)
{
  pin_1 = pin1;

  //initial EEPROM value
  note = 38;       //0-127
  noteRim = 39;    //0-127
  noteCup = 40;    //0-127
  threshold1 = 10; //*10 1-100
  scantime = 30;
  masktime = 10;
  sensitivity = 100; //*10 1-100

  //Give the instance a pad number.
  padNum = padIndex;
  padIndex = ++padIndex;
}

//MUX(4051) pin define
HelloDrumMUX_4051::HelloDrumMUX_4051(int pin1, int pin2, int pin3, int pinA) //s0,s1,s2, analogPin
{
  pin_1 = pin1;
  pin_2 = pin2;
  pin_3 = pin3;
  pin_A = pinA;
  selectPins[0] = pin_1;
  selectPins[1] = pin_2;
  selectPins[2] = pin_3;

  for (int i = 0; i < 3; i++)
  {
    pinMode(selectPins[i], OUTPUT);
    digitalWrite(selectPins[i], HIGH);
  }
  muxIndex = ++muxIndex;
  muxNum = muxIndex;
}

//MUX(4067) pin define
HelloDrumMUX_4067::HelloDrumMUX_4067(int pin1, int pin2, int pin3, int pin4, int pinA) //s0,s1,s2,s3,analogPin
{
  pin_1 = pin1;
  pin_2 = pin2;
  pin_3 = pin3;
  pin_4 = pin4;
  pin_A = pinA;
  selectPins[0] = pin_1;
  selectPins[1] = pin_2;
  selectPins[2] = pin_3;
  selectPins[3] = pin_4;

  for (int i = 0; i < 4; i++)
  {
    pinMode(selectPins[i], OUTPUT);
    digitalWrite(selectPins[i], HIGH);
  }
  muxIndex = ++muxIndex;
  muxNum = muxIndex;
}

//control button
HelloDrumButton::HelloDrumButton(int pin1, int pin2, int pin3, int pin4, int pin5)
{
  pin_1 = pin1; //EDIT
  pin_2 = pin2; //UP
  pin_3 = pin3; //DOWN
  pin_4 = pin4; //NEXT
  pin_5 = pin5; //BACK
}

//Knob
HelloDrumKnob::HelloDrumKnob(int pin1)
{
  pin_1 = pin1;
}

///////////////////// 1. SENSING without EEPROM //////////////////////////

void HelloDrum::singlePiezo(int sens, int thre1, int scan, int mask)
{
#ifdef ESP32
  int thre1Raw = thre1 * 40;
  int sensRaw = sens * 40;
#else
  int thre1Raw = thre1 * 10;
  int sensRaw = sens * 10;
#endif

  hit = false;
  piezoValue = analogRead(pin_1);

  //when the value > threshold
  if (piezoValue > thre1Raw && loopTimes == 0)
  {
    time_hit = millis(); //check the time pad hitted

    //compare time to cancel retrigger
    if (time_hit - time_end < mask)
    {
      flag = true;
    }
    else
    {
      velocity = piezoValue; //first peak
      loopTimes = ++loopTimes;
      flag = false;
    }
  }

  //peak scan start
  if (flag == false && loopTimes > 0)
  {
    if (loopTimes != scan)
    {
      if (piezoValue > velocity)
      {
        velocity = piezoValue;
      }
      loopTimes = ++loopTimes;
    }

    //scan end
    if (loopTimes == scan)
    {
      velocity = map(velocity, thre1Raw, sensRaw, 1, 127);

      if (velocity <= 1)
      {
        velocity = 1;
      }

      if (velocity > 127)
      {
        velocity = 127;
      }

      //velo = (velo * velo) / 126 + 1;

      hit = true;
      loopTimes = 0;
      flag = true;
    }
  }

  if (flag == true)
  {
    time_end = millis();
    flag = false;
  }
}

void HelloDrum::dualPiezo(int sens, int thre1, int scan, int mask)
{
#ifdef ESP32
  int thre1Raw = thre1 * 40;
  int sensRaw = sens * 40;
#else
  int thre1Raw = thre1 * 10;
  int sensRaw = sens * 10;
#endif

  hit = false;
  hitRim = false;
  piezoValue = analogRead(pin_1);
  RimPiezoValue = analogRead(pin_2);

  //when the value > threshold
  if ((piezoValue > thre1Raw && loopTimes == 0) || (RimPiezoValue > thre1Raw && loopTimes == 0))
  {
    time_hit = millis();

    if (time_hit - time_end < mask)
    {
      flag = true;
    }
    else
    {
      velocity = piezoValue; //first peak
      velocityRim = RimPiezoValue;
      loopTimes = ++loopTimes;
      flag = false;
    }
  }

  //peak scan start
  if (flag == false && loopTimes > 0)
  {
    if (loopTimes != scan)
    {
      piezoValueSUM = piezoValueSUM + piezoValue;
      RimPiezoValueSUM = RimPiezoValueSUM + RimPiezoValue;

      if (piezoValue > velocity)
      {
        velocity = piezoValue;
      }
      if (RimPiezoValue > velocityRim)
      {
        velocityRim = RimPiezoValue;
      }
      loopTimes = ++loopTimes;
    }

    //scan end
    if (loopTimes == scan)
    {

      bool Rimjudge;

      if (velocity >= velocityRim)
      {
        Rimjudge = false;
      }
      else if (velocity < velocityRim)
      {
        Rimjudge = true;
      }

      velocity = map(velocity, thre1Raw, sensRaw, 1, 127);
      velocityRim = map(velocityRim, thre1Raw, sensRaw, 1, 127);

      if (velocity <= 1)
      {
        velocity = 1;
      }

      if (velocity > 127)
      {
        velocity = 127;
      }

      if (velocityRim <= 1)
      {
        velocityRim = 1;
      }

      if (velocityRim > 127)
      {
        velocityRim = 127;
      }

      //if (piezoValueSUM > RimPiezoValueSUM)
      if (Rimjudge == false)
      {
        hit = true;
        loopTimes = 0;
        flag = true;
      }
      //else if (piezoValueSUM <= RimPiezoValueSUM)
      else if (Rimjudge == true)
      {
        velocity = velocityRim;
        hitRim = true;
        loopTimes = 0;
        flag = true;
      }
    }
  }

  if (flag == true)
  {
    time_end = millis();
    flag = false;
  }
}

void HelloDrum::HH(int sens, int thre1, int scan, int mask)
{

#ifdef ESP32
  int thre1Raw = thre1 * 40;
  int sensRaw = sens * 40;
#else
  int thre1Raw = thre1 * 10;
  int sensRaw = sens * 10;
#endif

  HHnum = padNum;
  hit = false;
  piezoValue = analogRead(pin_1);

  //when the value > threshold
  if (piezoValue > thre1Raw && loopTimes == 0)
  {
    time_hit = millis(); //check the time pad hitted

    //compare time to cancel retrigger
    if (time_hit - time_end < mask)
    {
      flag = true;
    }
    else
    {
      velocity = piezoValue; //first peak
      loopTimes = ++loopTimes;
      flag = false;
    }
  }

  //peak scan start
  if (flag == false && loopTimes > 0)
  {
    if (loopTimes != scan)
    {
      if (piezoValue > velocity)
      {
        velocity = piezoValue;
      }
      loopTimes = ++loopTimes;
    }

    //scan end
    if (loopTimes == scan)
    {
      velocity = map(velocity, thre1Raw, sensRaw, 1, 127);

      if (velocity <= 1)
      {
        velocity = 1;
      }

      if (velocity > 127)
      {
        velocity = 127;
      }

      //velo = (velo * velo) / 126 + 1;
      hit = true;
      loopTimes = 0;
      flag = true;
    }
  }

  if (flag == true)
  {
    time_end = millis();
    flag = false;
  }
}

////////////////////////////////////////////
void HelloDrum::HH2zone(int sens, int thre1, int scan, int mask)
{

#ifdef ESP32
  int thre1Raw = thre1 * 40;
  int sensRaw = sens * 40;
  int edgeThreshold = 200;
#else
  int thre1Raw = thre1 * 10;
  int sensRaw = sens * 10;
  int edgeThreshold = 50;
#endif

  HHnum = padNum;
  hit = false;
  hitRim = false;
  hitCup = false;
  choke = false;

  piezoValue = analogRead(pin_1);
  sensorValue = analogRead(pin_2);

  //when the value > threshold
  if ((piezoValue > thre1Raw && loopTimes == 0) || (sensorValue > edgeThreshold && loopTimes == 0))
  {
    time_hit = millis(); //check the time pad hitted

    if (time_hit - time_end < mask)
    {
      flag = true;
    }
    else
    {
      velocity = abs(piezoValue - sensorValue); //first peak //ここコメント化ありうる。
      firstSensorValue = sensorValue;
      loopTimes = ++loopTimes;
      flag = false;
    }
  }

  //peak scan start
  if (flag == false && loopTimes > 0)
  {
    if (loopTimes != scan)
    {
      if (abs(piezoValue - sensorValue) > velocity)
      {
        velocity = abs(piezoValue - sensorValue);
      }
      if (sensorValue > firstSensorValue && loopTimes < 5)
      {
        firstSensorValue = sensorValue;
      }
      loopTimes = ++loopTimes;
    }

    //scan end
    if (loopTimes == scan)
    {
      lastSensorValue = sensorValue;
      velocity = map(velocity, thre1Raw, sensRaw, 1, 127);

      if (velocity <= 1)
      {
        velocity = 1;
      }

      if (velocity > 127)
      {
        velocity = 127;
      }

      //bow
      if (firstSensorValue < edgeThreshold && lastSensorValue < edgeThreshold)
      {
        flag = true;
        hit = true;
      }

      //edge
      else if (firstSensorValue > edgeThreshold && lastSensorValue < edgeThreshold)
      {
        hitRim = true;
        flag = true;
      }

      //choke
      else if (lastSensorValue > edgeThreshold && firstSensorValue > edgeThreshold)
      {
        choke = true;
        flag = true;
      }
      loopTimes = 0;
    }
  }

  if (flag == true)
  {
    time_end = millis();
    flag = false;
  }
}

void HelloDrum::cymbal2zone(int sens, int thre1, int scan, int mask)
{

#ifdef ESP32
  int thre1Raw = thre1 * 40;
  int sensRaw = sens * 40;
  int edgeThreshold = 200;
#else
  int thre1Raw = thre1 * 10;
  int sensRaw = sens * 10;
  int edgeThreshold = 50;
#endif

  hit = false;
  hitRim = false;
  choke = false;

  piezoValue = analogRead(pin_1);
  sensorValue = analogRead(pin_2);

  //when the value > threshold
  if ((piezoValue > thre1Raw && loopTimes == 0) || (sensorValue > edgeThreshold && loopTimes == 0))
  {
    time_hit = millis(); //check the time pad hitted

    if (time_hit - time_end < mask)
    {
      flag = true;
    }
    else
    {
      velocity = abs(piezoValue - sensorValue); //first peak //ここコメント化ありうる。
      firstSensorValue = sensorValue;
      loopTimes = ++loopTimes;
      flag = false;
    }
  }

  //peak scan start
  if (flag == false && loopTimes > 0)
  {
    if (loopTimes != scan)
    {
      if (abs(piezoValue - sensorValue) > velocity)
      {
        velocity = abs(piezoValue - sensorValue);
      }
      if (sensorValue > firstSensorValue && loopTimes < 5)
      {
        firstSensorValue = sensorValue;
      }
      loopTimes = ++loopTimes;
    }

    //scan end
    if (loopTimes == scan)
    {
      lastSensorValue = sensorValue;
      velocity = map(velocity, thre1Raw, sensRaw, 1, 127);

      if (velocity <= 1)
      {
        velocity = 1;
      }

      if (velocity > 127)
      {
        velocity = 127;
      }

      //bow
      if (firstSensorValue < edgeThreshold && lastSensorValue < edgeThreshold)
      {
        flag = true;
        hit = true;
      }

      //edge
      else if (firstSensorValue > edgeThreshold && lastSensorValue < edgeThreshold)
      {
        hitRim = true;
        flag = true;
      }

      //choke
      else if (lastSensorValue > edgeThreshold && firstSensorValue > edgeThreshold)
      {
        choke = true;
        flag = true;
      }
      loopTimes = 0;
    }
  }

  if (flag == true)
  {
    time_end = millis();
    flag = false;
  }
}

void HelloDrum::cymbal3zone(int sens, int thre1, int scan, int mask)
{

#ifdef ESP32
  int thre1Raw = thre1 * 40;
  int sensRaw = sens * 40;
  int edgeThreshold = 200;
  int cupThreshold = 2000;
  int loopTimesCup = 25;
#else
  int thre1Raw = thre1 * 10;
  int sensRaw = sens * 10;
  int edgeThreshold = 50;
  int cupThreshold = 520;
  int loopTimesCup = 10; //?
#endif

  hit = false;
  hitRim = false;
  hitCup = false;
  choke = false;

  piezoValue = analogRead(pin_1);
  sensorValue = analogRead(pin_2);

  //when the value > threshold
  if ((piezoValue > thre1Raw && loopTimes == 0) || (sensorValue > edgeThreshold && loopTimes == 0))
  {
    time_hit = millis(); //check the time pad hitted

    if (time_hit - time_end < mask)
    {
      flag = true;
    }
    else
    {
      velocity = abs(piezoValue - sensorValue); //first peak //ここコメント化ありうる。
      firstSensorValue = sensorValue;
      loopTimes = ++loopTimes;
      flag = false;
    }
  }

  //peak scan start
  if (flag == false && loopTimes > 0)
  {
    if (loopTimes != scan)
    {
      if (loopTimes == loopTimesCup)
      {
        velocityCup = 1;
      }
      if (abs(piezoValue - sensorValue) > velocity)
      {
        velocity = abs(piezoValue - sensorValue);
        velocityCup = velocity;
      }
      if (sensorValue > firstSensorValue && loopTimes < 5)
      {
        firstSensorValue = sensorValue;
      }
      loopTimes = ++loopTimes;
    }

    //scan end
    if (loopTimes == scan)
    {
      lastSensorValue = sensorValue;
      velocity = map(velocity, thre1Raw, sensRaw, 1, 127);
      velocityCup = map(velocityCup, thre1Raw, sensRaw, 1, 127);

      if (velocity <= 1)
      {
        velocity = 1;
      }

      if (velocity > 127)
      {
        velocity = 127;
      }

      if (velocityCup <= 1)
      {
        velocityCup = 1;
      }

      if (velocityCup > 127)
      {
        velocityCup = 127;
      }

      //bow
      if (firstSensorValue < edgeThreshold && lastSensorValue < edgeThreshold)
      {
        flag = true;
        hit = true;
      }

      //edge
      else if (firstSensorValue > edgeThreshold && firstSensorValue < cupThreshold && lastSensorValue < edgeThreshold)
      {
        hitRim = true;
        flag = true;
      }

      //cup
      else if (firstSensorValue > cupThreshold && lastSensorValue < edgeThreshold)
      {
        velocity = velocityCup;
        hitCup = true;
        flag = true;
      }

      //choke
      else if (firstSensorValue > edgeThreshold && lastSensorValue > edgeThreshold)
      {
        choke = true;
        flag = true;
      }
      loopTimes = 0;
    }
  }

  if (flag == true)
  {
    time_end = millis();
    flag = false;
  }
}

void HelloDrum::TCRT5000(int sens, int thre1, int thre2, int scan)
{
  int TCRT = analogRead(pin_1);

#ifdef ESP32
  int thre1Raw = thre1 * 40;
  int thre2Raw = thre2 * 40;
  int sensRaw = sens * 40;
  TCRT = 4096 - TCRT;
#else
  int thre1Raw = thre1 * 10;
  int thre2Raw = thre2 * 10;
  int sensRaw = sens * 10;
  TCRT = 1024 - TCRT;
#endif

  HHCnum = padNum;
  velocity = 0;
  openHH = false;
  closeHH = false;

  if (TCRT > thre2Raw && closeHH == false && pedalVelocityFlag == false && pedalFlag == false)
  {
    time_hit_pedal_1 = millis();
    pedalVelocityFlag = true;
  }

  else if (TCRT > sens && pedalFlag == false)
  {
    time_hit_pedal_2 = millis();

    velocity = time_hit_pedal_2 - time_hit_pedal_1;
    velocity = map(velocity, scan * 100, 0, 1, 127);

    if (velocity <= 1)
    {
      velocity = 1;
    }

    if (velocity > 127)
    {
      velocity = 127;
    }

    closeHH = true;
    openHH = false;
    pedalFlag = true;
    pedalVelocityFlag = false;
  }

  if (TCRT < thre2Raw && pedalFlag == true)
  {
    pedalFlag = false;
    closeHH = false;
    openHH = true;
  }

  /////////////////////// HIHAT PEDAL CC

  /* 
#ifdef __AVR__
  TCRT = map(TCRT, thre1Raw, sensRaw, 0, 127);

  if (TCRT > 127)
  {
    TCRT = 127;
  }
  if (TCRT < 0)
  {
    TCRT = 0;
  }
#endif
*/

  //#ifdef ESP32
  TCRT = map(TCRT, thre1Raw, sensRaw, 0, 127);
  //6段階に分ける

  if (TCRT < 20)
  {
    TCRT = 0;
  }

  else if (TCRT >= 20 && TCRT < 40)
  {
    TCRT = 20;
  }

  else if (TCRT >= 40 && TCRT < 60)
  {
    TCRT = 40;
  }

  else if (TCRT >= 60 && TCRT < 80)
  {
    TCRT = 60;
  }

  else if (TCRT >= 80 && TCRT < 100)
  {
    TCRT = 80;
  }

  else if (TCRT >= 100 && TCRT < 120)
  {
    TCRT = 100;
  }

  else if (TCRT >= 120)
  {
    TCRT = 127;
  }
  //#endif

  if (exTCRT != TCRT)
  {
    pedalCC = TCRT;
    moving = true;
    exTCRT = TCRT;
  }

  else
  {
    moving = false;
  }
}

void HelloDrum::FSR(int sens, int thre1, int thre2, int scan)
{

#ifdef ESP32
  int thre1Raw = thre1 * 40;
  int thre2Raw = thre2 * 40;
  int sensRaw = sens * 40;
#else
  int thre1Raw = thre1 * 10;
  int thre2Raw = thre2 * 10;
  int sensRaw = sens * 10;
#endif

  HHCnum = padNum;
  velocity = 0;
  openHH = false;
  closeHH = false;
  int FSR = analogRead(pin_1);

  if (FSR > thre2Raw && closeHH == false && pedalVelocityFlag == false && pedalFlag == false)
  {
    time_hit_pedal_1 = millis();
    pedalVelocityFlag = true;
  }

  else if (FSR > sens && pedalFlag == false)
  {
    time_hit_pedal_2 = millis();

    velocity = time_hit_pedal_2 - time_hit_pedal_1;
    velocity = map(velocity, scan * 100, 0, 1, 127);

    if (velocity <= 1)
    {
      velocity = 1;
    }

    if (velocity > 127)
    {
      velocity = 127;
    }

    closeHH = true;
    openHH = false;
    pedalFlag = true;
    pedalVelocityFlag = false;
  }

  if (FSR < thre2Raw && pedalFlag == true)
  {
    pedalFlag = false;
    closeHH = false;
    openHH = true;
  }

  /////////////////////// HIHAT PEDAL CC

  /* 
#ifdef __AVR__
  FSR = map(FSR, thre1Raw, sensRaw, 0, 127);

  if (FSR > 127)
  {
    FSR = 127;
  }
  if (FSR < 0)
  {
    FSR = 0;
  }
#endif
*/

  //#ifdef ESP32
  FSR = map(FSR, thre1Raw, sensRaw, 0, 127);
  //6段階に分ける

  if (FSR < 20)
  {
    FSR = 0;
  }

  else if (FSR >= 20 && FSR < 40)
  {
    FSR = 20;
  }

  else if (FSR >= 40 && FSR < 60)
  {
    FSR = 40;
  }

  else if (FSR >= 60 && FSR < 80)
  {
    FSR = 60;
  }

  else if (FSR >= 80 && FSR < 100)
  {
    FSR = 80;
  }

  else if (FSR >= 100 && FSR < 120)
  {
    FSR = 100;
  }

  else if (FSR >= 120)
  {
    FSR = 127;
  }
  //#endif

  if (exFSR != FSR)
  {
    pedalCC = FSR;
    moving = true;
    exFSR = FSR;
  }

  else
  {
    moving = false;
  }
}

////////////////// 2. SENSING WITH LCD & EEPROM //////////////////////

void HelloDrum::singlePiezo()
{

#ifdef ESP32
  int threshold1Raw = threshold1 * 40;
  int sensitivityRaw = sensitivity * 40;
#else
  int threshold1Raw = threshold1 * 10;
  int sensitivityRaw = sensitivity * 10;
#endif

  hit = false;
  piezoValue = analogRead(pin_1);

  //when the value > threshold
  if (piezoValue > threshold1Raw && loopTimes == 0)
  {
    time_hit = millis(); //check the time pad hitted

    //compare time to cancel retrigger
    if (time_hit - time_end < masktime)
    {
      flag = true;
    }
    else
    {
      velocity = piezoValue; //first peak
      loopTimes = ++loopTimes;
      flag = false;
    }
  }

  //peak scan start
  if (flag == false && loopTimes > 0)
  {
    if (loopTimes != scantime)
    {
      if (piezoValue > velocity)
      {
        velocity = piezoValue;
      }
      loopTimes = ++loopTimes;
    }

    //scan end
    if (loopTimes == scantime)
    {
      velocity = map(velocity, threshold1Raw, sensitivityRaw, 1, 127);

      if (velocity <= 1)
      {
        velocity = 1;
      }

      if (velocity > 127)
      {
        velocity = 127;
      }

      //velo = (velo * velo) / 126 + 1;

      showVelocity = velocity;
      showLCD = true;
      hit = true;
      padIndex = padNum;
      loopTimes = 0;
      flag = true;
    }
  }

  if (flag == true)
  {
    time_end = millis();
    flag = false;
  }
}

void HelloDrum::dualPiezo()
{

#ifdef ESP32
  int threshold1Raw = threshold1 * 40;
  int sensitivityRaw = sensitivity * 40;
#else
  int threshold1Raw = threshold1 * 10;
  int sensitivityRaw = sensitivity * 10;
#endif

  hit = false;
  hitRim = false;
  piezoValue = analogRead(pin_1);
  RimPiezoValue = analogRead(pin_2);

  //when the value > threshold
  if ((piezoValue > threshold1Raw && loopTimes == 0) || (RimPiezoValue > threshold1Raw && loopTimes == 0))
  {
    time_hit = millis();

    if (time_hit - time_end < masktime)
    {
      flag = true;
    }
    else
    {
      velocity = piezoValue; //first peak
      velocityRim = RimPiezoValue;
      loopTimes = ++loopTimes;
      flag = false;
    }
  }

  //peak scan start
  if (flag == false && loopTimes > 0)
  {
    if (loopTimes != scantime)
    {
      piezoValueSUM = piezoValueSUM + piezoValue;
      RimPiezoValueSUM = RimPiezoValueSUM + RimPiezoValue;

      if (piezoValue > velocity)
      {
        velocity = piezoValue;
      }
      if (RimPiezoValue > velocityRim)
      {
        velocityRim = RimPiezoValue;
      }
      loopTimes = ++loopTimes;
    }

    //scan end
    if (loopTimes == scantime)
    {

      bool Rimjudge;

      if (velocity >= velocityRim)
      {
        Rimjudge = false;
      }
      else if (velocity < velocityRim)
      {
        Rimjudge = true;
      }

      velocity = map(velocity, threshold1Raw, sensitivityRaw, 1, 127);
      velocityRim = map(velocityRim, threshold1Raw, sensitivityRaw, 1, 127);

      if (velocity <= 1)
      {
        velocity = 1;
      }

      if (velocity > 127)
      {
        velocity = 127;
      }

      if (velocityRim <= 1)
      {
        velocityRim = 1;
      }

      if (velocityRim > 127)
      {
        velocityRim = 127;
      }

      //if (piezoValueSUM > RimPiezoValueSUM)
      if (Rimjudge == false)
      {
        showVelocity = velocity;
        showLCD = true;
        hit = true;
        padIndex = padNum;
        loopTimes = 0;
        flag = true;
      }
      //else if (piezoValueSUM <= RimPiezoValueSUM)
      else if (Rimjudge == true)
      {
        velocity = velocityRim;
        showVelocity = velocity;
        showLCD = true;
        hitRim = true;
        padIndex = padNum;
        loopTimes = 0;
        flag = true;
      }
    }
  }

  if (flag == true)
  {
    time_end = millis();
    flag = false;
  }
}

void HelloDrum::HH()
{

#ifdef ESP32
  int threshold1Raw = threshold1 * 40;
  int sensitivityRaw = sensitivity * 40;
#else
  int threshold1Raw = threshold1 * 10;
  int sensitivityRaw = sensitivity * 10;
#endif

  HHnum = padNum;
  hit = false;
  piezoValue = analogRead(pin_1);

  //when the value > threshold
  if (piezoValue > threshold1Raw && loopTimes == 0)
  {
    time_hit = millis(); //check the time pad hitted

    //compare time to cancel retrigger
    if (time_hit - time_end < masktime)
    {
      flag = true;
    }
    else
    {
      velocity = piezoValue; //first peak
      loopTimes = ++loopTimes;
      flag = false;
    }
  }

  //peak scan start
  if (flag == false && loopTimes > 0)
  {
    if (loopTimes != scantime)
    {
      if (piezoValue > velocity)
      {
        velocity = piezoValue;
      }
      loopTimes = ++loopTimes;
    }

    //scan end
    if (loopTimes == scantime)
    {
      velocity = map(velocity, threshold1Raw, sensitivityRaw, 1, 127);

      if (velocity <= 1)
      {
        velocity = 1;
      }

      if (velocity > 127)
      {
        velocity = 127;
      }

      //velo = (velo * velo) / 126 + 1;

      showVelocity = velocity;
      showLCD = true;
      hit = true;
      padIndex = padNum;
      loopTimes = 0;
      flag = true;
    }
  }

  if (flag == true)
  {
    time_end = millis();
    flag = false;
  }
}

void HelloDrum::HH2zone()
{

#ifdef ESP32
  int threshold1Raw = threshold1 * 40;
  int sensitivityRaw = sensitivity * 40;
  int edgeThreshold = 200;
#else
  int threshold1Raw = threshold1 * 10;
  int sensitivityRaw = sensitivity * 10;
  int edgeThreshold = 50;
#endif

  HHnum = padNum;
  hit = false;
  hitRim = false;
  choke = false;

  piezoValue = analogRead(pin_1);
  sensorValue = analogRead(pin_2);

  //when the value > threshold
  if ((piezoValue > threshold1Raw && loopTimes == 0) || (sensorValue > edgeThreshold && loopTimes == 0))
  {
    time_hit = millis(); //check the time pad hitted

    if (time_hit - time_end < masktime)
    {
      flag = true;
    }
    else
    {
      velocity = abs(piezoValue - sensorValue); //first peak
      firstSensorValue = sensorValue;
      loopTimes = ++loopTimes;
      flag = false;
    }
  }

  //peak scan start
  if (flag == false && loopTimes > 0)
  {
    if (loopTimes != scantime)
    {
      if (abs(piezoValue - sensorValue) > velocity)
      {
        velocity = abs(piezoValue - sensorValue);
      }
      if (sensorValue > firstSensorValue && loopTimes < 5)
      {
        firstSensorValue = sensorValue;
      }
      loopTimes = ++loopTimes;
    }

    //scan end
    if (loopTimes == scantime)
    {
      lastSensorValue = sensorValue;
      velocity = map(velocity, threshold1Raw, sensitivityRaw, 1, 127);

      if (velocity <= 1)
      {
        velocity = 1;
      }

      if (velocity > 127)
      {
        velocity = 127;
      }

      //bow
      if (firstSensorValue < edgeThreshold && lastSensorValue < edgeThreshold)
      {
        showVelocity = velocity;
        showLCD = true;
        flag = true;
        hit = true;
        padIndex = padNum;
      }

      //edge
      else if (firstSensorValue > edgeThreshold && lastSensorValue < edgeThreshold)
      {
        showVelocity = velocity;
        showLCD = true;
        hitRim = true;
        flag = true;
        padIndex = padNum;
      }

      //choke
      else if (lastSensorValue > edgeThreshold && firstSensorValue > edgeThreshold)
      {
        choke = true;
        flag = true;
      }
      loopTimes = 0;
    }
  }

  if (flag == true)
  {
    time_end = millis();
    flag = false;
  }
}

////////////////////////////////////////////

void HelloDrum::cymbal2zone()
{

#ifdef ESP32
  int threshold1Raw = threshold1 * 40;
  int sensitivityRaw = sensitivity * 40;
  int edgeThreshold = 200;
#else
  int threshold1Raw = threshold1 * 10;
  int sensitivityRaw = sensitivity * 10;
  int edgeThreshold = 50;
#endif

  hit = false;
  hitRim = false;
  choke = false;

  piezoValue = analogRead(pin_1);
  sensorValue = analogRead(pin_2);

  //when the value > threshold
  if ((piezoValue > threshold1Raw && loopTimes == 0) || (sensorValue > edgeThreshold && loopTimes == 0))
  {
    time_hit = millis(); //check the time pad hitted

    if (time_hit - time_end < masktime)
    {
      flag = true;
    }
    else
    {
      velocity = abs(piezoValue - sensorValue); //first peak
      firstSensorValue = sensorValue;
      loopTimes = ++loopTimes;
      flag = false;
    }
  }

  //peak scan start
  if (flag == false && loopTimes > 0)
  {
    if (loopTimes != scantime)
    {
      if (abs(piezoValue - sensorValue) > velocity)
      {
        velocity = abs(piezoValue - sensorValue);
      }
      if (sensorValue > firstSensorValue && loopTimes < 5)
      {
        firstSensorValue = sensorValue;
      }
      loopTimes = ++loopTimes;
    }

    //scan end
    if (loopTimes == scantime)
    {
      lastSensorValue = sensorValue;
      velocity = map(velocity, threshold1Raw, sensitivityRaw, 1, 127);

      if (velocity <= 1)
      {
        velocity = 1;
      }

      if (velocity > 127)
      {
        velocity = 127;
      }

      //bow
      if (firstSensorValue < edgeThreshold && lastSensorValue < edgeThreshold)
      {
        showVelocity = velocity;
        showLCD = true;
        flag = true;
        hit = true;
        padIndex = padNum;
      }

      //edge
      else if (firstSensorValue > edgeThreshold && lastSensorValue < edgeThreshold)
      {
        showVelocity = velocity;
        showLCD = true;
        hitRim = true;
        flag = true;
        padIndex = padNum;
      }

      //choke
      else if (lastSensorValue > edgeThreshold && firstSensorValue > edgeThreshold)
      {
        choke = true;
        flag = true;
      }
      loopTimes = 0;
    }
  }

  if (flag == true)
  {
    time_end = millis();
    flag = false;
  }
}

void HelloDrum::cymbal3zone()
{

#ifdef ESP32
  int threshold1Raw = threshold1 * 40;
  int sensitivityRaw = sensitivity * 40;
  int edgeThreshold = 200;
  int cupThreshold = 2000;
  int loopTimesCup = 25;
#else
  int threshold1Raw = threshold1 * 10;
  int sensitivityRaw = sensitivity * 10;
  int edgeThreshold = 50;
  int cupThreshold = 520;
  int loopTimesCup = 10; //?
#endif

  hit = false;
  hitRim = false;
  hitCup = false;
  choke = false;

  piezoValue = analogRead(pin_1);
  sensorValue = analogRead(pin_2);

  //when the value > threshold
  if ((piezoValue > threshold1Raw && loopTimes == 0) || (sensorValue > edgeThreshold && loopTimes == 0))
  {
    time_hit = millis(); //check the time pad hitted

    if (time_hit - time_end < masktime)
    {
      flag = true;
    }
    else
    {
      velocity = abs(piezoValue - sensorValue); //first peak
      firstSensorValue = sensorValue;
      loopTimes = ++loopTimes;
      flag = false;
    }
  }

  //peak scan start
  if (flag == false && loopTimes > 0)
  {
    if (loopTimes != scantime)
    {
      if (loopTimes == loopTimesCup)
      {
        velocityCup = 1;
      }
      if (abs(piezoValue - sensorValue) > velocity)
      {
        velocity = abs(piezoValue - sensorValue);
        velocityCup = velocity;
      }
      if (sensorValue > firstSensorValue && loopTimes < 5)
      {
        firstSensorValue = sensorValue;
      }
      loopTimes = ++loopTimes;
    }

    //scan end
    if (loopTimes == scantime)
    {
      lastSensorValue = sensorValue;
      velocity = map(velocity, threshold1Raw, sensitivityRaw, 1, 127);
      velocityCup = map(velocityCup, threshold1Raw, sensitivityRaw, 1, 127);

      if (velocity <= 1)
      {
        velocity = 1;
      }

      if (velocity > 127)
      {
        velocity = 127;
      }

      //bow
      if (firstSensorValue < edgeThreshold && lastSensorValue < edgeThreshold)
      {
        showVelocity = velocity;
        showLCD = true;
        flag = true;
        hit = true;
        padIndex = padNum;
      }

      //edge
      else if (firstSensorValue > edgeThreshold && firstSensorValue < cupThreshold && lastSensorValue < edgeThreshold)
      {
        showVelocity = velocity;
        showLCD = true;
        hitRim = true;
        flag = true;
        padIndex = padNum;
      }

      //cup
      else if (firstSensorValue > cupThreshold && lastSensorValue < edgeThreshold)
      {
        velocity = velocityCup;
        showVelocity = velocity;
        showLCD = true;
        hitCup = true;
        flag = true;
        padIndex = padNum;
      }

      //choke
      else if (lastSensorValue > edgeThreshold && firstSensorValue > edgeThreshold)
      {
        choke = true;
        flag = true;
      }
      loopTimes = 0;
    }
  }

  if (flag == true)
  {
    time_end = millis();
    flag = false;
  }
}

void HelloDrum::TCRT5000()
{
  int TCRT = analogRead(pin_1);

#ifdef ESP32
  int threshold1Raw = threshold1 * 40;
  int threshold2Raw = threshold2 * 40;
  int sensitivityRaw = sensitivity * 40;
  TCRT = 4096 - TCRT;
#else
  int threshold1Raw = threshold1 * 10;
  int threshold2Raw = threshold2 * 10;
  int sensitivityRaw = sensitivity * 10;
  TCRT = 1024 - TCRT;
#endif

  HHCnum = padNum;
  velocity = 0;
  openHH = false;
  closeHH = false;

  if (TCRT > threshold2Raw && closeHH == false && pedalVelocityFlag == false && pedalFlag == false)
  {
    time_hit_pedal_1 = millis();
    pedalVelocityFlag = true;
  }

  else if (TCRT > sensitivity && pedalFlag == false)
  {
    time_hit_pedal_2 = millis();

    velocity = time_hit_pedal_2 - time_hit_pedal_1;
    velocity = map(velocity, scantime * 100, 0, 1, 127);

    if (velocity <= 1)
    {
      velocity = 1;
    }

    if (velocity > 127)
    {
      velocity = 127;
    }

    closeHH = true;
    openHH = false;
    pedalFlag = true;
    pedalVelocityFlag = false;
  }

  if (TCRT < threshold2Raw && pedalFlag == true)
  {
    pedalFlag = false;
    closeHH = false;
    openHH = true;
  }

  /////////////////////// HIHAT PEDAL CC

  /* 
#ifdef __AVR__
  TCRT = map(TCRT, thre1Raw, sensRaw, 0, 127);

  if (TCRT > 127)
  {
    TCRT = 127;
  }
  if (TCRT < 0)
  {
    TCRT = 0;
  }
#endif
*/

  //#ifdef ESP32
  TCRT = map(TCRT, threshold1Raw, sensitivityRaw, 0, 127);
  //6段階に分ける

  if (TCRT < 20)
  {
    TCRT = 0;
  }

  else if (TCRT >= 20 && TCRT < 40)
  {
    TCRT = 20;
  }

  else if (TCRT >= 40 && TCRT < 60)
  {
    TCRT = 40;
  }

  else if (TCRT >= 60 && TCRT < 80)
  {
    TCRT = 60;
  }

  else if (TCRT >= 80 && TCRT < 100)
  {
    TCRT = 80;
  }

  else if (TCRT >= 100 && TCRT < 120)
  {
    TCRT = 100;
  }

  else if (TCRT >= 120)
  {
    TCRT = 127;
  }
  //#endif

  if (exTCRT != TCRT)
  {
    pedalCC = TCRT;
    moving = true;
    exTCRT = TCRT;
  }

  else
  {
    moving = false;
  }
}

void HelloDrum::FSR()
{

#ifdef ESP32
  int threshold1Raw = threshold1 * 40;
  int threshold2Raw = threshold2 * 40;
  int sensitivityRaw = sensitivity * 40;
#else
  int threshold1Raw = threshold1 * 10;
  int threshold2Raw = threshold2 * 10;
  int sensitivityRaw = sensitivity * 10;
#endif

  HHCnum = padNum;
  velocity = 0;
  openHH = false;
  closeHH = false;
  int FSR = analogRead(pin_1);

  if (FSR > threshold2Raw && closeHH == false && pedalVelocityFlag == false && pedalFlag == false)
  {
    time_hit_pedal_1 = millis();
    pedalVelocityFlag = true;
  }

  else if (FSR > sensitivity && pedalFlag == false)
  {
    time_hit_pedal_2 = millis();

    velocity = time_hit_pedal_2 - time_hit_pedal_1;
    velocity = map(velocity, scantime * 100, 0, 1, 127);

    if (velocity <= 1)
    {
      velocity = 1;
    }

    if (velocity > 127)
    {
      velocity = 127;
    }

    closeHH = true;
    openHH = false;
    pedalFlag = true;
    pedalVelocityFlag = false;
  }

  if (FSR < threshold2Raw && pedalFlag == true)
  {
    pedalFlag = false;
    closeHH = false;
    openHH = true;
  }

  /////////////////////// HIHAT PEDAL CC

  /* 
#ifdef __AVR__
  FSR = map(FSR, thre1Raw, sensRaw, 0, 127);

  if (FSR > 127)
  {
    FSR = 127;
  }
  if (FSR < 0)
  {
    FSR = 0;
  }
#endif
*/

  //#ifdef ESP32
  FSR = map(FSR, threshold1Raw, sensitivityRaw, 0, 127);
  //6段階に分ける

  if (FSR < 20)
  {
    FSR = 0;
  }

  else if (FSR >= 20 && FSR < 40)
  {
    FSR = 20;
  }

  else if (FSR >= 40 && FSR < 60)
  {
    FSR = 40;
  }

  else if (FSR >= 60 && FSR < 80)
  {
    FSR = 60;
  }

  else if (FSR >= 80 && FSR < 100)
  {
    FSR = 80;
  }

  else if (FSR >= 100 && FSR < 120)
  {
    FSR = 100;
  }

  else if (FSR >= 120)
  {
    FSR = 127;
  }
  //#endif

  if (exFSR != FSR)
  {
    pedalCC = FSR;
    moving = true;
    exFSR = FSR;
  }

  else
  {
    moving = false;
  }
}

/////////////////////////////// 3. MUX SCANNING ///////////////////////////////////

//4051
void HelloDrumMUX_4051::scan()
{

  for (byte pin = muxNum * 8; pin < (muxNum + 1) * 8; pin++)
  {

    for (int i = 0; i < 3; i++)
    {
      if (pin & (1 << i))
        digitalWrite(selectPins[i], HIGH);
      else
        digitalWrite(selectPins[i], LOW);
    }

    rawValue[pin] = analogRead(pin_A);
  }
}

//4067
void HelloDrumMUX_4067::scan()
{

  for (byte pin = muxNum * 16; pin < (muxNum + 1) * 16; pin++)
  {

    for (int i = 0; i < 4; i++)
    {
      if (pin & (1 << i))
        digitalWrite(selectPins[i], HIGH);
      else
        digitalWrite(selectPins[i], LOW);
    }

    rawValue[pin] = analogRead(pin_A);
  }
}

//////////////////////// 4. MUX SENSING without EEPROM //////////////////////////

void HelloDrum::singlePiezoMUX(int sens, int thre1, int scan, int mask)
{

#ifdef ESP32
  int thre1Raw = thre1 * 40;
  int sensRaw = sens * 40;
#else
  int thre1Raw = thre1 * 10;
  int sensRaw = sens * 10;
#endif

  hit = false;
  piezoValue = rawValue[pin_1];

  //when the value > threshold
  if (piezoValue > thre1Raw && loopTimes == 0)
  {
    time_hit = millis(); //check the time pad hitted

    //compare time to cancel retrigger
    if (time_hit - time_end < mask)
    {
      flag = true;
    }
    else
    {
      velocity = piezoValue; //first peak
      loopTimes = ++loopTimes;
      flag = false;
    }
  }

  //peak scan start
  if (flag == false && loopTimes > 0)
  {
    if (loopTimes != scan)
    {
      if (piezoValue > velocity)
      {
        velocity = piezoValue;
      }
      loopTimes = ++loopTimes;
    }

    //scan end
    if (loopTimes == scan)
    {
      velocity = map(velocity, thre1Raw, sensRaw, 1, 127);

      if (velocity <= 1)
      {
        velocity = 1;
      }

      if (velocity > 127)
      {
        velocity = 127;
      }

      //velo = (velo * velo) / 126 + 1;
      hit = true;
      loopTimes = 0;
      flag = true;
    }
  }

  if (flag == true)
  {
    time_end = millis();
    flag = false;
  }
}

void HelloDrum::dualPiezoMUX(int sens, int thre1, int scan, int mask)
{

#ifdef ESP32
  int thre1Raw = thre1 * 40;
  int sensRaw = sens * 40;
#else
  int thre1Raw = thre1 * 10;
  int sensRaw = sens * 10;
#endif

  hit = false;
  hitRim = false;
  piezoValue = rawValue[pin_1];
  RimPiezoValue = rawValue[pin_2];

  //when the value > threshold
  if ((piezoValue > thre1Raw && loopTimes == 0) || (RimPiezoValue > thre1Raw && loopTimes == 0))
  {
    time_hit = millis();

    if (time_hit - time_end < mask)
    {
      flag = true;
    }
    else
    {
      velocity = piezoValue; //first peak
      velocityRim = RimPiezoValue;
      loopTimes = ++loopTimes;
      flag = false;
    }
  }

  //peak scan start
  if (flag == false && loopTimes > 0)
  {
    if (loopTimes != scan)
    {
      piezoValueSUM = piezoValueSUM + piezoValue;
      RimPiezoValueSUM = RimPiezoValueSUM + RimPiezoValue;

      if (piezoValue > velocity)
      {
        velocity = piezoValue;
      }
      if (RimPiezoValue > velocityRim)
      {
        velocityRim = RimPiezoValue;
      }
      loopTimes = ++loopTimes;
    }

    //scan end
    if (loopTimes == scan)
    {

      bool Rimjudge;

      if (velocity >= velocityRim)
      {
        Rimjudge = false;
      }
      else if (velocity < velocityRim)
      {
        Rimjudge = true;
      }

      velocity = map(velocity, thre1Raw, sensRaw, 1, 127);
      velocityRim = map(velocityRim, thre1Raw, sensRaw, 1, 127);

      if (velocity <= 1)
      {
        velocity = 1;
      }

      if (velocity > 127)
      {
        velocity = 127;
      }

      if (velocityRim <= 1)
      {
        velocityRim = 1;
      }

      if (velocityRim > 127)
      {
        velocityRim = 127;
      }

      //if (piezoValueSUM > RimPiezoValueSUM)
      if (Rimjudge == false)
      {
        hit = true;
        loopTimes = 0;
        flag = true;
      }
      //else if (piezoValueSUM <= RimPiezoValueSUM)
      else if (Rimjudge == true)
      {
        velocity = velocityRim;
        hitRim = true;
        loopTimes = 0;
        flag = true;
      }
    }
  }

  if (flag == true)
  {
    time_end = millis();
    flag = false;
  }
}

void HelloDrum::HHMUX(int sens, int thre1, int scan, int mask)
{

#ifdef ESP32
  int thre1Raw = thre1 * 40;
  int sensRaw = sens * 40;
#else
  int thre1Raw = thre1 * 10;
  int sensRaw = sens * 10;
#endif

  HHnum = padNum;
  hit = false;
  piezoValue = rawValue[pin_1];

  //when the value > threshold
  if (piezoValue > thre1Raw && loopTimes == 0)
  {
    time_hit = millis(); //check the time pad hitted

    //compare time to cancel retrigger
    if (time_hit - time_end < mask)
    {
      flag = true;
    }
    else
    {
      velocity = piezoValue; //first peak
      loopTimes = ++loopTimes;
      flag = false;
    }
  }

  //peak scan start
  if (flag == false && loopTimes > 0)
  {
    if (loopTimes != scan)
    {
      if (piezoValue > velocity)
      {
        velocity = piezoValue;
      }
      loopTimes = ++loopTimes;
    }

    //scan end
    if (loopTimes == scan)
    {
      velocity = map(velocity, thre1Raw, sensRaw, 1, 127);

      if (velocity <= 1)
      {
        velocity = 1;
      }

      if (velocity > 127)
      {
        velocity = 127;
      }

      //velo = (velo * velo) / 126 + 1;
      hit = true;
      loopTimes = 0;
      flag = true;
    }
  }

  if (flag == true)
  {
    time_end = millis();
    flag = false;
  }
}

////////////////////////////////////////////
void HelloDrum::HH2zoneMUX(int sens, int thre1, int scan, int mask)
{

#ifdef ESP32
  int thre1Raw = thre1 * 40;
  int sensRaw = sens * 40;
  int edgeThreshold = 200;
#else
  int thre1Raw = thre1 * 10;
  int sensRaw = sens * 10;
  int edgeThreshold = 50;
#endif

  HHnum = padNum;
  hit = false;
  hitRim = false;
  choke = false;

  piezoValue = rawValue[pin_1];
  sensorValue = rawValue[pin_2];

  //when the value > threshold
  if ((piezoValue > thre1Raw && loopTimes == 0) || (sensorValue > edgeThreshold && loopTimes == 0))
  {
    time_hit = millis(); //check the time pad hitted

    if (time_hit - time_end < mask)
    {
      flag = true;
    }
    else
    {
      velocity = abs(piezoValue - sensorValue); //first peak //ここコメント化ありうる。
      firstSensorValue = sensorValue;
      loopTimes = ++loopTimes;
      flag = false;
    }
  }

  //peak scan start
  if (flag == false && loopTimes > 0)
  {
    if (loopTimes != scan)
    {
      if (abs(piezoValue - sensorValue) > velocity)
      {
        velocity = abs(piezoValue - sensorValue);
      }
      if (sensorValue > firstSensorValue && loopTimes < 5)
      {
        firstSensorValue = sensorValue;
      }
      loopTimes = ++loopTimes;
    }

    //scan end
    if (loopTimes == scan)
    {
      lastSensorValue = sensorValue;
      velocity = map(velocity, thre1Raw, sensRaw, 1, 127);

      if (velocity <= 1)
      {
        velocity = 1;
      }

      if (velocity > 127)
      {
        velocity = 127;
      }

      //bow
      if (firstSensorValue < edgeThreshold && lastSensorValue < edgeThreshold)
      {
        flag = true;
        hit = true;
      }

      //edge
      else if (firstSensorValue > edgeThreshold && lastSensorValue < edgeThreshold)
      {
        hitRim = true;
        flag = true;
      }

      //choke
      else if (lastSensorValue > edgeThreshold && firstSensorValue > edgeThreshold)
      {
        choke = true;
        flag = true;
      }
      loopTimes = 0;
    }
  }

  if (flag == true)
  {
    time_end = millis();
    flag = false;
  }
}

void HelloDrum::cymbal2zoneMUX(int sens, int thre1, int scan, int mask)
{

#ifdef ESP32
  int thre1Raw = thre1 * 40;
  int sensRaw = sens * 40;
  int edgeThreshold = 200;
#else
  int thre1Raw = thre1 * 10;
  int sensRaw = sens * 10;
  int edgeThreshold = 50;
#endif

  hit = false;
  hitRim = false;
  choke = false;

  piezoValue = rawValue[pin_1];
  sensorValue = rawValue[pin_2];

  //when the value > threshold
  if ((piezoValue > thre1Raw && loopTimes == 0) || (sensorValue > edgeThreshold && loopTimes == 0))
  {
    time_hit = millis(); //check the time pad hitted

    if (time_hit - time_end < mask)
    {
      flag = true;
    }
    else
    {
      velocity = abs(piezoValue - sensorValue); //first peak //ここコメント化ありうる。
      firstSensorValue = sensorValue;
      loopTimes = ++loopTimes;
      flag = false;
    }
  }

  //peak scan start
  if (flag == false && loopTimes > 0)
  {
    if (loopTimes != scan)
    {
      if (abs(piezoValue - sensorValue) > velocity)
      {
        velocity = abs(piezoValue - sensorValue);
      }
      if (sensorValue > firstSensorValue && loopTimes < 5)
      {
        firstSensorValue = sensorValue;
      }
      loopTimes = ++loopTimes;
    }

    //scan end
    if (loopTimes == scan)
    {
      lastSensorValue = sensorValue;
      velocity = map(velocity, thre1Raw, sensRaw, 1, 127);

      if (velocity <= 1)
      {
        velocity = 1;
      }

      if (velocity > 127)
      {
        velocity = 127;
      }

      //bow
      if (firstSensorValue < edgeThreshold && lastSensorValue < edgeThreshold)
      {
        flag = true;
        hit = true;
      }

      //edge
      else if (firstSensorValue > edgeThreshold && lastSensorValue < edgeThreshold)
      {
        hitRim = true;
        flag = true;
      }

      //choke
      else if (lastSensorValue > edgeThreshold && firstSensorValue > edgeThreshold)
      {
        choke = true;
        flag = true;
      }
      loopTimes = 0;
    }
  }

  if (flag == true)
  {
    time_end = millis();
    flag = false;
  }
}

void HelloDrum::cymbal3zoneMUX(int sens, int thre1, int scan, int mask)
{

#ifdef ESP32
  int thre1Raw = thre1 * 40;
  int sensRaw = sens * 40;
  int edgeThreshold = 200;
  int cupThreshold = 2000;
  int loopTimesCup = 25;
#else
  int thre1Raw = thre1 * 10;
  int sensRaw = sens * 10;
  int edgeThreshold = 50;
  int cupThreshold = 520;
  int loopTimesCup = 10; //?
#endif

  hit = false;
  hitRim = false;
  hitCup = false;
  choke = false;

  piezoValue = rawValue[pin_1];
  sensorValue = rawValue[pin_2];

  //when the value > threshold
  if ((piezoValue > thre1Raw && loopTimes == 0) || (sensorValue > edgeThreshold && loopTimes == 0))
  {
    time_hit = millis(); //check the time pad hitted

    if (time_hit - time_end < mask)
    {
      flag = true;
    }
    else
    {
      velocity = abs(piezoValue - sensorValue); //first peak //ここコメント化ありうる。
      firstSensorValue = sensorValue;
      loopTimes = ++loopTimes;
      flag = false;
    }
  }

  //peak scan start
  if (flag == false && loopTimes > 0)
  {
    if (loopTimes != scan)
    {
      if (loopTimes == loopTimesCup)
      {
        velocityCup = 1;
      }
      if (abs(piezoValue - sensorValue) > velocity)
      {
        velocity = abs(piezoValue - sensorValue);
        velocityCup = velocity;
      }
      if (sensorValue > firstSensorValue && loopTimes < 5)
      {
        firstSensorValue = sensorValue;
      }
      loopTimes = ++loopTimes;
    }

    //scan end
    if (loopTimes == scan)
    {
      lastSensorValue = sensorValue;
      velocity = map(velocity, thre1Raw, sensRaw, 1, 127);
      velocityCup = map(velocityCup, thre1Raw, sensRaw, 1, 127);

      if (velocity <= 1)
      {
        velocity = 1;
      }

      if (velocity > 127)
      {
        velocity = 127;
      }

      if (velocityCup <= 1)
      {
        velocityCup = 1;
      }

      if (velocityCup > 127)
      {
        velocityCup = 127;
      }

      //bow
      if (firstSensorValue < edgeThreshold && lastSensorValue < edgeThreshold)
      {
        flag = true;
        hit = true;
      }

      //edge
      else if (firstSensorValue > edgeThreshold && firstSensorValue < cupThreshold && lastSensorValue < edgeThreshold)
      {
        hitRim = true;
        flag = true;
      }

      //cup
      else if (firstSensorValue > cupThreshold && lastSensorValue < edgeThreshold)
      {
        velocity = velocityCup;
        hitCup = true;
        flag = true;
      }

      //choke
      else if (lastSensorValue > edgeThreshold && firstSensorValue > edgeThreshold)
      {
        choke = true;
        flag = true;
      }
      loopTimes = 0;
    }
  }

  if (flag == true)
  {
    time_end = millis();
    flag = false;
  }
}

void HelloDrum::TCRT5000MUX(int sens, int thre1, int thre2, int scan)
{
  int TCRT = rawValue[pin_1];

#ifdef ESP32
  int thre1Raw = thre1 * 40;
  int thre2Raw = thre2 * 40;
  int sensRaw = sens * 40;
  TCRT = 4096 - TCRT;
#else
  int thre1Raw = thre1 * 10;
  int thre2Raw = thre2 * 10;
  int sensRaw = sens * 10;
  TCRT = 1024 - TCRT;
#endif

  HHCnum = padNum;
  velocity = 0;
  openHH = false;
  closeHH = false;

  if (TCRT > thre2Raw && closeHH == false && pedalVelocityFlag == false && pedalFlag == false)
  {
    time_hit_pedal_1 = millis();
    pedalVelocityFlag = true;
  }

  else if (TCRT > sens && pedalFlag == false)
  {
    time_hit_pedal_2 = millis();

    velocity = time_hit_pedal_2 - time_hit_pedal_1;
    velocity = map(velocity, scan * 100, 0, 1, 127);

    if (velocity <= 1)
    {
      velocity = 1;
    }

    if (velocity > 127)
    {
      velocity = 127;
    }

    closeHH = true;
    openHH = false;
    pedalFlag = true;
    pedalVelocityFlag = false;
  }

  if (TCRT < thre2Raw && pedalFlag == true)
  {
    pedalFlag = false;
    closeHH = false;
    openHH = true;
  }

  /////////////////////// HIHAT PEDAL CC

  /* 
#ifdef __AVR__
  TCRT = map(TCRT, thre1Raw, sensRaw, 0, 127);

  if (TCRT > 127)
  {
    TCRT = 127;
  }
  if (TCRT < 0)
  {
    TCRT = 0;
  }
#endif
*/

  //#ifdef ESP32
  TCRT = map(TCRT, thre1Raw, sensRaw, 0, 127);
  //6段階に分ける

  if (TCRT < 20)
  {
    TCRT = 0;
  }

  else if (TCRT >= 20 && TCRT < 40)
  {
    TCRT = 20;
  }

  else if (TCRT >= 40 && TCRT < 60)
  {
    TCRT = 40;
  }

  else if (TCRT >= 60 && TCRT < 80)
  {
    TCRT = 60;
  }

  else if (TCRT >= 80 && TCRT < 100)
  {
    TCRT = 80;
  }

  else if (TCRT >= 100 && TCRT < 120)
  {
    TCRT = 100;
  }

  else if (TCRT >= 120)
  {
    TCRT = 127;
  }
  //#endif

  if (exTCRT != TCRT)
  {
    pedalCC = TCRT;
    moving = true;
    exTCRT = TCRT;
  }

  else
  {
    moving = false;
  }
}

//with EEPROM と比較！
void HelloDrum::FSRMUX(int sens, int thre1, int thre2, int scan)
{

#ifdef ESP32
  int thre1Raw = thre1 * 40;
  int thre2Raw = thre2 * 40;
  int sensRaw = sens * 40;
#else
  int thre1Raw = thre1 * 10;
  int thre2Raw = thre2 * 10;
  int sensRaw = sens * 10;
#endif

  HHCnum = padNum;
  velocity = 0;
  openHH = false;
  closeHH = false;
  int FSR = rawValue[pin_1];

  if (FSR > thre2Raw && closeHH == false && pedalVelocityFlag == false && pedalFlag == false)
  {
    time_hit_pedal_1 = millis();
    pedalVelocityFlag = true;
  }

  else if (FSR > sens && pedalFlag == false)
  {
    time_hit_pedal_2 = millis();

    velocity = time_hit_pedal_2 - time_hit_pedal_1;
    velocity = map(velocity, scan * 100, 0, 1, 127);

    if (velocity <= 1)
    {
      velocity = 1;
    }

    if (velocity > 127)
    {
      velocity = 127;
    }

    closeHH = true;
    openHH = false;
    pedalFlag = true;
    pedalVelocityFlag = false;
  }

  if (FSR < thre2Raw && pedalFlag == true)
  {
    pedalFlag = false;
    closeHH = false;
    openHH = true;
  }

  /////////////////////// HIHAT PEDAL CC

  /* 
#ifdef __AVR__
  FSR = map(FSR, thre1Raw, sensRaw, 0, 127);

  if (FSR > 127)
  {
    FSR = 127;
  }
  if (FSR < 0)
  {
    FSR = 0;
  }
#endif
*/

  //#ifdef ESP32
  FSR = map(FSR, thre1Raw, sensRaw, 0, 127);
  //6段階に分ける

  if (FSR < 20)
  {
    FSR = 0;
  }

  else if (FSR >= 20 && FSR < 40)
  {
    FSR = 20;
  }

  else if (FSR >= 40 && FSR < 60)
  {
    FSR = 40;
  }

  else if (FSR >= 60 && FSR < 80)
  {
    FSR = 60;
  }

  else if (FSR >= 80 && FSR < 100)
  {
    FSR = 80;
  }

  else if (FSR >= 100 && FSR < 120)
  {
    FSR = 100;
  }

  else if (FSR >= 120)
  {
    FSR = 127;
  }
  //#endif

  if (exFSR != FSR)
  {
    pedalCC = FSR;
    moving = true;
    exFSR = FSR;
  }

  else
  {
    moving = false;
  }
}

//////////////////////// 5. MUX SENSING with EEPROM //////////////////////////

void HelloDrum::singlePiezoMUX()
{

#ifdef ESP32
  int threshold1Raw = threshold1 * 40;
  int sensitivityRaw = sensitivity * 40;
#else
  int threshold1Raw = threshold1 * 10;
  int sensitivityRaw = sensitivity * 10;
#endif

  hit = false;
  piezoValue = rawValue[pin_1];

  //when the value > threshold
  if (piezoValue > threshold1Raw && loopTimes == 0)
  {
    time_hit = millis(); //check the time pad hitted

    //compare time to cancel retrigger
    if (time_hit - time_end < masktime)
    {
      flag = true;
    }
    else
    {
      velocity = piezoValue; //first peak
      loopTimes = ++loopTimes;
      flag = false;
    }
  }

  //peak scan start
  if (flag == false && loopTimes > 0)
  {
    if (loopTimes != scantime)
    {
      if (piezoValue > velocity)
      {
        velocity = piezoValue;
      }
      loopTimes = ++loopTimes;
    }

    //scan end
    if (loopTimes == scantime)
    {
      velocity = map(velocity, threshold1Raw, sensitivityRaw, 1, 127);

      if (velocity <= 1)
      {
        velocity = 1;
      }

      if (velocity > 127)
      {
        velocity = 127;
      }

      //velo = (velo * velo) / 126 + 1;

      showVelocity = velocity;
      showLCD = true;
      hit = true;
      padIndex = padNum;
      loopTimes = 0;
      flag = true;
    }
  }

  if (flag == true)
  {
    time_end = millis();
    flag = false;
  }
}

void HelloDrum::dualPiezoMUX()
{

#ifdef ESP32
  int threshold1Raw = threshold1 * 40;
  int sensitivityRaw = sensitivity * 40;
#else
  int threshold1Raw = threshold1 * 10;
  int sensitivityRaw = sensitivity * 10;
#endif

  hit = false;
  hitRim = false;
  piezoValue = rawValue[pin_1];
  RimPiezoValue = rawValue[pin_2];

  //when the value > threshold
  if ((piezoValue > threshold1Raw && loopTimes == 0) || (RimPiezoValue > threshold1Raw && loopTimes == 0))
  {
    time_hit = millis();

    if (time_hit - time_end < masktime)
    {
      flag = true;
    }
    else
    {
      velocity = piezoValue; //first peak
      velocityRim = RimPiezoValue;
      loopTimes = ++loopTimes;
      flag = false;
    }
  }

  //peak scan start
  if (flag == false && loopTimes > 0)
  {
    if (loopTimes != scantime)
    {
      piezoValueSUM = piezoValueSUM + piezoValue;
      RimPiezoValueSUM = RimPiezoValueSUM + RimPiezoValue;

      if (piezoValue > velocity)
      {
        velocity = piezoValue;
      }
      if (RimPiezoValue > velocityRim)
      {
        velocityRim = RimPiezoValue;
      }
      loopTimes = ++loopTimes;
    }

    //scan end
    if (loopTimes == scantime)
    {

      bool Rimjudge;

      if (velocity >= velocityRim)
      {
        Rimjudge = false;
      }
      else if (velocity < velocityRim)
      {
        Rimjudge = true;
      }

      velocity = map(velocity, threshold1Raw, sensitivityRaw, 1, 127);
      velocityRim = map(velocityRim, threshold1Raw, sensitivityRaw, 1, 127);

      if (velocity <= 1)
      {
        velocity = 1;
      }

      if (velocity > 127)
      {
        velocity = 127;
      }

      if (velocityRim <= 1)
      {
        velocityRim = 1;
      }

      if (velocityRim > 127)
      {
        velocityRim = 127;
      }

      //if (piezoValueSUM > RimPiezoValueSUM)
      if (Rimjudge == false)
      {
        showVelocity = velocity;
        showLCD = true;
        hit = true;
        padIndex = padNum;
        loopTimes = 0;
        flag = true;
      }
      //else if (piezoValueSUM <= RimPiezoValueSUM)
      else if (Rimjudge == true)
      {
        velocity = velocityRim;
        showVelocity = velocity;
        showLCD = true;
        hitRim = true;
        padIndex = padNum;
        loopTimes = 0;
        flag = true;
      }
    }
  }

  if (flag == true)
  {
    time_end = millis();
    flag = false;
  }
}

void HelloDrum::HHMUX()
{

#ifdef ESP32
  int threshold1Raw = threshold1 * 40;
  int sensitivityRaw = sensitivity * 40;
#else
  int threshold1Raw = threshold1 * 10;
  int sensitivityRaw = sensitivity * 10;
#endif

  HHnum = padNum;
  hit = false;
  piezoValue = rawValue[pin_1];

  //when the value > threshold
  if (piezoValue > threshold1Raw && loopTimes == 0)
  {
    time_hit = millis(); //check the time pad hitted

    //compare time to cancel retrigger
    if (time_hit - time_end < masktime)
    {
      flag = true;
    }
    else
    {
      velocity = piezoValue; //first peak
      loopTimes = ++loopTimes;
      flag = false;
    }
  }

  //peak scan start
  if (flag == false && loopTimes > 0)
  {
    if (loopTimes != scantime)
    {
      if (piezoValue > velocity)
      {
        velocity = piezoValue;
      }
      loopTimes = ++loopTimes;
    }

    //scan end
    if (loopTimes == scantime)
    {
      velocity = map(velocity, threshold1Raw, sensitivityRaw, 1, 127);

      if (velocity <= 1)
      {
        velocity = 1;
      }

      if (velocity > 127)
      {
        velocity = 127;
      }

      //velo = (velo * velo) / 126 + 1;

      showVelocity = velocity;
      showLCD = true;
      hit = true;
      padIndex = padNum;
      loopTimes = 0;
      flag = true;
    }
  }

  if (flag == true)
  {
    time_end = millis();
    flag = false;
  }
}

void HelloDrum::HH2zoneMUX()
{

#ifdef ESP32
  int threshold1Raw = threshold1 * 40;
  int sensitivityRaw = sensitivity * 40;
  int edgeThreshold = 200;
#else
  int threshold1Raw = threshold1 * 10;
  int sensitivityRaw = sensitivity * 10;
  int edgeThreshold = 50;
#endif

  HHnum = padNum;
  hit = false;
  hitRim = false;
  hitCup = false;
  choke = false;

  piezoValue = rawValue[pin_1];
  sensorValue = rawValue[pin_2];

  //when the value > threshold
  if ((piezoValue > threshold1Raw && loopTimes == 0) || (sensorValue > edgeThreshold && loopTimes == 0))
  {
    time_hit = millis(); //check the time pad hitted

    if (time_hit - time_end < masktime)
    {
      flag = true;
    }
    else
    {
      velocity = abs(piezoValue - sensorValue); //first peak
      firstSensorValue = sensorValue;
      loopTimes = ++loopTimes;
      flag = false;
    }
  }

  //peak scan start
  if (flag == false && loopTimes > 0)
  {
    if (loopTimes != scantime)
    {
      if (abs(piezoValue - sensorValue) > velocity)
      {
        velocity = abs(piezoValue - sensorValue);
      }
      if (sensorValue > firstSensorValue && loopTimes < 5)
      {
        firstSensorValue = sensorValue;
      }
      loopTimes = ++loopTimes;
    }

    //scan end
    if (loopTimes == scantime)
    {
      lastSensorValue = sensorValue;
      velocity = map(velocity, threshold1Raw, sensitivityRaw, 1, 127);

      if (velocity <= 1)
      {
        velocity = 1;
      }

      if (velocity > 127)
      {
        velocity = 127;
      }

      //bow
      if (firstSensorValue < edgeThreshold && lastSensorValue < edgeThreshold)
      {
        showVelocity = velocity;
        showLCD = true;
        flag = true;
        hit = true;
        padIndex = padNum;
      }

      //edge
      else if (firstSensorValue > edgeThreshold && lastSensorValue < edgeThreshold)
      {
        showVelocity = velocity;
        showLCD = true;
        hitRim = true;
        flag = true;
        padIndex = padNum;
      }

      //choke
      else if (lastSensorValue > edgeThreshold && firstSensorValue > edgeThreshold)
      {
        choke = true;
        flag = true;
      }
      loopTimes = 0;
    }
  }

  if (flag == true)
  {
    time_end = millis();
    flag = false;
  }
}

////////////////////////////////////////////

void HelloDrum::cymbal2zoneMUX()
{

#ifdef ESP32
  int threshold1Raw = threshold1 * 40;
  int sensitivityRaw = sensitivity * 40;
  int edgeThreshold = 200;
#else
  int threshold1Raw = threshold1 * 10;
  int sensitivityRaw = sensitivity * 10;
  int edgeThreshold = 50;
#endif

  hit = false;
  hitRim = false;
  hitCup = false;
  choke = false;

  piezoValue = rawValue[pin_1];
  sensorValue = rawValue[pin_2];

  //when the value > threshold
  if ((piezoValue > threshold1Raw && loopTimes == 0) || (sensorValue > edgeThreshold && loopTimes == 0))
  {
    time_hit = millis(); //check the time pad hitted

    if (time_hit - time_end < masktime)
    {
      flag = true;
    }
    else
    {
      velocity = abs(piezoValue - sensorValue); //first peak
      firstSensorValue = sensorValue;
      loopTimes = ++loopTimes;
      flag = false;
    }
  }

  //peak scan start
  if (flag == false && loopTimes > 0)
  {
    if (loopTimes != scantime)
    {
      if (abs(piezoValue - sensorValue) > velocity)
      {
        velocity = abs(piezoValue - sensorValue);
      }
      if (sensorValue > firstSensorValue && loopTimes < 5)
      {
        firstSensorValue = sensorValue;
      }
      loopTimes = ++loopTimes;
    }

    //scan end
    if (loopTimes == scantime)
    {
      lastSensorValue = sensorValue;
      velocity = map(velocity, threshold1Raw, sensitivityRaw, 1, 127);

      if (velocity <= 1)
      {
        velocity = 1;
      }

      if (velocity > 127)
      {
        velocity = 127;
      }

      //bow
      if (firstSensorValue < edgeThreshold && lastSensorValue < edgeThreshold)
      {
        showVelocity = velocity;
        showLCD = true;
        flag = true;
        hit = true;
        padIndex = padNum;
      }

      //edge
      else if (firstSensorValue > edgeThreshold && lastSensorValue < edgeThreshold)
      {
        showVelocity = velocity;
        showLCD = true;
        hitRim = true;
        flag = true;
        padIndex = padNum;
      }

      //choke
      else if (lastSensorValue > edgeThreshold && firstSensorValue > edgeThreshold)
      {
        choke = true;
        flag = true;
      }
      loopTimes = 0;
    }
  }

  if (flag == true)
  {
    time_end = millis();
    flag = false;
  }
}

void HelloDrum::cymbal3zoneMUX()
{

#ifdef ESP32
  int threshold1Raw = threshold1 * 40;
  int sensitivityRaw = sensitivity * 40;
  int edgeThreshold = 200;
  int cupThreshold = 2000;
  int loopTimesCup = 25;
#else
  int threshold1Raw = threshold1 * 10;
  int sensitivityRaw = sensitivity * 10;
  int edgeThreshold = 50;
  int cupThreshold = 520;
  int loopTimesCup = 10; //?
#endif

  hit = false;
  hitRim = false;
  hitCup = false;
  choke = false;

  piezoValue = rawValue[pin_1];
  sensorValue = rawValue[pin_2];

  //when the value > threshold
  if ((piezoValue > threshold1Raw && loopTimes == 0) || (sensorValue > edgeThreshold && loopTimes == 0))
  {
    time_hit = millis(); //check the time pad hitted

    if (time_hit - time_end < masktime)
    {
      flag = true;
    }
    else
    {
      velocity = abs(piezoValue - sensorValue); //first peak
      firstSensorValue = sensorValue;
      loopTimes = ++loopTimes;
      flag = false;
    }
  }

  //peak scan start
  if (flag == false && loopTimes > 0)
  {
    if (loopTimes != scantime)
    {
      if (loopTimes == loopTimesCup)
      {
        velocityCup = 1;
      }
      if (abs(piezoValue - sensorValue) > velocity || abs(piezoValue - sensorValue) > velocityCup)
      {
        velocity = abs(piezoValue - sensorValue);
        velocityCup = velocity;
      }
      if (sensorValue > firstSensorValue && loopTimes < 5)
      {
        firstSensorValue = sensorValue;
      }
      loopTimes = ++loopTimes;
    }

    //scan end
    if (loopTimes == scantime)
    {
      lastSensorValue = sensorValue;
      velocity = map(velocity, threshold1Raw, sensitivityRaw, 1, 127);
      velocityCup = map(velocityCup, threshold1Raw, sensitivityRaw, 1, 127);

      if (velocity <= 1)
      {
        velocity = 1;
      }

      if (velocity > 127)
      {
        velocity = 127;
      }

      if (velocityCup <= 1)
      {
        velocityCup = 1;
      }

      if (velocityCup > 127)
      {
        velocityCup = 127;
      }

      //bow
      if (firstSensorValue < edgeThreshold && lastSensorValue < edgeThreshold)
      {
        showVelocity = velocity;
        showLCD = true;
        flag = true;
        hit = true;
        padIndex = padNum;
      }

      //edge
      else if (firstSensorValue > edgeThreshold && firstSensorValue < cupThreshold && lastSensorValue < edgeThreshold)
      {
        showVelocity = velocity;
        showLCD = true;
        hitRim = true;
        flag = true;
        padIndex = padNum;
      }

      //cup
      else if (firstSensorValue > cupThreshold && lastSensorValue < edgeThreshold)
      {
        velocity = velocityCup;
        showVelocity = velocity;
        showLCD = true;
        hitCup = true;
        flag = true;
        padIndex = padNum;
      }

      //choke
      else if (lastSensorValue > edgeThreshold && firstSensorValue > edgeThreshold)
      {
        choke = true;
        flag = true;
      }
      loopTimes = 0;
    }
  }

  if (flag == true)
  {
    time_end = millis();
    flag = false;
  }
}

void HelloDrum::TCRT5000MUX()
{
  int TCRT = rawValue[pin_1];

#ifdef ESP32
  int threshold1Raw = threshold1 * 40;
  int threshold2Raw = threshold2 * 40;
  int sensitivityRaw = sensitivity * 40;
  TCRT = 4096 - TCRT;
#else
  int threshold1Raw = threshold1 * 10;
  int threshold2Raw = threshold2 * 10;
  int sensitivityRaw = sensitivity * 10;
  TCRT = 1024 - TCRT;
#endif

  HHCnum = padNum;
  velocity = 0;
  openHH = false;
  closeHH = false;

  if (TCRT > threshold2Raw && closeHH == false && pedalVelocityFlag == false && pedalFlag == false)
  {
    time_hit_pedal_1 = millis();
    pedalVelocityFlag = true;
  }

  else if (TCRT > sensitivity && pedalFlag == false)
  {
    time_hit_pedal_2 = millis();

    velocity = time_hit_pedal_2 - time_hit_pedal_1;
    velocity = map(velocity, scantime * 100, 0, 1, 127);

    if (velocity <= 1)
    {
      velocity = 1;
    }

    if (velocity > 127)
    {
      velocity = 127;
    }

    closeHH = true;
    openHH = false;
    pedalFlag = true;
    pedalVelocityFlag = false;
  }

  if (TCRT < threshold2Raw && pedalFlag == true)
  {
    pedalFlag = false;
    closeHH = false;
    openHH = true;
  }

  /////////////////////// HIHAT PEDAL CC

  /* 
#ifdef __AVR__
  TCRT = map(TCRT, thre1Raw, sensRaw, 0, 127);

  if (TCRT > 127)
  {
    TCRT = 127;
  }
  if (TCRT < 0)
  {
    TCRT = 0;
  }
#endif
*/

  //#ifdef ESP32
  TCRT = map(TCRT, threshold1Raw, sensitivityRaw, 0, 127);
  //6段階に分ける

  if (TCRT < 20)
  {
    TCRT = 0;
  }

  else if (TCRT >= 20 && TCRT < 40)
  {
    TCRT = 20;
  }

  else if (TCRT >= 40 && TCRT < 60)
  {
    TCRT = 40;
  }

  else if (TCRT >= 60 && TCRT < 80)
  {
    TCRT = 60;
  }

  else if (TCRT >= 80 && TCRT < 100)
  {
    TCRT = 80;
  }

  else if (TCRT >= 100 && TCRT < 120)
  {
    TCRT = 100;
  }

  else if (TCRT >= 120)
  {
    TCRT = 127;
  }
  //#endif

  if (exTCRT != TCRT)
  {
    pedalCC = TCRT;
    moving = true;
    exTCRT = TCRT;
  }

  else
  {
    moving = false;
  }
}

void HelloDrum::FSRMUX()
{

#ifdef ESP32
  int threshold1Raw = threshold1 * 40;
  int threshold2Raw = threshold2 * 40;
  int sensitivityRaw = sensitivity * 40;
#else
  int threshold1Raw = threshold1 * 10;
  int threshold2Raw = threshold2 * 10;
  int sensitivityRaw = sensitivity * 10;
#endif

  HHCnum = padNum;
  velocity = 0;
  openHH = false;
  closeHH = false;
  int FSR = rawValue[pin_1];

  if (FSR > threshold2Raw && closeHH == false && pedalVelocityFlag == false && pedalFlag == false)
  {
    time_hit_pedal_1 = millis();
    pedalVelocityFlag = true;
  }

  else if (FSR > sensitivity && pedalFlag == false)
  {
    time_hit_pedal_2 = millis();

    velocity = time_hit_pedal_2 - time_hit_pedal_1;
    velocity = map(velocity, scantime * 100, 0, 1, 127);

    if (velocity <= 1)
    {
      velocity = 1;
    }

    if (velocity > 127)
    {
      velocity = 127;
    }

    closeHH = true;
    openHH = false;
    pedalFlag = true;
    pedalVelocityFlag = false;
  }

  if (FSR < threshold2Raw && pedalFlag == true)
  {
    pedalFlag = false;
    closeHH = false;
    openHH = true;
  }

  /////////////////////// HIHAT PEDAL CC

  /* 
#ifdef __AVR__
  FSR = map(FSR, thre1Raw, sensRaw, 0, 127);

  if (FSR > 127)
  {
    FSR = 127;
  }
  if (FSR < 0)
  {
    FSR = 0;
  }
#endif
*/

  //#ifdef ESP32
  FSR = map(FSR, threshold1Raw, sensitivityRaw, 0, 127);
  //6段階に分ける

  if (FSR < 20)
  {
    FSR = 0;
  }

  else if (FSR >= 20 && FSR < 40)
  {
    FSR = 20;
  }

  else if (FSR >= 40 && FSR < 60)
  {
    FSR = 40;
  }

  else if (FSR >= 60 && FSR < 80)
  {
    FSR = 60;
  }

  else if (FSR >= 80 && FSR < 100)
  {
    FSR = 80;
  }

  else if (FSR >= 100 && FSR < 120)
  {
    FSR = 100;
  }

  else if (FSR >= 120)
  {
    FSR = 127;
  }
  //#endif

  if (exFSR != FSR)
  {
    pedalCC = FSR;
    moving = true;
    exFSR = FSR;
  }

  else
  {
    moving = false;
  }
}

//////////////////////////// 6. EEPROM SETTING  //////////////////////////////

#ifdef ESP32

void HelloDrum::settingEnable()
{

  //When EDIT button pushed
  if (padNum == nameIndex)
  {

    //When UP button pushed
    if (button_up == LOW && buttonState == true && editCheck == true)
    {

      switch (itemNumber)
      {
      case 0:
        sensitivity = sensitivity + UP[itemNumber];
        if (sensitivity > 100)
        {
          sensitivity = 100;
        }
        EEPROM_ESP.write(padNum * 7, sensitivity);
        break;

      case 1:
        threshold1 = threshold1 + UP[itemNumber];
        if (threshold1 > 100)
        {
          threshold1 = 100;
        }
        EEPROM_ESP.write((padNum * 7) + 1, threshold1);
        break;

      case 2:
        scantime = scantime + UP[itemNumber];
        if (scantime > 250)
        {
          scantime = 250;
        }
        EEPROM_ESP.write((padNum * 7) + 2, scantime);
        break;

      case 3:
        masktime = masktime + UP[itemNumber];
        if (masktime > 250)
        {
          masktime = 250;
        }
        EEPROM_ESP.write((padNum * 7) + 3, masktime);
        break;

      case 4:
        note = note + UP[itemNumber];
        if (note > 127)
        {
          note = 127;
        }
        EEPROM_ESP.write((padNum * 7) + 4, note);
        noteOpen = note;
        break;

      case 5:
        noteRim = noteRim + UP[itemNumber];
        if (noteRim > 127)
        {
          noteRim = 127;
        }
        EEPROM_ESP.write((padNum * 7) + 5, noteRim);
        noteClose = noteRim;
        noteOpenEdge = noteRim;
        break;

      case 6:
        noteCup = noteCup + UP[itemNumber];
        if (noteCup > 127)
        {
          noteCup = 127;
        }
        EEPROM_ESP.write((padNum * 7) + 6, noteCup);
        noteCloseEdge = noteCup;
        break;
      }
      EEPROM_ESP.commit();
      change = true;
      push = true;
      buttonState = false;
      delay(30);
    }

    //When Down button pushed
    if (button_down == LOW && buttonState == true && editCheck == true)
    {

      switch (itemNumber)
      {
      case 0:
        sensitivity = sensitivity - UP[itemNumber];
        if (sensitivity < 1)
        {
          sensitivity = 1;
        }
        EEPROM_ESP.write(padNum * 7, sensitivity);
        break;

      case 1:
        threshold1 = threshold1 - UP[itemNumber];
        if (threshold1 < 1)
        {
          threshold1 = 1;
        }
        EEPROM_ESP.write((padNum * 7) + 1, threshold1);
        break;

      case 2:
        scantime = scantime - UP[itemNumber];
        if (scantime < 1)
        {
          scantime = 1;
        }
        EEPROM_ESP.write((padNum * 7) + 2, scantime);
        break;

      case 3:
        masktime = masktime - UP[itemNumber];
        if (masktime < 1)
        {
          masktime = 1;
        }
        EEPROM_ESP.write((padNum * 7) + 3, masktime);
        break;

      case 4:
        note = note - UP[itemNumber];
        if (note < 0)
        {
          note = 0;
        }
        EEPROM_ESP.write((padNum * 7) + 4, note);
        noteOpen = note;
        break;

      case 5:
        noteRim = noteRim - UP[itemNumber];
        if (noteRim < 0)
        {
          noteRim = 0;
        }
        EEPROM_ESP.write((padNum * 7) + 5, noteRim);
        noteClose = noteRim;
        noteOpenEdge = noteRim;
        break;

      case 6:
        noteCup = noteCup - UP[itemNumber];
        if (noteCup < 0)
        {
          noteCup = 0;
        }
        EEPROM_ESP.write((padNum * 7) + 6, noteCup);
        noteCloseEdge = noteCup;
        break;
      }
      EEPROM_ESP.commit();
      change = true;
      push = true;
      buttonState = false;
      delay(30);
    }

    if (itemNumber == 0)
    {
      value = sensitivity;
    }

    else if (itemNumber == 1)
    {
      value = threshold1;
    }

    else if (itemNumber == 2)
    {
      value = scantime;
    }

    else if (itemNumber == 3)
    {
      value = masktime;
    }

    else if (itemNumber == 4)
    {
      value = note;
    }

    else if (itemNumber == 5)
    {
      value = noteRim;
    }

    else if (itemNumber == 6)
    {
      value = noteCup;
    }

    showValue = value;
  }
}

#else

void HelloDrum::settingEnable()
{

  //When EDIT button pushed
  if (padNum == nameIndex)
  {

    //When UP button pushed
    if (button_up == LOW && buttonState == true && editCheck == true)
    {

      switch (itemNumber)
      {
      case 0:
        sensitivity = sensitivity + UP[itemNumber];
        if (sensitivity > 100)
        {
          sensitivity = 100;
        }
        EEPROM.write(padNum * 7, sensitivity);
        break;

      case 1:
        threshold1 = threshold1 + UP[itemNumber];
        if (threshold1 > 10)
        {
          threshold1 = 10;
        }
        EEPROM.write((padNum * 7) + 1, threshold1);
        break;

      case 2:
        scantime = scantime + UP[itemNumber];
        if (scantime > 250)
        {
          scantime = 250;
        }
        EEPROM.write((padNum * 7) + 2, scantime);
        break;

      case 3:
        masktime = masktime + UP[itemNumber];
        if (masktime > 250)
        {
          masktime = 250;
        }
        EEPROM.write((padNum * 7) + 3, masktime);
        break;

      case 4:
        note = note + UP[itemNumber];
        if (note > 127)
        {
          note = 127;
        }
        EEPROM.write((padNum * 7) + 4, note);
        noteOpen = note;
        break;

      case 5:
        noteRim = noteRim + UP[itemNumber];
        if (noteRim > 127)
        {
          noteRim = 127;
        }
        EEPROM.write((padNum * 7) + 5, noteRim);
        noteClose = noteRim;
        noteOpenEdge = noteRim;
        break;

      case 6:
        noteCup = noteCup + UP[itemNumber];
        if (noteCup > 127)
        {
          noteCup = 127;
        }
        EEPROM.write((padNum * 7) + 6, noteCup);
        noteCloseEdge = noteCup;
        break;
      }
      change = true;
      push = true;
      buttonState = false;
      delay(30);
    }

    //When Down button pushed
    if (button_down == LOW && buttonState == true && editCheck == true)
    {

      switch (itemNumber)
      {
      case 0:
        sensitivity = sensitivity - UP[itemNumber];
        if (sensitivity < 1)
        {
          sensitivity = 1;
        }
        EEPROM.write(padNum * 7, sensitivity);
        break;

      case 1:
        threshold1 = threshold1 - UP[itemNumber];
        if (threshold1 < 1)
        {
          threshold1 = 1;
        }
        EEPROM.write((padNum * 7) + 1, threshold1);
        break;

      case 2:
        scantime = scantime - UP[itemNumber];
        if (scantime < 1)
        {
          scantime = 1;
        }
        EEPROM.write((padNum * 7) + 2, scantime);
        break;

      case 3:
        masktime = masktime - UP[itemNumber];
        if (masktime < 1)
        {
          masktime = 1;
        }
        EEPROM.write((padNum * 7) + 3, masktime);
        break;

      case 4:
        note = note - UP[itemNumber];
        if (note < 0)
        {
          note = 0;
        }
        EEPROM.write((padNum * 7) + 4, note);
        noteOpen = note;
        break;

      case 5:
        noteRim = noteRim - UP[itemNumber];
        if (noteRim < 0)
        {
          noteRim = 0;
        }
        EEPROM.write((padNum * 7) + 5, noteRim);
        noteClose = noteRim;
        noteOpenEdge = noteRim;
        break;

      case 6:
        noteCup = noteCup - UP[itemNumber];
        if (noteCup < 0)
        {
          noteCup = 0;
        }
        EEPROM.write((padNum * 7) + 6, noteCup);
        noteCloseEdge = noteCup;
        break;
      }
      change = true;
      push = true;
      buttonState = false;
      delay(30);
    }

    if (itemNumber == 0)
    {
      value = sensitivity;
    }

    else if (itemNumber == 1)
    {
      value = threshold1;
    }

    else if (itemNumber == 2)
    {
      value = scantime;
    }

    else if (itemNumber == 3)
    {
      value = masktime;
    }

    else if (itemNumber == 4)
    {
      value = note;
    }

    else if (itemNumber == 5)
    {
      value = noteRim;
    }

    else if (itemNumber == 6)
    {
      value = noteCup;
    }

    showValue = value;
  }
}
#endif

void HelloDrum::settingName(char *instrumentName)
{
  //Store the name of the pad in the array.
  showInstrument[nameIndex] = instrumentName;
  nameIndexMax = nameIndex;
  nameIndex = ++nameIndex;
}

#ifdef ESP32

void HelloDrum::loadMemory()
{
  //EEPROM_ESP.begin(1024);
  //どっかで一回入れる必要がある。

  //Read values from EEPROM.
  sensitivity = EEPROM_ESP.read((padNum * 7));
  threshold1 = EEPROM_ESP.read((padNum * 7) + 1);
  scantime = EEPROM_ESP.read((padNum * 7) + 2);
  masktime = EEPROM_ESP.read((padNum * 7) + 3);
  note = EEPROM_ESP.read((padNum * 7) + 4);
  noteOpen = EEPROM_ESP.read((padNum * 7) + 4);
  noteRim = EEPROM_ESP.read((padNum * 7) + 5);
  noteClose = EEPROM_ESP.read((padNum * 7) + 5);
  noteOpenEdge = EEPROM_ESP.read((padNum * 7) + 5);
  noteCup = EEPROM_ESP.read((padNum * 7) + 6);
  noteCloseEdge = EEPROM_ESP.read((padNum * 7) + 6);
  /*
  EEPROM_ESP.get((padNum * 7), sensitivity);
  EEPROM_ESP.get((padNum * 7) + 1, threshold1);
  EEPROM_ESP.get((padNum * 7) + 2, scantime);
  EEPROM_ESP.get((padNum * 7) + 3, masktime);
  EEPROM_ESP.get((padNum * 7) + 4, note);
  EEPROM_ESP.get((padNum * 7) + 4, noteOpen);
  EEPROM_ESP.get((padNum * 7) + 5, noteRim);
  EEPROM_ESP.get((padNum * 7) + 5, noteClose);
  EEPROM_ESP.get((padNum * 7) + 5, noteOpenEdge);
  EEPROM_ESP.get((padNum * 7) + 6, noteCup);
  EEPROM_ESP.get((padNum * 7) + 6, noteCloseEdge);
   */
}

void HelloDrum::initMemory()
{
  //Write initial value to EEPROM.
  EEPROM_ESP.write(padNum * 7, sensitivity);
  EEPROM_ESP.write((padNum * 7) + 1, threshold1);
  EEPROM_ESP.write((padNum * 7) + 2, scantime);
  EEPROM_ESP.write((padNum * 7) + 3, masktime);
  EEPROM_ESP.write((padNum * 7) + 4, note);
  EEPROM_ESP.write((padNum * 7) + 5, noteRim);
  EEPROM_ESP.write((padNum * 7) + 6, noteCup);
  EEPROM_ESP.commit();
}

#else

void HelloDrum::loadMemory()
{
  //Read values from EEPROM.
  sensitivity = EEPROM.read(padNum * 7);
  threshold1 = EEPROM.read((padNum * 7) + 1);
  scantime = EEPROM.read((padNum * 7) + 2);
  masktime = EEPROM.read((padNum * 7) + 3);
  note = EEPROM.read((padNum * 7) + 4);
  noteOpen = EEPROM.read((padNum * 7) + 4);
  noteRim = EEPROM.read((padNum * 7) + 5);
  noteClose = EEPROM.read((padNum * 7) + 5);
  noteOpenEdge = EEPROM.read((padNum * 7) + 5);
  noteCup = EEPROM.read((padNum * 7) + 6);
  noteCloseEdge = EEPROM.read((padNum * 7) + 6);
}

void HelloDrum::initMemory()
{
  //Write initial value to EEPROM.
  EEPROM.write(padNum * 7, sensitivity);
  EEPROM.write((padNum * 7) + 1, threshold1);
  EEPROM.write((padNum * 7) + 2, scantime);
  EEPROM.write((padNum * 7) + 3, masktime);
  EEPROM.write((padNum * 7) + 4, note);
  EEPROM.write((padNum * 7) + 5, noteRim);
  EEPROM.write((padNum * 7) + 6, noteCup);
}

#endif

///////////////////// 7. BUTONN //////////////////////////

void HelloDrumButton::readButtonState()
{

  pinMode(pin_1, INPUT_PULLUP);
  pinMode(pin_2, INPUT_PULLUP);
  pinMode(pin_3, INPUT_PULLUP);
  pinMode(pin_4, INPUT_PULLUP);
  pinMode(pin_5, INPUT_PULLUP);

  button_set = digitalRead(pin_1);
  button_up = digitalRead(pin_2);
  button_down = digitalRead(pin_3);
  button_next = digitalRead(pin_4);
  button_back = digitalRead(pin_5);

  ////////////////////////////// EDIT START////////////////////////////////

  if (button_set == LOW && buttonState == true && editCheck == false)
  {
    editCheck = true;
    edit = true;
    buttonState = false;
    buttonState_set = false;
    delay(30);
  }

  if (button_set == LOW && buttonState == true && editCheck == true)
  {
    editCheck = false;
    editdone = true;
    buttonState = false;
    buttonState_set = true;
    delay(30);
  }

  if (button_set == HIGH)
  {
    edit = false;
    editdone = false;
  }

  /////////////////////////// UP DOWN ///////////////////////////////////////

  if (button_up == LOW && buttonState == true && editCheck == false)
  {
    UPDOWN = ++UPDOWN;

    if (UPDOWN < 0)
    {
      UPDOWN = nameIndexMax;
    }
    if (UPDOWN > nameIndexMax)
    {
      UPDOWN = 0;
    }

    nameIndex = UPDOWN;
    push = true;
    buttonState = false;
    delay(30);
  }

  if (button_down == LOW && buttonState == true && editCheck == false)
  {
    UPDOWN = --UPDOWN;

    if (UPDOWN < 0)
    {
      UPDOWN = nameIndexMax;
    }
    if (UPDOWN > nameIndexMax)
    {
      UPDOWN = 0;
    }
    nameIndex = UPDOWN;
    push = true;
    buttonState = false;
    delay(30);
  }

  ///////////////////////////// NEXT BACK ////////////////////////////////

  if (button_next == LOW && buttonState == true && editCheck == false)
  {
    NEXTBACK = ++NEXTBACK;

    if (NEXTBACK < 0)
    {
      NEXTBACK = 6;
    }
    if (NEXTBACK > 6)
    {
      NEXTBACK = 0;
    }
    itemNumber = NEXTBACK;
    nameIndex = UPDOWN;
    push = true;
    buttonState = false;
    delay(30);
  }

  if (button_back == LOW && buttonState == true && editCheck == false)
  {
    NEXTBACK = --NEXTBACK;

    if (NEXTBACK < 0)
    {
      NEXTBACK = 6;
    }
    if (NEXTBACK > 6)
    {
      NEXTBACK = 0;
    }
    itemNumber = NEXTBACK;
    nameIndex = UPDOWN;
    push = true;
    buttonState = false;
    delay(30);
  }

  //When you take your hand off the button
  if (buttonState == false && button_up == HIGH && button_down == HIGH && button_next == HIGH && button_back == HIGH && button_set == HIGH)
  {
    push = false;
    buttonState = true;
    change = false;
  }
}

/////////////////////// 5. LCD  //////////////////////////
/*
void HelloDrumLCD::show()
{

  //Button Push
  if (push == true)
  {
    lcd.clear();
    lcd.print(showInstrument[nameIndex]);
    lcd.setCursor(0, 1);
    if (nameIndex == HHCnum)
    {
      lcd.print(itemHHC[itemNumber]);
      lcd.setCursor(13, 1);
      lcd.print(showValue);
    }

    else if (nameIndex == HHnum)
    {
      lcd.print(itemHH[itemNumber]);
      lcd.setCursor(13, 1);
      lcd.print(showValue);
    }

    else
    {
      lcd.print(item[itemNumber]);
      lcd.setCursor(13, 1);
      lcd.print(showValue);
    }
  }

  //Pad hit
  if (showLCD == true)
  {
    lcd.clear();
    lcd.print(showInstrument[padIndex]);
    lcd.setCursor(0, 1);
    lcd.print(showVelocity);
    showLCD = false;
  }

  //EDIT
  if (edit == true)
  {
    lcd.clear();
    lcd.print("EDIT");
    delay(500);
    lcd.clear();
    lcd.print(showInstrument[nameIndex]);
    lcd.setCursor(0, 1);
    lcd.print(item[itemNumber]);
    lcd.setCursor(13, 1);
    lcd.print(showValue);
  }

  //EDIT DONE
  if (editdone == true)
  {
    lcd.clear();
    lcd.print("EDIT DONE");
    delay(500);
    lcd.clear();
    lcd.print(showInstrument[nameIndex]);
    lcd.setCursor(0, 1);
    lcd.print(item[itemNumber]);
    lcd.setCursor(13, 1);
    lcd.print(showValue);
  }
}
*/

//////////////////////  KNOB  /////////////////////////

void HelloDrumKnob::read()
{
  knobValue = analogRead(pin_1);
}

void HelloDrumKnob::readMUX()
{
  knobValue = rawValue[pin_1];
}

//////////////////////////////////////////////////////////

//For Display

int HelloDrumButton::GetVelocity()
{
  return showVelocity;
}
int HelloDrumButton::GetSettingValue()
{
  return showValue;
}
bool HelloDrumButton::GetEditState()
{
  return edit;
}
bool HelloDrumButton::GetDisplayState()
{
  if (showLCD == true)
  {
    showLCD = false;
    showFlag = true;
  }
  else
  {
    showFlag = false;
  }
  return showFlag;
}
int HelloDrumButton::GetEditdoneState()
{
  return editdone;
}
bool HelloDrumButton::GetPushState()
{
  return push;
}
char *HelloDrumButton::GetPadName()
{
  return showInstrument[nameIndex];
}
char *HelloDrumButton::GetSettingItem()
{
  if (nameIndex == HHCnum)
  {
    return itemHHC[itemNumber];
  }
  else if (nameIndex == HHnum)
  {
    return itemHH[itemNumber];
  }
  else
  {
    return item[itemNumber];
  }
}
char *HelloDrumButton::GetHitPad()
{
  return showInstrument[padIndex];
}

/*
int HelloDrumButton::GetItemNumber()
{
  return itemNumber;
}
int HelloDrumButton::GetChangeState()
{
  return change;
}
 */