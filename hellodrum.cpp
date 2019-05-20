/*
  " HELLO DRUM LIBRARY" Ver.0.5

  Ver.0.5 - Created May 19, 2018
  Ver.0.6 - March 5, 2019
  by Ryo Kosaka

  https://open-e-drums.tumblr.com/
*/

#include "hellodrum.h"
#include "Arduino.h"
#include "LiquidCrystal.h"

#ifdef __AVR__
#include "EEPROM.h"
#endif

#ifdef ESP32
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
  threshold1 = 20;
  scantime = 5;
  masktime = 10;
  sensitivity = 80;

  //Give the instance a pad number.
  padNum = padIndex;
  padIndex = ++padIndex;
}

//Pad with a sensor.
HelloDrum::HelloDrum(int pin1)
{
  pin_1 = pin1;

  //initial EEPROM value
  note = 38;
  noteRim = 39;
  noteCup = 40;
  threshold1 = 20;
  scantime = 5;
  masktime = 10;
  sensitivity = 80;

  //Give the instance a pad number.
  padNum = padIndex;
  padIndex = ++padIndex;
}

HelloDrumButton::HelloDrumButton(int pin1, int pin2, int pin3, int pin4, int pin5)
{
  pin_1 = pin1; //EDIT
  pin_2 = pin2; //UP
  pin_3 = pin3; //DOWN
  pin_4 = pin4; //NEXT
  pin_5 = pin5; //BACK
}

HelloDrumLCD::HelloDrumLCD(int pin1, int pin2, int pin3, int pin4, int pin5, int pin6)
{
  pin_1 = pin1; //RS
  pin_2 = pin2; //Enable
  pin_3 = pin3; //D4
  pin_4 = pin4; //D5
  pin_5 = pin5; //D6
  pin_6 = pin6; //D7

  lcd = LiquidCrystal(pin_1, pin_2, pin_3, pin_4, pin_5, pin_6);
  lcd.begin(16,2);
  lcd.clear();
  lcd.print("hello, world!");
  lcd.setCursor(0, 1);
  lcd.print("hello, drum!");
}


///////////////////// 1. SENSING without EEPROM //////////////////////////

void HelloDrum::singlePiezo(int sens, int thre1, int scan, int mask) {

  int velo = 0;
  hit = false;
  piezoValue = analogRead(pin_1);
  //static float y[2] = {0};
  //y[1] = 0.8 * y[0] + 0.2 * analogRead(pin_1);
  //piezoValue = y[1];


  if (analogRead(pin_1) > thre1) {
    time_hit = millis(); //record hit time

    if (time_hit - time_end < mask) { //compare last hit time
      flag = true; 
    }

    //        if (piezoValue - exValue > thre2) {
    //          flag = false;
    //        }

    if (flag == false) {

      //peak scan
      for (int i = 0; i < scan; i++) {
        int peak = analogRead(pin_1);
        if (peak > velo) {
          velo = peak;
        }
        while (millis() - time_hit < 1);
      }

      //mapping
      velo = map(velo, thre1, sens, 1, 127);

      if (velo <= 1) {
        velo = 1;
      }

      if (velo > 127) {
        velo = 127;
      }

      //velo = (velo * velo) / 126 + 1;

      velocity = velo;
      flag = true;
      hit = true;
    }
  }

  if (flag == true) {
    time_end = millis();
    exValue = analogRead(pin_1);
    flag = false;
  }
}

void HelloDrum::dualPiezo(int sens, int thre1, int scan, int mask) {

  int velo = 0;
  int veloRim = 0;
  hit = false;
  hitRim = false;
  piezoValue = analogRead(pin_1);
  RimPiezoValue = analogRead(pin_2);

  if (analogRead(pin_1) > thre1) {
    time_hit = millis();

    if (time_hit - time_end < mask) {
      flag = true;
    }

    //        if (piezoValue - exValue > thre2) {
    //          flag = false;
    //        }

    if (flag == false) {

    for (int i = 0; i < scan; i++) {
      int peak = analogRead(pin_1);
      int peakRim = analogRead(pin_2);

      if (peak > velo) {
        velo = peak;
      }
      if (peakRim > veloRim) {
        veloRim = peakRim;
      }
      while (millis() - time_hit < 1);
    }

    if(veloRim > velo){
      veloRim = map(veloRim, thre1, sens, 1, 127);

      if (veloRim <= 1) {
        veloRim = 1;
      }

      if (veloRim > 127) {
        veloRim = 127;
      }

      velocity = veloRim;
      flag = true;
      hitRim = true;

    }

    else if(veloRim <= velo){

    velo = map(velo, thre1, sens, 1, 127);

    if (velo <= 1) {
      velo = 1;
    }

    if (velo > 127) {
      velo = 127;
    }

      //velo = (velo * velo) / 126 + 1;

      velocity = velo;
      flag = true;
      hit = true;
    }

  }

  }

  if (flag == true) {
    time_end = millis();
    exValue = analogRead(pin_1);
    flag = false;
  }
}
void HelloDrum::HH(int sens, int thre1, int scan, int mask) {

  int velo = 0;
  hit = false;
  piezoValue = analogRead(pin_1);
  //static float y[2] = {0};
  //y[1] = 0.8 * y[0] + 0.2 * analogRead(pin_1);
  //piezoValue = y[1];


  if (analogRead(pin_1) > thre1) {
    time_hit = millis(); //record hit time

    if (time_hit - time_end < mask) { //compare last hit time
      flag = true; 
    }

    //        if (piezoValue - exValue > thre2) {
    //          flag = false;
    //        }

    if (flag == false) {

      //peak scan
      for (int i = 0; i < scan; i++) {
        int peak = analogRead(pin_1);
        if (peak > velo) {
          velo = peak;
        }
        while (millis() - time_hit < 1);
      }

      //mapping
      velo = map(velo, thre1, sens, 1, 127);

      if (velo <= 1) {
        velo = 1;
      }

      if (velo > 127) {
        velo = 127;
      }

      //velo = (velo * velo) / 126 + 1;

      velocity = velo;
      flag = true;
      hit = true;
    }
  }

  if (flag == true) {
    time_end = millis();
    exValue = analogRead(pin_1);
    flag = false;
  }
}

////////////////////////////////////////////
void HelloDrum::HH2zone(int sens, int thre1, int scan, int mask) {

  int velo = 0;
  hit = false;
  hitRim = false;
  choke = false;
  int piezoValue = analogRead(pin_1);
  int sensorValue = analogRead(pin_2);

  //入り口でエッヂとボウ分けるか？
  //  if (abs(analogRead(pin_1) - analogRead(pin_2)) > thre1) {
  if (analogRead(pin_2) > thre1 || analogRead(pin_1) > thre1) {
    time_hit = millis();

    if (time_hit - time_end < mask) { //retrigger cancel
      flag = true;
    }

    //    if (analogRead(pin_1) - exValue > thre2) {
    //      flag = false;
    //    }

    if (flag == false) {
      for (int i = 0; i < scan; i++) {  //peak scan
        int peak = analogRead(pin_1);
        if (peak > velo) {
          velo = peak;
        }
        while (millis() - time_hit < 1);
      }

      //bow
      if (sensorValue < 50) {
        velo = map(velo, thre1, sens , 1, 127);

        if (velo <= 1) {
          velo = 1;
        }

        if (velo > 127) {
          velo = 127;
        }

        velocity = velo;
        flag = true;
        hit = true;
      }

      //edge
      else if (sensorValue > 500) {
        if (velo > 500) {
          velo = velo - 500;
        }
        else {
          velo = 500 - velo;
        }
        velo = map(velo, thre1, sens, 1, 127);

        if (velo <= 1) {
          velo = 1;
        }

        if (velo > 127) {
          velo = 127;
        }

        velocity = velo;
        flag = true;
        hitRim = true;
      }
    }
  }

  if (flag == true && piezoValue < thre1 && sensorValue < 10) {
    time_end = millis();
    flag = false;
  }

  if (chokeFlag == false && sensorValue > 500 && sensorValue < 900  && abs(piezoValue - sensorValue) < 10) {
    time_choke = millis();
    if (time_choke - time_end < 10) {
      chokeFlag = true;

    }
    else {
      choke = true;
      chokeFlag = true;
    }
  }

  if (flag == true &&  sensorValue < 10) {
    time_end = millis();
    chokeFlag = false;
  }
}


void HelloDrum::cymbal2zone(int sens, int thre1, int scan, int mask) {

  int velo = 0;
  hit = false;
  hitRim = false;
  choke = false;
  int piezoValue = analogRead(pin_1);
  int sensorValue = analogRead(pin_2);

  //入り口でエッヂとボウ分けるか？
  //  if (abs(analogRead(pin_1) - analogRead(pin_2)) > thre1) {
  if (analogRead(pin_2) > thre1 || analogRead(pin_1) > thre1) {
    time_hit = millis();

    if (time_hit - time_end < mask) { //retrigger cancel
      flag = true;
    }

    //    if (analogRead(pin_1) - exValue > thre2) {
    //      flag = false;
    //    }

    if (flag == false) {
      for (int i = 0; i < scan; i++) {  //peak scan
        int peak = analogRead(pin_1);
        if (peak > velo) {
          velo = peak;
        }
        while (millis() - time_hit < 1);
      }

      //bow
      if (sensorValue < 50) {
        velo = map(velo, thre1, sens , 1, 127);

        if (velo <= 1) {
          velo = 1;
        }

        if (velo > 127) {
          velo = 127;
        }

        velocity = velo;
        flag = true;
        hit = true;
      }

      //edge
      else if (sensorValue > 500) {
        if (velo > 500) {
          velo = velo - 500;
        }
        else {
          velo = 500 - velo;
        }
        velo = map(velo, thre1, sens, 1, 127);

        if (velo <= 1) {
          velo = 1;
        }

        if (velo > 127) {
          velo = 127;
        }

        velocity = velo;
        flag = true;
        hitRim = true;
      }
    }
  }

  if (flag == true && piezoValue < thre1 && sensorValue < 10) {
    time_end = millis();
    flag = false;
  }

  if (chokeFlag == false && sensorValue > 500 && sensorValue < 900  && abs(piezoValue - sensorValue) < 10) {
    time_choke = millis();
    if (time_choke - time_end < 10) {
      chokeFlag = true;

    }
    else {
      choke = true;
      chokeFlag = true;
    }
  }

  if (flag == true &&  sensorValue < 10) {
    time_end = millis();
    chokeFlag = false;
  }
}

void HelloDrum::cymbal3zone(int sens, int thre1, int scan, int mask) {

  int velo = 0;
  hit = false;
  hitRim = false;
  hitCup = false;
  choke = false;

  int piezoValue = analogRead(pin_1);
  int sensorValue = analogRead(pin_2);

  //入り口でエッヂとボウ分けるか？
  //  if (abs(analogRead(pin_1) - analogRead(pin_2)) > thre1) {
  if (analogRead(pin_2) > thre1 || analogRead(pin_1) > thre1) {
    time_hit = millis();

    if (time_hit - time_end < mask) { //retrigger cancel
      flag = true;
    }

    //    if (analogRead(pin_1) - exValue > thre2) {
    //      flag = false;
    //    }

    if (flag == false) {
      for (int i = 0; i < scan; i++) {  //peak scan
        int peak = analogRead(pin_1);
        if (peak > velo) {
          velo = peak;
        }
        while (millis() - time_hit < 1);
      }

      //bow
      if (sensorValue < 50) {
        velo = map(velo, thre1, sens , 1, 127);

        if (velo <= 1) {
          velo = 1;
        }

        if (velo > 127) {
          velo = 127;
        }

        velocity = velo;
        flag = true;
        hit = true;
      }

      //edge
      else if (sensorValue > 500 && sensorValue < 520) {
        if (velo > 500) {
          velo = velo - 500;
        }
        else {
          velo = 500 - velo;
        }
        velo = map(velo, thre1, sens, 1, 127);

        if (velo <= 1) {
          velo = 1;
        }

        if (velo > 127) {
          velo = 127;
        }

        velocity = velo;
        hitRim = true;
        flag = true;
      }

      //cup
      else if (sensorValue > 520) {
        //MIDI.sendNoteOn(53, 100, 10);   //(note, velocity, channel) cup note is 53
        //MIDI.sendNoteOff(53, 0, 10);
        velocity = 100;
        hitCup = true;
        flag = true;
      }
    }
  }

  if (flag == true && piezoValue < thre1 && sensorValue < 10) {
    time_end = millis();
    flag = false;
  }

  if (chokeFlag == false && sensorValue > 500 && sensorValue < 900  && abs(piezoValue - sensorValue) < 10) {
    time_choke = millis();
    if (time_choke - time_end < 10) {
      chokeFlag = true;

    }
    else {
      choke = true;
      chokeFlag = true;
    }
  }

  if (flag == true &&  sensorValue < 10) {
    time_end = millis();
    chokeFlag = false;
  }
}

void HelloDrum::TCRT5000(int sens, int thre1, int thre2, int scan) {

  int velo = 0;
  openHH = false;
  closeHH = false;
  int TCRT = analogRead(pin_1);

  if (TCRT < thre1 && closeHH == false && pedalVelocityFlag == false && pedalFlag == false) {
    time_hit_pedal_1 = millis();
    pedalVelocityFlag = true;
  }

  if (TCRT < thre2 && pedalFlag == false) {
    time_hit_pedal_2 = millis();

    velo = time_hit_pedal_2 - time_hit_pedal_1;
    velo = map(velo, scan, 0, 1, 127);

    if (velo <= 1) {
      velo = 1;
    }

    if (velo > 127) {
      velo = 127;
    }

    velocity = velo;
    closeHH = true;
    openHH = false;
    pedalFlag = true;
    pedalVelocityFlag = false;
  }


  if (TCRT > thre1 && closeHH == true) {
    closeHH = false;
    openHH = true;
  }

  /////////////////////// HIHAT PEDAL CC
  #ifdef _AVR_
  TCRT = map(TCRT, sens, 100, 0, 127);
  
  if (TCRT > 127) {
    TCRT = 127;
  }
  if (TCRT < 0) {
    TCRT = 0;
  }
  #endif

  #ifdef ESP32
  TCRT = map(TCRT, 4000, 100, 0, 127);
  //6段階に分ける

  if(TCRT < 20){
    TCRT = 0;
  }

  else if(TCRT >= 20 && TCRT < 40){
    TCRT = 20;
  }

  else if(TCRT >= 40 && TCRT < 60){
    TCRT = 40;
  }

  else if(TCRT >= 60 && TCRT < 80){
    TCRT = 60;
  }

  else if(TCRT >= 80 && TCRT < 100){
    TCRT = 80;
  }

  else if(TCRT >= 100 && TCRT < 120){
    TCRT = 100;
  }

  else if (TCRT >= 120) {
    TCRT = 127;
  }

  #endif

  if (exTCRT != TCRT) {
    pedalCC = TCRT;
    moving = true;
    exTCRT = TCRT;
  }

  else {
    moving = false;
  }
}

//with EEPROM と比較！
void HelloDrum::FSR(int sens, int thre1, int thre2, int scan) {

  int velo = 0;
  openHH = false;
  closeHH = false;
  int FSR = analogRead(pin_1);

  if (FSR < thre1 && closeHH == false && pedalVelocityFlag == false && pedalFlag == false) {
    time_hit_pedal_1 = millis();
    pedalVelocityFlag = true;
  }

  if (FSR < thre2 && pedalFlag == false) {
    time_hit_pedal_2 = millis();

    velo = time_hit_pedal_2 - time_hit_pedal_1;
    velo = map(velo, scan, 0, 1, 127);

    if (velo <= 1) {
      velo = 1;
    }

    if (velo > 127) {
      velo = 127;
    }

    velocity = velo;
    closeHH = true;
    openHH = false;
    pedalFlag = true;
    pedalVelocityFlag = false;
  }


  if (FSR > thre1 && closeHH == true) {
    closeHH = false;
    openHH = true;
  }

  /////////////////////// HIHAT PEDAL CC

  FSR = map(FSR, sens, 100, 0, 127);
  if (FSR > 127) {
    FSR = 127;
  }
  if (FSR < 0) {
    FSR = 0;
  }

  if (exFSR != FSR) {
    pedalCC = FSR;
    moving = true;
    exFSR = FSR;
  }

  else {
    moving = false;
  }
}

////////////////// 2. SENSING WITH LCD & EEPROM //////////////////////

void HelloDrum::singlePiezo() {
  int velo = 0;
  hit = false;
  piezoValue = analogRead(pin_1);

//when the value > threshold
  if (analogRead(pin_1) > threshold1) {
    time_hit = millis(); //check the time pad hitted

    //compare time to cancel retrigger
    if (time_hit - time_end < masktime) {
      flag = true;
    }

    if (flag == false) {
      for (int i = 0; i < scantime; i++) {
        int peak = analogRead(pin_1);
        if (peak > velo) {
          velo = peak; //peak is max value of 5 times scan.
        }
        while (millis() - time_hit < 1);
        //delay 1 milisecond.
        //scan the piezo value 5 times.
      }

      velo = map(velo, threshold1, sensitivity * 10, 1, 127);

      if (velo <= 1) {
        velo = 1;
      }

      if (velo > 127) {
        velo = 127;
      }

      //velo = (velo * velo) / 126 + 1;


      velocity = velo;
      showVelocity = velocity;
      flag = true; //ここの位置を検証すべき。showLCDとhitの下だと何か変わるか。
      showLCD = true;
      hit = true;
      padIndex = padNum;
    }
  }

  //scantime伸びてるのでは？？？
  if (flag == true) {
    time_end = millis();
    exValue = analogRead(pin_1);
    flag = false;
  }
}

void HelloDrum::dualPiezo() {

  int velo = 0;
  int veloRim = 0;
  hit = false;
  hitRim = false;
  piezoValue = analogRead(pin_1);
  RimPiezoValue = analogRead(pin_2);

  if (analogRead(pin_1) > threshold1) {
    time_hit = millis();

    if (time_hit - time_end < masktime) {
      flag = true;
    }

    //        if (piezoValue - exValue > thre2) {
    //          flag = false;
    //        }

    if (flag == false) {

    for (int i = 0; i < scantime; i++) {
      int peak = analogRead(pin_1);
      int peakRim = analogRead(pin_2);

      if (peak > velo) {
        velo = peak;
      }
      if (peakRim > veloRim) {
        veloRim = peakRim;
      }
      while (millis() - time_hit < 1);
    }

    if(veloRim > velo){
      veloRim = map(veloRim, threshold1, sensitivity*10, 1, 127);

      if (veloRim <= 1) {
        veloRim = 1;
      }

      if (veloRim > 127) {
        veloRim = 127;
      }

      velocity = veloRim;
      showVelocity = velocity;
      flag = true;
      showLCD = true;
      hitRim = true;
      padIndex = padNum;

    }

    else if(veloRim <= velo){

    velo = map(velo, threshold1, sensitivity*10, 1, 127);

    if (velo <= 1) {
      velo = 1;
    }

    if (velo > 127) {
      velo = 127;
    }

      //velo = (velo * velo) / 126 + 1;

      velocity = velo;
      showVelocity = velocity;
      flag = true;
      showLCD = true;
      hit = true;
      padIndex = padNum;
    }

  }

  }

  if (flag == true) {
    time_end = millis();
    exValue = analogRead(pin_1);
    flag = false;
  }
}

void HelloDrum::HH() {
  HHnum = padNum;
  int velo = 0;
  hit = false;
  piezoValue = analogRead(pin_1);

//when the value > threshold
  if (analogRead(pin_1) > threshold1) {
    time_hit = millis(); //check the time pad hitted

    //compare time to cancel retrigger
    if (time_hit - time_end < masktime) {
      flag = true;
    }

    if (flag == false) {
      for (int i = 0; i < scantime; i++) {
        int peak = analogRead(pin_1);
        if (peak > velo) {
          velo = peak; //peak is max value of 5 times scan.
        }
        while (millis() - time_hit < 1);
        //delay 1 milisecond.
        //scan the piezo value 5 times.
      }

      velo = map(velo, threshold1, sensitivity * 10, 1, 127);

      if (velo <= 1) {
        velo = 1;
      }

      if (velo > 127) {
        velo = 127;
      }

      //velo = (velo * velo) / 126 + 1;


      velocity = velo;
      showVelocity = velocity;
      flag = true; //ここの位置を検証すべき。showLCDとhitの下だと何か変わるか。
      showLCD = true;
      hit = true;
      padIndex = padNum;
    }
  }

  //scantime伸びてるのでは？？？
  if (flag == true) {
    time_end = millis();
    exValue = analogRead(pin_1);
    flag = false;
  }
}

void HelloDrum::HH2zone() {

  HHnum = padNum;
  int velo = 0;
  hit = false;
  hitRim = false;
  choke = false;
  int piezoValue = analogRead(pin_1);
  int sensorValue = analogRead(pin_2);

  //入り口でエッヂとボウ分けるか？
  //  if (abs(analogRead(pin_1) - analogRead(pin_2)) > thre1) {
  if (analogRead(pin_2) > threshold1 || analogRead(pin_1) > threshold1) {
    time_hit = millis();

    if (time_hit - time_end < masktime) { //retrigger cancel
      flag = true;
    }

    //    if (analogRead(pin_1) - exValue > thre2) {
    //      flag = false;
    //    }

    if (flag == false) {
      for (int i = 0; i < scantime; i++) {  //peak scan
        int peak = analogRead(pin_1);
        if (peak > velo) {
          velo = peak;
        }
        while (millis() - time_hit < 1);
      }

      //bow
      if (sensorValue < 50) {
        velo = map(velo, threshold1, sensitivity , 1, 127);

        if (velo <= 1) {
          velo = 1;
        }

        if (velo > 127) {
          velo = 127;
        }

        velocity = velo;
        showVelocity = velocity;
        flag = true;
        showLCD = true;
        hit = true;
        padIndex = padNum;
      }

      //edge
      else if (sensorValue > 50) {
        if (velo > 500) {
          velo = velo - 500;
        }
        else {
          velo = 500 - velo;
        }
        velo = map(velo, threshold1, sensitivity, 1, 127);

        if (velo <= 1) {
          velo = 1;
        }

        if (velo > 127) {
          velo = 127;
        }

        velocity = velo;
        showVelocity = velocity;
        flag = true;
        showLCD = true;
        hitRim = true;
        padIndex = padNum;
      }
    }
  }

  if (flag == true && piezoValue < threshold1 && sensorValue < 10) {
    time_end = millis();
    flag = false;
  }

  if (chokeFlag == false && sensorValue > 500 && sensorValue < 900  && abs(piezoValue - sensorValue) < 10) {
    time_choke = millis();
    if (time_choke - time_end < 10) {
      chokeFlag = true;

    }
    else {
      choke = true;
      chokeFlag = true;
    }
  }

  if (flag == true &&  sensorValue < 10) {
    time_end = millis();
    chokeFlag = false;
  }
}

////////////////////////////////////////////

void HelloDrum::cymbal2zone() {

  int velo = 0;
  hit = false;
  hitRim = false;
  choke = false;
  int piezoValue = analogRead(pin_1);
  int sensorValue = analogRead(pin_2);

  //入り口でエッヂとボウ分けるか？
  //  if (abs(analogRead(pin_1) - analogRead(pin_2)) > thre1) {
  if (analogRead(pin_2) > threshold1 || analogRead(pin_1) > threshold1) {
    time_hit = millis();

    if (time_hit - time_end < masktime) { //retrigger cancel
      flag = true;
    }

    //    if (analogRead(pin_1) - exValue > thre2) {
    //      flag = false;
    //    }

    if (flag == false) {
      for (int i = 0; i < scantime; i++) {  //peak scan
        int peak = analogRead(pin_1);
        if (peak > velo) {
          velo = peak;
        }
        while (millis() - time_hit < 1);
      }

      //bow
      if (sensorValue < 50) {
        velo = map(velo, threshold1, sensitivity , 1, 127);

        if (velo <= 1) {
          velo = 1;
        }

        if (velo > 127) {
          velo = 127;
        }

        velocity = velo;
        showVelocity = velocity;
        flag = true;
        showLCD = true;
        hit = true;
        padIndex = padNum;
      }

      //edge
      else if (sensorValue > 50) {
        if (velo > 500) {
          velo = velo - 500;
        }
        else {
          velo = 500 - velo;
        }
        velo = map(velo, threshold1, sensitivity, 1, 127);

        if (velo <= 1) {
          velo = 1;
        }

        if (velo > 127) {
          velo = 127;
        }

        velocity = velo;
        showVelocity = velocity;
        flag = true;
        showLCD = true;
        hitRim = true;
        padIndex = padNum;
      }
    }
  }

  if (flag == true && piezoValue < threshold1 && sensorValue < 10) {
    time_end = millis();
    flag = false;
  }

  if (chokeFlag == false && sensorValue > 500 && sensorValue < 900  && abs(piezoValue - sensorValue) < 10) {
    time_choke = millis();
    if (time_choke - time_end < 10) {
      chokeFlag = true;

    }
    else {
      choke = true;
      chokeFlag = true;
    }
  }

  if (flag == true &&  sensorValue < 10) {
    time_end = millis();
    chokeFlag = false;
  }
}

void HelloDrum::cymbal3zone() {

  int velo = 0;
  hit = false;
  hitRim = false;
  hitCup = false;
  choke = false;

  int piezoValue = analogRead(pin_1);
  int sensorValue = analogRead(pin_2);

  //入り口でエッヂとボウ分けるか？
  //  if (abs(analogRead(pin_1) - analogRead(pin_2)) > thre1) {
  if (analogRead(pin_2) > threshold1 || analogRead(pin_1) > threshold1) {
    time_hit = millis();

    if (time_hit - time_end < masktime) { //retrigger cancel
      flag = true;
    }

    //    if (analogRead(pin_1) - exValue > thre2) {
    //      flag = false;
    //    }

    if (flag == false) {
      for (int i = 0; i < scantime; i++) {  //peak scan
        int peak = analogRead(pin_1);
        if (peak > velo) {
          velo = peak;
        }
        while (millis() - time_hit < 1);
      }

      //bow
      if (sensorValue < 50) {
        velo = map(velo, threshold1, sensitivity , 1, 127);

        if (velo <= 1) {
          velo = 1;
        }

        if (velo > 127) {
          velo = 127;
        }

        velocity = velo;
        showVelocity = velocity;
        flag = true;
        showLCD = true;
        hit = true;
        padIndex = padNum;
      }

      //edge
      else if (sensorValue > 500 && sensorValue < 520) {
        if (velo > 500) {
          velo = velo - 500;
        }
        else {
          velo = 500 - velo;
        }
        velo = map(velo, threshold1, sensitivity, 1, 127);

        if (velo <= 1) {
          velo = 1;
        }

        if (velo > 127) {
          velo = 127;
        }

        velocity = velo;
        showVelocity = velocity;
        flag = true;
        showLCD = true;
        hitRim = true;
        padIndex = padNum;
      }

      //cup
      else if (sensorValue > 520) {
        velocity = 100;
        showVelocity = velocity;
        flag = true;
        showLCD = true;
        hitCup = true;
        padIndex = padNum;
      }
    }
  }

  if (flag == true && piezoValue < threshold1 && sensorValue < 10) {
    time_end = millis();
    flag = false;
  }

  if (chokeFlag == false && sensorValue > 500 && sensorValue < 900  && abs(piezoValue - sensorValue) < 10) {
    time_choke = millis();
    if (time_choke - time_end < 10) {
      chokeFlag = true;

    }
    else {
      choke = true;
      chokeFlag = true;
    }
  }

  if (flag == true &&  sensorValue < 10) {
    time_end = millis();
    chokeFlag = false;
  }
}

void HelloDrum::TCRT5000() {

  threshold2 = scantime;
  scantime = masktime;
  
  HHCnum = padNum;
  int velo = 0;
  int TCRT = analogRead(pin_1);
  closeHH = false;
  settingHHC = true;

  if (TCRT < threshold1 * 10 && pedalVelocityFlag == false && pedalFlag == false) {
    time_hit_pedal_1 = millis();
    pedalVelocityFlag = true;
  }

  if (TCRT < threshold2 * 10 && pedalFlag == false) {
    time_hit_pedal_2 = millis();

    //傾き＝VELOCITY
    velo = time_hit_pedal_2 - time_hit_pedal_1;
    velo = map(velo, scantime, 0, 1, 127);
    //velo = map(velo, 50, 0, 1, 127);

    if (velo <= 1) {
      velo = 1;
    }

    if (velo > 127) {
      velo = 127;
    }

    velocity = velo;
    showVelocity = velocity;
    closeHH = true;
    showLCD = true;
    padIndex = padNum;
    pedalFlag = true;
    pedalVelocityFlag = false;
  }


  if (TCRT > threshold1 * 10 && pedalFlag == true) {
    pedalFlag = false;
  }

  //HIHAT PEDAL CC
  TCRT = map(TCRT, sensitivity * 10, 100, 0, 127);
  //TCRT = map(TCRT, 900, 100, 0, 127);
  if (TCRT > 127) {
    TCRT = 127;
  }
  if (TCRT < 0) {
    TCRT = 0;
  }

  if (exTCRT != TCRT) {
    pedalCC = TCRT;
    moving = true;
    exTCRT = TCRT;
  }

  else {
    moving = false;
  }
}

void HelloDrum::FSR() {

  threshold2 = scantime;
  scantime = masktime;
  
  HHCnum = padNum;
  int velo = 0;
  int FSR = analogRead(pin_1);
  closeHH = false;

  if (FSR < threshold1 * 10 && pedalVelocityFlag == false && pedalFlag == false) {
    time_hit_pedal_1 = millis();
    pedalVelocityFlag = true;
  }

  if (FSR < threshold2 * 10 && pedalFlag == false) {
    time_hit_pedal_2 = millis();

    velo = time_hit_pedal_2 - time_hit_pedal_1;
    velo = map(velo, scantime, 0, 1, 127);
    //velo = map(velo, 50, 0, 1, 127);

    if (velo <= 1) {
      velo = 1;
    }

    if (velo > 127) {
      velo = 127;
    }

    velocity = velo;
    showVelocity = velocity;
    closeHH = true;
    showLCD = true;
    padIndex = padNum;
    pedalFlag = true;
    pedalVelocityFlag = false;
  }


  if (FSR > threshold1 * 10 && pedalFlag == true) {
    pedalFlag = false;
  }

  //HIHAT PEDAL CC
  FSR = map(FSR, sensitivity * 10, 100, 0, 127);
  //TCRT = map(TCRT, 900, 100, 0, 127);
  if (FSR > 127) {
    FSR = 127;
  }
  if (FSR < 0) {
    FSR = 0;
  }

  if (exFSR != FSR) {
    pedalCC = FSR;
    moving = true;
    exFSR = FSR;
  }

  else {
    moving = false;
  }
}


////////////////// 3. EEPROM SETTING  ////////////////////

#ifdef __AVR__

void HelloDrum::settingEnable(){

  //When EDIT button pushed
  if(padNum == nameIndex){

    //When UP button pushed
    if(button_up == LOW && buttonState == true  && editCheck == true){

	switch (itemNumber){
    	 case 0:
    	 sensitivity = sensitivity + UP[itemNumber];
       EEPROM.write(padNum*7, sensitivity);
    	 break;

    	 case 1:
    	 threshold1 = threshold1 + UP[itemNumber];
       EEPROM.write((padNum*7) + 1, threshold1);
    	 break;

    	 case 2:
    	 scantime = scantime + UP[itemNumber];
       EEPROM.write((padNum*7) + 2, scantime);
    	 break;

    	 case 3:
    	 masktime = masktime + UP[itemNumber];
       EEPROM.write((padNum*7) + 3, masktime);
    	 break;

    	 case 4:
    	 note = note + UP[itemNumber];
       EEPROM.write((padNum*7) + 4, note);
       noteOpen = note;
    	 break;

       case 5:
       noteRim = noteRim + UP[itemNumber];
       EEPROM.write((padNum*7) + 5, noteRim);
       noteClose = noteRim;
       noteOpenEdge = noteRim;
       break;

       case 6:
       noteCup = noteCup + UP[itemNumber];
       EEPROM.write((padNum*7) + 6, noteCup);
       noteCloseEdge = noteCup;
       break;
      }
      change = true;
      push = true;
      buttonState = false;
      delay(30);
    }

    //When Down button pushed
    if(button_down == LOW && buttonState == true  && editCheck == true){

      switch (itemNumber){
    	 case 0:
    	 sensitivity = sensitivity - UP[itemNumber];
       EEPROM.write(padNum*7, sensitivity);
    	 break;

    	 case 1:
    	 threshold1 = threshold1 - UP[itemNumber];
       EEPROM.write((padNum*7) + 1, threshold1);
    	 break;

    	 case 2:
    	 scantime = scantime - UP[itemNumber];
       EEPROM.write((padNum*7) + 2, scantime);
    	 break;

    	 case 3:
    	 masktime = masktime - UP[itemNumber];
       EEPROM.write((padNum*7) + 3, masktime);
    	 break;

    	 case 4:
    	 note = note - UP[itemNumber];
       EEPROM.write((padNum*7) + 4, note);
       noteOpen = note;
    	 break;

       case 5:
       noteRim = noteRim - UP[itemNumber];
       EEPROM.write((padNum*7) + 5, noteRim);
       noteClose = noteRim;
       noteOpenEdge = noteRim;
       break;

       case 6:
       noteCup = noteCup - UP[itemNumber];
       EEPROM.write((padNum*7) + 6, noteCup);
       noteCloseEdge = noteCup;
       break;
      }
      change = true;
      push = true;
      buttonState = false;
      delay(30);
    }


    if(itemNumber == 0){
		  value = sensitivity;
	  }

	  else if (itemNumber == 1){
		  value = threshold1;
	  }

    else if (itemNumber == 2){
      value = scantime;
    }

    else if (itemNumber == 3){
      value = masktime;
    }

    else if (itemNumber == 4){
      value = note;
    }

    else if (itemNumber == 5){
      value = noteRim;
    }

    else if (itemNumber == 6){
      value = noteCup;
    }

    showValue = value;
  }
}

void HelloDrum::settingName(char *instrumentName){
  //Store the name of the pad in the array.
  showInstrument[nameIndex] = instrumentName;
  nameIndexMax = nameIndex;
  nameIndex = ++nameIndex;
}

void HelloDrum::loadMemory(){
  //Read values from EEPROM.
  sensitivity = EEPROM.read(padNum*7);
  threshold1 = EEPROM.read((padNum*7) + 1);
  scantime = EEPROM.read((padNum*7) + 2);
  masktime = EEPROM.read((padNum*7) + 3);
  note = EEPROM.read((padNum*7) + 4);
  noteOpen = EEPROM.read((padNum*7) + 4);
  noteRim = EEPROM.read((padNum*7) + 5);
  noteClose = EEPROM.read((padNum*7) + 5);
  noteOpenEdge = EEPROM.read((padNum*7) + 5);
  noteCup = EEPROM.read((padNum*7) + 6);
  noteCloseEdge = EEPROM.read((padNum*7) + 6);
}

void HelloDrum::initMemory(){
  //Write initial value to EEPROM.
  EEPROM.write(padNum*7, sensitivity);
  EEPROM.write((padNum*7) + 1, threshold1);
  EEPROM.write((padNum*7) + 2, scantime);
  EEPROM.write((padNum*7) + 3, masktime);
  EEPROM.write((padNum*7) + 4, note);
  EEPROM.write((padNum*7) + 5, noteRim);
  EEPROM.write((padNum*7) + 6, noteCup);
}

#endif

///////////////////// 4. BUTONN //////////////////////////

void HelloDrumButton::readButtonState() {

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

  if (button_set == LOW && buttonState == true && editCheck == false) {
    editCheck = true;
    edit = true;
    buttonState = false;
    buttonState_set = false;
    delay(30);
  }

  if (button_set == LOW && buttonState == true && editCheck == true) {
    editCheck = false;
    editdone = true;
    buttonState = false;
    buttonState_set = true;
    delay(30);
  }

  if (button_set == HIGH){
    edit = false;
    editdone =false;
  }

  /////////////////////////// UP DOWN ///////////////////////////////////////

  if (button_up == LOW && buttonState == true && editCheck == false) {
    UPDOWN = ++UPDOWN;

    if (UPDOWN < 0) {
      UPDOWN = nameIndexMax;
    }
    if (UPDOWN > nameIndexMax) {
      UPDOWN = 0;
    }

    nameIndex = UPDOWN;
    push = true;
    buttonState = false;
    delay(30);
  }

  if (button_down == LOW && buttonState == true && editCheck == false) {
    UPDOWN = --UPDOWN;

    if (UPDOWN < 0) {
      UPDOWN = nameIndexMax;
    }
    if (UPDOWN > nameIndexMax) {
      UPDOWN = 0;
    }
    nameIndex = UPDOWN;
    push = true;
    buttonState = false;
    delay(30);
  }

  ///////////////////////////// NEXT BACK ////////////////////////////////

  if (button_next == LOW && buttonState == true && editCheck == false) {
    NEXTBACK = ++NEXTBACK;

    if (NEXTBACK < 0) {
      NEXTBACK = 6;
    }
    if (NEXTBACK > 6) {
      NEXTBACK = 0;
    }
    itemNumber = NEXTBACK;
    nameIndex = UPDOWN;
    push = true;
    buttonState = false;
    delay(30);
  }

  if (button_back == LOW && buttonState == true && editCheck == false) {
    NEXTBACK = --NEXTBACK;

    if (NEXTBACK < 0) {
      NEXTBACK = 6;
    }
    if (NEXTBACK > 6) {
      NEXTBACK = 0;
    }
    itemNumber = NEXTBACK;
    nameIndex = UPDOWN;
    push = true;
    buttonState = false;
    delay(30);
  }

  //When you take your hand off the button
  if (buttonState == false && button_up == HIGH && button_down == HIGH && button_next == HIGH && button_back == HIGH && button_set == HIGH) {
    push = false;
    buttonState = true;
    change = false;
  }
}

/////////////////////// 5. LCD  //////////////////////////

void HelloDrumLCD::show(){

  //Button Push
  if(push == true){
    lcd.clear();
    lcd.print(showInstrument[nameIndex]);
    lcd.setCursor(0, 1);
    if(nameIndex == HHCnum){
      lcd.print(itemHHC[itemNumber]);
      lcd.setCursor(13, 1);
      lcd.print(showValue);
    }

    else if(nameIndex == HHnum){
      lcd.print(itemHH[itemNumber]);
      lcd.setCursor(13, 1);
      lcd.print(showValue);
    }

    else{
      lcd.print(item[itemNumber]);
      lcd.setCursor(13, 1);
      lcd.print(showValue);
    }
  }

  //Pad hit
  if(showLCD == true){
    lcd.clear();
    lcd.print(showInstrument[padIndex]);
    lcd.setCursor(0, 1);
    lcd.print(showVelocity);
    showLCD = false;
  }

  //EDIT
  if(edit == true){
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
  if(editdone == true){
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

//////////////////////////////////////////////////////////

//For debug.
char* HelloDrum::GetItem(int i) {
  return item[i];
}
int HelloDrumButton::GetItemNumber(){
  return itemNumber;
}
int HelloDrumButton::GetEditState(){
  return edit;
}
int HelloDrumButton::GetEditdoneState(){
  return editdone;
}
int HelloDrumButton::GetPushState(){
  return push;
}
int HelloDrumButton::GetChangeState(){
  return change;
}
