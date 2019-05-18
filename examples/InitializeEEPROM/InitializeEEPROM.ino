
/*
  EXAMPLE - Initialization of EEPROM

  With this sample code, you can initialize the EEPROM.
  Please write this code befor making your e-drum, if you want to use setting mode with this library.
  When writing is completed, the initial value setting is completed.
  
  The values after initialization are as follows.
    
    Sensitivity = 80
    Threshold = 20
    Scan Time = 5
    Mask Time = 10
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
//if you use arduino MEGA, uncomment below lines.
/*
HelloDrum pad_6(6);
HelloDrum pad_7(7);
HelloDrum pad_8(8);
HelloDrum pad_9(9);
HelloDrum pad_10(10);
HelloDrum pad_11(11);
HelloDrum pad_12(12);
HelloDrum pad_13(13);
HelloDrum pad_14(14);
HelloDrum pad_15(15);
*/

void setup() {
  
  //Initialize
  pad_0.initMemory();
  pad_1.initMemory();
  pad_2.initMemory();
  pad_3.initMemory();
  pad_4.initMemory();
  pad_5.initMemory();
  //if you use arduino MEGA, uncomment below lines.
  /*
  pad_6.initMemory();
  pad_7.initMemory();
  pad_8.initMemory();
  pad_9.initMemory();
  pad_10.initMemory();
  pad_11.initMemory();
  pad_12.initMemory();
  pad_13.initMemory();
  pad_14.initMemory();
  pad_15.initMemory();
  */
}

void loop() {
}

