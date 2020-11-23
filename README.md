# HelloDrum Arduino Library
[![arduino-library-badge](https://www.ardu-badge.com/badge/Hello%20Drum.svg?)](https://www.ardu-badge.com/Hello%20Drum)   
This is a library for making E-Drum with Arduino.  
**Ver.0.7.7(11/1/2020) Work in progress.** 

## Description

This is a library for making E-Drum with Arduino.  
By using it with Arduino MIDI Library, you can make E-drum.  

Project Page :[https://open-e-drums.com/](https://open-e-drums.com/)  
Blog :[https://open-e-drums.tumblr.com/](https://open-e-drums.tumblr.com/)  
YouTube :[https://www.youtube.com/channel/UCNCDcIO26xL_NhI04QY-v4A](https://www.youtube.com/channel/UCNCDcIO26xL_NhI04QY-v4A)  
3D Models of Pad :[https://www.thingiverse.com/RyoKosaka/designs](https://www.thingiverse.com/RyoKosaka/designs)  

**This software is an alpha version, and is unsupported.  
Use at your own risk.**

## Features

- Single piezo pad, Dual piezo pad, 2-Zone cymbal, 3-Zone cymbal
- Compatible with Roland's 2 zone pads (PD Series)  
- Compatible with YAMAHA's 3 zone pads (XP Series)  
- Compatible with YAMAHA's 3 zone cymbal(PCY135/PCY155) and Roland's 2 zone cymbals(CY12C/CY5/CY8)
- Compatible with SoftPot, FSR and Optical(TCRT5000) type hi-hat controllers and Roland's hihat(VH10/VH11)
- Sensing with MUX(4051 and 4067)
- Setting mode with LCD or OLED
- Setting mode with LCD Keypad Shield (DFRobot, HiLetgo)
- Sensitivity, Threshold, Scan Time, Mask Time, Note Number, Velocity Curve can be set with each pad
- Save setting values with EEPROM
- Works with ESP32 and Teensy and AVR boards such as UNO and MEGA.
- Works with [MIDI Library](https://github.com/FortySevenEffects/arduino_midi_library), [BLE-MIDI Library](https://github.com/lathoub/Arduino-BLE-MIDI), [USB-MIDI library](https://github.com/lathoub/Arduino-USBMIDI).

## How to Use
- **Install**  
Use Arduino's Library Manager to install the library. Search for “hellodrum ”.  
If you use MIDI, also install the [MIDI Library](https://github.com/FortySevenEffects/arduino_midi_library). Or [BLE-MIDI Library](https://github.com/lathoub/Arduino-BLE-MIDI) or [USB-MIDI library](https://github.com/lathoub/Arduino-USBMIDI).

<img src="https://open-e-drums.com/images/ide.png" width="800px">

- **Initialize EEPROM**  
If you want to use EEPROM to store the settings, you will need to initialize the EEPROM.
Please write the sample code, example > EEPROM > InitializeEEPROM > InitializeEEPROM.ino, to your Arduino. Once it's written, the initialization is complete.  

- **Coding**
   ```cpp
    #include <hellodrum.h>
    #include <MIDI.h>
    MIDI_CREATE_DEFAULT_INSTANCE();

    //Please name your piezo.
    //The piezo named snare is connected to the A0 pin
    HelloDrum snare(0);

    //Setting
    byte SNARE[6] = {
      80, //sensitivity 
      10, //threshold
      20, //scantime
      20, //masktime
      38, //note
      1   //curve type
    }; 

    void setup()
    {
        MIDI.begin(10);
        snare.setCurve(SNARE[5]); //Set velocity curve 
    }

    void loop()
    {
        //Sensing
        snare.singlePiezo(SNARE[0], SNARE[1], SNARE[2], SNARE[3]); //(sensitivity, threshold, scantime, masktime)

        //Sending MIDI signals
        if (snare.hit == true) {
            MIDI.sendNoteOn(SNARE[4], snare.velocity, 10);  //(note, velocity, channel)
            MIDI.sendNoteOff(SNARE[4], 0, 10);
        }
    }
    ```

- **Coding (MUX)**:
   ```cpp
    #include <hellodrum.h>
    #include <MIDI.h>
    MIDI_CREATE_DEFAULT_INSTANCE();

    //Define MUX Pins
    HelloDrumMUX_4051 mux(2,3,4,0);//D2, D3, D4, A0
    
    //Please name your piezo.
    //The piezo named snare is connected to MUX 0 pin
    HelloDrum snare(0);

    //Setting
    byte SNARE[6] = {
      80,  //sensitivity
      10,  //threshold
      20,  //scantime
      20,  //masktime
      38,  //note
      1    //curve type
    }; 

    void setup()
    {
        MIDI.begin(10);
        snare.setCurve(SNARE[5]); //Set velocity curve 
    }

    void loop()
    {
        //scanning all pin of mux
        mux.scan();

        //Sensing
        snare.singlePiezoMUX(SNARE[0], SNARE[1], SNARE[2], SNARE[3]); //(sensitivity, threshold, scantime, masktime)

        //Sending MIDI signals
        if (snare.hit == true) {
            MIDI.sendNoteOn(SNARE[4], snare.velocity, 10);  //(note, velocity, channel)
            MIDI.sendNoteOff(SNARE[4], 0, 10);
        }
    }
    ```  

    [Check instruction.md](https://github.com/RyoKosaka/HelloDrum-arduino-Library/blob/master/docs/instruction.md) for more info on coding.

## Using Arduino MIDI Library

[FortySevenEffects Arduino MIDI Library](https://github.com/FortySevenEffects/arduino_midi_library)   
There are three ways to communicate with a PC using MIDI with an arduino.

1. Rewrite arduino's USB chip (UNO,MEGA only)
    - https://www.arduino.cc/en/Hacking/DFUProgramming8U2
    - http://morecatlab.akiba.coocan.jp/lab/index.php/aruino/midi-firmware-for-arduino-uno-moco/?lang=en
2. Using Hairless MIDI (Easiest way)
    - https://projectgus.github.io/hairless-midiserial/
    - https://open-e-drums.tumblr.com/post/171304647319/using-hairless-midi
3. Using a MIDI terminal and a MIDI-USB cable
    - https://www.arduino.cc/en/Tutorial/Midi
    - https://open-e-drums.tumblr.com/post/171168448524/using-midi-socket-with-arduino


```cpp
#include <hellodrum.h>
#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

//...
```

## Using USB-MIDI Library

[lathoub Arduino-USBMIDI](https://github.com/lathoub/Arduino-USBMIDI)   
If you are using atmega32u4 (Arduino Leonardo, Arduino Micro, Arduino Pro Micro...), you can use USB-MIDI library. No additional software is needed, the 32u4 is recognized as a MIDI device.

```cpp
#include <hellodrum.h>
#include <USB-MIDI.h>
USBMIDI_CREATE_DEFAULT_INSTANCE();

//...
```

## Using BLE-MIDI Library with ESP32

[lathoub Arduino-BLE-MIDI](https://github.com/lathoub/Arduino-BLE-MIDI)   
It is very easy to use BLE-MIDI with ESP32.  
You can find a device named "BLE-MIDI".

```cpp
#include <hellodrum.h>
#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32.h>
BLEMIDI_CREATE_DEFAULT_INSTANCE();

//...
```
Please check the KORG's documentation for instructions on how to connect.  
[KORG Bluetooth MIDI Connection Guide](https://cdn.korg.com/us/support/download/files/7c456d3daad3b027197b3fda1f87dce7.pdf?response-content-disposition=inline%3Bfilename%3DBluetooth_MIDI_SettingG_E1.pdf&response-content-type=application%2Fpdf%3B) 

## Sensing Method and Setting Values
**Be sure to check here first if it is not working properly.**  
[Check sensing.md](https://github.com/RyoKosaka/HelloDrum-arduino-Library/blob/master/docs/sensing.md) for sensing methods.  
[Check setting.md](https://github.com/RyoKosaka/HelloDrum-arduino-Library/blob/master/docs/setting.md) for setting values. 

## Circuit
[Check circuit.md](https://github.com/RyoKosaka/HelloDrum-arduino-Library/blob/master/docs/circuit.md) for how to connect the pad and the board.

## Pads

The STL data of pads from 6 inches to 12 inches, hi-hat controllers(<https://www.thingiverse.com/RyoKosaka/designs>)

## Release History

* 0.7.7
   - Bug fix for ESP32
   - Add and Update sample codes about BLE-MIDI, USB-MIDI
* 0.7.6
   - Bug fix for LCD and buttons
   - Add and Update sample codes
* 0.7.5
   - Bug fix for ESP32
   - Bug fix for hihatControl()
   - Update sample codes
   - Add pullup mode to deal with floating pins (Beta)
   - Add debug mode
* 0.7.4
   - Add velocity curve function
   - FSR() and TCRT5000() integrated into hihatControl()
   - Update circuits
   - Add circuit images
   - Update sensing algorithm
   - Add sensing figure
   - Update sample code
   - Organize the source code
* 0.7.3
   - Update variables type
   - Add button function for LCD keypad shield
   - Add sample code "lcdShield.ino" for LCD keypad shield
* 0.7.2
   - Update sample code
   - Add Knob function
   - Add sample code for Teensy
* 0.7.1
   - Sensing with 16ch MUX(4067) is available
   - Update sample code
   - Organize folders and files
   - Add library.properties
   - Teensy3.2 has been tested
* 0.7.0
   - Improved sensing
   - Dual Piezo sensing available (Test)
   - ESP32 EEPROM available
   - Setting mode with I2C LCD or I2C OLED available
   - Add sample code
* 0.6.0
   - Sensing with MUX(4051) is available
   - Add BLE MIDI sample code with ESP32
   - Hihat Contorller with FSR is available
* 0.5.0
   - Setting mode available
   - Display function by LCD is available
   - Saving function of setting items by EEPROM is available
   - Improved sensing of TCRT 5000 hi-hat controller
* 0.1.0
   - Work in progress

## ToDo
- rimGain
- retriggerCancel
- rotaryEncoder

## Author

[@tnctrekit](https://twitter.com/tnctrekit)  
[Works](https://ryokosaka.com)

## Licence

[MIT](http://opensource.org/licenses/mit-license.php)
