# cc1101_rf_Gateway uC v1.0.15 with cc110x support 

### Getting started


System to receive digital signals and provide them to other systems for demodulation. Currently tested with 433 MHz, but not limited to that frequency or media.


Just clone the repo and open the project file with Visual Studio (only available for windows) or VSCode.
You can also open it with the Arduino IDE. 
Compile it and have fun.
If you are using the Arduino IDE, you have to copy all the libs into your sketch folder and modify some includes.

### Using cc1101_rf_Gateway in FHEM

1. check if you have the current SIGNALduino source in the update list
 (source: https://raw.githubusercontent.com/RFD-FHEM/RFFHEM/master/controls_signalduino.txt)
2. update your FHEM
3. define one SIGNALduino device
 (define <name> SIGNALduino <device>)
4. check the reading 'state' and if communication is successful it says 'opened'

### Tested microcontrollers

* Arduino Nano
* Arduino Pro Mini
* ESP32 (ESP32-WROOM-32 / ESP32-WROOM-32D)
* ESP8266
* RadinoCC1101


### You found a bug

First, sorry. This software is not perfect.
1. Open a issue
-With helpful title - use descriptive keywords in the title and body so others can find your bug (avoiding duplicates).
- Which branch, what microcontroller, what setup
- Steps to reproduce the problem, with actual vs. expected results
- If you find a bug in our code, post the files and the lines. 

### Contributing

1. Open one ore more issue for your development.
2. Ask to be added to our repository or just fork it.
3. Make your modifications and test them.
4. Create a branch (git checkout -b my_branch)
5. Commit your changes (git commit -am "<some description>")
6 .Push to a developer branch (git push dev-<xyz >my_branch)
7. Open a Pull Request, put some useful informations there, what your extension does and why we should add it, reference to the open issues which are fixed whith this pull requet.
