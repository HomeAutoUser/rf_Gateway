# cc1101_rf_Gateway uC v1.0.17pre with cc110x support 

### Getting started


System to receive digital signals and provide them to other systems for demodulation. A CC1101 is used as receiver. The cabling is analogue and compatible with the SIGNALduino. You can toggle up to 4 reception modes with a minimum time of 30 seconds. It can be operated via the WEB interface or the uC command (telnet). Standard operation is possible via FHEM. <br>Current reception modes are (values can be compiled in or compiled out yourself) :
  - CC110x Factory Default (standard values according to data sheet)
  - Avantek
  - Bresser_5in1
  - Bresser_6in1
  - Bresser_7in1
  - Fine_Offset_WH51_434
  - Fine_Offset_WH51_868
  - Fine_Offset_WH57_434
  - Fine_Offset_WH57_868
  - Inkbird_IBS_P01R
  - KOPP_FC
  - Lacrosse_mode1
  - Lacrosse_mode2
  - OOK - Modulation on 433MHz
  - PCA301
  - Rojaflex
  - X_Sense
  - ... some development´s


Just clone the repo and open the project file with Visual Studio (only available for windows) or VSCode.
You can also open it with the Arduino IDE. 
Compile it and have fun.
If you are using the Arduino IDE, you have to copy all the libs into your sketch folder and modify some includes.<br>
required libraries: [digitalWriteFast](https://www.arduino.cc/reference/en/libraries/digitalwritefast/), [WebSockets](https://www.arduino.cc/reference/en/libraries/websockets/)

### Using cc1101_rf_Gateway in FHEM

1. check if you have the current SIGNALduino source in the update list <br> ( source: https://raw.githubusercontent.com/RFD-FHEM/RFFHEM/master/controls_signalduino.txt )
2. update your FHEM
3. define one SIGNALduino device <br> ( define \<name\> SIGNALduino \<device\> )
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

  
### some pictures with WEB interface on a ESP8266 (WEB interface only on uC with WLAN)
  (some pictures | reception can be compiled differently)
  ![HOME](./screenshots/picture1.png)
  -
  ![CC110x](./screenshots/picture2.png)
  -
  ![CC110x register](./screenshots/picture3.png)
  -
  ![CC110x reception](./screenshots/picture4.png)
  -
  ![RAW data](./screenshots/picture5.png)
  -
  ![WLAN](./screenshots/picture6.png)
