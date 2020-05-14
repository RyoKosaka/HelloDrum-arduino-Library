# HelloDrum Arduino Library
This is a library for making E-Drum with Arduino.  
**Ver.0.7.6(5/1/2020) Work in progress.** 

## Description

This is a library for making E-Drum with Arduino.  
By using it with Arduino MIDI Library, you can make E-drum.  

Project Page :[https://open-e-drums.com/](https://open-e-drums.com/)  
Blog :[https://open-e-drums.tumblr.com/](https://open-e-drums.tumblr.com/)  
YouTube :[https://www.youtube.com/channel/UCNCDcIO26xL_NhI04QY-v4A](https://www.youtube.com/channel/UCNCDcIO26xL_NhI04QY-v4A)  
3D Models of Pad :[https://www.thingiverse.com/RyoKosaka/designs](https://www.thingiverse.com/RyoKosaka/designs)  

This software is an alpha version, and is unsupported.
Use at your own risk.

## Features

- Single piezo pad, Dual piezo pad, 2-Zone cymbal, 3-Zone cymbal
- Compatible with Roland's 2 zone pads (PD Series)  
- Compatible with YAMAHA's 3 zone cymbal(PCY135/PCY155) and Roland's 2 zone cymbals(CY12C/CY5/CY8)
- Compatible with SoftPot, FSR and Optical(TCRT5000) type hi-hat controllers and Roland's hihat(VH10/VH11)
- Sensing with MUX(4051 and 4067)
- Setting mode with LCD or OLED
- Setting mode with LCD Keypad Shield (DFRobot, HiLetgo)
- Sensitivity, Threshold, Scan Time, Mask Time, Note Number, Velocity Curve can be set with each pad
- Save setting values with EEPROM
- Works with ESP32 and Teensy and AVR boards such as UNO and MEGA.

## How to Use
- **Install**  
Use Arduino's Library Manager to install the library. Search for “hellodrum ”.  
If you use MIDI, also install the MIDI Library.  
<img src="https://open-e-drums.com/images/ide.png" width="800px">

- **Initialize EEPROM**  
If you want to use EEPROM to store the settings, you will need to initialize the EEPROM.
Please write the sample code, example > EEPROM > InitializeEEPROM > InitializeEEPROM.ino, to your Arduino. Once it's written, the initialization is complete.  

- **Coding**:
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

    [Check instruction.md](https://github.com/RyoKosaka/HelloDrum-arduino-Library/blob/master/instruction.md)

## Sensing and Setting Values
- Single Piezo Sensing  
<img src="https://open-e-drums.com/images/sensing/single.png" width="800px">  
If the value of the piezo exceeds the threshold, the scan will start. The scan will be done for the scantime you set. The highest piezo value during the scan is the peak. The peak value is converted to a velocity of 0-127. The peak is converted to velocity with the sensitivity value as the maximum value and the threshold value as the minimum value. In other words, if you set the threshold value to the same value as the sensitivity, the velocity will be constant.
Depending on the rigidity of the pad, the piezo vibrates for a while. So, you can set a mask time to prevent retriggering. If you are in mask time, the scan will not start even if the piezo value exceeds the threshold.

    - Sensitivity (1 - 100)
    - Threshold (1 - 100)
    - Scan Time (1 - 100) milliseconds
    - Mask Time (1 - 100) milliseconds
    - Velocity Curve Type (0 - 4)

- Dual Piezo Sensing  
<img src="https://open-e-drums.com/images/sensing/dual.png" width="800px">  
Since it's piezo sensing, it's basically the same as single piezo sensing. By comparing the signals from the piezo for the head to the piezo for the rim, we can determine which part of the pad was hit. On most pads, when the head is hit, the piezo for the rim also vibrates. Similarly, the piezo for the head vibrates when the rim is hit. The difference(d) between the peak of the piezo for the rim and the peak of the piezo for the head is used to judge. This can be set in rimSensitivity.  

    - Sensitivity (1 - 100)
    - Threshold (1 - 100)
    - Scan Time (1 - 100) milliseconds
    - Mask Time (1 - 100) milliseconds
    - Rim Sensitivity (1 - 100)
    - Rim Threshold (1 - 100)
    - Velocity Curve Type (0 - 4)

- 2-Zone Cymbal Sensing  
<img src="https://open-e-drums.com/images/sensing/cy2.png" width="800px">  
A film switch is used for the edge of the cymbal. This is a quicker response than the piezo. There is no need to scan. Because there is only ON and OFF. To determine the hit position, the value of the film switch at the beginning of the scan and the value of the film switch at the end of the scan are used. If it's a hit to the edge, the film switch will be low by the time the scan is over. On the other hand, if the film switch value is high at the beginning and end of the scan, it is determined to be a choke.  

    - Sensitivity (1 - 100)
    - Threshold (1 - 100)
    - Scan Time (1 - 100) milliseconds
    - Mask Time (1 - 100) milliseconds
    - Edge Threshold (1 - 100)
    - Velocity Curve Type (0 - 4)

- 3-Zone Cymbal Sensing  
<img src="https://open-e-drums.com/images/sensing/cy3.png" width="800px">  
It's almost the same as 2-Zone. So far, it only supports YAMAHA's PCY135/155.  
In the PCY135/155, the film switch value is divided by the voltage divider circuit. So, you need to set the Edge Threshold and Cup Threshold. Since the Cup switch has a higher value, set the Cup Threshold higher than the Edge.  
When I checked with the PCY-155  
    Cup Threshold : 4  
    Edge Threshold : 2       
it worked well.

    - Sensitivity (1 - 100)
    - Threshold (1 - 100)
    - Scan Time (1 - 100) milliseconds
    - Mask Time (1 - 100) milliseconds
    - Edge Threshold (1 - 100)
    - Cup Threshold (1 - 100)
    - Velocity Curve Type (0 - 4)

- Hihat Controller Sensing  
<img src="https://open-e-drums.com/images/sensing/hhc.png" width="800px">  
Whether it's the FSR, the infrared sensor (TCRT5000), or Roland's VH-10, they are the same.  
Record the time you started closing the hi-hat and the time you finished closing it. Then, the time it took to close the hi-hat is converted to the velocity. Only ScanStart and ScanEnd and PedalSensitivity are relevant to the velocity. The MIDI CC value can be set in the Sensitivity and Threshold. Velocity Curve here is only applied to MIDI CC values.  

    - Sensitivity (1 - 100)
    - Threshold (1 - 100)
    - Scan Start (1 - 100)
    - Scan End (1 - 100)
    - Pedal Sensitivity (1 - 100)
    - Velocity Curve Type (0 - 4)

## Velocity Curve

 - Velocity Curve Type  
 <img src="https://open-e-drums.com/images/sensing/curve.png" width="800px">  


## Circuit
- Single Piezo  
<img src="https://open-e-drums.com/images/circuit/074/singlePiezo.png" width="800px">

- Single Piezo with Potentiometer  
<img src="https://open-e-drums.com/images/circuit/074/singlePiezoPot.png" width="800px">

- Dual Piezo  
<img src="https://open-e-drums.com/images/circuit/074/dualPiezo.png" width="800px">  

- Roland PD series  
<img src="https://open-e-drums.com/images/circuit/074/PD.png" width="800px">

- FSR  
<img src="https://open-e-drums.com/images/circuit/074/FSR.png" width="800px">

- Roland VH-10/VH-11  
<img src="https://open-e-drums.com/images/circuit/074/VH.png" width="800px">

- TCRT5000  
<img src="https://open-e-drums.com/images/circuit/074/TCRT.png" width="800px">

- Cymbals  
<img src="https://open-e-drums.com/images/circuit/074/cymbal.png" width="800px">

- Roland PD-8  
<img src="https://open-e-drums.com/images/circuit/074/PD8.png" width="800px">

- Button  
<img src="https://open-e-drums.com/images/circuit/074/button.png" width="800px">

- LCD Keypad Shield  
<img src="https://open-e-drums.com/images/circuit/074/keypad.png" width="800px">

- Arduino UNO with MUX  
<img src="https://open-e-drums.com/images/circuit/MUX_uno.png" width="600px">

- ESP32 with MUX  
<img src="https://open-e-drums.com/images/circuit/MUX_esp32.png" width="600px">

## Pads

The STL data of pads from 6 inches to 12 inches, hi-hat controllers(<https://www.thingiverse.com/RyoKosaka/designs>)

## Release History


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
