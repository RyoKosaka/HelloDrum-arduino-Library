
/*
  EXAMPLE - Initialization of EEPROM

  With this sample code, you can initialize the EEPROM.
  Please write this code only once, if you want to use setting mode with this library.
  When writing is completed, the initial value setting is completed.
  So let's set the value using the "SensingWithLCD" sample.
  
  The values after initialization are as follows.
    
    Sensitivity = 80
    Retrigger Cancel = 10
    Threshold 1 = 20
    Threshold 2 = 50
    Note = 38
    Note of rim/Edge = 39
    Note of cup = 40

  https://open-e-drums.tumblr.com/
*/


#include <hellodrum.h>

HelloDrum pad_0(0);
HelloDrum pad_1(1);
HelloDrum pad_2(2);
HelloDrum pad_3(3);
HelloDrum pad_4(4);
HelloDrum pad_5(5);
HelloDrum pad_6(6);
HelloDrum pad_7(7);
HelloDrum pad_8(8);
HelloDrum pad_9(9);

void setup() {
  
  //Initialize 10 pads.
  pad_0.initMemory();
  pad_1.initMemory();
  pad_2.initMemory();
  pad_3.initMemory();
  pad_4.initMemory();
  pad_5.initMemory();
  pad_6.initMemory();
  pad_7.initMemory();
  pad_8.initMemory();
  pad_9.initMemory();
  
}

void loop() {
}

