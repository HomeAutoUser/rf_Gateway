/*
  Copyright (c) 2022, HomeAutoUser & elektron-bbs
  All rights reserved.
*/
#include <Arduino.h>
#include <EEPROM.h>
#include <digitalWriteFast.h>                 // https://github.com/ArminJo/digitalWriteFast
#include "config.h"
int16_t RSSI_dez;                             // for the output on web server
uint8_t rssi;
#ifdef CC110x
#include "cc110x.h"
#elif RFM69
#include "rfm69.h"
uint8_t SX1231_CrcOn;
#endif

#ifdef CODE_ESP
Data myArraySRAM;
#endif

#if defined (WMBus_S) || defined (WMBus_T)
#include "mbus.h"
#endif

#include "macros.h"
#include "functions.h"

/* Settings for OOK messages without Sync Pulse (MU) */
#ifdef OOK_MU_433
#define MsgLenMin               24            // message minimum length
#define MsgLenMax               254           // message maximum length
#define PatMaxCnt               8             // pattern, maximum number (number 8 -> FHEM SIGNALduino compatible)
#define PatTol                  0.20          // pattern tolerance
#ifdef CODE_ESP
#define FIFO_LENGTH             200           // 200 from SIGNALduino FW
#else
#define FIFO_LENGTH             90            // 90 from SIGNALduino FW
#endif
#define t_maxP 32000                          // Zeitdauer maximum für gültigen Puls in µs
#define t_minP 90                             // Zeitdauer minimum für gültigen Puls in µs
String msg;
volatile unsigned long lastTime;              // Zeit, letzte Aktion
int16_t ArPaT[PatMaxCnt];                     // Pattern Array für Zeiten
signed long ArPaSu[PatMaxCnt];                // Pattern Summe, aller gehörigen Pulse
byte ArPaCnt[PatMaxCnt];                      // Pattern Counter, der Anzahl Pulse
byte PatNmb = 0;                              // Pattern aktuelle Nummer 0 - 9
byte MsgLen;                                  // ToDo, kann ggf ersetzt werden durch message.valcount
int16_t first;                                // Zeiger auf den ersten Puffereintrag
int16_t last;                                 // Zeiger auf den letzten Puffereintrag
byte TiOv = 0;                                // Marker - Time Overflow (SIGNALduino Kürzel p; )
byte PatMAX = 0;                              // Marker - maximale Pattern erreicht und neuer unbekannter würde folgen (SIGNALduino Kürzel e; )
#include "SimpleFIFO.h"
SimpleFIFO<int16_t, FIFO_LENGTH> FiFo;        // store FIFO_LENGTH # ints
#endif

/* varible´s for output */
extern const char compile_date[];
const char PROGMEM compile_date[] = __DATE__ " " __TIME__;
extern const char TXT_VERSION[];

#ifdef COUNT_LOOP
uint32_t countLoop;
#endif

#ifdef SIGNALduino_comp
/*  SIGNALduino helpful information !!!
    1) for sduino compatible need version the true value from regex -> if ($hash->{version} =~ m/cc1101/) | check in 00_SIGNALduino.pm
    2) output xFSK RAW msg must have format MN;D=9004806AA3;R=52;
*/
byte Chip_writeReg_offset = 2;
/* compatibility with SIGNALduino Firmware
   - for FHEM raw cmd W --> write register manually | W0444 write to adr 0x02 value 44
   - there is no offset when operated via the web interface */
#define FWtxtPart1              " SIGNALduino compatible "
#define MSG_BUILD_Data          F("MN;D=")
#define MSG_BUILD_RSSI          F(";R=")
#define MSG_BUILD_AFC           F(";A=")
#else
#define FWtxtPart1              " "
#define MSG_BUILD_Data          F("data: ")
#define MSG_BUILD_RSSI          F("; RSSI=")
#define MSG_BUILD_AFC           F("; FREQAFC=")
byte Chip_writeReg_offset = 0;
#endif  // END - SIGNALduino_comp || no compatible

#ifndef CHIP_RFNAME
#define CHIP_RFNAME             "rf_Gateway"
#endif

const char TXT_VERSION[] PROGMEM = FWVer FWtxtPart1 CHIP_RFNAME " (" FWVerDate ") ";
byte RegBeforeChange[REGISTER_MAX + 1];

/* varible´s for Toggle */
uint8_t ToggleArray[NUMBER_OF_MODES];         // Toggle, enable/disable Mode
uint8_t ToggleTimeMode[NUMBER_OF_MODES];      // Toggle, Zeit pro Mode in Sekunden
byte ToggleCnt = 0;                           // Toggle, Anzahl aktiver Modi

byte MOD_FORMAT;                              // Marker - Modulation
volatile byte FSK_RAW;                        // Marker - FSK Modulation RAW interrupt
byte ReceiveModeNr;                           // activated protocol
uint8_t ChipFound;                            // chip VERSION

/* predefinitions of the functions */
inline void doDetect();
void NO_Chip();
void Interupt_Variant(byte nr);
void decode(const int pulse); // only MU
void findpatt(int val); // only MU
void InputCommand(String input);
void ToggleOnOff();

/* varible´s for other */
#define BUFFER_MAX 70                         // maximale Anzahl der zu sendenden Zeichen
int8_t freqErr = 0;                           // CC110x automatic Frequency Synthesizer Control
int8_t freqOffAcc = 0;                        // CC110x automatic Frequency Synthesizer Control
float freqErrAvg = 0;                         // CC110x automatic Frequency Synthesizer Control
uint8_t freqAfc = 0;                          // CC110x AFC an oder aus
int16_t freqOffset;                           // Frequency offset
uint32_t msgCount;                            // Nachrichtenzähler über alle empfangenen Nachrichten
uint32_t msgCountMode[NUMBER_OF_MODES];       // Nachrichtenzähler pro Mode, Größe anpassen nach Anzahl Modes in cc110x.h/rfm69.h!
byte client_now;                              // aktueller Telnet-Client, wo Daten empfangen werden
unsigned long secTick = 0;                    // Zeit, zu der die Uhr zuletzt „tickte“
unsigned long toggleTick = 0;
unsigned long uptime = 0;

/* --- all SETTINGS for the ESP8266 ----------------------------------------------------------------------------------------------------------- */
#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
ESP8266WebServer HttpServer(80);
ADC_MODE(ADC_VCC);              // vcc read
#endif
/* --- END - all SETTINGS for the ESP8266 ----------------------------------------------------------------------------------------------------- */

/* --- all SETTINGS for the ESP32-- ----------------------------------------------------------------------------------------------------------- */
#ifdef ARDUINO_ARCH_ESP32
#include <WebServer.h>
#include <WiFi.h>
#include <esp_wps.h>
WebServer HttpServer(80);
int wifiEventId;
#include "soc/soc.h"            // for disable brownout
#include "soc/rtc_cntl_reg.h"   // for disable brownout
#endif
/* --- END - all SETTINGS for the ESP32-- ----------------------------------------------------------------------------------------------------- */

/* --- all SETTINGS for the ESP8266 and ESP32 ------------------------------------------------------------------------------------------------- */
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
#include <ArduinoOTA.h>
#include <WebSocketsServer.h>
#define WEBSOCKETS_SAVE_RAM                   // moves all Header strings to Flash (~300 Byte)
//#define WEBSOCKETS_SERVER_CLIENT_MAX  2     // default 5
WebSocketsServer webSocket = WebSocketsServer(81);
String webSocketSite[WEBSOCKETS_SERVER_CLIENT_MAX] = {};

/*
   https://github.com/lorol/LITTLEFS/issues/43#issuecomment-1537234709
   https://github.com/lorol/LITTLEFS --> Ths library is now part of Arduino esp32 core v2
   ESP32 Bibliothek -> LittleFS_esp32
*/

#ifdef ESP32_core_v1
#include <LITTLEFS.h>
#define LittleFS LITTLEFS
#else
#include <LittleFS.h>
#endif

WiFiServer TelnetServer(TELNET_PORT);
WiFiClient TelnetClient[TELNET_CLIENTS_MAX];

byte TELNET_CLIENTS_ARRAY[TELNET_CLIENTS_MAX];
byte TELNET_CLIENT_COUNTER = 0;
bool TELNET_ConnectionEstablished;            // Telnet Flag for successfully handled connection
byte WLAN_reco_cnt = 0;                       // counter for connection, if cnt 3, WLAN jump zu AP
String OwnStationHostname = WLAN_hostname;
byte used_dhcp = 0;                           // IP-Adresse mittels DHCP
boolean WLAN_AP = false;                      // WiFi AP opened

#define SENDDATA_LENGTH     129               // Weboberfläche | senden - max Länge
char senddata_esp[SENDDATA_LENGTH];           // Weboberfläche | senden - Daten
uint8_t msgRepeats;                           // Weboberfläche | senden - Wiederholungszähler RAW von Einstellung
uint32_t msgSendInterval;                     // Weboberfläche | senden - Intervall Zeit
unsigned long msgSendStart;                   // Weboberfläche | senden - Startpunkt Zeit

IPAddress eip;                                // static IP - IP-Adresse
IPAddress esnm;                               // static IP - Subnetzmaske
IPAddress esgw;                               // static IP - Standard-Gateway
IPAddress edns;                               // static IP - Domain Name Server

unsigned long uptimeReset = 0;
#include "websites.h"
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
void Telnet();

/* --- END - all SETTINGS for the ESP8266 and ESP32 ------------------------------------------------------------------------------------------- */
#else
//#define ICACHE_RAM_ATTR
#define FPSTR String
#endif


#ifdef CC110x
/* --------------------------------------------------------------------------------------------------------------------------------- void Interupt */
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
IRAM_ATTR void Interupt() { /* Pulseauswertung */
#else
void Interupt() {
#endif
  if (MOD_FORMAT != 3) {    /* not OOK */
    FSK_RAW = 1;
  } else {                  /* OOK */
#ifdef OOK_MU_433
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
      if (digitalReadFast(GDO2)) {  // Wenn jetzt high ist, dann muss vorher low gewesen sein, und dafür gilt die gemessene Dauer.
        sDuration = -sDuration;
      }
      FiFo.enqueue(sDuration);      // add an sDuration
    }                               // sonst => Müll
#endif
  }
}
#endif

/* --------------------------------------------------------------------------------------------------------------------------------- void Interupt_Variant */
void Interupt_Variant(byte nr) {
#ifdef debug_chip
  Serial.print(F("[DB] Interupt_Variant called with mode nr: ")); Serial.println(nr);
#endif
#ifdef CC110x
  detachInterrupt(digitalPinToInterrupt(GDO2));
  CC110x_CmdStrobe(CC110x_SIDLE); // Exit RX / TX, turn off frequency synthesizer and exit Wake-On-Radio mode if applicable
#endif
  if (nr != 1) {  // all other Modes except user setting
    Chip_writeRegFor(nr); // write all registers
  } else {        // only Chip user setting
    for (byte i = 1; i < REGISTER_MAX; i++) {
      uint8_t addr = i;
#ifdef RFM69
      if (i >= 80) {
        addr = SX1231_RegAddrTranslate[i - 80];
      }
#endif
      Chip_writeReg(addr, EEPROMread(i));
    }
  }
  String ReceiveModeName = getModeName(nr);
#ifdef debug_chip
  Serial.print(F("[DB] Interupt_Variant ReceiveModeName: ")); Serial.println(ReceiveModeName);
#endif
#ifdef CC110x
  if (ReceiveModeName[0] == 'W') {  // WMBUS
    FSK_RAW = 2;
  } else {
    FSK_RAW = 0;
    MOD_FORMAT = (Chip_readReg(0x12, READ_BURST) & 0b01110000 ) >> 4;
    if (MOD_FORMAT != 3) {          // FSK
#if defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_PRO)
      // https://forum.arduino.cc/t/is-it-possible-to-detach-interrupt-in-the-function-called-with-interrupt/614918/13
      EIFR = 0b00000001; // External Interrupt Flag Register, Pin D2 = INT0, clear the flag
#endif
#ifdef debug_chip
      Serial.print(F("[DB] Interupt_Variant attachInterrupt: ")); Serial.println(F("RISING"));
#endif
      attachInterrupt(digitalPinToInterrupt(GDO2), Interupt, RISING); /* "Bei steigender Flanke auf dem Interruptpin" --> "Führe die Interupt Routine aus" */
    } else {                        // OOK
#ifdef debug_chip
      Serial.print(F("[DB] Interupt_Variant attachInterrupt: ")); Serial.println(F("CHANGE"));
#endif
      attachInterrupt(digitalPinToInterrupt(GDO2), Interupt, CHANGE); /* "Bei wechselnder Flanke auf dem Interruptpin" --> "Führe die Interupt Routine aus" */
    }
  }
#elif RFM69
  if (ReceiveModeName[0] == 'W') {  // WMBUS
    FSK_RAW = 2;
  } else {
    FSK_RAW = 0;
  }
#endif
#ifdef debug_chip
  Serial.print(F("[DB] Interupt_Variant FSK_RAW: ")); Serial.println(FSK_RAW);
#endif
#ifdef CC110x
  // With the CC1100, the TEST registers had to be written after wakeup in order to accomplish this.
  if (ChipFound == 0x03) { // CC1100
    if ((Chip_readReg(CC110x_FIFOTHR, READ_BURST) & 0b01000000) >> 6) { // CC1101 ADC_RETENTION - The ADC_RETENTION bit should be set to 1 before going into SLEEP mode if settings with an RX filter bandwidth below 325 kHz are wanted at time of wake-up.
      Chip_writeReg(CC110x_FIFOTHR, Chip_readReg(CC110x_FIFOTHR, READ_BURST) & 0b00001111); // Bit 7:4 Reserved, Write 0 for compatibility with possible future extensions.
      Chip_writeReg(CC110x_TEST2, 0x81); // RX filter bandwidth ≤ 325 kHz, TEST2 = 0x81
      Chip_writeReg(CC110x_TEST1, 0x35); // RX filter bandwidth ≤ 325 kHz, TEST1 = 0x35
      Chip_writeReg(CC110x_TEST0, 0x09); // after reset 0x0B - Enable VCO selection calibration stage when 1
#ifdef debug_chip
      char hex[3];
      onlyDecToHex2Digit(Chip_readReg(CC110x_FIFOTHR, READ_BURST) & 0b00001111, hex);
      Serial.print(F("[DB] Interupt_Variant writeReg: CC1100 FIFOTHR=0x")); Serial.println(hex);
      Serial.println(F("[DB] Interupt_Variant writeReg: CC1100 TEST2=0x81, TEST1=0x35, TEST0=0x09"));
#endif
    } else {
      Chip_writeReg(CC110x_TEST2, 0x88); // RX filter bandwidth > 325 kHz, TEST2 = 0x88
      Chip_writeReg(CC110x_TEST1, 0x31); // RX filter bandwidth > 325 kHz, TEST1 = 0x31
      Chip_writeReg(CC110x_TEST0, 0x09); // after reset 0x0B - Enable VCO selection calibration stage when 1
#ifdef debug_chip
      Serial.println(F("[DB] Interupt_Variant writeReg: CC1100 TEST2=0x88, TEST1=0x31, TEST0=0x09"));
#endif
    }
  }
  CC110x_CmdStrobe(CC110x_SFRX); // Flush the RX FIFO buffer. Only issue SFRX in IDLE or RXFIFO_OVERFLOW states
#elif RFM69
  Chip_writeReg(0x28, 0x10);     // FIFO are cleared when this bit is set.
  SX1231_CrcOn = (Chip_readReg(0x37, READ_BURST) & 0b00010000) >> 4; // RegPacketConfig1, CrcOn - Enables CRC calculation/check (Tx/Rx).
#endif
  Chip_setReceiveMode();         // start receive mode
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  WebSocket_chip();
  WebSocket_detail(1);
  WebSocket_modes();
#endif
}


/* --------------------------------------------------------------------------------------------------------------------------------- void setup */
void setup() {
#ifdef ARDUINO_ARCH_ESP32
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  // disable brownout detector - TODO nur erforderlich bei schlechter Stromversorgung
#endif
  Serial.begin(SerialSpeed);
  Serial.setTimeout(Timeout_Serial); /* sets the maximum milliseconds to wait for serial data. It defaults to 1000 milliseconds. */
  Serial.println();
  while (!Serial) {
    ; /* wait for serial port to connect. Needed for native USB */
  }
#ifdef CC110x
  pinModeFast(GDO0, INPUT); // for WMBUS
  //  pinModeFast(GDO0, OUTPUT);        // TODO wird evtl. für Senden MU gebraucht
  //  digitalWriteFast(GDO0, LOW);  // TODO wird evtl. für Senden MU gebraucht
  pinModeFast(GDO2, INPUT);
#endif
  pinModeFast(LED, OUTPUT);
  digitalWriteFast(LED, LOW);

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32) /* code for ESP8266 and ESP32 */
  /* interner Flash-Speicher */
  if (!LittleFS.begin()) {
#ifdef debug
    Serial.println(F("[DB] LittleFS mount failed, formatting filesystem"));
#endif  // END - debug
    LittleFS.format();
  } else {
#ifdef debug
    Serial.println(F("[DB] Starting LittleFS"));
#endif  // END - debug
  }
  File logFile = LittleFS.open("/files/log.txt", "w"); /* Datei mit Schreibrechten öffnen, wird erstellt wenn nicht vorhanden */
  if (!logFile) {
#ifdef debug
    Serial.println(F("[DB] LittleFS file creation failed"));
#endif  // END - debug
  } else {
    String logText = F("Systemstart (");
#ifdef ARDUINO_ARCH_ESP8266
    logText += ESP.getResetReason();
#endif

#ifdef ARDUINO_ARCH_ESP32
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
#endif  // END - ARDUINO_ARCH_ESP32
    logText += ')';
    logFile.close();          // Schließen der Datei
    appendLogFile(logText);
  }

  EEPROM.begin(EEPROM_SIZE);  // Puffergröße die verwendet werden soll
  eip = EEPROMread_ipaddress(EEPROM_ADDR_IP);
  esnm = EEPROMread_ipaddress(EEPROM_ADDR_NETMASK);
  esgw = EEPROMread_ipaddress(EEPROM_ADDR_GATEWAY);
  edns = EEPROMread_ipaddress(EEPROM_ADDR_DNS);
  used_dhcp = EEPROMread(EEPROM_ADDR_DHCP);
  if (used_dhcp > 1) {
    used_dhcp = 1;
    EEPROMwrite(EEPROM_ADDR_DHCP, 1);
  }

#ifdef debug_wifi
  Serial.print(F("[DB] WIFI SSID         ")); Serial.println(EEPROMread_string(EEPROM_ADDR_SSID));
  Serial.print(F("[DB] WIFI Passwort     ")); Serial.println(EEPROMread_string(EEPROM_ADDR_PASS));
  Serial.print(F("[DB] WIFI DHCP         ")); Serial.println(EEPROMread(EEPROM_ADDR_DHCP));
  Serial.print(F("[DB] WIFI AP           ")); Serial.println(EEPROMread(EEPROM_ADDR_AP));
  Serial.print(F("[DB] WIFI Adr IP       ")); Serial.println(eip);
  Serial.print(F("[DB] WIFI Adr Gateway  ")); Serial.println(esgw);
  Serial.print(F("[DB] WIFI Adr DNS      ")); Serial.println(edns);
  Serial.print(F("[DB] WIFI Adr NetMask  ")); Serial.println(esnm);
#endif  // END - debug_wifi

#ifdef ARDUINO_ARCH_ESP8266
  uint32_t chipID = ESP.getChipId();
#endif

#ifdef ARDUINO_ARCH_ESP32
  uint64_t macAddress = ESP.getEfuseMac();
  uint64_t macAddressTrunc = macAddress << 40;
  uint32_t chipID = macAddressTrunc >> 40;
#endif

  WiFi.disconnect();
  //  WiFi.setOutputPower(0);
  OwnStationHostname.replace("_", "-");         // Unterstrich ersetzen, nicht zulässig im Hostnamen
  OwnStationHostname += '-';
  OwnStationHostname += String(chipID, HEX);
  WiFi.setHostname(OwnStationHostname.c_str()); // WIFI set hostname
#ifdef ARDUINO_ARCH_ESP32
  WiFi.onEvent(ESP32_WiFiEvent);                // all WiFi-Events
#endif

  if (EEPROMread(EEPROM_ADDR_AP) == 255 || EEPROMread(EEPROM_ADDR_AP) == 1) {
    start_WLAN_AP(OwnStationHostname, WLAN_password_ap);
  }
  if (EEPROMread(EEPROM_ADDR_AP) == 0) {
    start_WLAN_STATION(EEPROMread_string(EEPROM_ADDR_SSID), EEPROMread_string(EEPROM_ADDR_PASS));
  }

  /* Arduino OTA Update – Update über WLAN */
#ifdef debug_wifi
  ArduinoOTA.onStart([]() {
    Serial.println(F("[DB] OTA - Start"));
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("[DB] Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("[DB] OTA - Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println(F("[DB] OTA - Auth Failed"));
    else if (error == OTA_BEGIN_ERROR) Serial.println(F("[DB] OTA - Begin Failed"));
    else if (error == OTA_CONNECT_ERROR) Serial.println(F("[DB] OTA - Connect Failed"));
    else if (error == OTA_RECEIVE_ERROR) Serial.println(F("[DB] OTA - Receive Failed"));
    else if (error == OTA_END_ERROR) Serial.println(F("[DB] OTA - End Failed"));
  });
  Serial.println(F("[DB] Starting OTA"));
#endif  // END - debug_wifi
  ArduinoOTA.begin();

#ifdef debug_telnet
  Serial.println(F("[DB] Starting TelnetServer"));
#endif
  TelnetServer.begin();
  TelnetServer.setNoDelay(true);

#ifdef debug_wifi
  Serial.println(F("[DB] Starting HttpServer"));
#endif
  routing_websites();   // load all routes to site´s
  HttpServer.begin();

#ifdef debug_wifi
  Serial.println(F("[DB] Starting WebSocket"));
#endif
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  // start heartbeat (optional)
  // ping server every 15000 ms
  // expect pong from server within 3000 ms
  // consider connection disconnected if pong is not received 2 times
  webSocket.enableHeartbeat(15000, 3000, 2);
#endif

#ifdef debug
#if defined(ARDUINO_AVR_NANO) || defined(ARDUINO_RADINOCC1101) || defined(ARDUINO_AVR_PRO)  // ARDUINO_AVR_NANO || ARDUINO_RADINOCC1101 || ARDUINO_AVR_PRO
  Serial.println(F("[DB] -> found board without WLAN"));
#elif defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)                          // ARDUINO_ARCH_ESP8266 || ARDUINO_ARCH_ESP32
  Serial.println(F("[DB] -> found board with WLAN"));
#else   // unknown board
  Serial.println(F("[DB] -> found unknown board"));
#endif  // END - BOARDS
#endif  // END debug

  ReceiveModeNr = 255;
  for (uint8_t modeNr = 0; modeNr < NUMBER_OF_MODES; modeNr++) {
    ToggleTimeMode[modeNr] = EEPROM.read(EEPROM_ADDR_ToggleTime + modeNr);  // scan time all modes
    ToggleArray[modeNr] = EEPROM.read(EEPROM_ADDR_ToggleMode + modeNr);     // read all modes if enabled/disabled
    if (ToggleArray[modeNr] > 1) {  // reset after flash is cleared
      ToggleArray[modeNr] = 0;      // disable mode
      EEPROM.write(EEPROM_ADDR_ToggleMode + modeNr, 0);
#ifdef CODE_ESP
      EEPROM.commit();
#endif
    }
    ToggleCnt += ToggleArray[modeNr]; // count enabled modes
    if (ReceiveModeNr == 255 && ToggleArray[modeNr] == 1) { // found first enbabled mode
      ReceiveModeNr = modeNr;
      toggleTick = ToggleTimeMode[modeNr]; // set toggle time
    }
  }
#ifdef debug_chip
  Serial.println(F("Available modes:"));
  for (uint8_t x = 0; x < NUMBER_OF_MODES; x++) {
    Serial.print(x);
    Serial.print(F(": "));
    Serial.println(getModeName(x));
  }
  Serial.print(F("[DB] ToggleCnt:       ")); Serial.println(ToggleCnt);
  Serial.print(F("[DB] ReceiveModeNr:   ")); Serial.println(ReceiveModeNr);
  Serial.print(F("[DB] ReceiveModeName: ")); Serial.println(getModeName(ReceiveModeNr));
  delay(10000);
#endif
  ChipInit();
  if (ToggleCnt == 1) {                 // wechseln in den zuletzt aktivierten Empfangsmodus
    Interupt_Variant(ReceiveModeNr);    // Empfangsvariante & Register einstellen
  }
}

/* --------------------------------------------------------------------------------------------------------------------------------- void setup end */
void loop() {
#ifdef COUNT_LOOP
  countLoop++;
#endif
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  /* https://arduino-esp8266.readthedocs.io/en/3.1.2/reference.html#timing-and-delays
     delay(ms) pauses the sketch for a given number of milliseconds and allows WiFi and TCP/IP tasks to run. */
  // delay(1);
#ifdef ARDUINO_ARCH_ESP8266
  yield();
#endif

  ArduinoOTA.handle();        // OTA Updates
  Telnet();                   // Telnet Input´s
  HttpServer.handleClient();
  webSocket.loop();           // 53 µS
#endif

  if ((millis() - secTick) >= 1000UL) { // jede Sekunde
    secTick += 1000UL;
    uptime++;
#ifdef COUNT_LOOP
    Serial.println(countLoop);
    countLoop = 0;
#endif
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
    WebSocket_index();                  // Dauer: ohne connect ca. 100 µS, 1 Client ca. 1700 µS, 2 Clients ca. 2300 µS
#endif
    if (ToggleCnt > 1) {                // Toggle Option
      if (millis() - toggleTick > (uint32_t)ToggleTimeMode[ReceiveModeNr] * 1000 ) { // Abfragen, ob Zeit zum einschalten erreicht
        toggleTick = millis();                  // Zeit merken, an der eingeschaltet wurde
        ToggleOnOff();
      }
    }
    /* ### to DEBUG ###
       print RAM-Info, see https://arduino-esp8266.readthedocs.io/en/latest/libraries.html#esp-specific-apis */
    /* if (uptime % 5 == 0) {
      Serial.print(F("ESP8266 FreeHeap:          "));
      Serial.println(ESP.getFreeHeap());         // free heap size.
      Serial.print(F("ESP8266 MaxFreeBlockSize:  "));
      Serial.println(ESP.getMaxFreeBlockSize()); // largest contiguous free RAM block in the heap, useful for checking heap fragmentation. NOTE: Maximum malloc() -able block will be smaller due to memory manager overheads.
      Serial.print(F("ESP8266 HeapFragmentation: "));
      Serial.println(ESP.getHeapFragmentation()); // fragmentation metric (0% is clean, more than ~50% is not harmless)
      } */
  }

  if (Serial.available() > 0) { // Serial Input
#ifdef ARDUINO_ARCH_ESP32
    String msgSerial = "";
    msgSerial.reserve(48); // TODO - Wie lang ist das längste Kommando?
    while (Serial.available()) {
      msgSerial += String(char(Serial.read()));   // only ESP32 | readString() no function
    }
#else
    String msgSerial = Serial.readString(); // other all
#endif
    msgSerial.trim();                       // String, strip off any leading/trailing space and \r \n
#ifdef debug
    Serial.print(F("[DB] Serial input: ")); Serial.println(msgSerial);
#endif  // END - debug
    client_now = 255;                      // current client is set where data is received
    InputCommand(msgSerial);
  }

#ifdef RFM69
  if (FSK_RAW != 2) { // no WMBUS
    if (SX1231_CrcOn) { // CrcOn - 1  On
      FSK_RAW = (Chip_readReg(0x28, 0) & 0b00000010) >> 1; // RegIrqFlags2, CrcOk - Set in Rx when the CRC of the payload is Ok. Cleared when FIFO is empty.
    } else {
      FSK_RAW = (Chip_readReg(0x28, 0) & 0b00100000) >> 5; // RegIrqFlags2, FifoLevel - Set when the number of bytes in the FIFO strictly exceeds FifoThreshold, else cleared.
    }
  }
#endif

#if defined (WMBus_S) || defined (WMBus_T)
  if (FSK_RAW == 2) { // WMBUS
    mbus_task();
  }
#endif

  /* not OOK */
  if ( (FSK_RAW == 1) && (ChipFound) ) { /* Received data | RX (not OOK !!!) */
    FSK_RAW = 0;
    digitalWriteFast(LED, HIGH);    /* LED on */
    rssi = Chip_readRSSI();
#ifdef CC110x
    CC110x_readFreqErr();
#endif

#if defined(debug_cc110x_ms) &&  defined(CC110x)    /* MARCSTATE – Main Radio Control State Machine State */
#ifdef CODE_AVR
    Serial.print(F("[DB] CC110x_MARCSTATE ")); Serial.println(Chip_readReg(CC110x_MARCSTATE, READ_BURST), HEX);
#elif CODE_ESP
    MSG_OUTPUTALL(F("[DB] CC110x_MARCSTATE ")); MSG_OUTPUTALLLN(String(Chip_readReg(CC110x_MARCSTATE, READ_BURST), HEX));
#endif  // END - CODE_AVR || CODE_ESP
#endif  // END - if defined(debug_cc110x_ms) &&  defined(CC110x)

    uint8_t ReceiveModePKTLEN = pgm_read_word(&(Registers[ReceiveModeNr].PKTLEN));
    if (ReceiveModePKTLEN == 0) {                                // by user and factory setting
      ReceiveModePKTLEN = Chip_readReg(CHIP_PKTLEN, READ_BURST); // direct PKTLEN register
    }
    uint8_t uiBuffer[ReceiveModePKTLEN];                         // Array anlegen
    Chip_readBurstReg(uiBuffer, CHIP_RXFIFO, ReceiveModePKTLEN); // Daten aus dem FIFO lesen
    // msgOutput_MN(uint8_t * data, uint16_t lenData, uint8_t wmbusFrameTypeB, uint8_t lqi, uint8_t rssi, int8_t freqErr);
    msgOutput_MN(uiBuffer, ReceiveModePKTLEN, 0, 0, rssi, freqErr); // MN - Nachricht erstellen und ausgeben

#if defined(debug_cc110x_ms) &&  defined(CC110x)    /* MARCSTATE – Main Radio Control State Machine State */
#ifdef CODE_AVR
    Serial.print(F("[DB] CC110x_MARCSTATE ")); Serial.println(Chip_readReg(CC110x_MARCSTATE, READ_BURST), HEX);
#elif CODE_ESP
    MSG_OUTPUTALL(F("[DB] CC110x_MARCSTATE ")); MSG_OUTPUTALLLN(String(Chip_readReg(CC110x_MARCSTATE, READ_BURST), HEX));
#endif  // END - CODE_AVR || CODE_ESP
#endif  // END - if defined(debug_cc110x_ms) &&  defined(CC110x)

    Chip_setReceiveMode();      // start receive mode
    digitalWriteFast(LED, LOW); /* LED off */
  } else {
#ifdef OOK_MU_433
    /* OOK */
    int aktVal = 0;
    while (FiFo.count() > 0 ) {       // Puffer auslesen und an Dekoder uebergeben
      aktVal = FiFo.dequeue();    // get next element
      decode(aktVal);
#ifdef ARDUINO_ARCH_ESP8266
      if (FiFo.count() < 120) {
        yield();
      }
#endif
    }
#endif
  }

  /* only for send data from webfronted -> ESP devices */
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  if (msgRepeats > 0) {
    if (millis() >= msgSendStart) {
      digitalWriteFast(LED, HIGH);    // LED on
      uint8_t PKTLENis = Chip_readReg(CHIP_PKTLEN, READ_BURST); // old packet length
      uint8_t len = strlen(senddata_esp) / 2;
      Chip_writeReg(CHIP_PKTLEN, len);
#ifdef RFM69
      Chip_writeReg(0x3C, len - 1); // FifoThreshold setting
#endif
      Chip_sendFIFO(senddata_esp);
#ifdef CC110x
      for (uint8_t i = 0; i < 255; i++) {
        if (Chip_readReg(CC110x_MARCSTATE, READ_BURST) == 0x01) {  // 1 (0x01) IDLE
          break;
        }
        delay(1);
      }
#endif
      msgSendStart = millis() + msgSendInterval;
      msgRepeats--;
      Chip_writeReg(CHIP_PKTLEN, PKTLENis); // restore old packet length
#ifdef RFM69
      Chip_writeReg(0x3C, PKTLENis - 1);    // restore FifoThreshold setting
#endif
      Chip_setReceiveMode();                // start receive mode
      if (webSocket.connectedClients() > 0) {
        String txt = F("TX,");
        txt += msgRepeats;
        txt += ',';
        txt += msgSendInterval;
        for (uint8_t num = 0; num < WEBSOCKETS_SERVER_CLIENT_MAX; num++) {
          if (webSocketSite[num] == "/raw") {
            webSocket.sendTXT(num, txt);
          }
        }
      }
      digitalWriteFast(LED, LOW);    // LED off
    }
  }
#endif
}
/* --------------------------------------------------------------------------------------------------------------------------------- void loop end */

String getModeName(const uint8_t modeNr) {
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  uint8_t len = strlen_P((char*)pgm_read_dword(&(Registers[modeNr].regName)));
  char buf[len + 1] = {};
  return strcpy_P(buf, (char*)pgm_read_dword(&(Registers[modeNr].regName)));
#else
  uint8_t len = strlen_P((char*)pgm_read_word(&(Registers[modeNr].regName)));
  char buf[len + 1] = {};
  return strcpy_P(buf, (char*)pgm_read_word(&(Registers[modeNr].regName)));
#endif
}

void msgOutput_MN(uint8_t * data, uint16_t lenData, uint8_t frameTypeB, uint8_t lqi, uint8_t rssi, int8_t freqErr) {
  char chHex[3]; // for hex output
  msgCount++;
  msgCountMode[ReceiveModeNr]++;
  String msg = "";
  msg.reserve(255);
  MSG_BUILD_MN(char(2));        // STX
  MSG_BUILD_MN(MSG_BUILD_Data); // "MN;D=" | "data: "
  if (frameTypeB) {
    MSG_BUILD_MN('Y'); // special marker for frame type B
  }
  for (uint16_t i = 0; i < lenData; i++) {
    onlyDecToHex2Digit(data[i], chHex); // convert 1 byte to 2 hex char
    MSG_BUILD_MN(chHex);
  }
  if (FSK_RAW == 2) {
    onlyDecToHex2Digit(lqi, chHex); // convert 1 byte to 2 hex char
    MSG_BUILD_MN(chHex);  // LQI
    onlyDecToHex2Digit(rssi, chHex); // convert 1 byte to 2 hex char
    MSG_BUILD_MN(chHex); // RSSI
  }
  MSG_BUILD_MN(MSG_BUILD_RSSI); // ";R=" | "; RSSI="
  MSG_BUILD_MN(rssi);
  MSG_BUILD_MN(MSG_BUILD_AFC);  // ";A=" | "; FREQAFC="
  MSG_BUILD_MN(freqErr);
  MSG_BUILD_MN(';');
  MSG_BUILD_MN(char(3));        // ETX
#ifndef SIGNALduino_comp
  MSG_BUILD_MN(char(13));       // CR
#endif
  MSG_BUILD_MN(char(10));       // LF
#ifdef CODE_ESP
  MSG_OUTPUTALL(msg);   /* output msg to all */
  WebSocket_raw(msg.substring(6, msg.indexOf(";", 7))); // Dauer: kein client ca. 100 µS, 1 client ca. 900 µS, 2 clients ca. 1250 µS
#endif
}

/* #### ab hier, Funktionen mit Makroabhängigkeit #### */
void ToggleOnOff() {
#ifdef CODE_ESP
  String tmp = "";    // for temp outputs print
  tmp.reserve(256);
  client_now = 255;   /* to view message over serial */
#endif
#ifdef debug
  MSG_BUILD(F("[DB] ToggleCnt=")); MSG_BUILD_LF(ToggleCnt);
#ifdef CODE_ESP
  MSG_OUTPUT(tmp);
  tmp = "";
#endif
#endif  // END - debug
  if (ToggleCnt == 0) {
#ifdef debug
    MSG_BUILD_LF(F("[DB] Toggle STOPPED, no toggle values in togglebank!"));
#endif  // END - debug
    return;
  } else {
    ReceiveModeNr++;
    if (ReceiveModeNr >= NUMBER_OF_MODES) {
      ReceiveModeNr = 0;
    }
    while (ToggleArray[ReceiveModeNr] == 0) {
      ReceiveModeNr++;
      if (ReceiveModeNr == NUMBER_OF_MODES) {
        ReceiveModeNr = 0;
      }
    }
  }
#ifdef debug
  MSG_BUILD(F("[DB] Toggle (output all)    | switched to ")); MSG_BUILD_LF(getModeName(ReceiveModeNr));
#ifdef CODE_ESP
  MSG_OUTPUT(tmp);
#endif
#endif  // END - debug
  Interupt_Variant(ReceiveModeNr);    // set receive variant & register
}
/* ----------------------------------------------------------------- void ToggleOnOff end */


void InputCommand(String input) { /* all InputCommand´s , String | Char | marker, 255 = Serial | 0...254 = Telnet */
#ifdef CODE_ESP
  String tmp = "";        // for temp outputs print
  tmp.reserve(256);
#endif
#ifdef CC110x
  uint8_t uiBuffer[47];   // Array anlegen
#endif
  char chHex[3]; // for hex output
#ifdef debug
  for (byte i = 0; i < input.length(); i++) {
    MSG_BUILD(F("DB InputCommand [")); MSG_BUILD(i); MSG_BUILD(F("] = ")); MSG_BUILD_LF(input[i]);
  }
#ifdef CODE_ESP
  MSG_OUTPUT(tmp);
  tmp = "";
#endif
#endif  // END - debug

  switch (input[0]) { /* integrated ? t tob tot x C C<n><n> C99 CG C3E C0DnF I M P R V W<n><n><n><n> WS<n><n> */
    case '?':             /* command ? */
      MSG_BUILD(F("e, foff, ft, t, tob, tot, x, C, C3E, CG, CEA, CDA, E, I, M, P, R, SN, V, W, WS\n"));
#ifdef CODE_ESP
      MSG_OUTPUT(tmp);
#endif
      break;             /* -#-#-#-#- - - next case - - - #-#-#-#- */
    case 'f':            /* command f */
      if (ChipFound == 0) {
        NO_Chip();
      } else {

        if (input[1] == 'o' && input[2] == 'f' && input[3] == 'f') { /* command foff<n> 21040 */
          if (isNumeric(input.substring(4)) == 1) {
            freqOffset = input.substring(4).toFloat() * 1000;
            if (freqOffset > 10000 || freqOffset < -10000) {
              freqOffset = 0;
              MSG_BUILD(F("CC110x_Freq.Offset resets (The value is not in the range of -10 to 10 MHz)\n"));
            }
            EEPROM.put(EEPROM_ADDR_FOFFSET, freqOffset);
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
            EEPROM.commit();
#endif
#ifdef CC110x
            Chip_writeReg(CC110x_FSCTRL0, 0);  // 0x0C: FSCTRL0 – Frequency Synthesizer Control
#endif
            MSG_BUILD(F("Chip Freq.Offset saved to ")); MSG_BUILD_fl((freqOffset / 1000.0), 3); MSG_BUILD(F(" MHz\n"));
            ChipInit();
          } else {
            MSG_BUILD(F("CC110x Freq.Offset value is not nummeric\n"));
          }
#ifdef CODE_ESP
          MSG_OUTPUT(tmp);
#endif
        }

        if (input[1] == 't' && !input[2]) { /* command ft */
          MSG_BUILD(F("Send test signal (30 * 0xAA), 50 repetitions, with current settings.\n"));
#ifdef CODE_ESP
          MSG_OUTPUT(tmp);
#endif
          input = F("SN;D=AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA;R=50;");
          InputCommand(input);
        }
      }
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */
    case 'e': /* command e - set registers to default*/
      if (ChipFound == 0) {
        NO_Chip();
      } else { // ToDo - Toggle läuft weiter - evtl. zurück setzen?
        for (byte i = 0; i < NUMBER_OF_MODES; i++) {
#ifdef CC110x
          if (getModeName(i) == F("ASK/OOK 433 MHz")) {
            Interupt_Variant(i);
            break;
          }
#elif RFM69
          if (getModeName(i) == F("Chip factory default")) {
            Interupt_Variant(i);
            break;
          }
#endif
        }
      }
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */

    case 't':
      if (!input[1]) { /* command t - get uptime*/
        MSG_BUILD_LF(uptime);
#ifdef CODE_ESP
        MSG_OUTPUT(tmp);
#endif
      } else {

        if (input[1] && input[1] == 'o' && input[2]) { /* command tob<mm><n> & tot<mm><nnn> */
          if (ChipFound == 0) {
            NO_Chip();
          } else {
            if (input[2] == 't') { // command tot<mm><nnn> - toggletime
              if (isNumeric(input.substring(3)) && input.length() < 9) { // command tot<mm><nnn> (00-99,0-999) OK
                uint8_t modeNr = input.substring(3, 5).toInt();
                uint16_t intTime = input.substring(5).toInt();
                if (modeNr >= NUMBER_OF_MODES) { // command tot<mm><nnn> mode NOT OK
#ifdef debug
                  MSG_BUILD(F("[DB] Input, Toggle mode not available [0-")); MSG_BUILD(NUMBER_OF_MODES - 1); MSG_BUILD_LF(F("]\n"));
#endif
                } else if (intTime < TOGGLE_TIME_MIN || intTime > TOGGLE_TIME_MAX) { // command tot<mm><nnn> time NOT OK
#ifdef debug
                  MSG_BUILD(F("[DB] Input, Toggle time not in range [")); MSG_BUILD(TOGGLE_TIME_MIN);
                  MSG_BUILD('-'); MSG_BUILD(TOGGLE_TIME_MAX); MSG_BUILD(F(" seconds]\n"));
#endif
                } else { // command tot<mm><nnn> OK
#ifdef debug
                  MSG_BUILD(F("[DB] Input, Toggle time mode ")); MSG_BUILD(modeNr);
                  MSG_BUILD(F(" set to ")); MSG_BUILD(intTime); MSG_BUILD(F(" seconds\n"));
#endif
                  ToggleTimeMode[modeNr] = intTime; // set toggle time
                  EEPROM.write(EEPROM_ADDR_ToggleTime + modeNr, intTime); // save toggle time
#ifdef CODE_ESP
                  EEPROM.commit();
#endif
                  ToggleCnt = 0;
                  for (uint8_t modeNr = 0; modeNr < NUMBER_OF_MODES; modeNr++) {
                    ToggleCnt += ToggleArray[modeNr];
                  }
                  if (ToggleCnt == 0) {
#ifdef debug
                    MSG_BUILD_LF(F("[DB] Input, Toggle time set, but no mode active"));
#endif
                  } else if (ToggleCnt == 1) {
#ifdef debug
                    MSG_BUILD_LF(F("[DB] Input, Toggle time set, but only one mode active"));
#endif
                  } else {
#ifdef debug
                    MSG_BUILD(F("[DB] Input, Toggle starts, ")); MSG_BUILD(ToggleCnt); MSG_BUILD(F(" modes active\n"));
#endif
                  }
                }
              } else { // command tos<mm><nnn> (00-99, 0-999) NOT OK
#ifdef debug
                MSG_BUILD(F("[DB] Input, Toggle command incorrect, must be tos<mm><nnn> (mm=00-99,nnn=0-255)\n"));
#endif
              }
#ifdef CODE_ESP
              MSG_OUTPUT(tmp);
#endif
            } else if (input[2] == 'b') { // command tob 88, tob99 or tob<nr><0|1>
              if (isNumeric(input.substring(3))) { // check command tob<0|1><nr> or tob99 or tob88
#ifdef debug
                MSG_BUILD(F("[DB] Input, cmd tob ")); MSG_BUILD(input.substring(3)); MSG_BUILD(F(" accepted\n"));
#endif  // END - debug
                if (input[3] == '8' && input[4] == '8') {        // command tob88 -> scan all modes
#ifdef debug
                  MSG_BUILD(F("[DB] Input, scan mode active (mode changes every 60 seconds\n"));
#endif  // END - debug
                  for (uint8_t modeNr = 0; modeNr < NUMBER_OF_MODES; modeNr++) {
                    ToggleArray[modeNr] = 1;       // enable mode
                    ToggleTimeMode[modeNr] = 60;   // set toggle time to 60 seconds
                  }
                  ToggleCnt = NUMBER_OF_MODES - 1; // enable toggle
                  ReceiveModeNr = NUMBER_OF_MODES - 1;
                  toggleTick = millis();
                } else if (input[3] == '9' && input[4] == '9') { // command tob99 -> reset togglebank
#ifdef debug
                  MSG_BUILD(F("[DB] Input, toggleBank reset and STOP Toggle\n"));
#endif  // END - debug
                  for (byte i = 0; i < NUMBER_OF_MODES; i++) {
                    ToggleArray[i] = 0; // disable mode
                    EEPROMwrite(i + EEPROM_ADDR_ToggleMode, 0);
                  }
                  ToggleCnt = 0; // stop toggle
                } else {                                                 // command tob<nr><0|1> -> enable/disable toggle mode
                  uint8_t enable = input.substring(5, 6).toInt(); // tob130
                  if (enable > 1) { // command tob<nr><0|1> enable|disable NOT OK
#ifdef debug
                    MSG_BUILD(F("[DB] Input, last digit incorrect [0|1]")); MSG_BUILD(F("\n"));
#endif
                  } else if (input.substring(3, 5).toInt() >= NUMBER_OF_MODES) { // command tob<nr><0|1> mode NOT OK
#ifdef debug
                    MSG_BUILD(F("[DB] Input, mode number greater available modes [")); MSG_BUILD(NUMBER_OF_MODES - 1); MSG_BUILD(F("]\n"));
#endif
                  } else { // command tob<nr><0|1> enable|disable and mode OK
                    ReceiveModeNr = input.substring(3, 5).toInt();
                    ToggleArray[ReceiveModeNr] = enable; // set mode enable/disable
                    EEPROMwrite(ReceiveModeNr + EEPROM_ADDR_ToggleMode, enable);
                    ToggleCnt = 0;
#ifdef debug
                    MSG_BUILD(F("[DB] Input, ToggleBank mode ")); MSG_BUILD(getModeName(ReceiveModeNr));
                    MSG_BUILD(F(" set to ")); MSG_BUILD(enable); MSG_BUILD(F("\n"));
#endif
                    for (uint8_t modeNr = 0; modeNr < NUMBER_OF_MODES; modeNr++) {
                      ToggleCnt += ToggleArray[modeNr];
                      if (ToggleArray[modeNr] == 1) {
                        ReceiveModeNr = modeNr;
                      }
                    }
                    if (ToggleCnt == 0) {
                      ReceiveModeNr = 255;
#ifdef CC110x
                      CC110x_CmdStrobe(CC110x_SIDLE); // Exit RX / TX, turn off frequency synthesizer and exit Wake-On-Radio mode if applicable
#elif RFM69
                      SX1231_setIdleMode();           // SX1231 start idle mode
#endif
                    } else if (ToggleCnt == 1) {
                      Interupt_Variant(ReceiveModeNr);
                    } else if (ToggleCnt > 1) {
                      toggleTick = millis();
                      Interupt_Variant(ReceiveModeNr);
                    }
                  }
                }
              } else { // command tob<nn><m> (00-99, 0-999) or tob88 or tob NOT OK
#ifdef debug
                MSG_BUILD(F("[DB] Input, Command tob incorrect, must be tob<m><nn> (m=0|1 n=0-99), tob88 or tob99\n"));
#endif
              }
            }
#ifdef CODE_ESP
            MSG_OUTPUT(tmp);
#endif
          }
        }
      }
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */

#ifdef CC110x // TODO SX1231
    case 'x': /* command x - write patable*/
      if (ChipFound == 0) {
        NO_Chip();
      } else {
        if (input[1] && input[2] && !input[3]) {
          if (isHexadecimalDigit(input[1]) && isHexadecimalDigit(input[2])) {
            for (byte i = 0; i < 8; i++) {
              if (i == 1) {
                uiBuffer[i] = hexToDec(input.substring(1));
                EEPROMwrite(EEPROM_ADDR_PATABLE + i, uiBuffer[i]);
              } else {
                uiBuffer[i] = 0;
                EEPROMwrite(EEPROM_ADDR_PATABLE + i, 0);
              }
            }
            CC110x_writeBurstReg(uiBuffer, CC110x_PATABLE, 8);
            MSG_BUILD(F("write ")); MSG_BUILD(input[1]); MSG_BUILD(input[2]); MSG_BUILD(F(" to PATABLE done\n"));
#ifdef CODE_ESP
            MSG_OUTPUT(tmp);
#endif
          }
        }
      }
      break; /* -#-#-#-#- - - next case - - - #-#-#-#- */
#endif  // END - CC110x

    case 'C':
      if (ChipFound == 0) {
        NO_Chip();
      } else {
        if (!input[1]) { /* command C - Read all values from Register */
          MSG_BUILD(F("Current register (address = value (value EEPROM)\n"));
#ifdef CODE_ESP
          MSG_OUTPUT(tmp);
#endif
          for (byte i = 0; i <= REGISTER_MAX; i++) {
#ifdef CODE_ESP
            tmp = "";
#endif
            MSG_BUILD(F("Addr. 0x"));
            uint8_t addr = i;
#ifdef RFM69
            if (i >= 80) {
              addr = SX1231_RegAddrTranslate[i - 80];
            }
#endif
            onlyDecToHex2Digit(addr, chHex); // convert 1 byte to 2 hex char
            MSG_BUILD(chHex);
            MSG_BUILD(F(" = 0x"));
#ifdef RFM69
            if (i == 0) { // SX1231 Register 0x00 FIFO nicht lesen
              MSG_BUILD(F("00"));
            } else {
              onlyDecToHex2Digit((Chip_readReg(addr, READ_BURST)), chHex); // convert 1 byte to 2 hex char
              MSG_BUILD(chHex);
            }
#else
            onlyDecToHex2Digit((Chip_readReg(addr, READ_BURST)), chHex); // convert 1 byte to 2 hex char
            MSG_BUILD(chHex);
#endif
            MSG_BUILD(F(" (0x"));
            onlyDecToHex2Digit(EEPROMread(i), chHex); // convert 1 byte to 2 hex char
            MSG_BUILD(chHex);
            MSG_BUILD(F(")\n"));
#ifdef CODE_ESP
            MSG_OUTPUT(tmp);
#endif
          }

        } else if (input[1] && input[2] && !input[3]) { /* command C<n><n> - Read HEX adress values from Register, set raw CDA/CEA - disable/enable AFC */
          uint8_t Cret = hexToDec(input.substring(1));
          if (Cret <= REGISTER_STATUS_MAX) { // CC110x - inc. Status Registers, SX1231 - all Registers
#ifdef SIGNALduino_comp
            MSG_BUILD('C');
            onlyDecToHex2Digit(Cret, chHex); // convert 1 byte to 2 hex char
            MSG_BUILD(chHex);
            MSG_BUILD(F(" = "));
            onlyDecToHex2Digit((Chip_readReg(Cret, READ_BURST)), chHex); // convert 1 byte to 2 hex char
            MSG_BUILD_LF(chHex);
#else
            MSG_BUILD(F("Reg. 0x"));
            MSG_BUILD(onlyDecToHex2Digit(Cret));
            MSG_BUILD(F(" = 0x"));
            MSG_BUILD_LF(onlyDecToHex2Digit(Chip_readReg(Cret, READ_BURST)));
#endif  // END - SIGNALduino_comp
#ifdef CODE_ESP
            MSG_OUTPUT(tmp);
#endif

#ifdef CC110x
          } else if (Cret == 0x99) { /* command C99 - ccreg */
            Chip_readBurstReg(uiBuffer, 0x00, 47);
            for (uint8_t i = 0; i < 0x2f; i++) {
              if (i == 0 || i == 0x10 || i == 0x20) {
                if (i > 0) {
                  MSG_BUILD(' ');
                }
                MSG_BUILD(F("ccreg "));
                onlyDecToHex2Digit(i, chHex); // convert 1 byte to 2 hex char
                MSG_BUILD(chHex);
                MSG_BUILD(F(": "));
              }
              onlyDecToHex2Digit(uiBuffer[i], chHex); // convert 1 byte to 2 hex char
              MSG_BUILD(chHex);
              MSG_BUILD(' ');
            }
            MSG_BUILD("\n");
#ifdef CODE_ESP
            MSG_OUTPUT(tmp);
#endif

          } else if (Cret == 0x3E) { // command C3E - patable, only CC110x
            Chip_readBurstReg(uiBuffer, 0x3E, 8);
            MSG_BUILD(F("C3E ="));
            for (byte i = 0; i < 8; i++) {
              MSG_BUILD(' ');
              onlyDecToHex2Digit(uiBuffer[i], chHex); // convert 1 byte to 2 hex char
              MSG_BUILD(chHex);
            }
            MSG_BUILD("\n");
#ifdef CODE_ESP
            MSG_OUTPUT(tmp);
#endif
#endif// END - CC110x

          } else if (Cret == 0xDA || Cret == 0xEA) {  // command CDA/CEA - disable/enable AFC
            freqAfc = input[1] - 'D';             // CC110x AFC 0 oder 1
            freqOffAcc = 0;                           // reset cc110x afc offset
            freqErrAvg = 0;                           // reset cc110x afc average
            MSG_BUILD_LF(freqAfc == 1 ? F("AFC enabled") : F("AFC disabled"));
#ifdef CODE_ESP
            MSG_OUTPUT(tmp);
#endif
#ifdef CC110x
            Chip_writeReg(CC110x_FSCTRL0, 0);         // reset Register 0x0C: FSCTRL0 – Frequency Synthesizer Control
#elif RFM69
            SX1231_afc(freqAfc);                      // AfcAutoOn, 0  AFC is performed each time AfcStart is set, 1  AFC is performed each time Rx mode is entered
#endif  // END - CC110x || RFM69
            EEPROMwrite(EEPROM_ADDR_AFC, freqAfc);
#ifdef CODE_ESP
            EEPROM.commit();
#endif  // END - defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
          }

        } else if (input[1] == 'G' && !input[2]) { /* command CG - get config*/
          uint8_t mu = 1;
          uint8_t mn = 0;
          if (MOD_FORMAT != 3) {  // not OOK
            mu = 0;
            mn = 1;
          }
          MSG_BUILD(F("MS=0;MU="));
          MSG_BUILD(mu);
          MSG_BUILD(F(";MC=0;MN="));
          MSG_BUILD(mn);
          MSG_BUILD(F(";AFC="));
          MSG_BUILD(freqAfc);         // AFC an oder aus
          MSG_BUILD(F(";Mred=0\n"));
#ifdef CODE_ESP
          MSG_OUTPUT(tmp);
#endif

#ifdef CC110x
        } else if (input == "C0DnF") { /* command C0DnF - get ccconf */
          Chip_readBurstReg(uiBuffer, 0x0D, 18);
          MSG_BUILD(F("C0Dn11="));
          for (byte i = 0; i <= 17; i++) {
            onlyDecToHex2Digit(uiBuffer[i], chHex); // convert 1 byte to 2 hex char
            MSG_BUILD(chHex);
          }
          MSG_BUILD("\n");
#ifdef CODE_ESP
          MSG_OUTPUT(tmp);
#endif
#endif// END - CC110x
        }
      }
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */

    case 'E': /* command E */
      if (!input[1]) {
        EEPROMread_table();
      }
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */

    case 'I': /* command I */
      if (!input[1]) {
        MSG_BUILD(F("# # #   current status   # # #\n"));
        String ReceiveModeName = "";
        if (ReceiveModeNr > NUMBER_OF_MODES && ChipFound) {         // ReceiveModeName if uC restart
          ReceiveModeName = F("Chip configuration");
        } else if (ReceiveModeNr < NUMBER_OF_MODES && ChipFound) {  // ReceiveModeName is set with command m<n>
          ReceiveModeName = getModeName(ReceiveModeNr);
        } else if (ChipFound == 0) {                          // ReceiveModeName if no chip
          ReceiveModeName = F("Chip NOT recognized");
        }
        if (ChipFound) {
#ifdef CC110x
          onlyDecToHex2Digit((Chip_readReg(CC110x_MARCSTATE, READ_BURST)), chHex); // convert 1 byte to 2 hex char
          MSG_BUILD(F("CC110x MARCSTATE:  ")); MSG_BUILD_LF(chHex);
#elif RFM69
          onlyDecToHex2Digit(((Chip_readReg(0x01, READ_BURST) & 0b00011100) >> 2), chHex); // convert 1 byte to 2 hex char
          MSG_BUILD(F("SX1231 op mode:    ")); MSG_BUILD_LF(chHex);
#endif  // END - CC110x || RFM69
          MSG_BUILD(F("Chip Freq. Afc:    ")); MSG_BUILD_LF(freqAfc == 1 ? F("on (1)") : F("off (0)"));
          MSG_BUILD(F("Chip Freq. offset: ")); MSG_BUILD_fl((freqOffset / 1000.0), 3); MSG_BUILD(F(" MHz\n"));
          MSG_BUILD(F("Receive mode:      ")); MSG_BUILD_LF(ReceiveModeName);
          MSG_BUILD(F("Message count:     ")); MSG_BUILD_LF(msgCount);
          MSG_BUILD(F("Enabled mode(s):   "));
          for (uint8_t modeNr = 0; modeNr < NUMBER_OF_MODES; modeNr++) {
            if (ToggleArray[modeNr]) {
              MSG_BUILD(modeNr);
              MSG_BUILD(' ');
            }
          }
          MSG_BUILD(F("\n"));
#ifdef CODE_ESP
          MSG_OUTPUT(tmp);
#endif
        } else {
          NO_Chip();
        }
      }

      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */

    case 'M': /* command M */
      if (!input[1]) {
        MSG_BUILD_LF(F("available register modes:"));
        for (uint8_t i = 0; i < NUMBER_OF_MODES; i++) {
          if (i < 10) {
            MSG_BUILD('0');
          }
          MSG_BUILD(i);
          MSG_BUILD(F(" - "));
          MSG_BUILD(getModeName(i));
          MSG_BUILD(F(" ("));
          MSG_BUILD(ToggleArray[i] == 1 ? F("enabled") : F("disabled"));
          if (ToggleArray[i]) {
            MSG_BUILD(F(", msg count "));
            MSG_BUILD(msgCountMode[i]);
            MSG_BUILD(F(", scan time "));
            MSG_BUILD(ToggleTimeMode[i]);
          }
          MSG_BUILD_LF(')');
        }
#ifdef CODE_ESP
        MSG_OUTPUT(tmp);
#endif
      }
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */

    case 'P': /* command P - Ping */
      if (!input[1]) {
        MSG_BUILD_LF(F("OK"));
      }
#ifdef CODE_ESP
      MSG_OUTPUT(tmp);
#endif
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */

    case 'R': /* command R */
      if (!input[1]) {
        MSG_BUILD_LF(freeRam());
      }
#ifdef CODE_ESP
      MSG_OUTPUT(tmp);
#endif
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */

    case 'S': /* command S */
      if (ChipFound == 0) {
        NO_Chip();
      } else {
        if (input[1] && input[1] == 'N' && input[2] == ';') { /* command SN | SN;R=5;D=9A46036AC8D3923EAEB470AB; */
#ifdef debug
          MSG_BUILD_LF(F("[DB] Input | SN; raw message"));
#endif  // END - debug
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
                MSG_BUILD(F("[DB] Input | SN; found ")); MSG_BUILD_LF(ptr);
#endif  // END - debug
                if ((String(ptr).substring(2)).toInt() != 0) { /* repeats */
                  rep = (String(ptr).substring(2)).toInt();
#ifdef debug
                  MSG_BUILD_LF(F("[DB] Input | SN; takeover repeats"));
#endif  // END - debug
                }
              } else if (strstr(ptr, "D=")) { /* datapart */
#ifdef debug
                MSG_BUILD(F("[DB] Input | SN; found ")); MSG_BUILD(ptr);
                MSG_BUILD(F(" with length ")); MSG_BUILD_LF(String(ptr + 2).length());
#endif  // END - debug
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
                  MSG_BUILD_LF(F("SN; found odd number of nibbles, no send !!!"));
                }
              }
              ptr = strtok(NULL, delimiter);
            }
            /* SEND */
            if (datavalid == 1) {
              digitalWriteFast(LED, HIGH);  // LED on
#ifdef debug
              MSG_BUILD(F("[DB] Input | SN; valid and ready to send with repeats=")); MSG_BUILD_LF(rep);
              MSG_BUILD_LF(senddata);
#endif  // END - debug
              uint8_t PKTLENis = Chip_readReg(CHIP_PKTLEN, READ_BURST); // old packet length
#ifdef debug
              MSG_BUILD(F("[DB] SEND from FHEM, old PKTLEN is ")); MSG_BUILD_LF(PKTLENis);
#endif  // END - debug
              uint8_t len = strlen(senddata) / 2;
              Chip_writeReg(CHIP_PKTLEN, len);
#ifdef RFM69
              Chip_writeReg(0x3C, len - 1); // FifoThreshold setting
#endif  // END - RFM69
#ifdef debug
              MSG_BUILD(F("[DB] SEND from FHEM, act PKTLEN is ")); MSG_BUILD_LF(len);
#endif  // END - debug

              /*
                AVANTEK           SN;D=08C114844FDA5CA2;R=1;
                Bresser 5in1      SN;D=E7527FF78FF7EFF8FDD7BBCAFF18AD80087008100702284435000002;R=1;
                Fine_Offset_WH57  SN;D=5740C655043F01DED4;R=3;
                Lacrosse_mode1    SN;D=9006106A62;R=1;
              */
              for (uint8_t repeats = 1; repeats <= rep; repeats++) {
                Chip_sendFIFO(senddata);
                delay(100);
              }
              Chip_writeReg(CHIP_PKTLEN, PKTLENis); // restore old packet length
#ifdef RFM69
              Chip_writeReg(0x3C, PKTLENis - 1);    // restore FifoThreshold setting
#endif
              Chip_setReceiveMode();                // enable RX
              digitalWriteFast(LED, LOW);           // LED off
            }
          }
#ifdef CODE_ESP
          MSG_OUTPUT(tmp);
#endif
        }
      }
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */

    case 'V': /* command V */
      if (!input[1]) {
#ifdef CODE_AVR
        Serial.print((__FlashStringHelper*)TXT_VERSION);
        Serial.print(F("- compiled at "));
        Serial.println((__FlashStringHelper*)compile_date);
#elif CODE_ESP
        MSG_BUILD(FPSTR(TXT_VERSION)); MSG_BUILD(F("- compiled at ")); MSG_BUILD_LF(FPSTR(compile_date));
        MSG_OUTPUT(tmp);
#endif  // END - CODE_AVR || CODE_ESP
      }
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */

    case 'W': /* command W - write register*/
      if (ChipFound == 0) {
        NO_Chip();
      } else {
        if (input[1] && input[1] != 'S' && input[1] > 47 && input[1] < CMD_W_REG_MAX && input[2] && input[3] && input[4] && !input[5]) {
          /* command W1203 | only adress smaller 3E -> input[4] > 47 && input[4] < 52 for W0... W1... W2... W3... (CC110x, RFM69 W0 - W7 */
          if (isHexadecimalDigit(input[1]) && isHexadecimalDigit(input[2]) && isHexadecimalDigit(input[3]) && isHexadecimalDigit(input[4])) {
            byte adr_dec = hexToDec(input.substring(1, 3));
            if (adr_dec >= Chip_writeReg_offset) { // > 2
              byte val_dec = hexToDec(input.substring(3));
#ifdef debug
              char chHex1[3]; // for hex output
              onlyDecToHex2Digit(adr_dec - Chip_writeReg_offset, chHex1);
              MSG_BUILD(F("[DB] InputCommand | cmd W with adr=0x")); MSG_BUILD(input.substring(1, 3));
              MSG_BUILD(F(" value=0x")); MSG_BUILD_LF(input.substring(3));
              MSG_BUILD(F("[DB] write to register/EEPROM  adr=0x")); MSG_BUILD(chHex1);
              MSG_BUILD(F(" offset=")); MSG_BUILD_LF(Chip_writeReg_offset);
#ifdef CODE_ESP
              MSG_OUTPUT(tmp);
#endif
#endif  // END - debug
              Chip_writeReg(adr_dec - Chip_writeReg_offset, val_dec);   // write in chip | adr - 2 because of sduino firmware
              EEPROMwrite(adr_dec - Chip_writeReg_offset, val_dec);     // write in flash
#ifdef CC110x
              Chip_writeReg(CC110x_FSCTRL0, 0);                         // 0x0C: FSCTRL0 – Frequency Synthesizer Control
#endif  // END - CC110x
              // ReceiveModeName = getModeName(1);
              ReceiveModeNr = 1;                         // activated protocol
            }
          }
        }

#ifdef CC110x
        else if (input[1] == 'S' && input[2] == '3' && isHexadecimalDigit(input[3]) && !input[4]) {
          /* command WS34 ... | from 0x30 to 0x3D */
          uint8_t reg = hexToDec(input.substring(2, 4));
          onlyDecToHex2Digit(( CC110x_CmdStrobe(reg) >> 4 ), chHex); // convert 1 byte to 2 hex char
          delay(1);
          char chHex1[3]; // for hex output
          onlyDecToHex2Digit(( CC110x_CmdStrobe(CC110x_SNOP) >> 4 ), chHex1); // convert 1 byte to 2 hex char
#ifdef debug
          MSG_BUILD(F("cmdStrobeReg ")); MSG_BUILD(input.substring(2, 4)); MSG_BUILD(F(" chipStatus "));
          MSG_BUILD(chHex); MSG_BUILD(F(" delay1 ")); MSG_BUILD_LF(chHex1);
#ifdef CODE_ESP
          MSG_OUTPUT(tmp);
#endif
#endif  // END - debug
        }
#endif  // END - CC110x
      }
      break; /* -#-#-#-#- - - next case - - - #-#-#-#- */
    default:
      break; /* -#-#-#-#- - - next case - - - #-#-#-#- */
  }
}
/* ----------------------------------------------------------------- void InputCommand end */


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
        TelnetClient[i] = TelnetServer.accept();
        String logText = F("Telnet client ");
        logText += TelnetClient[i].remoteIP().toString();
        logText += F(" connected");
#ifdef debug_telnet
        Serial.print(F("[DB] "));
        Serial.println(logText);
#endif  // END - debug_telnet
        appendLogFile(logText); // append to logfile
        TelnetClient[i].flush(); /* clear input buffer, else you get strange characters */
        TelnetClient[i].print(F("Telnet session (")); TelnetClient[i].print(i);
        TelnetClient[i].print(F(") started to ")); TelnetClient[i].println(TelnetClient[i].localIP());

        TELNET_CLIENTS_ARRAY[i] = 1;
        TELNET_ConnectionEstablished = true;
        break;
      } else {
        /* Serial.println("Session is in use"); */
      }
    }

    if (TELNET_ConnectionEstablished == false) {
      Serial.println(F("No free sessions ... drop connection"));
      TelnetServer.accept().stop();

      for (uint8_t i = 0; i < TELNET_CLIENTS_MAX; i++) {
        if (TelnetClient[i] || TelnetClient[i].connected()) {
          TelnetClient[i].println(F("An other user cannot connect ... TELNET_CLIENTS_MAX limit is reached!"));
        }
      }
    }
  }

  String msgTelnet = "";
  for (uint8_t i = 0; i < TELNET_CLIENTS_MAX; i++) {
    if (TelnetClient[i] && TelnetClient[i].connected()) {
      TelnetClient[i].setTimeout(Timeout_Telnet); /* sets the maximum milliseconds to wait for serial data. It defaults to 1000 milliseconds. */

      if (TelnetClient[i].available() > 0) {
#ifdef debug_telnet
        Serial.print(F("[DB] Telnet, Data from session ")); Serial.print(i); Serial.print(F(" with length "));
        Serial.println(TelnetClient[i].available());
#endif  // END - debug_telnet
        client_now = i; /* current telnet client is set where data is received */
      }
      while (TelnetClient[i].available()) { /* get data from the telnet client */
#ifdef ARDUINO_ARCH_ESP8266
        msgTelnet = TelnetClient[i].readString();           /* only ESP8266 */
#elif ARDUINO_ARCH_ESP32
        msgTelnet += String(char(TelnetClient[i].read()));  /* only ESP32 | readString() no function | .read() empties .available() character by character */
#endif  // END - ARDUINO_ARCH_ESP8266 || ARDUINO_ARCH_ESP32
      }

      if (msgTelnet.length() > 0 && msgTelnet.length() <= BUFFER_MAX) {
        msgTelnet.trim();   /* String, strip off any leading/trailing space and \r \n */
        InputCommand(msgTelnet);
        msgTelnet = "";
      }
    }
  }
}


/* #### Prozessereignis: Neue Daten vom Client empfangen #### */
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_PING: /* pong will be send automatically */
#ifdef debug_websocket
      Serial.printf("[DB] WebSocket [%u] connected - Ping!\n", num);
#endif  // END - debug_websocket
      break;
    case WStype_PONG: /* // answer to a ping we send */
#ifdef debug_websocket
      Serial.printf("[DB] WebSocket [%u] connected - Pong!\n", num);
#endif  // END - debug_websocket
      break;
    case WStype_DISCONNECTED:
#ifdef debug_websocket
      Serial.printf("[DB] WebSocket [%u] disconnected!\n", num);
#endif  // END - debug_websocket
      webSocketSite[num] = "";
      break;
    case WStype_CONNECTED:
      {
#ifdef debug_websocket
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[DB] WebSocket [%u] connected - from %d.%d.%d.%d - site %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
#endif  // END - debug_websocket
        webSocketSite[num] = (char * )payload;
        webSocket.sendTXT(num, "Connected"); /* send message to client */
        if (webSocketSite[num] == "/detail") {
          WebSocket_detail(0); // Send values of registers before changes to the /detail web page.
        }
      }
      break;
    case WStype_TEXT: /*  */
      // webSocket.sendTXT(num, "message here"); /* send message to client */
      // webSocket.broadcastTXT("message here"); /* send data to all connected clients */
      {

#ifdef debug_websocket
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[DB] WebSocket [%u] receive - from %d.%d.%d.%d - site %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
#endif  // END - debug_websocket
        String payloadString = (const char *)payload;
        if (payloadString == "detail") {
          WebSocket_detail(1); // Send values of the registers at the request of the website /detail.
        } else if (payloadString == "chip") {
          WebSocket_chip();
        } else if (payloadString == "help") {
          WebSocket_help();
        } else if (payloadString.substring(0, 5) == "send,") {    /* input: send,1269A5900F41,2,3 */
          String hex;
          uint8_t pos1;
          uint8_t pos2;
          pos1 = payloadString.indexOf(',', 5);
          hex = payloadString.substring(5, pos1);
          pos2 = payloadString.indexOf(',', pos1 + 1);
          msgRepeats = payloadString.substring(pos1 + 1, pos2).toInt() + 1;
          msgSendStart = 0;
          msgSendInterval = payloadString.substring(pos2 + 1).toInt();
          hex.toCharArray(senddata_esp, hex.length() + 1);
#ifdef debug_chip
          Serial.print(F("[DB] send, msgRepeats:      ")); Serial.println(msgRepeats);
          Serial.print(F("[DB] send, msgSendInterval: ")); Serial.println(msgSendInterval);
          Serial.print(F("[DB] send, senddata:        ")); Serial.println(senddata_esp);
#endif  // END - debug_chip
        } else if (payloadString == "modes") {
          WebSocket_modes();
        } else if (payloadString.substring(0, 4) == "imp,") {
          WebSocket_imp(payloadString);
        }
      }
      break;
    case WStype_BIN: /* binary files receive */
      length += 0; // without comes warning: unused parameter 'length'
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
#endif  // END - defined (ARDUINO_ARCH_ESP8266) || defined (ARDUINO_ARCH_ESP32)


/* for OOK modulation */
#ifdef OOK_MU_433
void decode(const int pulse) {  /* Pulsübernahme und Weitergabe */
  if (MsgLen > 0) {
    last = first;
  } else {
    last = 0;
  }
  first = pulse;
  // Pulsprüfung und Weitergabe an Patternprüfung
  bool valid = (MsgLen == 0 || last == 0 || (first ^ last) < 0);   // true if a and b have opposite signs
  valid &= (MsgLen == MsgLenMax) ? false : true;
  valid &= ( (first > -t_maxP) && (first < t_maxP) );         // if low maxPulse detected, start processMessage()
#ifdef debug_cc110x_MU
  Serial.print(F("[DB] PC:")); Serial.print(PatNmb); Serial.print(F(" ML:")); Serial.print(MsgLen); Serial.print(F(" v:")); Serial.print(valid);
  Serial.print(F(" | ")); Serial.print(first); Serial.print(F("    ")); Serial.println(last);
#endif  // END - debug_cc110x_MU
  if (valid) {
    findpatt(first);
  } else {
#ifdef debug_cc110x_MU
    Serial.println(F("[DB] -- RESET --"));
#endif  // END - debug_cc110x_MU
    msgOutput_MU();
  }
}

void msgOutput_MU() { /* Nachrichtenausgabe */
  if (MsgLen >= MsgLenMin) {
    rssi = Chip_readReg(CC110x_RSSI, READ_BURST);  // not converted
    digitalWriteFast(LED, HIGH);      // LED on
    uint8_t CP_PaNum = 0;
    int16_t PulseAvgMin = 32767;
    String raw = "MU"; // 423456   code in flash
    raw.reserve(360);
    for (uint8_t i = 0; i <= PatNmb; i++) {
      int16_t PulseAvg = ArPaSu[i] / ArPaCnt[i];
      raw += ";P"; raw += i; raw += '='; raw += PulseAvg; // 423460   code in flash
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
    //    raw += F("w="); raw += valid; raw += ';';    /* letzter Puls zu vorherigen Puls msgMU valid bzw. unvalid /  */
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
    Chip_readRSSI(); // TODO - wird am Beginn msgOutput_MU schon gelesen, allerdings nicht dezimal
    WebSocket_raw(raw);
#endif  // END - defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
    raw.replace("M", "M");
    raw += char(3); raw += char(10);
#ifdef CODE_AVR
    Serial.print(
#elif CODE_ESP
    MSG_OUTPUTALL(
#endif  // END - CODE_AVR || CODE_ESP
      raw);
    msgCount++;
    msgCountMode[ReceiveModeNr]++;
    digitalWriteFast(LED, LOW);  // LED off
  }
  // Zurücksetzen nach Nachrichtenbau oder max. Länge
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


void findpatt(int val) {  /* Patterneinsortierung */
  for (uint8_t i = 0; i < PatMaxCnt; i++) {
    if (MsgLen == 0) {  /* ### nach RESET ### */
      msg = i;
      ArPaCnt[i] = 1;
      ArPaT[i] = val;
      ArPaSu[i] = val;
      MsgLen++;
#ifdef debug_cc110x_MU
      Serial.print(F("[DB] "));
      Serial.print(i); Serial.print(F(" | ")); Serial.print(ArPaT[i]); Serial.print(F(" msgL0: ")); Serial.print(val);
      Serial.print(F(" l: ")); Serial.print(last); Serial.print(F(" PatN: ")); Serial.print(PatNmb); Serial.print(F(" msgL: ")); Serial.print(MsgLen); Serial.print(F(" Fc: ")); Serial.println(FiFo.count());
#endif  // END - debug_cc110x_MU
      break;
      /* ### in Tolleranz und gefunden ### */
    } else if ( (val > 0 && val > ArPaT[i] * (1 - PatTol) && val < ArPaT[i] * (1 + PatTol)) ||
                (val < 0 && val < ArPaT[i] * (1 - PatTol) && val > ArPaT[i] * (1 + PatTol)) ) {
      msg += i;
      ArPaCnt[i]++;
      ArPaSu[i] += val;
      MsgLen++;
#ifdef debug_cc110x_MU
      Serial.print(F("[DB] "));
      Serial.print(i); Serial.print(F(" | ")); Serial.print(ArPaT[i]); Serial.print(F(" Pa T: ")); Serial.print(val);
      Serial.print(F(" l: ")); Serial.print(last); Serial.print(F(" PatN: ")); Serial.print(PatNmb); Serial.print(F(" msgL: ")); Serial.print(MsgLen); Serial.print(F(" Fc: ")); Serial.println(FiFo.count());
#endif  // END - debug_cc110x_MU
      break;
    } else if (i < (PatMaxCnt - 1) && ArPaT[i + 1] == 0 ) { /* ### nächste freie Pattern ### */
      msg += i + 1;
      PatNmb++;
      ArPaCnt[i + 1]++;
      ArPaT[i + 1] = val;
      ArPaSu[i + 1] += val;
      MsgLen++;
#ifdef debug_cc110x_MU
      Serial.print(F("[DB] "));
      Serial.print(i); Serial.print(F(" | ")); Serial.print(ArPaT[i]); Serial.print(F(" Pa f: ")); Serial.print(val);
      Serial.print(F(" l: ")); Serial.print(last); Serial.print(F(" PatN: ")); Serial.print(PatNmb); Serial.print(F(" msgL: ")); Serial.print(MsgLen); Serial.print(F(" Fc: ")); Serial.println(FiFo.count());
#endif  // END - debug_cc110x_MU
      break;
    } else if (i == (PatMaxCnt - 1)) {  /* ### Anzahl vor definierter Pattern ist erreicht ### */
#ifdef debug_cc110x_MU
      Serial.print(F("[DB] ")); Serial.print(F("PC max! | MsgLen: "));
      Serial.print(MsgLen); Serial.print(F(" | MsgLenMin: ")); Serial.println(MsgLenMin);
#endif  // END - debug_cc110x_MU
      PatMAX = 1;
      msgOutput_MU();
      break;
    }
  }
}
#endif  // END - #ifndef OOK_MU_433


void NO_Chip() {
  String tmp = F("Operation not executable (no CHIP found)\n");
#ifdef CODE_AVR
  MSG_BUILD(tmp);
#elif CODE_ESP
  MSG_OUTPUT(tmp);
#endif  // END - CODE_AVR || CODE_ESP
}

#if defined ARDUINO_ARCH_ESP8266 || defined ARDUINO_ARCH_ESP32
void appendLogFile(String logText) {
  File logFile = LittleFS.open("/files/log.txt", "a");  // open logfile for append to end of file
  if (logFile) {
    if (uptime == 0) {
      logFile.print(millis() / 1000);
    } else {
      logFile.print(uptime);
    }
    logFile.print(F(" - "));
    logFile.println(logText);
    logFile.close();
  }
}
#endif  // END - defined ARDUINO_ARCH_ESP8266 || defined ARDUINO_ARCH_ESP32
