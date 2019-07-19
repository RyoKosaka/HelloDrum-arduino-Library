# HelloDrum-arduino-Library
This is a library for making E-Drum with Arduino.  
**Ver.0.7(7/20/2019) Work in progress.** 

## Description

This is a library for making E-Drum with Arduino.  
By using it with Arduino MIDI Library, you can make E-drum.  
Blog :[https://open-e-drums.tumblr.com/](https://open-e-drums.tumblr.com/)

This software is an alpha version, and is unsupported.
Use at your own risk.

## Features

- Single piezo pad, Dual Piezo pad, 2 zone cymbal, 3 zone cymbal
- It is compatible with YAMAHA's 3 zone cymbal.(PCY135/PCY155)
- Sensitivity, Threshold, Scan Time, Mask Time, Note Number can be set with each piezo.
- Both optical(TCRT5000) and FSR type hi-hat controllers can be used.
- Sensing with MUX(4051)

## How to Use
- Coding:
   ```cpp
    #include <hellodrum.h>
    #include <MIDI.h>
    MIDI_CREATE_DEFAULT_INSTANCE();

    //Please name your piezo.
    //The piezo named snare is connected to the A0 pin
    HelloDrum snare(0);

    //Setting
    int SNARE[5] = {
      80, //sensitivity 
      10,  //threshold
      20,   //scantime
      20,  //masktime
      38   //note
    }; 

    void setup()
    {
        MIDI.begin(10);
    }

    void loop()
    {
        //Sensing
        snare.singlePiezo(SNARE[0], SNARE[1], SNARE[2], SNARE[3]);

        //Sending MIDI signals
        if (snare.hit == true) {
            MIDI.sendNoteOn(SNARE[4], snare.velocity, 10);  //(note, velocity, channel)
            MIDI.sendNoteOff(SNARE[4], 0, 10);
        }
    }
    ```

- Coding (MUX):
   ```cpp
    #include <hellodrum.h>
    #include <MIDI.h>
    MIDI_CREATE_DEFAULT_INSTANCE();

    //Define MUX Pins
    HelloDrumMUX mux(2,3,4,0);//D2, D3, D4, A0
    
    //Please name your piezo.
    //The piezo named snare is connected to MUX 0 pin
    HelloDrum snare(0);

    //Setting
    int SNARE[5] = {
      80, //sensitivity
      10,  //threshold
      20,   //scantime
      20,  //masktime
      38   //note
    }; 

    void setup()
    {
        MIDI.begin(10);
    }

    void loop()
    {
        //scanning all pin of mux
        mux.scan();

        //Sensing
        snare.singlePiezoMUX(SNARE[0], SNARE[1], SNARE[2], SNARE[3]);

        //Sending MIDI signals
        if (snare.hit == true) {
            MIDI.sendNoteOn(SNARE[4], snare.velocity, 10);  //(note, velocity, channel)
            MIDI.sendNoteOff(SNARE[4], 0, 10);
        }
    }
    ```

[Check instruction.md](https://github.com/RyoKosaka/HelloDrum-arduino-Library/blob/master/instruction.md)

## Circuit
- Single Piezo  
<img src="https://raw.githubusercontent.com/RyoKosaka/HelloDrum-arduino-Library/master/circuit/singlePiezo.png" width="400px">

- Dual Piezo  
<img src="https://raw.githubusercontent.com/RyoKosaka/HelloDrum-arduino-Library/master/circuit/dualPiezo.png" width="400px">

- TCRT5000  
<img src="https://raw.githubusercontent.com/RyoKosaka/HelloDrum-arduino-Library/master/circuit/TCRT5000.png" width="400px">

- FSR  
<img src="https://raw.githubusercontent.com/RyoKosaka/HelloDrum-arduino-Library/master/circuit/FSR.png" width="400px">

- YAMAHA PCY135/PCY155  
<img src="https://raw.githubusercontent.com/RyoKosaka/HelloDrum-arduino-Library/master/circuit/YAMAHA_PCY135_155.png" width="400px">

- Arduino UNO with MUX  
<img src="https://raw.githubusercontent.com/RyoKosaka/HelloDrum-arduino-Library/master/circuit/MUX_uno.png" width="400px">

- ESP32 with MUX  
<img src="https://raw.githubusercontent.com/RyoKosaka/HelloDrum-arduino-Library/master/circuit/MUX_esp32.png" width="400px">

## Pads

The STL data of pads from 6 inches to 12 inches.(<https://www.thingiverse.com/RyoKosaka/designs>)

## Install

1. Download ZIP
2. Import a .zip Library (<https://www.arduino.cc/en/Guide/Libraries#toc4>)
3. Import Arduino MIDI Library (<https://playground.arduino.cc/Main/MIDILibrary>)

## Release History

* 0.7
   - Improved sensing
   - Dual Piezo sensing available (Test)
   - ESP32 EEPROM available
   - Setting mode with I2C LCD or I2C OLED available
   - Add sample code
* 0.6
   - Sensing with MUX(4051) is available
   - Add BLE MIDI sample code with ESP32
   - Hihat Contorller with FSR is available
* 0.5
   - Setting mode available
   - Display function by LCD is available
   - Saving function of setting items by EEPROM is available
   - Improved sensing of TCRT 5000 hi-hat controller
* 0.1
   - Work in progress

## Contributing

1. Fork it  
2. Create your feature branch
3. Commit your changes
4. Push to the branch
5. Create new Pull Request

## Author

[@tnctrekit](https://twitter.com/tnctrekit)  
[Works](https://ryokosaka.com)

## Licence

[MIT](http://opensource.org/licenses/mit-license.php)