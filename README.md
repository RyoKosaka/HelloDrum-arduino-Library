# HelloDrum-arduino-Library
This is a library for making E-Drum with Arduino.  
**Ver.0.5(5/21/2018)**

## Description

This is a library for making E-Drum with Arduino.  
By using it with Arduino MIDI Library, you can make E-drum.  
[Open E-Drums](https://open-e-drums.tumblr.com/)

## Features

- Single piezo pad, Dual Piezo pad, 2 zone cymbal, 3 zone cymbal
- It is compatible with YAMAHA's 3 zone cymbal.(PCY135/PCY155)
- Sensitivity, threshold, retrigger cancellation, note number can be set with each piezo. 
- Both optical(TCRT5000) and FSR type hi-hat controllers can be used.

## How to Use

[Check instruction.md](https://github.com/RyoKosaka/HelloDrum-arduino-Library/blob/master/instruction.md)

## Circuit
- Single Piezo
![sp](https://github.com/RyoKosaka/HelloDrum-arduino-Library/blob/master/circuit/singlePiezo.png)

- Dual Piezo
![dp](https://github.com/RyoKosaka/HelloDrum-arduino-Library/blob/master/circuit/dualPiezo.png)

- TCRT5000
![tcrt](https://github.com/RyoKosaka/HelloDrum-arduino-Library/blob/master/circuit/TCRT5000.png)

- YAMAHA PCY135/PCY155
![pcy](https://github.com/RyoKosaka/HelloDrum-arduino-Library/blob/master/circuit/YAMAHA_PCY135_155.png)


## Pads

The STL data of pads from 6 inches to 12 inches.(<https://www.thingiverse.com/RyoKosaka/designs>)

## Install

1. Download ZIP
2. Import a .zip Library (<https://www.arduino.cc/en/Guide/Libraries#toc4>)
3. Import Arduino MIDI Library (<https://playground.arduino.cc/Main/MIDILibrary>)

## Release History

* 0.1
    * Work in progress
* 0.5
   - Setting mode available
   - Display function by LCD is available
   - Saving function of setting items by EEPROM is available
   - Improved sensing of TCRT 5000 hi-hat controller

## Contributing

1. Fork it  
2. Create your feature branch
3. Commit your changes
4. Push to the branch
5. Create new Pull Request

## Author

[@tnctrekit](https://twitter.com/tnctrekit)  
[Works](https://www.tumblr.com/blog/ryokosaka)

## Licence

[MIT](http://opensource.org/licenses/mit-license.php)
