This article briefly explains how to use the library.  
[**HelloDrum-arduino-Library**](https://github.com/RyoKosaka/HelloDrum-arduino-Library)


## Install

1. Download ZIP
2. Import a .zip Library ()
3. Import Arduino MIDI Library ()

## Sample Code

The library has some sample code.  
[**"minimalDrumKit.ino"**](https://github.com/RyoKosaka/HelloDrum-arduino-Library/blob/master/examples/minimalDrumKit/minimalDrumkit.ino) can save set values by LCD and EEPROM. This article mainly explains this code.  

## Initialize EEPROM

Before using "minimalDrumKit.ino", you need to initialize EEPROM.  
I made sample code that can be initialized simply by writing to arduino. Please write [**"InitializeEEPROM.ino"**](https://github.com/RyoKosaka/HelloDrum-arduino-Library/blob/master/examples/InitializeEEPROM/InitializeEEPROM.ino) to arduino. Once written, the initialization is completed.  

The values after initialization are as follows.
    
Sensitivity = 80  
Retrigger Cancel = 10  
Threshold 1 = 20  
Threshold 2 = 50  
Note = 38  
Note of rim/Edge = 39  
Note of cup = 40  

## Circuit

**Pads & Controller Circuit:**  
Piezo for kick to A0.  
Piezo for snare to A1.  
Piezo for hihat to A2.  
TCRT5000 for hihat controller to A3  
Yamaha PCY135/155 to A4,A5  

**Buttons Circuit:**  
Button for EDIT to digital pin 6
Button for UP to digital pin 7  
Button for DOWN to digital pin 8  
Button for NEXT to digital pin 9  
Button for BACK to digital pin 10
![img](https://www.arduino.cc/en/uploads/Tutorial/inputPullupButton.png)

[**LCD Circuit:**](https://www.arduino.cc/en/Tutorial/HelloWorld)  
LCD RS pin to digital pin 12  
LCD Enable pin to digital pin 11  
LCD D4 pin to digital pin 5  
LCD D5 pin to digital pin 4  
LCD D6 pin to digital pin 3  
LCD D7 pin to digital pin 2  

## minimalDrumKit.ino

["**minimalDrumKit.ino**"](https://github.com/RyoKosaka/HelloDrum-arduino-Library/blob/master/examples/minimalDrumKit/minimalDrumkit.ino)  
I will explain in order from the top.  
```cpp
//Please name your pad and controller.
HelloDrum kick(0);
HelloDrum snare(1);
HelloDrum hihat(2);
HelloDrum hihatControl(3);
HelloDrum ride(4, 5);
```  
**HelloDrum** is a class related to pads and controllers.  
I put the name kick on the pad connected to No. 0 of the analog pin. The same is true for snare and hihat, hihatControl.


  
```cpp
HelloDrumButton button(6, 7, 8, 9, 10); //(EDIT,UP,DOWN,NEXT,BACK)
HelloDrumLCD lcd(12, 11, 5, 4, 3, 2); //(rs, en, d4, d5, d6, d7)
```  
**HelloDrumButton** is a class related to buttons used for setting.  
**HelloDrumLCD** is a class related to LCD display.  
Like the pad, decide the digital pin to be connected and name it.


  
```cpp
kick.settingName("KICK");
snare.settingName("SNARE");
hihat.settingName("HIHAT");
hihatControl.settingName("HIHAT PEDAL");
ride.settingName("RIDE");
```  
**settingName** is a method for determining the display name on the LCD.  
kick is displayed on the LCD as KICK.  
**The order of settingName should be the same as that of HelloDrum.**  


  
```cpp
kick.loadMemory();
snare.loadMemory();
hihat.loadMemory();
hihatControl.loadMemory();
ride.loadMemory();
```  
**loadMemory** is a method to read the setting values recorded in EEPROM.  
**The order of loadMemory should be the same as that of HelloDrum.**  



```cpp
button.readButtonState();
lcd.show();
```  
To the contents of loop().
**readButtonState** is a method to read the state of the button as its name suggests. Check which button was pressed.  
As the name implies, **show** is a method to display the setting items and the velocity when hitting the pad on the LCD.



  
```cpp
kick.settingEnable();
snare.settingEnable();
hihat.settingEnable();
hihatControl.settingEnable();
ride.settingEnable();
```  
**settingEnable** is a method to activate the setting mode.  
With this method you can record the value of the setting item to EEPROM.  


```cpp
kick.singlePiezo();
snare.singlePiezo();
hihat.singlePiezo();
hihatControl.TCRT5000();
ride.cymbal3zone();
```  
Then, lines 100 to 104 are the methods that sense pads and controllers.  
In this method only sensing is done.  
With this method, the strength you hit is converted to velocity in 127 steps.  
There are several types of sensing methods, as the algorithm is slightly different depending on the pad.  

However, as for the sensing of cymbals, there are things that go wrong. I will improve it in the future.  
Also, although not yet implemented, we plan to add a method called dualPiezo(), FSR().  



```cpp
if (kick.hit == true) {
  MIDI.sendNoteOn(kick.note, kick.velocity, 10);  //(note, velocity, channel)
  MIDI.sendNoteOff(kick.note, 0, 10);
}
```  
Well, as sensing has just been done, we only send MIDI signals. Lines 108-111.  
I think that you can understand immediately after reading the if statement. If kick.hit is true, it means that kick's pad is hit, so send a MIDI signal.  
That's it. The same is true for snares.


```cpp
if (hihat.hit == true) {
  //check open or close
  //1.open
  if (hihatControl.openHH == true) {
    MIDI.sendNoteOn(hihat.note, hihat.velocity, 10);  //(note of open, velocity, channel)
    MIDI.sendNoteOff(hihat.note, 0, 10);
  }
  //2.close
  else {
    MIDI.sendNoteOn(hihat.noteCup, hihat.velocity, 10);  //(note of close, velocity, channel)
    MIDI.sendNoteOff(hihat.noteCup, 0, 10);
  }
}
```
In case of hi-hat it is divided according to the value of the controller.
Since openHH becomes true when the controller is open, open note (note) in case of true. If not, close note (noteCup).  
The same case is divided for ride.  

```cpp
//when hihat is closed
if (hihatControl.closeHH == true) {
  MIDI.sendNoteOn(hihatControl.note, hihatControl.velocity, 10);  //(note of pedal, velocity, channel)
  MIDI.sendNoteOff(hihatControl.note, 0, 10);
}

//sending state of pedal with controll change
if (hihatControl.moving == true) {
  MIDI.sendControlChange(4, hihatControl.pedalCC, 10);
}
```  
The hi-hat controller also supports the velocity when closed, so you can send MIDI signals just like singlePiezo. Lines 136-139.  
If the sound source is compatible, you can change the tone by the degree of opening of the hi-hat. Lines 142-144.  
I confirmed the operation with EZdrummer 2. 

```cpp
if (ride.choke == true) {
  MIDI.sendPolyPressure(ride.note, 127, 10);
  MIDI.sendPolyPressure(ride.noteRim, 127, 10);
  MIDI.sendPolyPressure(ride.noteCup, 127, 10);
  MIDI.sendPolyPressure(ride.note, 0, 10);
  MIDI.sendPolyPressure(ride.noteRim, 0, 10);
  MIDI.sendPolyPressure(ride.noteCup, 0, 10);
}
```  
PolyPressure is a MIDI signal that shows the strength of the power to hold down the keyboard. Lines 166-174.
When choke becomes true, it sets the value to 127 (maximize it) and sends it.


##Setting Mode

Please refer to [**this video**](http://www.youtube.com/watch?v=She6CrFEwQw)  
In this video, "minimalDrumKit.ino" which is sample code is used.  
[![img](http://img.youtube.com/vi/She6CrFEwQw/0.jpg)](http://www.youtube.com/watch?v=She6CrFEwQw)