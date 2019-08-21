# HelloDrum Arduino Library
This is a library for making E-Drum with Arduino.  
**Ver.0.7.1(8/3/2019) Work in progress.** 

## Description

This is a library for making E-Drum with Arduino.  
By using it with Arduino MIDI Library, you can make E-drum.  
Blog :[https://open-e-drums.tumblr.com/](https://open-e-drums.tumblr.com/)  
YouTube :[https://www.youtube.com/channel/UCNCDcIO26xL_NhI04QY-v4A](https://www.youtube.com/channel/UCNCDcIO26xL_NhI04QY-v4A)

This software is an alpha version, and is unsupported.
Use at your own risk.

## Features

- Single piezo pad, Dual Piezo pad, 2 zone cymbal, 3 zone cymbal
- Compatible with YAMAHA's 3 zone cymbal(PCY135/PCY155)
- Compatible with SoftPot, FSR and Optical(TCRT5000) type hi-hat controllers
- Sensing with MUX(4051 and 4067)
- Setting mode with LCD or OLED
- Sensitivity, Threshold, Scan Time, Mask Time, Note Number can be set with each pad
- Works with ESP32 and Teensy and AVR boards such as UNO and MEGA.

## How to Use
- Install

Use Arduino's Library Manager to install the library. Search for “hellodrum ”.  
If you use MIDI, also install the MIDI Library.
<img src="https://open-e-drums.com/images/ide.png" width="800px">


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
      10, //threshold
      20, //scantime
      20, //masktime
      38  //note
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

- Coding (MUX 4051):
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
    int SNARE[5] = {
      80,  //sensitivity
      10,  //threshold
      20,  //scantime
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
<img src="https://open-e-drums.com/images/circuit/singlePiezo.png" width="400px">

- Dual Piezo  
<img src="https://open-e-drums.com/images/circuit/dualPiezo.png" width="400px">

- TCRT5000  
<img src="https://open-e-drums.com/images/circuit/TCRT5000.png" width="400px">

- FSR  
<img src="https://open-e-drums.com/images/circuit/FSR.png" width="400px">

- YAMAHA PCY135/PCY155  
<img src="https://open-e-drums.com/images/circuit/YAMAHA_PCY135_155.png" width="400px">

- Arduino UNO with MUX  
<img src="https://open-e-drums.com/images/circuit/MUX_uno.png" width="600px">

- ESP32 with MUX  
<img src="https://open-e-drums.com/images/circuit/MUX_esp32.png" width="600px">

## Pads

The STL data of pads from 6 inches to 12 inches, hi-hat controllers(<https://www.thingiverse.com/RyoKosaka/designs>)

## Release History

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
