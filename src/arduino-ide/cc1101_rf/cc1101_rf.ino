/*
  Copyright (c) 2022, HomeAutoUser & elektron-bbs
  All rights reserved.

  - Arduino Nano OHNE debug´s | FreeRam -> 974
  Der Sketch verwendet 23612 Bytes (76%) des Programmspeicherplatzes. Das Maximum sind 30720 Bytes.
  Globale Variablen verwenden 678 Bytes (33%) des dynamischen Speichers, 1370 Bytes für lokale Variablen verbleiben. Das Maximum sind 2048 Bytes.

  - Arduino Pro / Arduino Pro Mini OHNE debug´s | FreeRam -> ?
  Der Sketch verwendet 23696 Bytes (77%) des Programmspeicherplatzes. Das Maximum sind 30720 Bytes.
  Globale Variablen verwenden 678 Bytes (33%) des dynamischen Speichers, 1370 Bytes für lokale Variablen verbleiben. Das Maximum sind 2048 Bytes.

  - Arduino radino CC1101 OHNE debug´s | FreeRam -> ?
  Der Sketch verwendet 25982 Bytes (90%) des Programmspeicherplatzes. Das Maximum sind 28672 Bytes.
  Globale Variablen verwenden 647 Bytes des dynamischen Speichers.

  - ESP8266 OHNE debug´s (alle Protokolle) | FreeRam -> 33296
  . Variables and constants in RAM (global, static), used 39316 / 80192 bytes (49%)
  ║   SEGMENT  BYTES    DESCRIPTION
  ╠══ DATA     1808     initialized variables
  ╠══ RODATA   4764     constants
  ╚══ BSS      32744    zeroed variables
  . Instruction RAM (IRAM_ATTR, ICACHE_RAM_ATTR), used 61555 / 65536 bytes (93%)
  ║   SEGMENT  BYTES    DESCRIPTION
  ╠══ ICACHE   32768    reserved space for flash instruction cache
  ╚══ IRAM     28787    code in IRAM
  . Code in flash (default, ICACHE_FLASH_ATTR), used 425252 / 1048576 bytes (40%)
  ║   SEGMENT  BYTES    DESCRIPTION
  ╚══ IROM     425252   code in flash

  - ESP32 OHNE debug´s (alle Protokolle) | FreeRam -> 198524
  Der Sketch verwendet 947149 Bytes (72%) des Programmspeicherplatzes. Das Maximum sind 1310720 Bytes.
  Globale Variablen verwenden 51352 Bytes (15%) des dynamischen Speichers, 276328 Bytes für lokale Variablen verbleiben. Das Maximum sind 327680 Bytes.

  - ein Register ca. 82 Bytes des Programmspeicherplatzes & 82 Bytes Globale Variablen (aktuell ca. 14 x 82 --> 1148 Bytes)

  commands:
  ?                 - available commands
  fafc<n>           - frequency automatic control | valid value 0 (off) or 1 (on)
  foff<n>           - frequency offset
  ft                - frequency testsignal SN;R=99;D=FF;
  m<0-6>            - register´s in Firmware (Avantek, Lacrosse, Bresser ...)
  m<arraysizemax>   - "Developer gadget to test functions"
  t                 - get uptime
  tob<0-3><0-6>     - set togglebank (0-3) to value | <n> bank , <n> mode or 99 | 99 reset togglebank n (set to -)
  tob88             - scan all modes
  tob99             - reset togglebank              | set to { - | - | - | - }
  tos<n>            - toggletime in milliseconds (min defined in config.h)
  x                 - write cc1101_patable
  C                 - reads complete CC110x register
  C<n>              - reads one register adress (0-61)
  C99               - get ccreg99
  C3E               - get ccpatable
  C0DnF             - get ccconf
  E                 - reads complete EEPROM
  I                 - current status
  M                 - all available registers (mode´s)
  P                 - PING
  R                 - get FreeRam
  SN                - send data
  V                 - get Version
  W                 - write register
  WS                - write Strobes Command

  EEPROM addresses:
    0 -  34 (46)    - CC110x - register values ​​for reception (protocol)
   48               - CC110x - activated protocol - array value -> activated_mode_nr
   49 -  52         - ToggleProtocols
   53               - FW - identifier 1 -> DE
   54               - FW - identifier 2 -> 22
   56 -  59         - ToggleTime (value / 100 -> max 255000 milliseconds -> max 4 1/4 minutes)
   60 -  91         - WIFI SSID
   92 - 155         - WIFI Passwort
  156 - 159         - WIFI Adr IP
  160 - 163         - WIFI Adr Gateway
  164 - 167         - WIFI Adr DNS
  168 - 171         - WIFI Adr NetMask
  172               - WIFI DHCP
  173               - WIFI AP
  174 - 182         - CC110x - PATABLE
  183 - 184         - FW - Checksum over packet_length
  185 - 188         - CC110x - freq Offset
  189               - CC110x afc on/off

  notes ToDO:
  - ? wenn dev Protokolle aktiv, diese bei ScanAll überspringen
  - ? MQTT Weiterleitung
  - ? Logfile Systemstart
*/

#include <Arduino.h>
#include <digitalWriteFast.h>           // https://github.com/ArminJo/digitalWriteFast
#include "config.h"
int RSSI_dez;                           // for the output on web server
#include "cc110x.h"
#include "macros.h"
#include "functions.h"
#include "register.h"

/* Settings for OOK messages without Sync Pulse (MU) */
#define MsgLenMin               24      // message minimum length
#define MsgLenMax               254     // message maximum length
#define PatMaxCnt               8       // pattern, maximum number (number 8 -> FHEM SIGNALduino compatible)
#define PatTol                  0.20    // pattern tolerance
#define FIFO_LENGTH             90      // 90 from SIGNALduino FW
#include "SimpleFIFO.h"
SimpleFIFO<int16_t, FIFO_LENGTH> FiFo;  // store FIFO_LENGTH # ints

/* --- all SETTINGS for the ESP8266 ----------------------------------------------------------------------------------------------------------- */
#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
ESP8266WebServer HttpServer(80);
ADC_MODE(ADC_VCC);                  // vcc read
#endif
/* --- END - all SETTINGS for the ESP8266 ----------------------------------------------------------------------------------------------------- */

/* --- all SETTINGS for the ESP32-- ----------------------------------------------------------------------------------------------------------- */
#ifdef ARDUINO_ARCH_ESP32
#include <WebServer.h>
#include <WiFi.h>
#include <esp_wps.h>
WebServer HttpServer(80);
#endif
/* --- END - all SETTINGS for the ESP32-- ----------------------------------------------------------------------------------------------------- */

/* --- all SETTINGS for the ESP8266 and ESP32 ------------------------------------------------------------------------------------------------- */
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
#include <ArduinoOTA.h>
#include <WebSocketsServer.h>
#define WEBSOCKETS_SAVE_RAM              // moves all Header strings to Flash (~300 Byte)
//#define WEBSOCKETS_SERVER_CLIENT_MAX  2  // default 5
WebSocketsServer webSocket = WebSocketsServer(81);
String webSocketSite[WEBSOCKETS_SERVER_CLIENT_MAX] = {};

/* https://unsinnsbasis.de/littlefs-esp32/
   https://unsinnsbasis.de/littlefs-esp32-teil2/

   https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html
   https://randomnerdtutorials.com/install-esp8266-nodemcu-littlefs-arduino/
   https://42project.net/esp8266-webserverinhalte-wie-bilder-png-und-jpeg-aus-dem-internen-flash-speicher-laden/

  https://github.com/lorol/LITTLEFS/issues/43#issuecomment-1537234709

  https://github.com/lorol/LITTLEFS --> Ths library is now part of Arduino esp32 core v2

  ESP32 Bibliothek -> LittleFS_esp32
*/

#include <LittleFS.h>
WiFiServer TelnetServer(TELNET_PORT);
WiFiClient TelnetClient[TELNET_CLIENTS_MAX];

byte TELNET_CLIENTS_ARRAY[TELNET_CLIENTS_MAX];
byte TELNET_CLIENT_COUNTER = 0;
bool TELNET_ConnectionEstablished;            // Telnet Flag for successfully handled connection
byte WLAN_reco_cnt = 0;                       // counter for connection, if cnt 3, WLAN jump zu AP
const char* ssid_ap = WLAN_ssid_ap;
const char* password_ap = WLAN_password_ap;
String OwnStationHostname = WLAN_hostname;
String used_ssid;                             // for the output on web server
String used_ssid_mac;                         // for the output on web server
String used_ssid_pass;                        // for the output on web server
byte used_dhcp = 0;                           // IP-Adresse mittels DHCP
boolean WLAN_AP = false;                      // WiFi AP opened
boolean WLAN_OK = false;                      // WiFi connected
uint8_t WifiNetworks;                         // Anzahl Wifi-Netzwerke

IPAddress eip;                                // static IP - IP-Adresse
IPAddress esnm;                               // static IP - Subnetzmaske
IPAddress esgw;                               // static IP - Standard-Gateway
IPAddress edns;                               // static IP - Domain Name Server
/* --- END - all SETTINGS for the ESP8266 and ESP32 ------------------------------------------------------------------------------------------- */
#else
#define ICACHE_RAM_ATTR
#define FPSTR String
#endif

/* varible´s for output */
const char compile_date[] = __DATE__ " " __TIME__;

#ifdef SIGNALduino_comp
/*  SIGNALduino helpful information !!!
    1) for sduino compatible need version the true value from RegEx check with -> [V\s.*SIGNAL(?:duino|ESP|STM).*(?:\s\d\d:\d\d:\d\d)]
    2) version output must have cc1101 -> check in 00_SIGNALduino.pm
    3) output xFSK RAW msg must have format MN;D=9004806AA3;R=52;
*/

static const char PROGMEM TXT_VERSION[] = "V 1.17pre SIGNALduino compatible cc1101_rf_Gateway (Websocket) "; // PROGMEM used 40004
byte CC1101_writeReg_offset = 2; // stimmt das noch?
#else
static const char PROGMEM TXT_VERSION[] = "V 1.17pre cc1101_rf_Gateway (Websocket) ";
byte CC1101_writeReg_offset = 0;
#endif

/* varible´s for Toggle */
byte ToggleOrder[4] = { 255, 255, 255, 255 }; /* Toggle, order */
byte ToggleValues = 0;                        /* Toggle, registervalue */
byte ToggleCnt = 0;                           /* Toggle, register counter for loop */
boolean ToggleAll = false;                    /* Toggle, all (scan modes) */
unsigned long ToggleTime = 0;                 /* Toggle, Time in ms (0 - 4294967295) */

/* Settings for OOK messages without Sync Pulse (MU) */
#define t_maxP 32000                          // Zeitdauer maximum für gültigen Puls in µs
#define t_minP 75                             // Zeitdauer minimum für gültigen Puls in µs
unsigned long lastTime = 0;                   // Zeit, letzte Aktion
int16_t ArPaT[PatMaxCnt];                     // Pattern Array für Zeiten
signed long ArPaSu[PatMaxCnt];                // Pattern Summe, aller gehörigen Pulse
byte ArPaCnt[PatMaxCnt];                      // Pattern Counter, der Anzahl Pulse
byte PatNmb = 0;                              // Pattern aktuelle Nummer 0 - 9
byte MsgLen;                                  // ToDo, kann ggf ersetzt werden durch message.valcount
int16_t first;                                // Pointer to first buffer entry
int16_t last;                                 // Pointer to last buffer entry
String msg;                                   // RAW (only serial/telnet, not HTML output) & Serial Input
byte TiOv = 0;                                // Marker - Time Overflow (SIGNALduino Kürzel p; )
byte PatMAX = 0;                              // Marker - maximale Pattern erreicht und neuer unbekannter würde folgen (SIGNALduino Kürzel e; )
byte MOD_FORMAT;                              // Marker - Modulation
byte FSK_RAW;                                 // Marker - FSK Modulation RAW interrupt
bool valid;

/* predefinitions of the functions */
inline void doDetect();
void Interupt_Variant(byte nr);
void MSGBuild();
void PatReset();
void decode(const int pulse);
void findpatt(int val);
/* END - Settings for OOK messages without Sync Pulse (MU) - END */

/* varible´s for other */
#define BUFFER_MAX 70                         /* !!! maximum number of characters to send !!! */
int8_t freqErr = 0;                           /* for automatic Frequency Synthesizer Control */
int8_t freqOffAcc = 0;                        /* for automatic Frequency Synthesizer Control */
float freqErrAvg = 0;                         /* for automatic Frequency Synthesizer Control */
boolean freqAfc = 0;                          /* AFC on or off */
uint32_t msgCount = 0;                        /* message counter over all received messages */
byte client_now;
unsigned long secTick = 0;                    /* time that the clock last "ticked" */
unsigned long toggleTick = 0;
unsigned long uptime = 0;

/* now all websites, all settings are available here */
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
#include "websites.h"
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
#endif


/* void predefinitions */
void InputCommand(char* buf_input);
void ToggleOnOff();

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
void Telnet();
#endif


/* --------------------------------------------------------------------------------------------------------------------------------- void Interupt */
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
IRAM_ATTR void Interupt() {     /* Pulseauswertung */
#else
void Interupt() {
#endif
  if (MOD_FORMAT != 3) { /* not OOK */
    FSK_RAW = 1;
  } else { /* OOK */
    const unsigned long Time = micros();
    const signed long duration = Time - lastTime;
    lastTime = Time;
    if (duration >= t_minP) {       // kleinste zulässige Pulslänge
      int sDuration;
      if (duration < t_maxP) {      // größte zulässige Pulslänge, max = 32000
        sDuration = int(duration);  // das wirft bereits hier unnötige Nullen raus und vergrössert den Wertebereich
      } else {
        sDuration = t_maxP;         // Maximalwert
      }
      if (digitalReadFast(GDO2)) {  // Wenn jetzt high ist, dann muss vorher low gewesen sein, und dafuer gilt die gemessene Dauer.
        sDuration = -sDuration;
      }
      FiFo.enqueue(sDuration);      // add an sDuration
    }                               // else => trash
  }
}

/* --------------------------------------------------------------------------------------------------------------------------------- void Interupt_Variant */
void Interupt_Variant(byte nr) {
  CC1101_cmdStrobe(CC1101_SIDLE); /* Exit RX / TX, turn off frequency synthesizer and exit Wake-On-Radio mode if applicable */
  CC1101_writeRegFor(Registers[nr].reg_val, Registers[nr].length, Registers[nr].name);
  CC1101_cmdStrobe(CC1101_SFRX);  /* Flush the RX FIFO buffer. Only issue SFRX in IDLE or RXFIFO_OVERFLOW states */

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  WebSocket_cc110x();             /* WebSocket Verarbeitung */
  WebSocket_cc110x_detail();
  WebSocket_cc110x_modes();
#else
  delay(10);
#endif

  MOD_FORMAT = ( CC1101_readReg(0x12, READ_BURST) & 0b01110000 ) >> 4;
  if (MOD_FORMAT != 3) {
    attachInterrupt(digitalPinToInterrupt(GDO2), Interupt, RISING); /* "Bei steigender Flanke auf dem Interruptpin" --> "Führe die Interupt Routine aus" */
  } else {
    attachInterrupt(digitalPinToInterrupt(GDO2), Interupt, CHANGE); /* "Bei wechselnder Flanke auf dem Interruptpin" --> "Führe die Interupt Routine aus" */
  }
  CC1101_cmdStrobe(CC1101_SRX);   /* Enable RX. Perform calibration first if coming from IDLE and MCSM0.FS_AUTOCAL=1 */
}

/* --------------------------------------------------------------------------------------------------------------------------------- void setup */
void setup() {
  msg.reserve(255);
  Serial.begin(SerialSpeed);
  Serial.setTimeout(Timeout_Serial); /* sets the maximum milliseconds to wait for serial data. It defaults to 1000 milliseconds. */
  Serial.println();

  while (!Serial) {
    ; /* wait for serial port to connect. Needed for native USB */
  }

  pinMode(GDO0, OUTPUT);
  digitalWriteFast(GDO0, LOW);
  pinMode(GDO2, INPUT);
  pinMode(LED, OUTPUT);
  digitalWriteFast(LED, LOW);

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32) /* code for ESP8266 and ESP32 */
  /* interner Flash-Speicher */
  if (!LittleFS.begin()) {
#if defined debug
    Serial.println(F("LittleFS mount failed, formatting filesystem"));
#endif
    LittleFS.format();
  } else {
#if defined debug
    Serial.println(F("Starting LittleFS"));
#endif
  }
  File logfile = LittleFS.open("/files/log.txt", "w"); /* Datei mit Schreibrechten öffnen, wird erstellt wenn nicht vorhanden */
  if (logfile) {
    String logText = String(uptime);
    logText += F(" - Systemstart (");
#if defined(ARDUINO_ARCH_ESP8266)
    logText += ESP.getResetReason();
#endif
#if defined(ARDUINO_ARCH_ESP32)
    esp_reset_reason_t reset_reason = esp_reset_reason();
    logText += reset_reason;
    logText += F(" - ");
    switch (reset_reason) {
      case ESP_RST_UNKNOWN:
        logText += F("UNKNOWN");
        break;
      case ESP_RST_POWERON:
        logText += F("POWERON");
        break;
      case ESP_RST_EXT:
        logText += F("external pin");
        break;
      case ESP_RST_SW:
        logText += F("software restart");
        break;
      case ESP_RST_PANIC:
        logText += F("software exception/panic");
        break;
      case ESP_RST_INT_WDT:
        logText += F("interrupt watchdog");
        break;
      case ESP_RST_TASK_WDT:
        logText += F("task watchdog");
        break;
      case ESP_RST_WDT:
        logText += F("other watchdog");
        break;
      case ESP_RST_DEEPSLEEP:
        logText += F("after deep sleep");
        break;
      case ESP_RST_BROWNOUT:
        logText += F("brownout");
        break;
      case ESP_RST_SDIO:
        logText += F("SDIO");
        break;
    }
#endif
    logText += ')';
    logfile.println(logText);
    logfile.close(); /* Schließen der Datei */
  }

  EEPROM.begin(EEPROM_SIZE); /* Puffergröße die verwendet werden soll */
  eip = EEPROMread_ipaddress(EEPROM_ADDR_IP);
  esnm = EEPROMread_ipaddress(EEPROM_ADDR_NETMASK);
  esgw = EEPROMread_ipaddress(EEPROM_ADDR_GATEWAY);
  edns = EEPROMread_ipaddress(EEPROM_ADDR_DNS);
  used_dhcp = EEPROMread(EEPROM_ADDR_DHCP);
  if (used_dhcp > 1) {
    used_dhcp = 1;
    EEPROMwrite(EEPROM_ADDR_DHCP, 1);
  }

#ifdef debug
  Serial.print(F("DB setup, read EEPROM - WIFI SSID         ")); Serial.println(EEPROMread_string(EEPROM_ADDR_SSID));
  Serial.print(F("DB setup, read EEPROM - WIFI Passwort     ")); Serial.println(EEPROMread_string(EEPROM_ADDR_PASS));
  Serial.print(F("DB setup, read EEPROM - WIFI DHCP         ")); Serial.println(EEPROMread(EEPROM_ADDR_DHCP));
  Serial.print(F("DB setup, read EEPROM - WIFI AP           ")); Serial.println(EEPROMread(EEPROM_ADDR_AP));
  Serial.print(F("DB setup, read EEPROM - WIFI Adr IP       ")); Serial.println(eip);
  Serial.print(F("DB setup, read EEPROM - WIFI Adr Gateway  ")); Serial.println(esgw);
  Serial.print(F("DB setup, read EEPROM - WIFI Adr DNS      ")); Serial.println(edns);
  Serial.print(F("DB setup, read EEPROM - WIFI Adr NetMask  ")); Serial.println(esnm);
#endif

  OwnStationHostname.replace("_", "-"); /* Unterstrich ersetzen, nicht zulässig im Hostnamen */
  if (EEPROMread(EEPROM_ADDR_AP) == 255 || EEPROMread(EEPROM_ADDR_AP) == 1) {
    start_WLAN_AP(ssid_ap, password_ap);
  }
  if (EEPROMread(EEPROM_ADDR_AP) == 0) {
    start_WLAN_STATION(EEPROMread_string(EEPROM_ADDR_SSID), EEPROMread_string(EEPROM_ADDR_PASS));
  }

  /* Arduino OTA Update – Update über WLAN */
#ifdef debug_wifi
  ArduinoOTA.onStart([]() {
    Serial.println(F("OTA - Start"));
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA - Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println(F("OTA - Auth Failed"));
    else if (error == OTA_BEGIN_ERROR) Serial.println(F("OTA - Begin Failed"));
    else if (error == OTA_CONNECT_ERROR) Serial.println(F("OTA - Connect Failed"));
    else if (error == OTA_RECEIVE_ERROR) Serial.println(F("OTA - Receive Failed"));
    else if (error == OTA_END_ERROR) Serial.println(F("OTA - End Failed"));
  });
  Serial.println(F("Starting OTA"));
#endif
  ArduinoOTA.begin();

#ifdef debug_wifi
  Serial.println(F("Starting TelnetServer"));
#endif
  TelnetServer.begin();
  TelnetServer.setNoDelay(true);

#ifdef debug_wifi
  Serial.println(F("Starting HttpServer"));
#endif
  routing_websites(); /* load all routes to site´s */
  HttpServer.begin();

#ifdef debug_wifi
  Serial.println(F("Starting WebSocket"));
#endif
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
#endif

#ifdef debug
#if defined(ARDUINO_AVR_NANO) || defined(ARDUINO_RADINOCC1101) || defined(ARDUINO_AVR_PRO)
  Serial.println(F("-> found board without WLAN"));
#elif defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32) /* code for ESP8266 and ESP32 */
  Serial.println(F("-> found board with WLAN"));
#else
  Serial.println(F("-> found unknown board"));
#endif    // END microcontrollers
#endif    // END debug

  CC1101_init();
  toggleTick = ToggleTime;
  if (ToggleTime == 0) { // switch to last activated receiving mode
    activated_mode_packet_length = Registers[activated_mode_nr].packet_length;
    Interupt_Variant(activated_mode_nr);    // set receive variant & register
  }
}

/* --------------------------------------------------------------------------------------------------------------------------------- void setup end */

void loop() {
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  /* https://arduino-esp8266.readthedocs.io/en/3.1.2/reference.html#timing-and-delays
     delay(ms) pauses the sketch for a given number of milliseconds and allows WiFi and TCP/IP tasks to run. */
  delay(1);

  ArduinoOTA.handle();  /* OTA Updates */
  Telnet();             /* Telnet Input´s */
  HttpServer.handleClient();
  webSocket.loop();     /* 53 µS */
#endif

  if ((millis() - secTick) >= 1000UL) { // jede Sekunde
    secTick += 1000UL;
    uptime++;
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
    WebSocket_index(); // Dauer: ohne connect ca. 100 µS, 1 Client ca. 1700 µS, 2 Clients ca. 2300 µS
#endif
    if (ToggleTime > 0) { /* Toggle Option */
      if (millis() - toggleTick > ToggleTime) { /* Abfragen, ob Zeit zum Einschalten erreicht */
        toggleTick = millis();                 /* Zeit merken, an der Eingeschaltet wurde. */
        ToggleOnOff();
      }
    }
  }

  if (Serial.available() > 0) { /* Serial Input´s */
    msg = Serial.readString();
    msg.trim();                                           /* String, strip off any leading/trailing space and \r \n */
    char BUFFER_Serial[msg.length() + 1];
    msg.toCharArray(BUFFER_Serial, msg.length() + 1);     /* String to char in buf */

    if (msg.length() > 0 && msg.length() <= BUFFER_MAX) {
#ifdef debug
#ifdef CODE_AVR
      Serial.print(F("DB loop, Serial.available > 0 ")); Serial.println(BUFFER_Serial);
#elif CODE_ESP
      MSG_OUTPUT(F("DB loop, Serial.available > 0 ")); MSG_OUTPUTLN(BUFFER_Serial);
#endif
#endif
      client_now = 255;                                     /* current client is set where data is received */
      InputCommand(BUFFER_Serial);
    }
  }

  /* only for test !!! HangOver ??? */
  //#ifdef debug_cc110x_ms
  //MSG_OUTPUTALL(F("DB CC1101_MARCSTATE ")); MSG_OUTPUTALLLN(CC1101_readReg(CC1101_MARCSTATE, READ_BURST), HEX); /* MARCSTATE – Main Radio Control State Machine State */
  //#endif

  /* not OOK */
  if ( (FSK_RAW == 1) && (CC1101_found == true) ) { /* Received data | RX (not OOK !!!) */
    FSK_RAW = 0;
    digitalWriteFast(LED, HIGH);    /* LED on */
    int rssi = CC1101_readRSSI();
    freqErr = CC1101_readReg(CC1101_FREQEST, READ_BURST);  // 0x32 (0xF2): FREQEST – Frequency Offset Estimate from Demodulator
    msgCount++;
    if (freqAfc == 1) {
      freqErrAvg = freqErrAvg - float(freqErrAvg / 8.0) + float(freqErr / 8.0);  // Mittelwert über Abweichung
      freqOffAcc += round(freqErrAvg);
      CC1101_writeReg(CC1101_FSCTRL0, freqOffAcc);  // 0x0C: FSCTRL0 – Frequency Synthesizer Control
    }
    msg = "";
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
    String html_raw = "";        // for the output on web server
    html_raw.reserve(128);
#endif
#ifdef debug_cc110x_ms    /* MARCSTATE – Main Radio Control State Machine State */
#ifdef CODE_AVR
    Serial.print(F("DB CC1101_MARCSTATE ")); Serial.println(CC1101_readReg(CC1101_MARCSTATE, READ_BURST), HEX);
#elif CODE_ESP
    MSG_OUTPUTALL(F("DB CC1101_MARCSTATE ")); MSG_OUTPUTALLLN(CC1101_readReg(CC1101_MARCSTATE, READ_BURST), HEX);
#endif
#endif
    uint8_t uiBuffer[activated_mode_packet_length]; // Array anlegen
    CC1101_readBurstReg(uiBuffer, CC1101_RXFIFO, activated_mode_packet_length); /* read data from FIFO */
#ifdef SIGNALduino_comp
#ifdef CODE_AVR
    Serial.print(char(2));     // STX
    Serial.print(F("MN;D="));  // "MN;D=" | "data: "
#elif CODE_ESP
    msg += char(2);     // STX
    msg += F("MN;D=");  // "MN;D=" | "data: "
#endif
#else
#ifdef CODE_AVR
    Serial.print(F("data: ")); // "MN;D=" | "data: "
#elif CODE_ESP
    msg += F("data: "); // "MN;D=" | "data: "
#endif
#endif
    for (byte i = 0; i < activated_mode_packet_length; i++) { /* RawData */
#ifdef CODE_AVR
      Serial.print(onlyDecToHex2Digit(uiBuffer[i]));
#elif CODE_ESP
      msg += onlyDecToHex2Digit(uiBuffer[i]);
#endif
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
      html_raw += onlyDecToHex2Digit(uiBuffer[i]);
#endif
    }
#ifdef SIGNALduino_comp
#ifdef CODE_AVR
    Serial.print(F(";R="));        // ";R=" | "; RSSI="
#elif CODE_ESP
    msg += F(";R=");        // ";R=" | "; RSSI="
#endif
#else
#ifdef CODE_AVR
    Serial.print(F(" RSSI="));     // ";R=" | "; RSSI="
#elif CODE_ESP
    msg += F(" RSSI=");     // ";R=" | "; RSSI="
#endif
#endif

#ifdef CODE_AVR
    Serial.print(rssi);
#elif CODE_ESP
    msg += rssi;
#endif

#ifdef SIGNALduino_comp
#ifdef CODE_AVR
    Serial.print(F(";A="));        // ";A=" | "; FREQAFC="
#elif CODE_ESP
    msg += F(";A=");        // ";A=" | "; FREQAFC="
#endif
#else
#ifdef CODE_AVR
    Serial.print(F("; FREQAFC=")); // ";A=" | "; FREQAFC="
#elif CODE_ESP
    msg += F("; FREQAFC="); // ";A=" | "; FREQAFC="
#endif
#endif

#ifdef CODE_AVR
    Serial.print(freqErr); Serial.print(';');
#elif CODE_ESP
    msg += freqErr; msg += ';';
#endif

#ifdef SIGNALduino_comp
#ifdef CODE_AVR
    Serial.print(char(3));         // ETX
#elif CODE_ESP
    msg += char(3);         // ETX
#endif
#else
#ifdef CODE_AVR
    Serial.print(char(13));        // CR
#elif CODE_ESP
    msg += char(13);        // CR
#endif
#endif

#ifdef CODE_AVR
    Serial.print(char(10));        // LF
#elif CODE_ESP
    msg += char(10);        // LF
    MSG_OUTPUTALL(msg); /* output msg to all */
#endif

    //Serial.println(CC1101_readReg(CC1101_MARCSTATE, READ_BURST), HEX);
#ifdef debug_cc110x_ms    /* MARCSTATE – Main Radio Control State Machine State */
#ifdef CODE_AVR
    Serial.print(F("DB CC1101_MARCSTATE ")); Serial.println(CC1101_readReg(CC1101_MARCSTATE, READ_BURST), HEX);
#elif CODE_ESP
    MSG_OUTPUTALL(F("DB CC1101_MARCSTATE ")); MSG_OUTPUTALLLN(CC1101_readReg(CC1101_MARCSTATE, READ_BURST), HEX);
#endif
#endif
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
    WebSocket_raw(html_raw); // Dauer: kein client ca. 100 µS, 1 client ca. 900 µS, 2 clients ca. 1250 µS
#endif
    if (CC1101_readReg(CC1101_MARCSTATE, READ_BURST) == 0x11) { // RXFIFO_OVERFLOW
      CC1101_cmdStrobe(CC1101_SFRX);
    }
    CC1101_cmdStrobe(CC1101_SRX);
    for (uint8_t i = 0; i < 255; i++) {
      if (CC1101_readReg(CC1101_MARCSTATE, READ_BURST) == 0x0D) { // RX
        break;
      }
      delay(1);
      if (i == 254) {
#ifdef CODE_AVR
        Serial.println(F("loop, ERROR read CC1101_MARCSTATE, READ_BURST !"));
#elif CODE_ESP
        MSG_OUTPUTALLLN(F("loop, ERROR read CC1101_MARCSTATE, READ_BURST !"));
#endif
      }
    }
    digitalWriteFast(LED, LOW); /* LED off */
  } else {
    /* OOK */
    while (FiFo.count() > 0 ) {         // Puffer auslesen und an Dekoder uebergeben
      int aktVal = FiFo.dequeue();      // get next element
      decode(aktVal);
    }
  }
}

/* --------------------------------------------------------------------------------------------------------------------------------- void loop end */


/*
   ################################
   Function´s with macro dependency
   ################################
*/

void ToggleOnOff() {
  detachInterrupt(digitalPinToInterrupt(GDO2));
#ifdef debug
#ifdef CODE_AVR
  Serial.print(F("DB Toggle | ToggleCnt=")); Serial.print((ToggleCnt + 1)); Serial.print(F(" ToggleValues=")); Serial.println(ToggleValues);
#elif CODE_ESP
  String tmp = F("DB Toggle | ToggleCnt="); tmp += (ToggleCnt + 1); tmp += F(" ToggleValues="); tmp += ToggleValues;
  MSG_OUTPUTLN(tmp);
#endif
#endif

  if (ToggleAll == true) {
    activated_mode_nr = ToggleCnt + 1;
    ToggleValues = RegistersCntMax - 1;
#ifdef debug
#ifdef CODE_AVR
    Serial.print(F("DB Toggle | ToggleAll activated_mode_nr ")); Serial.print(activated_mode_nr); Serial.print(F(", ToggleValues ")); Serial.println(ToggleValues);
#elif CODE_ESP
    tmp = F("DB Toggle | ToggleAll activated_mode_nr "); tmp += activated_mode_nr; tmp += F(", ToggleValues "); tmp += ToggleValues;
    MSG_OUTPUTLN(tmp);
#endif
#endif
  } else {
    if (ToggleValues <= 1) {
      ToggleTime = 0;
#ifdef debug
#ifdef CODE_AVR
      Serial.println(F("Toggle STOPPED, no toggle values in togglebank!"));
#elif CODE_ESP
      MSG_OUTPUTLN(F("Toggle STOPPED, no toggle values in togglebank!"));
#endif
#endif
      return;
    }
    activated_mode_nr = ToggleOrder[ToggleCnt];
  }

#ifdef debug
#ifdef CODE_AVR
  Serial.print(F("Toggle (output all)    | switched to ")); Serial.println(Registers[activated_mode_nr].name);
#elif CODE_ESP
  tmp = F("Toggle (output all)    | switched to "); tmp += Registers[activated_mode_nr].name;
  MSG_OUTPUTLN(tmp);
#endif
#endif

  activated_mode_packet_length = Registers[activated_mode_nr].packet_length;
  Interupt_Variant(activated_mode_nr);    // set receive variant & register
  ToggleCnt++;
  if (ToggleCnt >= ToggleValues) {
    ToggleCnt = 0;
  }
}


void InputCommand(char* buf_input) { /* all InputCommand´s , String | Char | marker, 255 = Serial | 0...254 = Telnet */
  String input = "";      // for all inputs
#ifdef CODE_ESP
  String tmp = "";        // for temp outputs print
#endif
  uint8_t uiBuffer[47];   // Array anlegen

  for (byte i = 0; i < strlen(buf_input); i++) {
    input += buf_input[i];
  }
  input += "\0";

#ifdef debug
#ifdef CODE_AVR
  Serial.print(F("DB InputCommand, String ")); Serial.println(input);

  for (byte i = 0; i < input.length(); i++) {
    Serial.print(F("DB InputCommand [")); Serial.print(i); Serial.print(F("] = ")); Serial.println(buf_input[i]);
  }
#elif CODE_ESP
  tmp += F("DB InputCommand, String "); tmp += input;
  MSG_OUTPUTLN(tmp);

  for (byte i = 0; i < input.length(); i++) {
    tmp = F("DB InputCommand ["); tmp += i; tmp += F("] = "); tmp += buf_input[i];
    MSG_OUTPUTLN(tmp);
  }
#endif
#endif

  switch (buf_input[0]) { /* integrated ? m t tob tos x C C<n><n> C99 CG C3E C0DnF I M P R V W<n><n><n><n> WS<n><n> */
    case '?':             /* command ? */
      if (!buf_input[1]) {
#ifdef CODE_AVR
        Serial.println(F("? Use one of fafc, foff, ft, m, t, tob, tos, x, C, C3E, CG, E, I, M, P, R, SN, V, W, WS"));
#elif CODE_ESP
        MSG_OUTPUTLN(F("? Use one of fafc, foff, ft, m, t, tob, tos, x, C, C3E, CG, E, I, M, P, R, SN, V, W, WS"));
#endif
      }
      break;                                                                                          /* -#-#-#-#- - - next case - - - #-#-#-#- */
    case 'f':                                                                                         /* command f */
      if (buf_input[1] == 'o' && buf_input[2] == 'f' && buf_input[3] == 'f') { /* command foff<n> */
        if (isNumeric(input.substring(4)) == 1) {
          Freq_offset = input.substring(4).toFloat();
          if (Freq_offset > 10.0 || Freq_offset < -10.0) {
            Freq_offset = 0;
#ifdef CODE_AVR
            Serial.println(F("CC110x_Freq.Offset resets (value is not range -10 to 10 MHz)"));
#elif CODE_ESP
            MSG_OUTPUTLN(F("CC110x_Freq.Offset resets (value is not range -10 to 10 MHz)"));
#endif
          }
          EEPROM.put(EEPROM_ADDR_FOFFSET, Freq_offset);
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
          EEPROM.commit();
#endif
          CC1101_writeReg(CC1101_FSCTRL0, 0);  // 0x0C: FSCTRL0 – Frequency Synthesizer Control
#ifdef CODE_AVR
          Serial.print(F("CC110x_Freq.Offset saved to ")); Serial.print(Freq_offset); Serial.println(F(" MHz"));
#elif CODE_ESP
          tmp = F("CC110x_Freq.Offset saved to "); tmp += Freq_offset; tmp += F(" MHz");
#endif
        } else {
#ifdef CODE_AVR
          Serial.println(F("CC110x_Freq.Offset value is not nummeric"));
#elif CODE_ESP
          tmp = F("CC110x_Freq.Offset value is not nummeric");
#endif
        }
#ifdef CODE_ESP
        MSG_OUTPUTLN(tmp);
#endif
      }

      if (buf_input[1] == 'a' && buf_input[2] == 'f' && buf_input[3] == 'c' && !buf_input[5]) { /* command fafc<n> */
        CC1101_writeReg(CC1101_FSCTRL0, 0);                                                     // 0x0C: FSCTRL0 – Frequency Synthesizer Control

        if (buf_input[4] == '0') { /* command fafc0 */
#ifdef CODE_AVR
          Serial.println(F("CC110x_Frequency, Afc off"));
#elif CODE_ESP
          tmp = F("CC110x_Frequency, Afc off");
#endif
          freqAfc = 0;
        }
        if (buf_input[4] == '1') { /* command fafc1 */
#ifdef CODE_AVR
          Serial.println(F("CC110x_Frequency, Afc on"));
#elif CODE_ESP
          tmp = F("CC110x_Frequency, Afc on");
#endif
          freqAfc = 1;
        }
#ifdef CODE_ESP
        MSG_OUTPUTLN(tmp);
#endif
        EEPROMwrite(EEPROM_ADDR_AFC, freqAfc);
      }

      if (buf_input[1] == 't' && !buf_input[2]) { /* command ft */
#ifdef CODE_AVR
        Serial.println(F("CC110x_Frequency, send testsignal"));
#elif CODE_ESP
        MSG_OUTPUTLN(F("CC110x_Frequency, send testsignal"));
#endif
        input = F("SN;D=AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA;R=50;");
        char In[input.length() + 1];
        input.toCharArray(In, input.length() + 1); /* String to char in buf */
        InputCommand(In);
        input = "";
      }
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */
    case 'm': /* command m */
      if (buf_input[1]) {
        /* command m<n>, check of decimal */
        if (isNumeric(input.substring(1)) == 1) {
          detachInterrupt(digitalPinToInterrupt(GDO2));
          ToggleAll = false;
          ToggleTime = 0;                                       // beendet Toggle, sonst Absturz nach tob88 und anschließend m2 wenn ToggleCnt > 3
          byte int_substr1_serial = input.substring(1).toInt(); /* everything after m to byte (old String) */
          if (int_substr1_serial <= RegistersCntMax) {
            /* "Normal Option" - set all Register value´s */
            if (int_substr1_serial <= (RegistersCntMax - 1)) {
              if (CC1101_found == false) {
#ifdef CODE_AVR
                Serial.println(F("Mode not settable (no CC1101 found)"));
#elif CODE_ESP
                MSG_OUTPUTLN(F("Mode not settable (no CC1101 found)"));
#endif
              } else {
                if (int_substr1_serial == 0) {
                  Freq_offset = 0;  // reset FrequencOffset
                }
#ifdef CODE_AVR
                Serial.print(F("set register to ")); Serial.println(Registers[int_substr1_serial].name);
#elif CODE_ESP
                tmp = F("set register to "); tmp += Registers[int_substr1_serial].name;
                MSG_OUTPUTLN(tmp);
#endif
                activated_mode_name = Registers[int_substr1_serial].name;
                activated_mode_nr = int_substr1_serial;
                activated_mode_packet_length = Registers[int_substr1_serial].packet_length;
                Interupt_Variant(int_substr1_serial);

#ifdef debug_cc110x_ms    /* MARCSTATE – Main Radio Control State Machine State */
#ifdef CODE_AVR
                Serial.print(F("DB CC1101_MARCSTATE ")); Serial.println(CC1101_readReg(CC1101_MARCSTATE, READ_BURST), HEX);
#elif CODE_ESP
                MSG_OUTPUTALL(F("DB CC1101_MARCSTATE ")); MSG_OUTPUTALLLN(CC1101_readReg(CC1101_MARCSTATE, READ_BURST), HEX);
#endif
#endif

#ifdef debug
#ifdef CODE_AVR
                Serial.println(F("DB Input, writes all current values to EEPROM"));
#elif CODE_ESP
                MSG_OUTPUTLN(F("DB Input, writes all current values to EEPROM"));
#endif
#endif
                for (byte i = 0; i < Registers[int_substr1_serial].length; i++) { /* write register values ​​to flash */
                  EEPROMwrite(i, pgm_read_byte_near(Registers[int_substr1_serial].reg_val + i));
                }
                EEPROMwrite(EEPROM_ADDR_Prot, int_substr1_serial); /* write enabled protocoll */
              }
            } else if (int_substr1_serial == RegistersCntMax) { /* "Special Option" */
#ifdef CODE_AVR
              Serial.println(F("Developer gadget to test functions"));
#elif CODE_ESP
              MSG_OUTPUTLN(F("Developer gadget to test functions"));
#endif
            }
          }
        }
      }
      break; /* -#-#-#-#- - - next case - - - #-#-#-#- */
    case 't':
      if (!buf_input[1]) { /* command t */
#ifdef CODE_AVR
        Serial.println(uptime);
#elif CODE_ESP
        MSG_OUTPUTLN(uptime);
#endif
      } else {
        if (buf_input[1] && buf_input[1] == 'o' && buf_input[2]) { /* command tob<n> & tos<n> */
          if (CC1101_found == true) {
            if (buf_input[2] == 's') { /* command tos<n> | allowed length 1 - 9 (max 12 completely serial) */
              unsigned long IntTime = input.substring(3).toInt();

              if (isNumeric(input.substring(3)) == 1 && (IntTime > ToggleTimeMax)) { /* command tos<n> > ToggleTimeMax -> max limit */
#ifdef CODE_AVR
                Serial.print(F("Toggle STOPPED, time too long [min ")); Serial.print(ToggleTimeMin);
                Serial.print(F(", max ")); Serial.print(ToggleTimeMax); Serial.println(']');
#elif CODE_ESP
                tmp = F("Toggle STOPPED, time too long [min "); tmp += ToggleTimeMin;
                tmp += F(", max "); tmp += ToggleTimeMax; tmp += ']';
                MSG_OUTPUTLN(tmp);
#endif
                ToggleTime = 0;
                ToggleAll = false;
              } else if (isNumeric(input.substring(3)) == 1 && (IntTime >= ToggleTimeMin)) { /* command tos<n> >= ToggleTime OK */
#ifdef CODE_AVR
                Serial.print(F("Toggle starts changing every ")); Serial.print(IntTime); Serial.println(F(" milliseconds"));
#elif CODE_ESP
                tmp = F("Toggle starts changing every "); tmp += IntTime; tmp += F(" milliseconds");
                MSG_OUTPUTLN(tmp);
#endif
                EEPROMwrite_long(EEPROM_ADDR_Toggle, IntTime); /* write to EEPROM */
                ToggleCnt = 0;                                 // sonst evtl. CC110x switched to mode 66, ⸮=
                ToggleTime = IntTime;
                ToggleValues = 0; /* counting Toggle values ​​and sorting into array */

                for (byte i = 0; i < 4; i++) {
                  if (ToggleArray[i] != 255) {
                    ToggleValues++;
                    ToggleOrder[ToggleValues - 1] = ToggleArray[i];
                  }
                }
              } else if (isNumeric(input.substring(3)) == 1 && (IntTime < ToggleTimeMin)) { /* command tos<n> < ToggleTimeMin -> min limit */
#ifdef CODE_AVR
                Serial.print(F("Toggle STOPPED, time to short [min ")); Serial.print(ToggleTimeMin); Serial.println(']');
#elif CODE_ESP
                tmp = F("Toggle STOPPED, time to short [min "); tmp += ToggleTimeMin; tmp += ']';
                MSG_OUTPUTLN(tmp);
#endif
                ToggleTime = 0;
              }
            } else if (buf_input[2] == 'b') { /* command tob */
              ToggleAll = false;
              if (input.substring(3, 4).toInt() <= 3) { /* command tob<0-3> */
#ifdef debug
#ifdef CODE_AVR
                Serial.print(F("DB Input | cmd tob ")); Serial.print(input.substring(3)); Serial.println(F(" accepted"));
#elif CODE_ESP
                tmp = F("DB Input | cmd tob "); tmp += input.substring(3); tmp += F(" accepted");
                MSG_OUTPUTLN(tmp);
#endif
#endif
                if (isNumeric(input.substring(4)) == 1) {
                  if (input.substring(4).toInt() == 99) {                                 /* command tob<0-3>99 -> reset togglebank <n> */
                    ToggleArray[input.substring(3, 4).toInt()] = 255;                     /* 255 is max value and reset value */
                    EEPROMwrite(input.substring(3, 4).toInt() + EEPROM_ADDR_ProtTo, 255); /* 255 is max value and reset value */
#ifdef CODE_AVR
                    Serial.print(F("ToggleBank ")); Serial.print(input.substring(3, 4)); Serial.println(F(" reset"));
#elif CODE_ESP
                    tmp = F("ToggleBank "); tmp += input.substring(3, 4); tmp += F(" reset");
#endif
                  } else if (input.substring(4).toInt() < RegistersCntMax) { /* command tob<0-3><n> -> set togglebank <n> */
                    ToggleArray[input.substring(3, 4).toInt()] = input.substring(4).toInt();
                    EEPROMwrite(input.substring(3, 4).toInt() + EEPROM_ADDR_ProtTo, input.substring(4).toInt());
#ifdef CODE_AVR
                    Serial.print(F("ToggleBank ")); Serial.print(input.substring(3, 4)); Serial.print(F(" set to "));
                    Serial.print(Registers[input.substring(4).toInt()].name); Serial.println(F(" mode"));
#elif CODE_ESP
                    tmp = F("ToggleBank "); tmp += input.substring(3, 4); tmp += F(" set to ");
                    tmp += Registers[input.substring(4).toInt()].name; tmp += F(" mode");
#endif
                  } else if (input.substring(4).toInt() > RegistersCntMax) {
#ifdef CODE_AVR
                    Serial.println(F("Mode number greater RegistersCntMax"));
                  }
#elif CODE_ESP
                    tmp = F("Mode number greater RegistersCntMax");
                  }
                  MSG_OUTPUTLN(tmp);
#endif
                }
                if (ToggleTime > 0) {
                  ToggleValues = 0; /* counting Toggle values ​​and sorting into array */
                  for (byte i = 0; i < 4; i++) {
                    if (EEPROMread(i + EEPROM_ADDR_ProtTo) != 255) {
                      ToggleValues++;
                      ToggleOrder[ToggleValues - 1] = EEPROMread(i + EEPROM_ADDR_ProtTo);
                    }
                  }
                }
              } else if (buf_input[3] == '9' && buf_input[4] == '9' && !buf_input[5]) { /* command tob99 -> reset togglebank */
#ifdef debug
#ifdef CODE_AVR
                Serial.println(F("DB Input, togglebank reset { - | - | - | - }"));
#elif CODE_ESP
                tmp = F("DB Input, togglebank reset { - | - | - | - }\n");
#endif
#endif
                for (byte i = 0; i < 4; i++) {
                  ToggleArray[i] = 255;
                  EEPROMwrite(i + EEPROM_ADDR_ProtTo, 255);  // DEZ 45  HEX 2D  ASCII -
                }
                ToggleValues = 0;
                ToggleTime = 0;
                EEPROMwrite_long(EEPROM_ADDR_Toggle, 0);

#ifdef CODE_AVR
                Serial.println(F("ToggleBank 0-3 reset and STOP Toggle"));
#elif CODE_ESP
                tmp += F("ToggleBank 0-3 reset and STOP Toggle");
                MSG_OUTPUTLN(tmp);
#endif
              } else if (buf_input[3] == '8' && buf_input[4] == '8' && !buf_input[5]) { /* command tob88 -> scan modes */
#ifdef debug
#ifdef CODE_AVR
                Serial.println(F("DB Input, scan modes"));
#elif CODE_ESP
                MSG_OUTPUTLN(F("DB Input, scan modes"));
#endif
#endif
                ToggleAll = true;
                ToggleTime = 15000;  // set to default and start
                ToggleOnOff();
              }
            }
          } else {
#ifdef CODE_AVR
            Serial.println(F("tob and tos not available (no CC1101 found)"));
#elif CODE_ESP
            MSG_OUTPUTLN(F("tob and tos not available (no CC1101 found)"));
#endif
          }
        }
      }
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */
    case 'x': /* command x */
      if (buf_input[1] && buf_input[2] && !buf_input[3]) {
        if (isHexadecimalDigit(buf_input[1]) && isHexadecimalDigit(buf_input[2])) {
          for (byte i = 0; i < 8; i++) {
            if (i == 1) {
              uiBuffer[i] = hexToDec(input.substring(1));
              EEPROMwrite(EEPROM_ADDR_PATABLE + i, uiBuffer[i]);
            } else {
              uiBuffer[i] = 0;
              EEPROMwrite(EEPROM_ADDR_PATABLE + i, 0);
            }
          }
          if (CC1101_found == false) {
#ifdef CODE_AVR
            Serial.println(F("Current registers unreadable, write patable stopped (no CC1101 found)"));
#elif CODE_ESP
            MSG_OUTPUTLN(F("Current registers unreadable, write patable stopped (no CC1101 found)"));
#endif
          } else {
            CC1101_writeBurstReg(uiBuffer, CC1101_PATABLE, 8);
#ifdef CODE_AVR
            Serial.print(F("write ")); Serial.print(buf_input[1]); Serial.print(buf_input[2]); Serial.println(F(" to PATABLE done"));
#elif CODE_ESP
            tmp = F("write "); tmp += buf_input[1]; tmp += buf_input[2]; tmp += F(" to PATABLE done");
            MSG_OUTPUTLN(tmp);
#endif
          }
        }
      }
      break; /* -#-#-#-#- - - next case - - - #-#-#-#- */
    case 'C':
      if (CC1101_found == false) {
#ifdef CODE_AVR
        Serial.println(F("Current registers unreadable (no CC1101 found)"));
#elif CODE_ESP
        MSG_OUTPUTLN(F("Current registers unreadable (no CC1101 found)"));
#endif
      } else {
        if (!buf_input[1]) { /* command C - Read all values from Register */
#ifdef CODE_AVR
          Serial.println(F("Current register (address - CC110x value - EEPROM value)"));

          for (byte i = 0; i <= 46; i++) {
            MSG_OUTPUT_DecToHEX_lz(i);
            Serial.print('=');
            MSG_OUTPUT_DecToHEX_lz(CC1101_readReg(i, READ_BURST));
            if (i < 41) {
              Serial.print(F(" ("));
              MSG_OUTPUT_DecToHEX_lz(EEPROMread(i));
              Serial.println(F(" EEPROM)"));
            } else {
              Serial.println(F(" (only test)"));
            }
          }
#elif CODE_ESP
          tmp = F("Current register (address - CC110x value - EEPROM value)\n");

          for (byte i = 0; i <= 46; i++) {
            tmp += onlyDecToHex2Digit(i); tmp += '='; tmp += onlyDecToHex2Digit(CC1101_readReg(i, READ_BURST));
            if (i < 41) {
              tmp += F(" ("); tmp += onlyDecToHex2Digit(EEPROMread(i)); tmp += F(" EEPROM)\n");
            } else {
              if (i != 46) {
                tmp += F(" (only test)\n");
              }
            }
          }
          tmp += F(" (only test)");
          MSG_OUTPUTLN(tmp);
#endif
        } else if (buf_input[1] && buf_input[2] && !buf_input[3]) { /* command C<n><n> - Read HEX adress values from Register */
          uint16_t Cret = hexToDec(input.substring(1));
          if (Cret <= 61) {
#ifdef CODE_AVR
#ifdef SIGNALduino_comp
            Serial.print('C');
#else
            Serial.print(F("0x"));
#endif
            Serial.print(input.substring(1));

#ifdef SIGNALduino_comp
            Serial.print(F(" = "));
#else
            Serial.print(F(" - 0x"));
#endif
            byte CC1101_ret = CC1101_readReg(Cret, READ_BURST);
            MSG_OUTPUT_DecToHEX_lz(CC1101_ret);
            Serial.println("");
#elif CODE_ESP
#ifdef SIGNALduino_comp
            tmp = 'C';
#else
            tmp = F("0x");
#endif
            tmp += input.substring(1);
#ifdef SIGNALduino_comp
            tmp += F(" = ");
#else
            tmp += F(" - 0x");
#endif
            byte CC1101_ret = CC1101_readReg(Cret, READ_BURST);
            tmp += onlyDecToHex2Digit(CC1101_ret);
            MSG_OUTPUTLN(tmp);
#endif
          } else if (Cret == 153) { /* command C99 - ccreg */
            CC1101_readBurstReg(uiBuffer, 0x00, 47);
#ifdef CODE_AVR
            for (uint8_t i = 0; i < 0x2f; i++) {
              if (i == 0 || i == 0x10 || i == 0x20) {
                if (i > 0) {
                  Serial.print(' ');
                }
                Serial.print(F("ccreg "));

                if (i == 0) {
                  Serial.print('0');
                }
                Serial.print(i, HEX);
                Serial.print(F(": "));
              }
              MSG_OUTPUT_DecToHEX_lz(uiBuffer[i]);
              Serial.print(' ');
            }
            Serial.println("");
#elif CODE_ESP
            tmp = "";
            for (uint8_t i = 0; i < 0x2f; i++) {
              if (i == 0 || i == 0x10 || i == 0x20) {
                if (i > 0) {
                  tmp += ' ';
                }
                tmp += F("ccreg "); tmp += onlyDecToHex2Digit(i); tmp += F(": ");
              }
              tmp += onlyDecToHex2Digit(uiBuffer[i]); tmp += ' ';
            }
            MSG_OUTPUTLN(tmp);
#endif
          } else if (Cret == 62) { /* command C3E - patable  */
            CC1101_readBurstReg(uiBuffer, 0x3E, 8);
#ifdef CODE_AVR
            Serial.print(F("C3E ="));
            for (byte i = 0; i < 8; i++) {
              Serial.print(' ');
              MSG_OUTPUT_DecToHEX_lz(uiBuffer[i]);
            }
            Serial.println("");
#elif CODE_ESP
            tmp = F("C3E =");

            for (byte i = 0; i < 8; i++) {
              tmp += ' '; tmp += onlyDecToHex2Digit(uiBuffer[i]);
            }
            MSG_OUTPUTLN(tmp);
#endif
          }
        } else if (buf_input[1] == 'G' && !buf_input[2]) { /* command CG */
#ifdef CODE_AVR
          Serial.println(F("MS=0;MU=0;MC=0;Mred=0;MN=1;"));
#elif CODE_ESP
          MSG_OUTPUTLN(F("MS=0;MU=0;MC=0;Mred=0;MN=1;"));
#endif
        } else if (buf_input[1] == '0' && buf_input[2] == 'D' && buf_input[3] == 'n' && buf_input[4] == 'F' && !buf_input[5]) { /* command C0DnF - conf */
#ifdef CODE_AVR
          Serial.print(F("C0Dn11="));
          for (byte i = 0; i <= 17; i++) {
            byte x = CC1101_readReg(0x0D + i, READ_BURST);
            MSG_OUTPUT_DecToHEX_lz(x);
          }
          Serial.println("");
#elif CODE_ESP
          tmp = F("C0Dn11=");
          for (byte i = 0; i <= 17; i++) {
            tmp += onlyDecToHex2Digit(CC1101_readReg(0x0D + i, READ_BURST));
          }
          MSG_OUTPUTLN(tmp);
#endif
        }
      }
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */
    case 'E': /* command E */
      if (!buf_input[1]) {
        EEPROMread_table();
      }
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */
    case 'I': /* command I */
      if (!buf_input[1]) {
#ifdef CODE_AVR
        Serial.println(F("# # #   current status   # # #"));
#elif CODE_ESP
        tmp = F("# # #   current status   # # #\n");
#endif
        if (activated_mode_name == "" && CC1101_found == true) {
          activated_mode_name = F("CC110x configuration");  // activated_mode_name if uC restart
        } else if (activated_mode_name != "" && CC1101_found == true) {
          // activated_mode_name is set with command m<n>
        } else {
          activated_mode_name = F("CC110x NOT recognized");  // activated_mode_name if no cc1101
        }

        if (CC1101_found == true) {
#ifdef CODE_AVR
          Serial.print(F("CC110x_MARCSTATE    ")); Serial.println(CC1101_readReg(CC1101_MARCSTATE, READ_BURST), HEX);  // MARCSTATE – Main Radio Control State Machine State
          Serial.print(F("CC110x_Freq.Afc     ")); Serial.println(freqAfc == 1 ? F("on (1)") : F("off (0)"));
          Serial.print(F("CC110x_Freq.Offset  ")); Serial.print(Freq_offset, 3); Serial.println(F(" MHz"));
        }
        Serial.print(F("ReceiveMode         ")); Serial.println(activated_mode_name);
        Serial.print(F("ToggleAll (Scan)    ")); Serial.println(ToggleAll == 1 ? "on" : "off");
        Serial.print(F("ToggleBank 0-3      { "));

        for (byte i = 0; i < 4; i++) {
          if (ToggleArray[i] == 255) {
            Serial.print('-');
          } else {
            Serial.print(ToggleArray[i]);
          }

          if (i != 3) {
            Serial.print(F(" | "));
          }
        }
        Serial.println(F(" }"));
        Serial.print(F("ToggleTime (ms)     "));
        Serial.println(ToggleTime);
#elif CODE_ESP
          tmp += F("CC110x_MARCSTATE    "); tmp += onlyDecToHex2Digit(CC1101_readReg(CC1101_MARCSTATE, READ_BURST));  tmp += "\n"; // MARCSTATE – Main Radio Control State Machine State
          tmp += F("CC110x_Freq.Afc     "); tmp += (freqAfc == 1 ? F("on (1)") : F("off (0)")); tmp += "\n";
          tmp += F("CC110x_Freq.Offset  "); tmp += String(Freq_offset, 3); tmp += F(" MHz"); tmp += "\n";
        }
        tmp += F("ReceiveMode         "); tmp += activated_mode_name; tmp += "\n";
        tmp += F("ToggleAll (Scan)    "); tmp += (ToggleAll == 1 ? "on" : "off"); tmp += "\n";
        tmp += F("ToggleBank 0-3      { ");

        for (byte i = 0; i < 4; i++) {
          if (ToggleArray[i] == 255) {
            tmp += '-';
          } else {
            tmp += ToggleArray[i];
          }

          if (i != 3) {
            tmp += F(" | ");
          }
        }
        tmp += F(" }"); tmp += "\n";
        tmp += F("ToggleTime (ms)     "); tmp += ToggleTime;
        MSG_OUTPUTLN(tmp);
#endif
      }
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */
    case 'M': /* command M */
      if (!buf_input[1]) {
#ifdef CODE_AVR
        Serial.println(F("available register modes:"));
        for (byte i = 0; i < RegistersCntMax; i++) {
          if (i < 10) {
            Serial.print(' ');
          }
          Serial.print(i);
          Serial.print(F(" - "));
          Serial.println(Registers[i].name);
        }
#elif CODE_ESP
        tmp = F("available register modes:");
        tmp += '\n';
        for (byte i = 0; i < RegistersCntMax; i++) {
          if (i < 10) {
            tmp += ' ';
          }
          tmp += i; tmp += F(" - "); tmp += Registers[i].name; tmp += '\n';
        }
        MSG_OUTPUTLN(tmp);
#endif
      }
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */
    case 'P': /* command P */
      if (!buf_input[1]) {
#ifdef CODE_AVR
        Serial.println(F("OK"));
#elif CODE_ESP
        MSG_OUTPUTLN(F("OK"));
#endif
      }
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */
    case 'R': /* command R */
      if (!buf_input[1]) {
#ifdef CODE_AVR
        Serial.println(freeRam());
#elif CODE_ESP
        MSG_OUTPUTLN(freeRam());
#endif
      }
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */
    case 'S': /* command S */
      if (CC1101_found == false) {
#ifdef CODE_AVR
        Serial.println(F("no sending possible (no CC1101 found)"));
#elif CODE_ESP
        MSG_OUTPUTLN(F("no sending possible (no CC1101 found)"));
#endif
      } else {
        if (buf_input[1] && buf_input[1] == 'N' && buf_input[2] == ';') { /* command SN */
#ifdef debug
#ifdef CODE_AVR
          Serial.println(F("DB Input | SN; raw message"));
#elif CODE_ESP
          MSG_OUTPUTLN(F("DB Input | SN; raw message"));
#endif
#endif
          // SN;R=5;D=9A46036AC8D3923EAEB470AB; //

          if (input.lastIndexOf(";") == int(input.length()) - 1) { /* check RAW msg END ; */
            char text[input.length()];
            input.toCharArray(text, input.length());
            char delimiter[] = ";";         // Teilsting-Begrenzer
            char* ptr;                      // NULL-Pointer to a memory address
            ptr = strtok(text, delimiter);  // Initialisieren u. erster Teilstring
            uint8_t rep = 1;
            byte datavalid = 1;
            char* senddata = {};            // NULL-Pointer to a memory address

            while (ptr != NULL) {
              if (strstr(ptr, "R=")) {
#ifdef debug
#ifdef CODE_AVR
                Serial.print(F("DB Input | SN; found ")); Serial.println(ptr);
#elif CODE_ESP
                tmp = F("DB Input | SN; found "); tmp += ptr; tmp += '\n';
#endif
#endif
                if ((String(ptr).substring(2)).toInt() != 0) { /* repeats */
                  rep = (String(ptr).substring(2)).toInt();
#ifdef debug
#ifdef CODE_AVR
                  Serial.println(F("DB Input | SN; takeover repeats"));
#elif CODE_ESP
                  tmp += F("DB Input | SN; takeover repeats");
                  MSG_OUTPUTLN(tmp);
#endif
#endif
                }
              } else if (strstr(ptr, "D=")) { /* datapart */
#ifdef debug
#ifdef CODE_AVR
                Serial.print(F("DB Input | SN; found ")); Serial.print(ptr);
                Serial.print(F(" with length ")); Serial.println(String(ptr + 2).length());
#elif CODE_ESP
                tmp = F("DB Input | SN; found "); tmp += ptr; tmp += F(" with length "); tmp += String(ptr + 2).length();
                MSG_OUTPUTLN(tmp);
#endif
#endif
                ptr = ptr + 2; /* cut D= */

                if (String(ptr).length() % 2 == 0) {                         /* check datapart is odd */
                  for (uint8_t pos = 0; pos < String(ptr).length(); pos++) { /* check datapart is HEX */
                    if (!isHexadecimalDigit(ptr[pos])) {
                      datavalid = 0;
                    };
                  }
                  if (datavalid == 1) {
                    senddata = ptr;
                  }
                } else {
                  datavalid = 0;
#ifdef CODE_AVR
                  Serial.println(F("SN; found odd number of nibbles, no send !!!"));
#elif CODE_ESP
                  MSG_OUTPUTLN(F("SN; found odd number of nibbles, no send !!!"));
#endif
                }
              }
              ptr = strtok(NULL, delimiter);
            }

            /* SEND */
            if (datavalid == 1) {
              digitalWriteFast(LED, HIGH);  // LED on
#ifdef debug
#ifdef CODE_AVR
              Serial.print(F("DB Input | SN; valid and ready to send with repeats=")); Serial.println(rep);
              Serial.println(senddata);
#elif CODE_ESP
              tmp = F("DB Input | SN; valid and ready to send with repeats="); tmp += rep;
              MSG_OUTPUTLN(tmp);
              MSG_OUTPUTLN(senddata);
#endif
#endif
              /*
                AVANTEK           SN;D=08C114844FDA5CA2;R=1;
                Bresser 5in1      SN;D=E7527FF78FF7EFF8FDD7BBCAFF18AD80087008100702284435000002;R=1;
                Fine_Offset_WH57  SN;D=5740C655043F01DED4;R=3;
                Lacrosse_mode1    SN;D=9006106A62;R=1;
              */
              for (uint8_t repeats = 1; repeats <= rep; repeats++) {
                CC1101_setTransmitMode(); /* enable TX */
                CC1101_sendFIFO(senddata);
                delay(100);
              }
              CC1101_setReceiveMode(); /* enable RX */
              digitalWriteFast(LED, LOW);  // LED off
            }
          }
        }
      }
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */
    case 'V': /* command V */
      if (!buf_input[1]) {
        // https://arduino-esp8266.readthedocs.io/en/3.1.2/reference.html#progmem
#ifdef CODE_AVR
        for (byte k = 0; k < strlen_P(TXT_VERSION); k++) {
          Serial.print((char)pgm_read_byte_near(TXT_VERSION + k));
        }
        Serial.println(compile_date);
#elif CODE_ESP
        tmp = FPSTR(TXT_VERSION); tmp += compile_date;
        MSG_OUTPUTLN(tmp);
#endif
      }
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */
    case 'W': /* command W */
      if (CC1101_found == true) {
        if (buf_input[1] && buf_input[1] != 'S' && buf_input[1] > 47 && buf_input[1] < 52 && buf_input[2] && buf_input[3] && buf_input[4] && !buf_input[5]) {
          /* command W1203 | only adress smaller 3E -> buf_input[4] > 47 && buf_input[4] < 52 for W0... W1... W2... W3... */
          byte adr_dec = hexToDec(input.substring(1, 3));
          byte val_dec = hexToDec(input.substring(3));
          //TODO, check function with offset !!!

          if (isHexadecimalDigit(buf_input[1]) && isHexadecimalDigit(buf_input[2]) && isHexadecimalDigit(buf_input[3]) && isHexadecimalDigit(buf_input[4])) {
#ifdef debug
#ifdef CODE_AVR
            Serial.print(F("DB Serial Input | cmd W with adr=")); MSG_OUTPUT_DecToHEX_lz(adr_dec);
            Serial.print(F(" and value=")); MSG_OUTPUT_DecToHEX_lz(val_dec); Serial.println("");
#elif CODE_ESP
            tmp = F("DB Serial Input | cmd W with adr="); tmp += onlyDecToHex2Digit(adr_dec);
            tmp += F(" and value="); tmp += onlyDecToHex2Digit(val_dec);
            MSG_OUTPUTLN(tmp);
#endif
#endif
            CC1101_writeReg(adr_dec - CC1101_writeReg_offset, val_dec);  // write in cc1101 | adr - 2 because of sduino firmware
            EEPROMwrite(adr_dec - CC1101_writeReg_offset, val_dec);      // write in flash
            CC1101_writeReg(CC1101_FSCTRL0, 0);                          // 0x0C: FSCTRL0 – Frequency Synthesizer Control
#ifdef CODE_AVR
            Serial.print('W'); MSG_OUTPUT_DecToHEX_lz(adr_dec); MSG_OUTPUT_DecToHEX_lz(val_dec); Serial.println("");
#elif CODE_ESP
            tmp = 'W'; tmp += onlyDecToHex2Digit(adr_dec); tmp += onlyDecToHex2Digit(val_dec);
            MSG_OUTPUTLN(tmp);
#endif
          }
        } else if (buf_input[1] == 'S' && buf_input[2] == '3' && isHexadecimalDigit(buf_input[3]) && !buf_input[4]) {
          /* command WS34 ... | from 0x30 to 0x3D */
#ifdef debug
#ifdef CODE_AVR
          Serial.print(F("DB Input | cmd WS with value=")); Serial.print(buf_input[2]); Serial.println(buf_input[3]);
#elif CODE_ESP
          tmp = F("DB Input | cmd WS with value="); tmp += String(buf_input[2]); tmp += String(buf_input[3]);
          MSG_OUTPUTLN(tmp);
#endif
#endif
          if (buf_input[3] == '4') {
            CC1101_cmdStrobe(CC1101_SRX);
#ifdef CODE_AVR
            Serial.println(F("cmdStrobeReg 34 chipStatus 00 delay1 01"));
#elif CODE_ESP
            MSG_OUTPUTLN(F("cmdStrobeReg 34 chipStatus 00 delay1 01"));
#endif
          }
          if (buf_input[3] == '6') {
            CC1101_cmdStrobe(CC1101_SIDLE);
#ifdef CODE_AVR
            Serial.println(F("cmdStrobeReg 36 chipStatus 01 delay1 00"));
#elif CODE_ESP
            MSG_OUTPUTLN(F("cmdStrobeReg 36 chipStatus 01 delay1 00"));
#endif
          }
        }
      } else {
#ifdef CODE_AVR
        Serial.println(F("W and WS not available (no CC1101 found)"));
#elif CODE_ESP
        MSG_OUTPUTLN(F("W and WS not available (no CC1101 found)"));
#endif
      }
      break; /* -#-#-#-#- - - next case - - - #-#-#-#- */
    default:
      break; /* -#-#-#-#- - - next case - - - #-#-#-#- */
  }
}


/* ############### !!! only ESP8266 and ESP32 !!! ############### */

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
void Telnet() {
  TELNET_CLIENT_COUNTER = 0;
  for (uint8_t i = 0; i < TELNET_CLIENTS_MAX; i++) {
    if (TelnetClient[i] && TelnetClient[i].connected()) { /* Cleanup disconnected session */
      TELNET_CLIENTS_ARRAY[i] = 1;
      TELNET_CLIENT_COUNTER++;
    } else {
      TELNET_CLIENTS_ARRAY[i] = 0;
    }
  }

  if (TelnetServer.hasClient()) {         /* Check new client connections */
    TELNET_ConnectionEstablished = false; /* Set to false */

    for (uint8_t i = 0; i < TELNET_CLIENTS_MAX; i++) {
      if (!TelnetClient[i]) { /* find free socket */
        TelnetClient[i] = TelnetServer.available();
        Serial.print(F("Telnet client "));
        Serial.print(TelnetClient[i].remoteIP());
        Serial.println(F(" connected new session"));
        File logfile = LittleFS.open("/files/log.txt", "a");  // open logfile for append to end of file
        if (logfile) {
          logfile.print(uptime);
          logfile.print(F(" - Telnet client "));
          logfile.print(TelnetClient[i].remoteIP());
          logfile.println(F(" connected"));
        }
        logfile.close();         // close logfile
        TelnetClient[i].flush(); /* clear input buffer, else you get strange characters */
        TelnetClient[i].print(F("Telnet session ("));
        TelnetClient[i].print(i);
        TelnetClient[i].print(F(") started to "));
        TelnetClient[i].println(TelnetClient[i].localIP());

        TELNET_CLIENTS_ARRAY[i] = 1;
        TELNET_ConnectionEstablished = true;
        break;
      } else {
        /* Serial.println("Session is in use"); */
      }
    }

    if (TELNET_ConnectionEstablished == false) {
      Serial.println(F("No free sessions ... drop connection"));
      TelnetServer.available().stop();

      for (uint8_t i = 0; i < TELNET_CLIENTS_MAX; i++) {
        if (TelnetClient[i] || TelnetClient[i].connected()) {
          TelnetClient[i].println(F("An other user cannot connect ... TELNET_CLIENTS_MAX limit is reached!"));
        }
      }
    }
  }

  for (uint8_t i = 0; i < TELNET_CLIENTS_MAX; i++) {
    if (TelnetClient[i] && TelnetClient[i].connected()) {
      TelnetClient[i].setTimeout(Timeout_Telnet); /* sets the maximum milliseconds to wait for serial data. It defaults to 1000 milliseconds. */

      while (TelnetClient[i].available()) { /* get data from the telnet client */
#ifdef debug
        Serial.print(F("DB Telnet, Data from session "));
        Serial.println(i);
#endif
        client_now = i; /* current telnet client is set where data is received */
        msg = TelnetClient[i].readString();
        msg.trim();   /* String, strip off any leading/trailing space and \r \n */
        char BUFFER_Telnet[msg.length() + 1];
        msg.toCharArray(BUFFER_Telnet, msg.length() + 1); /* String to char in buf */

        if (msg.length() > 0 && msg.length() <= BUFFER_MAX) {
          InputCommand(BUFFER_Telnet);
        }
      }
    }
  }
}

//=====================================================
//function process event: new data received from client
//=====================================================
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_PING: /* pong will be send automatically */
#if defined debug_websocket
      Serial.printf("WebSocket [%u] connected - Ping!\n", num);
#endif
      break;
    case WStype_PONG: /* // answer to a ping we send */
#if defined debug_websocket
      Serial.printf("WebSocket [%u] connected - Pong!\n", num);
#endif
      break;
    case WStype_DISCONNECTED:
#if defined debug_websocket
      Serial.printf("WebSocket [%u] disconnected!\n", num);
#endif
      webSocketSite[num] = "";
      break;
    case WStype_CONNECTED:
      {
#if defined debug_websocket
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("WebSocket [%u] connected - from %d.%d.%d.%d%s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
#endif
        webSocketSite[num] = (char * )payload;
        webSocket.sendTXT(num, "Connected"); /* send message to client */
      }
      break;
    case WStype_TEXT: /*  */
      // webSocket.sendTXT(num, "message here"); /* send message to client */
      // webSocket.broadcastTXT("message here"); /* send data to all connected clients */
      {
#if defined debug_websocket
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("WebSocket [%u] receive - from %d.%d.%d.%d %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
#endif
        String payloadString = (const char *)payload;
        if (payloadString == "cc110x_detail") {
          WebSocket_cc110x_detail();
        } else if (payloadString == "cc110x") {
          WebSocket_cc110x();
        } else if (payloadString == "help") {
          WebSocket_help();
        }
      }
      break;
    case WStype_BIN: /* binary files receive */
      // webSocket.sendBIN(num, payload, length); /* send message to client */
      break;
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;
  }
}
#endif /* END - ESP8266 & ESP 32*/


/* for OOK modulation */
void decode(const int pulse) {    /* Pulsübernahme und Weitergabe */
  if (MsgLen > 0) {
    last = first;
  } else {
    last = 0;
  }
  first = pulse;
  doDetect();
}


inline void doDetect() {      /* Pulsprüfung und Weitergabe an Patternprüfung */
  valid = (MsgLen == 0 || last == 0 || (first ^ last) < 0);   // true if a and b have opposite signs
  valid &= (MsgLen == MsgLenMax) ? false : true;
  valid &= ( (first > -t_maxP) && (first < t_maxP) );         // if low maxPulse detected, start processMessage()
#ifdef debug_cc110x_MU
  Serial.print(F("PC:")); Serial.print(PatNmb); Serial.print(F(" ML:")); Serial.print(MsgLen); Serial.print(F(" v:")); Serial.print(valid);
  Serial.print(F(" | ")); Serial.print(first); Serial.print(F("    ")); Serial.println(last);
#endif
  if (valid) {
    findpatt(first);
  } else {
#ifdef debug_cc110x_MU
    Serial.println(F("-- RESET --"));
#endif
    MSGBuild();
  }
}


void MSGBuild() {     /* Nachrichtenausgabe */
  if (MsgLen >= MsgLenMin) {
    uint8_t rssi = CC1101_readReg(0x34, 0xC0); // not converted
    digitalWriteFast(LED, HIGH);  // LED on
    uint8_t CP_PaNum = 0;
    int16_t PulseAvgMin = 32767;
    String raw = "";
    raw.reserve(360);
    raw += F("MU");
    for (uint8_t i = 0; i <= PatNmb; i++) {
      int16_t PulseAvg = ArPaSu[i] / ArPaCnt[i];
      raw += F(";P"); raw += i; raw += F("="); raw += PulseAvg;
      // search Clockpulse (CP=) - das funktioniert noch nicht richtig! --> kein richtiger Einfall ;-) TODO
      if (ArPaSu[i] > 0) { // HIGH-Pulse
        if (PulseAvg < PulseAvgMin) { // kürzeste Zeit
          PulseAvgMin = PulseAvg;     // kürzeste Zeit übernehmen
          CP_PaNum = i;
        }
        if (ArPaCnt[i] > ArPaCnt[CP_PaNum]) {
          CP_PaNum = i;               // ClockPulse übernehmen
        }
      }
    }
    raw += F(";D="); raw += msg; raw += F(";CP="); raw += CP_PaNum; raw += F(";R="); raw += rssi; raw += ';';
    if (MsgLen == MsgLenMax) {  /* max. Nachrichtenlänge erreicht */
      raw += F("O;");
    } else if (TiOv != 0) {     /* Timeoverflow größer 32000 -> Zwangstrennung */
      raw += F("p;");
    } else if (PatMAX == 1) {   /* max. Pattern erreicht und neuer unbekannter würde folgen */
      raw += F("e;");
    }
    raw += F("w="); raw += valid; raw += ';';    /* letzter Puls zu vorherigen Puls msgMU valid bzw. unvalid /  */
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
    CC1101_readRSSI();
    WebSocket_raw(raw);
#endif
    raw.replace("M", "M");
    raw += char(3); raw += char(10);
#ifdef CODE_AVR
    Serial.print(raw);
#elif CODE_ESP
    MSG_OUTPUTALL(raw);
#endif
    msgCount++;
    digitalWriteFast(LED, LOW);  // LED off
  }
  PatReset();
}


void findpatt(int val) {      /* Patterneinsortierung */
  for (uint8_t i = 0; i < PatMaxCnt; i++) {
    if (MsgLen == 0) {  /* ### nach RESET ### */
      msg = i;
      ArPaCnt[i] = 1;
      ArPaT[i] = val;
      ArPaSu[i] = val;
      MsgLen++;
#ifdef debug_cc110x_MU
      Serial.print(i); Serial.print(F(" | ")); Serial.print(ArPaT[i]); Serial.print(F(" msgL0: ")); Serial.print(val);
      Serial.print(F(" l: ")); Serial.print(last); Serial.print(F(" PatN: ")); Serial.print(PatNmb); Serial.print(F(" msgL: ")); Serial.print(MsgLen); Serial.print(F(" Fc: ")); Serial.println(FiFo.count());
#endif
      break;
      /* ### in Tolleranz und gefunden ### */
    } else if ( (val > 0 && val > ArPaT[i] * (1 - PatTol) && val < ArPaT[i] * (1 + PatTol)) ||
                (val < 0 && val < ArPaT[i] * (1 - PatTol) && val > ArPaT[i] * (1 + PatTol)) ) {
      msg += i;
      ArPaCnt[i]++;
      ArPaSu[i] += val;
      MsgLen++;
#ifdef debug_cc110x_MU
      Serial.print(i); Serial.print(F(" | ")); Serial.print(ArPaT[i]); Serial.print(F(" Pa T: ")); Serial.print(val);
      Serial.print(F(" l: ")); Serial.print(last); Serial.print(F(" PatN: ")); Serial.print(PatNmb); Serial.print(F(" msgL: ")); Serial.print(MsgLen); Serial.print(F(" Fc: ")); Serial.println(FiFo.count());
#endif
      break;
    } else if (i < (PatMaxCnt - 1) && ArPaT[i + 1] == 0 ) { /* ### nächste freie Pattern ### */
      msg += i + 1;
      PatNmb++;
      ArPaCnt[i + 1]++;
      ArPaT[i + 1] = val;
      ArPaSu[i + 1] += val;
      MsgLen++;
#ifdef debug_cc110x_MU
      Serial.print(i); Serial.print(F(" | ")); Serial.print(ArPaT[i]); Serial.print(F(" Pa f: ")); Serial.print(val);
      Serial.print(F(" l: ")); Serial.print(last); Serial.print(F(" PatN: ")); Serial.print(PatNmb); Serial.print(F(" msgL: ")); Serial.print(MsgLen); Serial.print(F(" Fc: ")); Serial.println(FiFo.count());
#endif
      break;
    } else if (i == (PatMaxCnt - 1)) {  /* ### Anzahl vor definierter Pattern ist erreicht ### */
#ifdef debug_cc110x_MU
      Serial.print(F("PC max! | MsgLen: ")); Serial.print(MsgLen); Serial.print(F(" | MsgLenMin: ")); Serial.println(MsgLenMin);
#endif
      PatMAX = 1;
      MSGBuild();
      break;
    }
  }
}


void PatReset() {     /* Zurücksetzen nach Nachrichtenbau oder max. Länge */
  msg = "";
  MsgLen = 0;
  PatMAX = 0;
  PatNmb = 0;
  TiOv = 0;

  for (uint8_t i = 0; i < PatMaxCnt; i++) {
    ArPaCnt[i] = 0;
    ArPaSu[i] = 0;
    ArPaT[i] = 0;
  }
}
