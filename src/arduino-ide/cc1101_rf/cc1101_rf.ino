/*
  Copyright (c) 2022, HomeAutoUser & elektron-bbs
  All rights reserved.
*/

#include <Arduino.h>
#include <EEPROM.h>
#include <digitalWriteFast.h>                 // https://github.com/ArminJo/digitalWriteFast
#include "config.h"
int RSSI_dez;                                 // for the output on web server
#ifdef CC110x
#include "cc110x.h"
#elif RFM69
#include "rfm69.h"
#endif

#include "macros.h"
#include "functions.h"


/* Settings for OOK messages without Sync Pulse (MU) */
#define MsgLenMin               24            // message minimum length
#define MsgLenMax               254           // message maximum length
#define PatMaxCnt               8             // pattern, maximum number (number 8 -> FHEM SIGNALduino compatible)
#define PatTol                  0.20          // pattern tolerance
#define FIFO_LENGTH             90            // 90 from SIGNALduino FW
#include "SimpleFIFO.h"
SimpleFIFO<int16_t, FIFO_LENGTH> FiFo;        // store FIFO_LENGTH # ints

/* varible´s for output */
extern const char compile_date[];
const char PROGMEM compile_date[] = __DATE__ " " __TIME__;
extern const char TXT_VERSION[];

#ifdef SIGNALduino_comp
/*  SIGNALduino helpful information !!!
    1) for sduino compatible need version the true value from regex -> if ($hash->{version} =~ m/cc1101/) | check in 00_SIGNALduino.pm
    2) output xFSK RAW msg must have format MN;D=9004806AA3;R=52;
*/

byte Chip_writeReg_offset = 2;    // for compatibility with SIGNALduino
#define FWtxtPart1        " SIGNALduino compatible "
#else
#define FWtxtPart1        " "
byte Chip_writeReg_offset = 0;
#endif  // END - SIGNALduino_comp || no compatible

#ifndef CHIP_RFNAME
#define CHIP_RFNAME       "rf_Gateway"
#endif

const char TXT_VERSION[] PROGMEM = FWVer FWtxtPart1 CHIP_RFNAME " (" FWVerDate ") ";
byte RegBeforeChange[REGISTER_MAX + 1];

/* varible´s for Toggle */
byte ToggleOrder[4] = { 255, 255, 255, 255 }; // Toggle, Reihenfolge
uint8_t ToggleArray[4] = {255, 255, 255, 255};
byte ToggleValues = 0;                        // Toggle, Registerwerte
byte ToggleCnt = 0;                           // Toggle, Registerzähler für Schleife
boolean ToggleAll = false;                    // Toggle, Markierung alles (Scan-Modus)
unsigned long ToggleTime = 0;                 // Toggle, Zeit in ms (0 - 4294967295)

/* Settings for OOK messages without Sync Pulse (MU) */
#define t_maxP 32000                          // Zeitdauer maximum für gültigen Puls in µs
#define t_minP 75                             // Zeitdauer minimum für gültigen Puls in µs
unsigned long lastTime = 0;                   // Zeit, letzte Aktion
int16_t ArPaT[PatMaxCnt];                     // Pattern Array für Zeiten
signed long ArPaSu[PatMaxCnt];                // Pattern Summe, aller gehörigen Pulse
byte ArPaCnt[PatMaxCnt];                      // Pattern Counter, der Anzahl Pulse
byte PatNmb = 0;                              // Pattern aktuelle Nummer 0 - 9
byte MsgLen;                                  // ToDo, kann ggf ersetzt werden durch message.valcount
int16_t first;                                // Zeiger auf den ersten Puffereintrag
int16_t last;                                 // Zeiger auf den letzten Puffereintrag
String msg;                                   // RAW (nur für serial/telnet, keine HTML Ausgabe) & Serial Input
byte TiOv = 0;                                // Marker - Time Overflow (SIGNALduino Kürzel p; )
byte PatMAX = 0;                              // Marker - maximale Pattern erreicht und neuer unbekannter würde folgen (SIGNALduino Kürzel e; )
byte MOD_FORMAT;                              // Marker - Modulation
byte FSK_RAW;                                 // Marker - FSK Modulation RAW interrupt
bool valid;

/* predefinitions of the functions */
inline void doDetect();
void NO_Chip();
void Interupt_Variant(byte nr);
void MSGBuild();
void PatReset();
void decode(const int pulse);
void findpatt(int val);
void InputCommand(String input);
void ToggleOnOff();

/* varible´s for other */
#define BUFFER_MAX 70                         // maximale Anzahl der zu sendenden Zeichen
int8_t freqErr = 0;                           // CC110x automatic Frequency Synthesizer Control
int8_t freqOffAcc = 0;                        // CC110x automatic Frequency Synthesizer Control
float freqErrAvg = 0;                         // CC110x automatic Frequency Synthesizer Control
uint8_t freqAfc = 0;                          // CC110x AFC an oder aus
float Freq_offset = 0;                        // Frequency offset
String ReceiveModeName;                       // name of active mode from array
uint32_t msgCount = 0;                        // Nachrichtenzähler über alle empfangenen Nachrichten
byte client_now;                              // aktueller Telnet-Client, wo Daten empfangen werden
unsigned long secTick = 0;                    // Zeit, zu der die Uhr zuletzt „tickte“
unsigned long toggleTick = 0;
unsigned long uptime = 0;

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
int wifiEventId;
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
uint32_t msgSendInterval = 0;                 // Weboberfläche | senden - Intervall Zeit
unsigned long msgSendStart = 0;               // Weboberfläche | senden - Startpunkt Zeit

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
#define ICACHE_RAM_ATTR
#define FPSTR String
#endif


#ifdef CC110x
/* --------------------------------------------------------------------------------------------------------------------------------- void Interupt */
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
IRAM_ATTR void Interupt() {     /* Pulseauswertung */
#else
void Interupt() {
#endif
  if (MOD_FORMAT != 3) {  /* not OOK */
    FSK_RAW = 1;
  } else {                /* OOK */
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
  }
}
#endif


/* --------------------------------------------------------------------------------------------------------------------------------- void Interupt_Variant */
void Interupt_Variant(byte nr) {
#ifdef CC110x
  CC110x_CmdStrobe(CC110x_SIDLE); // Exit RX / TX, turn off frequency synthesizer and exit Wake-On-Radio mode if applicable
#endif
  Chip_writeRegFor(Registers[nr].reg_val, Registers[nr].length, Registers[nr].name);
#ifdef CC110x
  CC110x_CmdStrobe(CC110x_SFRX);  // Flush the RX FIFO buffer. Only issue SFRX in IDLE or RXFIFO_OVERFLOW states
#elif RFM69
  Chip_writeReg(0x28, 0x10);      // FIFO are cleared when this bit is set.
#endif

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  WebSocket_modes();
#else
  delay(10);
#endif

#ifdef CC110x
  MOD_FORMAT = ( Chip_readReg(0x12, READ_BURST) & 0b01110000 ) >> 4;
  if (MOD_FORMAT != 3) {
    attachInterrupt(digitalPinToInterrupt(GDO2), Interupt, RISING); /* "Bei steigender Flanke auf dem Interruptpin" --> "Führe die Interupt Routine aus" */
  } else {
    attachInterrupt(digitalPinToInterrupt(GDO2), Interupt, CHANGE); /* "Bei wechselnder Flanke auf dem Interruptpin" --> "Führe die Interupt Routine aus" */
  }
#endif
  Chip_setReceiveMode();  // start receive mode
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  WebSocket_chip();
  WebSocket_detail(1);
#endif
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
#ifdef CC110x
  pinMode(GDO0, OUTPUT);
  digitalWriteFast(GDO0, LOW);
  pinMode(GDO2, INPUT);
#endif
  pinMode(LED, OUTPUT);
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
    logFile.close(); /* Schließen der Datei */
    appendLogFile(logText);
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
  OwnStationHostname.replace("_", "-"); /* Unterstrich ersetzen, nicht zulässig im Hostnamen */
  OwnStationHostname += '-';
  OwnStationHostname += String(chipID, HEX);
  WiFi.setHostname(OwnStationHostname.c_str()); /* WIFI set hostname */
#ifdef ARDUINO_ARCH_ESP32
  WiFi.onEvent(ESP32_WiFiEvent);                /* all WiFi-Events */
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
#endif

#ifdef debug
#if defined(ARDUINO_AVR_NANO) || defined(ARDUINO_RADINOCC1101) || defined(ARDUINO_AVR_PRO)  // ARDUINO_AVR_NANO || ARDUINO_RADINOCC1101 || ARDUINO_AVR_PRO
  Serial.println(F("[DB] -> found board without WLAN"));
#elif defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)                          // ARDUINO_ARCH_ESP8266 || ARDUINO_ARCH_ESP32
  Serial.println(F("[DB] -> found board with WLAN"));
#else // unknown board
  Serial.println(F("[DB] -> found unknown board"));
#endif  // END - BOARDS
#endif  // END debug

  ChipInit();
  toggleTick = ToggleTime;
  if (ToggleTime == 0) {                // wechseln in den zuletzt aktivierten Empfangsmodus
    ReceiveModePKTLEN = Registers[ReceiveModeNr].PKTLEN;
    Interupt_Variant(ReceiveModeNr);    // Empfangsvariante & Register einstellen
  }
}
/* --------------------------------------------------------------------------------------------------------------------------------- void setup end */


void loop() {
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  /* https://arduino-esp8266.readthedocs.io/en/3.1.2/reference.html#timing-and-delays
     delay(ms) pauses the sketch for a given number of milliseconds and allows WiFi and TCP/IP tasks to run. */
  delay(1);

  ArduinoOTA.handle();        // OTA Updates
  Telnet();                   // Telnet Input´s
  HttpServer.handleClient();
  webSocket.loop();           // 53 µS
#endif

  if ((millis() - secTick) >= 1000UL) { // jede Sekunde
    secTick += 1000UL;
    uptime++;
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
    WebSocket_index();                  // Dauer: ohne connect ca. 100 µS, 1 Client ca. 1700 µS, 2 Clients ca. 2300 µS
#endif
    if (ToggleTime > 0) {               // Toggle Option
      if (millis() - toggleTick > ToggleTime) { // Abfragen, ob Zeit zum einschalten erreicht
        toggleTick = millis();                  // Zeit merken, an der eingeschaltet wurde
        ToggleOnOff();
      }
    }

    /* ### to DEBUG ###
       print RAM-Info, see https://arduino-esp8266.readthedocs.io/en/latest/libraries.html#esp-specific-apis
    */
    //    if (uptime % 5 == 0) {
    //      Serial.print(F("ESP8266 FreeHeap:          "));
    //      Serial.println(ESP.getFreeHeap());         // free heap size.
    //      Serial.print(F("ESP8266 MaxFreeBlockSize:  "));
    //      Serial.println(ESP.getMaxFreeBlockSize()); // largest contiguous free RAM block in the heap, useful for checking heap fragmentation. NOTE: Maximum malloc() -able block will be smaller due to memory manager overheads.
    //      Serial.print(F("ESP8266 HeapFragmentation: "));
    //      Serial.println(ESP.getHeapFragmentation()); // fragmentation metric (0% is clean, more than ~50% is not harmless)
    //    }
  }

  if (Serial.available() > 0) { /* Serial Input´s */
    while (Serial.available()) {
#ifdef ARDUINO_ARCH_ESP32
      msg += String(char(Serial.read()));  /* only ESP32 | readString() no function | .read() empties .available() character by character */
#else
      msg = Serial.readString();           /* other all | String, strip off any leading/trailing space and \r \n */
#endif
    }
    Serial.flush();
    msg.trim();                 /* String, strip off any leading/trailing space and \r \n */

    if (msg.length() > 0 && msg.length() <= BUFFER_MAX) {
#ifdef debug
#ifdef CODE_AVR
      Serial.print(F("[DB] Serial.available > 0 ")); Serial.println(msg);
#elif CODE_ESP
      MSG_OUTPUT(F("[DB] Serial.available > 0 ")); MSG_OUTPUTLN(msg);
#endif  // END - CODE_AVR || CODE_ESP
#endif  // END - debug
      client_now = 255;         /* current client is set where data is received */
      InputCommand(msg);
      msg = "";     /* reset variable as it continues to be used elsewhere */
    }
  }

  /* only for test !!! HangOver ??? */
  //#ifdef debug_cc110x_ms
  //MSG_OUTPUTALL(F("DB CC110x_MARCSTATE ")); MSG_OUTPUTALLLN(Chip_readReg(CC110x_MARCSTATE, READ_BURST), HEX); /* MARCSTATE – Main Radio Control State Machine State */
  //#endif

#ifdef RFM69
  FSK_RAW = (Chip_readReg(0x28, 0) & 0b00100000) >> 5; // RegIrqFlags2, FifoLevel - Set when the number of bytes in the FIFO strictly exceeds FifoThreshold, else cleared.
#endif

  /* not OOK */
  if ( (FSK_RAW == 1) && (ChipFound == true) ) { /* Received data | RX (not OOK !!!) */
    FSK_RAW = 0;
    digitalWriteFast(LED, HIGH);    /* LED on */
    int rssi = Chip_readRSSI();
    msgCount++;
#ifdef CC110x
    freqErr = Chip_readReg(CC110x_FREQEST, READ_BURST);   // 0x32 (0xF2): FREQEST – Frequency Offset Estimate from Demodulator
    if (freqAfc == 1) {
      freqErrAvg = freqErrAvg - float(freqErrAvg / 8.0) + float(freqErr / 8.0);  // Mittelwert über Abweichung
      freqOffAcc += round(freqErrAvg);
      Chip_writeReg(CC110x_FSCTRL0, freqOffAcc);          // 0x0C: FSCTRL0 – Frequency Synthesizer Control
    }
#endif
    msg = "";
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
    String html_raw = "";   // für die Ausgabe auf dem Webserver
    html_raw.reserve(128);
#endif
#ifdef debug_cc110x_ms      /* MARCSTATE – Main Radio Control State Machine State */
#ifdef CODE_AVR
    Serial.print(F("[DB] CC110x_MARCSTATE ")); Serial.println(Chip_readReg(CC110x_MARCSTATE, READ_BURST), HEX);
#elif CODE_ESP
    MSG_OUTPUTALL(F("[DB] CC110x_MARCSTATE ")); MSG_OUTPUTALLLN(Chip_readReg(CC110x_MARCSTATE, READ_BURST), HEX);
#endif  // END - CODE_AVR || CODE_ESP
#endif  // END - debug_cc110x_ms
    uint8_t uiBuffer[ReceiveModePKTLEN];                             // Array anlegen
    Chip_readBurstReg(uiBuffer, CHIP_RXFIFO, ReceiveModePKTLEN); // Daten aus dem FIFO lesen
#ifdef SIGNALduino_comp
#ifdef CODE_AVR
    Serial.print(char(2));    // STX
    Serial.print(F("MN;D=")); // "MN;D=" | "data: "
#elif CODE_ESP
    msg += char(2);           // STX
    msg += F("MN;D=");        // "MN;D=" | "data: "
#endif  // END - CODE_AVR || CODE_ESP
#else
#ifdef CODE_AVR
    Serial.print(F("data: "));  // "MN;D=" | "data: "
#elif CODE_ESP
    msg += F("data: ");         // "MN;D=" | "data: "
#endif  // END - CODE_AVR || CODE_ESP
#endif
    for (byte i = 0; i < ReceiveModePKTLEN; i++) { /* RawData */
#ifdef CODE_AVR
      Serial.print(onlyDecToHex2Digit(uiBuffer[i]));
#elif CODE_ESP
      msg += onlyDecToHex2Digit(uiBuffer[i]);
#endif  // END - CODE_AVR || CODE_ESP
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
      html_raw += onlyDecToHex2Digit(uiBuffer[i]);
#endif
    }
#ifdef SIGNALduino_comp
#ifdef CODE_AVR
    Serial.print(F(";R="));     // ";R=" | "; RSSI="
#elif CODE_ESP
    msg += F(";R=");            // ";R=" | "; RSSI="
#endif  // END - CODE_AVR || CODE_ESP
#else
#ifdef CODE_AVR
    Serial.print(F(" RSSI="));  // ";R=" | "; RSSI="
#elif CODE_ESP
    msg += F(" RSSI=");         // ";R=" | "; RSSI="
#endif  // END - CODE_AVR || CODE_ESP
#endif

#ifdef CODE_AVR
    Serial.print(rssi);
#elif CODE_ESP
    msg += rssi;
#endif  // END - CODE_AVR || CODE_ESP

#ifdef SIGNALduino_comp
#ifdef CODE_AVR
    Serial.print(F(";A="));         // ";A=" | "; FREQAFC="
#elif CODE_ESP
    msg += F(";A=");                // ";A=" | "; FREQAFC="
#endif  // END - CODE_AVR || CODE_ESP
#else
#ifdef CODE_AVR
    Serial.print(F("; FREQAFC="));  // ";A=" | "; FREQAFC="
#elif CODE_ESP
    msg += F("; FREQAFC=");         // ";A=" | "; FREQAFC="
#endif  // END - CODE_AVR || CODE_ESP
#endif

#ifdef CODE_AVR
    Serial.print(freqErr); Serial.print(';');
#elif CODE_ESP
    msg += freqErr; msg += ';';
#endif  // END - CODE_AVR || CODE_ESP

#ifdef SIGNALduino_comp
#ifdef CODE_AVR
    Serial.print(char(3));    // ETX
#elif CODE_ESP
    msg += char(3);           // ETX
#endif  // END - CODE_AVR || CODE_ESP
#else
#ifdef CODE_AVR
    Serial.print(char(13));   // CR
#elif CODE_ESP
    msg += char(13);          // CR
#endif  // END - CODE_AVR || CODE_ESP
#endif

#ifdef CODE_AVR
    Serial.print(char(10));   // LF
#elif CODE_ESP
    msg += char(10);          // LF
    MSG_OUTPUTALL(msg);   /* output msg to all */
#endif  // END - CODE_AVR || CODE_ESP

    //Serial.println(Chip_readReg(CC110x_MARCSTATE, READ_BURST), HEX);
#ifdef debug_cc110x_ms    /* MARCSTATE – Main Radio Control State Machine State */
#ifdef CODE_AVR
    Serial.print(F("[DB] CC110x_MARCSTATE ")); Serial.println(Chip_readReg(CC110x_MARCSTATE, READ_BURST), HEX);
#elif CODE_ESP
    MSG_OUTPUTALL(F("[DB] CC110x_MARCSTATE ")); MSG_OUTPUTALLLN(Chip_readReg(CC110x_MARCSTATE, READ_BURST), HEX);
#endif  // END - CODE_AVR || CODE_ESP
#endif  // END - debug_cc110x_ms
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
    WebSocket_raw(html_raw);    // Dauer: kein client ca. 100 µS, 1 client ca. 900 µS, 2 clients ca. 1250 µS
#endif
    Chip_setReceiveMode(); // start receive mode
    digitalWriteFast(LED, LOW); /* LED off */
  } else {
    /* OOK */
    while (FiFo.count() > 0 ) {       // Puffer auslesen und an Dekoder uebergeben
      int aktVal = FiFo.dequeue();    // get next element
      decode(aktVal);
    }
  }

  /* only for send data from webfronted -> ESP devices */
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  if (msgRepeats > 0) {
    if (millis() >= msgSendStart) {
      uint8_t PKTLENis = Chip_readReg(CHIP_PKTLEN, READ_BURST); // old packet length
      uint8_t len = strlen(senddata_esp) / 2;
      Chip_writeReg(CHIP_PKTLEN, len);
#ifdef RFM69
      Chip_writeReg(0x3C, len - 1); // FifoThreshold setting
#endif
      Chip_sendFIFO(senddata_esp);
#ifdef CC110x
      for (uint8_t i = 0; i < 255; i++) {
        if (Chip_readReg(CC110x_MARCSTATE, READ_BURST) == 0x01) {  /* 1 (0x01) IDLE IDLE */
          break;
        }
        delay(1);
      }
#endif
      msgSendStart = millis() + msgSendInterval;
      msgRepeats--;
      Chip_writeReg(CHIP_PKTLEN, PKTLENis); // restore old packet length
#ifdef RFM69
      Chip_writeReg(0x3C, PKTLENis - 1); // restore FifoThreshold setting
#endif
      Chip_setReceiveMode(); // start receive mode
      if (msgRepeats == 0) {
        for (uint8_t num = 0; num < WEBSOCKETS_SERVER_CLIENT_MAX; num++) {
          if (webSocketSite[num] == "/raw") {
            webSocket.sendTXT(num, "TX_ready");
          }
        }
      }
    }
  }
#endif
}
/* --------------------------------------------------------------------------------------------------------------------------------- void loop end */


/* #### ab hier, Funktionen mit Makroabhängigkeit #### */
void ToggleOnOff() {
#ifdef CC110x
  detachInterrupt(digitalPinToInterrupt(GDO2));
#endif
#ifdef debug
#ifdef CODE_AVR
  Serial.print(F("[DB] ToggleCnt=")); Serial.print((ToggleCnt + 1)); Serial.print(F(" ToggleValues=")); Serial.println(ToggleValues);
#elif CODE_ESP
  String tmp = F("[DB] ToggleCnt="); tmp += (ToggleCnt + 1); tmp += F(" ToggleValues="); tmp += ToggleValues;
  MSG_OUTPUTLN(tmp);
#endif  // END - CODE_AVR
#endif  // END - debug

  if (ToggleAll == true) {
    ReceiveModeNr = ToggleCnt + 1;
    ToggleValues = RegistersMaxCnt - 1;
#ifdef debug
#ifdef CODE_AVR
    Serial.print(F("[DB] ToggleAll ReceiveModeNr ")); Serial.print(ReceiveModeNr); Serial.print(F(", ToggleValues ")); Serial.println(ToggleValues);
#elif CODE_ESP
    tmp = F("[DB] ToggleAll ReceiveModeNr "); tmp += ReceiveModeNr; tmp += F(", ToggleValues "); tmp += ToggleValues;
    MSG_OUTPUTLN(tmp);
#endif  // END - CODE_AVR || CODE_ESP
#endif  // END - debug
  } else {
    if (ToggleValues <= 1) {
      ToggleTime = 0;
#ifdef debug
#ifdef CODE_AVR
      Serial.println(
#elif CODE_ESP
      MSG_OUTPUTLN(
#endif  // END - CODE_AVR || CODE_ESP
        F("[DB] Toggle STOPPED, no toggle values in togglebank!"));
#endif  // END - debug
      return;
    }
    ReceiveModeNr = ToggleOrder[ToggleCnt];
  }

#ifdef debug
#ifdef CODE_AVR
  Serial.print(F("[DB] Toggle (output all)    | switched to ")); Serial.println(Registers[ReceiveModeNr].name);
#elif CODE_ESP
  tmp = F("[DB] Toggle (output all)    | switched to "); tmp += Registers[ReceiveModeNr].name;
  MSG_OUTPUTLN(tmp);
#endif  // END - CODE_AVR || CODE_ESP
#endif  // END - debug

  ReceiveModePKTLEN = Registers[ReceiveModeNr].PKTLEN;
  Interupt_Variant(ReceiveModeNr);    // set receive variant & register

  ToggleCnt++;
  if (ToggleCnt >= ToggleValues) {
    ToggleCnt = 0;
  }
}
/* ----------------------------------------------------------------- void ToggleOnOff end */


void InputCommand(String input) { /* all InputCommand´s , String | Char | marker, 255 = Serial | 0...254 = Telnet */
#ifdef CODE_ESP
  String tmp = "";        // for temp outputs print
#endif
  uint8_t uiBuffer[47];   // Array anlegen
  char buf_input[input.length() + 1];
  input.toCharArray(buf_input, input.length() + 1); /* String to char in buf */
#ifdef debug
  for (byte i = 0; i < input.length(); i++) {
#ifdef CODE_AVR
    Serial.print(F("DB InputCommand [")); Serial.print(i); Serial.print(F("] = ")); Serial.println(buf_input[i]);
#elif CODE_ESP
    tmp = F("[DB] InputCommand ["); tmp += i; tmp += F("] = "); tmp += buf_input[i];
    MSG_OUTPUTLN(tmp);
#endif  // END - CODE_AVR || CODE_ESP
  }
#endif  // END - debug

  switch (buf_input[0]) { /* integrated ? m t tob tos x C C<n><n> C99 CG C3E C0DnF I M P R V W<n><n><n><n> WS<n><n> */
    case '?':             /* command ? */
      if (!buf_input[1]) {
#ifdef CODE_AVR
        Serial.println(
#elif CODE_ESP
        MSG_OUTPUTLN(
#endif  // END - CODE_AVR || CODE_ESP
          F("e, foff, ft, m, t, tob, tos, x, C, C3E, CG, CEA, CDA, E, I, M, P, R, SN, V, W, WS"));
      }
      break;                                                                                          /* -#-#-#-#- - - next case - - - #-#-#-#- */
    case 'f':                                                                                         /* command f */
      if (ChipFound == false) {
        NO_Chip();
      } else {
        if (buf_input[1] == 'o' && buf_input[2] == 'f' && buf_input[3] == 'f') { /* command foff<n> */
          if (isNumeric(input.substring(4)) == 1) {
            Freq_offset = input.substring(4).toFloat();
            if (Freq_offset > 10.0 || Freq_offset < -10.0) {
              Freq_offset = 0;
#ifdef CODE_AVR
              Serial.println(
#elif CODE_ESP
              MSG_OUTPUTLN(
#endif  // END - CODE_AVR || CODE_ESP
                F("CC110x_Freq.Offset resets (The value is not in the range of -10 to 10 MHz)"));
            }
            EEPROM.put(EEPROM_ADDR_FOFFSET, Freq_offset);
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
            EEPROM.commit();
#endif  // END - defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
#ifdef CC110x
            Chip_writeReg(CC110x_FSCTRL0, 0);  // 0x0C: FSCTRL0 – Frequency Synthesizer Control
#endif  // END - CC110x
#ifdef CODE_AVR
            Serial.print(F("CC110x_Freq.Offset saved to ")); Serial.print(Freq_offset); Serial.println(F(" MHz"));
#elif CODE_ESP
            tmp = F("CC110x_Freq.Offset saved to "); tmp += Freq_offset; tmp += F(" MHz");
#endif  // END - CODE_AVR || CODE_ESP
          } else {
#ifdef CODE_AVR
            Serial.println(F("CC110x_Freq.Offset value is not nummeric"));
#elif CODE_ESP
            tmp = F("CC110x_Freq.Offset value is not nummeric");
#endif  // END - CODE_AVR || CODE_ESP
          }
#ifdef CODE_ESP
          MSG_OUTPUTLN(tmp);
#endif  // END - CODE_ESP
        }

        if (buf_input[1] == 't' && !buf_input[2]) { /* command ft */
#ifdef CODE_AVR
          Serial.println(
#elif CODE_ESP
          MSG_OUTPUTLN(
#endif  // END - CODE_AVR || CODE_ESP
            F("CC110x_Frequency, send testsignal"));
          input = F("SN;D=AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA;R=50;");
          InputCommand(input);
        }
      }
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */
    case 'e': /* command e */
      if (ChipFound == false) {
        NO_Chip();
        break;
      } else {
        for (byte i = 0; i < RegistersMaxCnt; i++) {
#ifdef CC110x
          if (strcmp(Registers[i].name, "OOK_MU_433") == 0) {
            // if (Registers[i].name == "OOK_MU_433") { // warning: comparison with string literal results in unspecified behavior [-Waddress]
            Interupt_Variant(i);
            break;
          }
#endif  // END - CC110x
        }
      }
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */
    case 'm': /* command m */
      if (buf_input[1]) {
        /* command m<n>, check of decimal */
        if (isNumeric(input.substring(1)) == 1) {
          if (ChipFound == false) {
            NO_Chip();
          } else {
#ifdef CC110x
            detachInterrupt(digitalPinToInterrupt(GDO2));
#endif  // END - CC110x
            ToggleAll = false;
            ToggleTime = 0;                                       // beendet Toggle, sonst Absturz nach tob88 und anschließend m2 wenn ToggleCnt > 3
            byte int_substr1_serial = input.substring(1).toInt(); /* everything after m to byte (old String) */
            if (int_substr1_serial <= RegistersMaxCnt) {
              /* "Normal Option" - set all Register value´s */
              if (int_substr1_serial <= (RegistersMaxCnt - 1)) {
                if (int_substr1_serial == 0) {
                  Freq_offset = 0;  // reset FrequencOffset
                }
#ifdef debug
#ifdef CODE_AVR
                Serial.print(F("set register to ")); Serial.println(Registers[int_substr1_serial].name);
#elif CODE_ESP
                tmp = F("set register to "); tmp += Registers[int_substr1_serial].name;
                MSG_OUTPUTLN(tmp);
#endif  // END - CODE_AVR || CODE_ESP
#endif  // END - debug
                ReceiveModeName = Registers[int_substr1_serial].name;
                ReceiveModeNr = int_substr1_serial;
                ReceiveModePKTLEN = Registers[int_substr1_serial].PKTLEN;
                Interupt_Variant(int_substr1_serial);

#ifdef debug_cc110x_ms    /* MARCSTATE – Main Radio Control State Machine State */
#ifdef CODE_AVR
                Serial.print(F("[DB] CC110x_MARCSTATE ")); Serial.println(Chip_readReg(CC110x_MARCSTATE, READ_BURST), HEX);
#elif CODE_ESP
                MSG_OUTPUTALL(F("[DB] CC110x_MARCSTATE ")); MSG_OUTPUTALLLN(Chip_readReg(CC110x_MARCSTATE, READ_BURST), HEX);
#endif  // END - CODE_AVR || CODE_ESP
#endif  // END - debug_cc110x_ms

#ifdef debug
#ifdef CODE_AVR
                Serial.println(
#elif CODE_ESP
                MSG_OUTPUTLN(
#endif  // END - CODE_AVR || CODE_ESP
                  F("[DB] Input, writes all current values to EEPROM"));
#endif  // END - debug
                for (byte i = 0; i < Registers[int_substr1_serial].length; i++) { /* write register values ​​to flash */
                  EEPROMwrite(i, pgm_read_byte_near(Registers[int_substr1_serial].reg_val + i));
                }
                EEPROMwrite(EEPROM_ADDR_Prot, int_substr1_serial); /* write enabled protocoll */
              } else if (int_substr1_serial == RegistersMaxCnt) { /* "Special Option" */
#ifdef CODE_AVR
                Serial.println(
#elif CODE_ESP
                MSG_OUTPUTLN(
#endif  // END - CODE_AVR || CODE_ESP
                  F("Developer gadget to test functions"));
              }
            }
          }
        }
      }
      break; /* -#-#-#-#- - - next case - - - #-#-#-#- */
    case 't':
      if (!buf_input[1]) { /* command t */
#ifdef CODE_AVR
        Serial.println(
#elif CODE_ESP
        MSG_OUTPUTLN(
#endif  // END - CODE_AVR || CODE_ESP
          uptime);
      } else {
        if (buf_input[1] && buf_input[1] == 'o' && buf_input[2]) { /* command tob<n> & tos<n> */
          if (ChipFound == false) {
            NO_Chip();
          } else {
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
#endif  // END - CODE_AVR || CODE_ESP
                ToggleTime = 0;
                ToggleAll = false;
              } else if (isNumeric(input.substring(3)) == 1 && (IntTime >= ToggleTimeMin)) { /* command tos<n> >= ToggleTime OK */
#ifdef CODE_AVR
                Serial.print(F("Toggle starts changing every ")); Serial.print(IntTime); Serial.println(F(" milliseconds"));
#elif CODE_ESP
                tmp = F("Toggle starts changing every "); tmp += IntTime; tmp += F(" milliseconds");
                MSG_OUTPUTLN(tmp);
#endif  // END - CODE_AVR || CODE_ESP
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
#endif  // END - CODE_AVR || CODE_ESP
                ToggleTime = 0;
              }
            } else if (buf_input[2] == 'b') { /* command tob */
              ToggleAll = false;
              if (input.substring(3, 4).toInt() <= 3) { /* command tob<0-3> */
#ifdef debug
#ifdef CODE_AVR
                Serial.print(F("[DB] Input | cmd tob ")); Serial.print(input.substring(3)); Serial.println(F(" accepted"));
#elif CODE_ESP
                tmp = F("[DB] Input | cmd tob "); tmp += input.substring(3); tmp += F(" accepted");
                MSG_OUTPUTLN(tmp);
#endif  // END - CODE_AVR || CODE_ESP
#endif  // END - debug
                if (isNumeric(input.substring(4)) == 1) {
                  if (input.substring(4).toInt() == 99) {                                 /* command tob<0-3>99 -> reset togglebank <n> */
                    ToggleArray[input.substring(3, 4).toInt()] = 255;                     /* 255 is max value and reset value */
                    EEPROMwrite(input.substring(3, 4).toInt() + EEPROM_ADDR_ProtTo, 255); /* 255 is max value and reset value */
#ifdef CODE_AVR
                    Serial.print(F("ToggleBank ")); Serial.print(input.substring(3, 4)); Serial.println(F(" reset"));
#elif CODE_ESP
                    tmp = F("ToggleBank "); tmp += input.substring(3, 4); tmp += F(" reset");
#endif  // END - CODE_AVR || CODE_ESP
                  } else if (input.substring(4).toInt() < RegistersMaxCnt) { /* command tob<0-3><n> -> set togglebank <n> */
                    ToggleArray[input.substring(3, 4).toInt()] = input.substring(4).toInt();
                    EEPROMwrite(input.substring(3, 4).toInt() + EEPROM_ADDR_ProtTo, input.substring(4).toInt());
#ifdef CODE_AVR
                    Serial.print(F("ToggleBank ")); Serial.print(input.substring(3, 4)); Serial.print(F(" set to "));
                    Serial.print(Registers[input.substring(4).toInt()].name); Serial.println(F(" mode"));
#elif CODE_ESP
                    tmp = F("ToggleBank "); tmp += input.substring(3, 4); tmp += F(" set to ");
                    tmp += Registers[input.substring(4).toInt()].name; tmp += F(" mode");
#endif  // END - CODE_AVR || CODE_ESP
                  } else if (input.substring(4).toInt() > RegistersMaxCnt) {
#ifdef CODE_AVR
                    Serial.println(F("Mode number greater RegistersMaxCnt"));
                  }
#elif CODE_ESP
                    tmp = F("Mode number greater RegistersMaxCnt");
                  }
                  MSG_OUTPUTLN(tmp);
#endif  // END - CODE_AVR || CODE_ESP
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
                Serial.println(F("[DB] Input, togglebank reset { - | - | - | - }"));
#elif CODE_ESP
                tmp = F("[DB] Input, togglebank reset { - | - | - | - }\n");
#endif  // END - CODE_AVR || CODE_ESP
#endif  // END - debug
                for (byte i = 0; i < 4; i++) {
                  ToggleArray[i] = 255;
                  EEPROMwrite(i + EEPROM_ADDR_ProtTo, 255);  // DEZ 45  HEX 2D  ASCII -
                }
                ToggleValues = 0;
                ToggleTime = 0;
                EEPROMwrite_long(EEPROM_ADDR_Toggle, 0);
#ifdef debug
#ifdef CODE_AVR
                Serial.println(F("ToggleBank 0-3 reset and STOP Toggle"));
#elif CODE_ESP
                tmp += F("ToggleBank 0-3 reset and STOP Toggle");
                MSG_OUTPUTLN(tmp);
#endif  // END - CODE_AVR || CODE_ESP
#endif  // END - debug
              } else if (buf_input[3] == '8' && buf_input[4] == '8' && !buf_input[5]) { /* command tob88 -> scan modes */
#ifdef CODE_AVR
                Serial.println(
#elif CODE_ESP
                MSG_OUTPUTLN(
#endif  // END - CODE_AVR || CODE_ESP
                  F("scan mode active (mode changes every 15 seconds, STOP with 'tos0')"));
                ToggleAll = true;
                ToggleTime = 15000;  // set to default and start
                ToggleOnOff();
              }
            }
          }
        }
      }
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */
#ifdef CC110x // TODO SX1231
    case 'x': /* command x - write patable*/
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
          if (ChipFound == false) {
            NO_Chip();
          } else {
            CC110x_writeBurstReg(uiBuffer, CC110x_PATABLE, 8);
#ifdef CODE_AVR
            Serial.print(F("write ")); Serial.print(buf_input[1]); Serial.print(buf_input[2]); Serial.println(F(" to PATABLE done"));
#elif CODE_ESP
            tmp = F("write "); tmp += buf_input[1]; tmp += buf_input[2]; tmp += F(" to PATABLE done");
            MSG_OUTPUTLN(tmp);
#endif  // END - CODE_AVR || CODE_ESP
          }
        }
      }
      break; /* -#-#-#-#- - - next case - - - #-#-#-#- */
#endif  // END - CC110x
    case 'C':
      if (ChipFound == false) {
        NO_Chip();
      } else {
        if (!buf_input[1]) { /* command C - Read all values from Register */
#ifdef CODE_AVR
          Serial.println(F("Current register (address - CC110x value - EEPROM value)"));

          for (byte i = 0; i <= 46; i++) {
            MSG_OUTPUT_DecToHEX_lz(i);
            Serial.print('=');
            MSG_OUTPUT_DecToHEX_lz(Chip_readReg(i, READ_BURST));
            if (i < 41) {
              Serial.print(F(" ("));
              MSG_OUTPUT_DecToHEX_lz(EEPROMread(i));
              Serial.println(F(" EEPROM)"));
            } else {
              Serial.println(F(" (only test)"));
            }
          }
#elif CODE_ESP

          tmp = F("Current register (address - ");
#ifdef CC110x
          tmp += F("CC110x");
#elif RFM69
          tmp += F("RFM69");
#endif  // END - CC110x || RFM69
          tmp += F(" value - EEPROM value)\n");

          for (byte i = 0; i <= REGISTER_MAX; i++) {
            if (i > 79) {
#ifdef RFM69
              tmp += onlyDecToHex2Digit(SX1231_RegAddrTranslate[i - 80]);
#endif  // END - RFM69
            } else {
              tmp += onlyDecToHex2Digit(i);
            }
            tmp += '='; tmp += onlyDecToHex2Digit(Chip_readReg(i, READ_BURST));
#ifdef CC110x
            if (i < 41) {
              tmp += F(" ("); tmp += onlyDecToHex2Digit(EEPROMread(i)); tmp += F(" EEPROM)\n");
            } else {
              if (i != REGISTER_MAX) {
                tmp += F(" (only test)\n");
              }
            }
#elif RFM69
            tmp += F(" ("); tmp += onlyDecToHex2Digit(EEPROMread(i)); tmp += onlyDecToHex2Digit(EEPROMread(i)); tmp += F(" EEPROM)\n");
#endif  // END - CC110x || RFM69
          }
#ifdef CC110x
          tmp += F(" (only test)");
#endif
          MSG_OUTPUTLN(tmp);
#endif  // END - CODE_AVR || CODE_ESP
        } else if (buf_input[1] && buf_input[2] && !buf_input[3]) { /* command C<n><n> - Read HEX adress values from Register, set raw CDA/CEA - disable/enable AFC */
          uint16_t Cret = hexToDec(input.substring(1));
          if (Cret <= 0x3D) {
#ifdef CODE_AVR
#ifdef SIGNALduino_comp
            Serial.print('C');
#else
            Serial.print(F("0x"));
#endif  // END - SIGNALduino_comp
            Serial.print(input.substring(1));

#ifdef SIGNALduino_comp
            Serial.print(F(" = "));
#else
            Serial.print(F(" - 0x"));
#endif  // END - SIGNALduino_comp
            byte CC110x_ret = Chip_readReg(Cret, READ_BURST);
            MSG_OUTPUT_DecToHEX_lz(CC110x_ret);
            Serial.println("");
#elif CODE_ESP
#ifdef SIGNALduino_comp
            tmp = 'C';
#else
            tmp = F("0x");
#endif  // END - SIGNALduino_comp
            tmp += input.substring(1);
#ifdef SIGNALduino_comp
            tmp += F(" = ");
#else
            tmp += F(" - 0x");
#endif  // END - SIGNALduino_comp
            byte CC110x_ret = Chip_readReg(Cret, READ_BURST);
            tmp += onlyDecToHex2Digit(CC110x_ret);
            MSG_OUTPUTLN(tmp);
#endif  // END - CODE_AVR || CODE_ESP
          } else if (Cret == 0x99) { /* command C99 - ccreg */
            Chip_readBurstReg(uiBuffer, 0x00, 47);
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
#endif  // END - CODE_AVR || CODE_ESP
          } else if (Cret == 0x3E) { /* command C3E - patable  */
            Chip_readBurstReg(uiBuffer, 0x3E, 8);
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
#endif  // END - CODE_AVR || CODE_ESP
          } else if (Cret == 0xDA || Cret == 0xEA) {  // command CDA/CEA - disable/enable AFC
            freqAfc = buf_input[1] - 'D';             // CC110x AFC 0 oder 1
            freqOffAcc = 0;                           // reset cc110x afc offset
            freqErrAvg = 0;                           // reset cc110x afc average
#ifdef CC110x
            Chip_writeReg(CC110x_FSCTRL0, 0);         // reset Register 0x0C: FSCTRL0 – Frequency Synthesizer Control
#elif RFM69
            SX1231_afc(freqAfc);                      // AfcAutoOn, 0  AFC is performed each time AfcStart is set, 1  AFC is performed each time Rx mode is entered
#endif  // END - CC110x || RFM69
            EEPROMwrite(EEPROM_ADDR_AFC, freqAfc);
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
            EEPROM.commit();
#endif  // END - defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
          }
        } else if (buf_input[1] == 'G' && !buf_input[2]) { /* command CG - get config*/
          uint8_t mu = 1;
          uint8_t mn = 0;
          if (MOD_FORMAT != 3) {  // not OOK
            mu = 0;
            mn = 1;
          }
#ifdef CODE_AVR
          Serial.print(F("MS=0;MU="));
          Serial.print(mu);
          Serial.print(F(";MC=0;MN="));
          Serial.print(mn);
          Serial.print(F(";AFC="));
          Serial.print(freqAfc);         // CC110x AFC an oder aus
          Serial.println(F(";Mred=0"));
#elif CODE_ESP
          tmp = F("MS=0;MU=");
          tmp += mu;
          tmp += F(";MC=0;MN=");
          tmp += mn;
          tmp += F(";AFC=");
          tmp += freqAfc;         // CC110x AFC an oder aus
          tmp += F(";Mred=0");
          MSG_OUTPUTLN(tmp);
#endif  // END - CODE_AVR || CODE_ESP
        } else if (input == "C0DnF") { /* command C0DnF - get ccconf */
#ifdef CODE_AVR
          Serial.print(F("C0Dn11="));
          for (byte i = 0; i <= 17; i++) {
            byte x = Chip_readReg(0x0D + i, READ_BURST);
            MSG_OUTPUT_DecToHEX_lz(x);
          }
          Serial.println("");
#elif CODE_ESP
          tmp = F("C0Dn11=");
          for (byte i = 0; i <= 17; i++) {
            tmp += onlyDecToHex2Digit(Chip_readReg(0x0D + i, READ_BURST));
          }
          MSG_OUTPUTLN(tmp);
#endif  // END - CODE_AVR || CODE_ESP
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
#endif  // END - CODE_AVR || CODE_ESP
        if (ReceiveModeName == "" && ChipFound == true) {         // ReceiveModeName if uC restart
          ReceiveModeName = F("CC110x configuration");
        } else if (ReceiveModeName != "" && ChipFound == true) {  // ReceiveModeName is set with command m<n>

        } else {                                                  // ReceiveModeName if no cc110x
          ReceiveModeName = F("CC110x NOT recognized");
        }

        if (ChipFound == true) {
#ifdef CODE_AVR
          Serial.print(F("CC110x_MARCSTATE    ")); Serial.println(Chip_readReg(CC110x_MARCSTATE, READ_BURST), HEX);  // MARCSTATE – Main Radio Control State Machine State
          Serial.print(F("CC110x_Freq.Afc     ")); Serial.println(freqAfc == 1 ? F("on (1)") : F("off (0)"));
          Serial.print(F("CC110x_Freq.Offset  ")); Serial.print(Freq_offset, 3); Serial.println(F(" MHz"));
        }
        Serial.print(F("ReceiveMode         ")); Serial.println(ReceiveModeName);
        Serial.print(F("ReceiveMessageCount ")); Serial.println(msgCount);

        if (ChipFound == true) {
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
        }
#elif CODE_ESP
#ifdef CC110x
          tmp += F("CC110x_MARCSTATE    "); tmp += onlyDecToHex2Digit(Chip_readReg(CC110x_MARCSTATE, READ_BURST));  tmp += "\n"; // MARCSTATE – Main Radio Control State Machine State
          tmp += F("CC110x_Freq.Afc     ");
#elif RFM69
          tmp += F("RFM69_Freq.Afc     ");
#endif  // END - CC110x || RFM69
          tmp += (freqAfc == 1 ? F("on (1)") : F("off (0)")); tmp += "\n";
#ifdef CC110x
          tmp += F("CC110x_Freq.Offset  ");
#elif RFM69
          tmp += F("RFM69_Freq.Offset  ");
#endif  // END - CC110x || RFM69
          tmp += String(Freq_offset, 3); tmp += F(" MHz"); tmp += "\n";
        }
        tmp += F("ReceiveMode         "); tmp += ReceiveModeName; tmp += "\n";
        tmp += F("ReceiveMessageCount "); tmp += msgCount; tmp += "\n";

        if (ChipFound == true) {
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
        }
        MSG_OUTPUTLN(tmp);
#endif  // END - CODE_AVR || CODE_ESP
      }
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */
    case 'M': /* command M */
      if (!buf_input[1]) {
#ifdef CODE_AVR
        Serial.println(F("available register modes:"));
        for (byte i = 0; i < RegistersMaxCnt; i++) {
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
        for (byte i = 0; i < RegistersMaxCnt; i++) {
          if (i < 10) {
            tmp += ' ';
          }
          tmp += i; tmp += F(" - "); tmp += Registers[i].name; tmp += '\n';
        }
        MSG_OUTPUTLN(tmp);
#endif  // END - CODE_AVR || CODE_ESP
      }
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */
    case 'P': /* command P */
      if (!buf_input[1]) {
#ifdef CODE_AVR
        Serial.println(
#elif CODE_ESP
        MSG_OUTPUTLN(
#endif  // END - CODE_AVR || CODE_ESP
          F("OK"));
      }
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */
    case 'R': /* command R */
      if (!buf_input[1]) {
#ifdef CODE_AVR
        Serial.println(
#elif CODE_ESP
        MSG_OUTPUTLN(
#endif  // END - CODE_AVR || CODE_ESP
          freeRam());
      }
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */
    case 'S': /* command S */
      if (ChipFound == false) {
        NO_Chip();
      } else {
        if (buf_input[1] && buf_input[1] == 'N' && buf_input[2] == ';') { /* command SN | SN;R=5;D=9A46036AC8D3923EAEB470AB; */
#ifdef debug
#ifdef CODE_AVR
          Serial.println(
#elif CODE_ESP
          MSG_OUTPUTLN(
#endif  // END - CODE_AVR || CODE_ESP
            F("[DB] Input | SN; raw message"));
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
#ifdef CODE_AVR
                Serial.print(F("[DB] Input | SN; found ")); Serial.println(ptr);
#elif CODE_ESP
                tmp = F("[DB] Input | SN; found "); tmp += ptr; tmp += '\n';
#endif  // END - CODE_AVR || CODE_ESP
#endif  // END - debug
                if ((String(ptr).substring(2)).toInt() != 0) { /* repeats */
                  rep = (String(ptr).substring(2)).toInt();
#ifdef debug
#ifdef CODE_AVR
                  Serial.println(F("[DB] Input | SN; takeover repeats"));
#elif CODE_ESP
                  tmp += F("[DB] Input | SN; takeover repeats");
                  MSG_OUTPUTLN(tmp);
#endif  // END - CODE_AVR || CODE_ESP
#endif  // END - debug
                }
              } else if (strstr(ptr, "D=")) { /* datapart */
#ifdef debug
#ifdef CODE_AVR
                Serial.print(F("[DB] Input | SN; found ")); Serial.print(ptr);
                Serial.print(F(" with length ")); Serial.println(String(ptr + 2).length());
#elif CODE_ESP
                tmp = F("[DB] Input | SN; found "); tmp += ptr; tmp += F(" with length "); tmp += String(ptr + 2).length();
                MSG_OUTPUTLN(tmp);
#endif  // END - CODE_AVR || CODE_ESP
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
#ifdef CODE_AVR
                  Serial.println(
#elif CODE_ESP
                  MSG_OUTPUTLN(
#endif  // END - CODE_AVR || CODE_ESP
                    F("SN; found odd number of nibbles, no send !!!"));
                }
              }
              ptr = strtok(NULL, delimiter);
            }

            /* SEND */
            if (datavalid == 1) {
              digitalWriteFast(LED, HIGH);  // LED on
#ifdef debug
#ifdef CODE_AVR
              Serial.print(F("[DB] Input | SN; valid and ready to send with repeats=")); Serial.println(rep);
              Serial.println(senddata);
#elif CODE_ESP
              tmp = F("[DB] Input | SN; valid and ready to send with repeats="); tmp += rep;
              MSG_OUTPUTLN(tmp);
              MSG_OUTPUTLN(senddata);
#endif  // END - CODE_AVR || CODE_ESP
#endif  // END - debug
              uint8_t PKTLENis = Chip_readReg(CHIP_PKTLEN, READ_BURST); // old packet length
#ifdef debug
#ifdef CODE_AVR
              Serial.print(F("[DB] SEND from FHEM, old PKTLEN is ")); Serial.println(PKTLENis);
#elif CODE_ESP
              tmp = F("[DB] SEND from FHEM, old PKTLEN is "); tmp += PKTLENis;
              MSG_OUTPUTLN(tmp);
#endif  // END - CODE_AVR || CODE_ESP
#endif  // END - debug
              uint8_t len = strlen(senddata) / 2;
              Chip_writeReg(CHIP_PKTLEN, len);
#ifdef RFM69
              Chip_writeReg(0x3C, len - 1); // FifoThreshold setting
#endif  // END - RFM69
#ifdef debug
#ifdef CODE_AVR
              Serial.print(F("[DB] SEND from FHEM, act PKTLEN is ")); Serial.println(len);
#elif CODE_ESP
              tmp = F("[DB] SEND from FHEM, act PKTLEN is "); tmp += len;
              MSG_OUTPUTLN(tmp);
#endif  // END - CODE_AVR || CODE_ESP
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
        tmp = FPSTR(TXT_VERSION); tmp += FPSTR(compile_date);
        MSG_OUTPUTLN(tmp);
#endif  // END - CODE_AVR || CODE_ESP
      }
      break;  /* -#-#-#-#- - - next case - - - #-#-#-#- */
    case 'W': /* command W - write register*/
      if (ChipFound == false) {
        NO_Chip();
      } else {
        if (buf_input[1] && buf_input[1] != 'S' && buf_input[1] > 47 && buf_input[1] < CMD_W_REG_MAX && buf_input[2] && buf_input[3] && buf_input[4] && !buf_input[5]) {
          /* command W1203 | only adress smaller 3E -> buf_input[4] > 47 && buf_input[4] < 52 for W0... W1... W2... W3... (CC110x, RFM69 W0 - W7 */
          if (isHexadecimalDigit(buf_input[1]) && isHexadecimalDigit(buf_input[2]) && isHexadecimalDigit(buf_input[3]) && isHexadecimalDigit(buf_input[4])) {
            byte adr_dec = hexToDec(input.substring(1, 3));
            if (adr_dec >= Chip_writeReg_offset) {
              byte val_dec = hexToDec(input.substring(3));
#ifdef debug
#ifdef CODE_AVR
              Serial.print(F("[DB] Serial Input | cmd W with adr=0x")); Serial.print(onlyDecToHex2Digit(adr_dec));
              Serial.print(F(" value=0x")); Serial.println(onlyDecToHex2Digit(val_dec));
#elif CODE_ESP
              tmp = F("[DB] Serial Input | cmd W with adr=0x"); tmp += onlyDecToHex2Digit(adr_dec);
              tmp += F(" value=0x"); tmp += onlyDecToHex2Digit(val_dec);
              MSG_OUTPUTLN(tmp);
#endif  // END - CODE_AVR || CODE_ESP
#endif  // END - debug
              Chip_writeReg(adr_dec - Chip_writeReg_offset, val_dec);   // write in chip | adr - 2 because of sduino firmware
              EEPROMwrite(adr_dec - Chip_writeReg_offset, val_dec);     // write in flash
#ifdef CC110x
              Chip_writeReg(CC110x_FSCTRL0, 0);                         // 0x0C: FSCTRL0 – Frequency Synthesizer Control
#endif  // END - CC110x
#ifdef debug
#ifdef CODE_AVR
              Serial.print('W'); MSG_OUTPUT_DecToHEX_lz(adr_dec); MSG_OUTPUT_DecToHEX_lz(val_dec); Serial.println("");
#elif CODE_ESP
              tmp = 'W'; tmp += onlyDecToHex2Digit(adr_dec); tmp += onlyDecToHex2Digit(val_dec);
              MSG_OUTPUTLN(tmp);
#endif  // END - CODE_AVR || CODE_ESP
#endif  // END - debug
              ReceiveModeName = FPSTR(RECEIVE_MODE_USER);
            }
          }
        }
#ifdef CC110x
        else if (buf_input[1] == 'S' && buf_input[2] == '3' && isHexadecimalDigit(buf_input[3]) && !buf_input[4]) {
          /* command WS34 ... | from 0x30 to 0x3D */
          uint8_t reg = hexToDec(input.substring(2, 4));
          String val = onlyDecToHex2Digit( CC110x_CmdStrobe(reg) >> 4 );
          delay(1);
          String val1 = onlyDecToHex2Digit( CC110x_CmdStrobe(CC110x_SNOP) >> 4 );
#ifdef debug
#ifdef CODE_AVR
          Serial.print(F("cmdStrobeReg ")); Serial.print(onlyDecToHex2Digit(reg)); Serial.print(F(" chipStatus "));
          Serial.print(val); Serial.print(F(" delay1 ")); Serial.println(val1);
#elif CODE_ESP
          tmp = F("cmdStrobeReg "); tmp += onlyDecToHex2Digit(reg); tmp += F(" chipStatus "); tmp += val; tmp += F(" delay1 "); tmp += val1;
          MSG_OUTPUTLN(tmp);
#endif  // END - CODE_AVR || CODE_ESP
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
        TelnetClient[i] = TelnetServer.available();
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
      TelnetServer.available().stop();

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
void decode(const int pulse) {  /* Pulsübernahme und Weitergabe */
  if (MsgLen > 0) {
    last = first;
  } else {
    last = 0;
  }
  first = pulse;
  doDetect();
}


inline void doDetect() {  /* Pulsprüfung und Weitergabe an Patternprüfung */
  valid = (MsgLen == 0 || last == 0 || (first ^ last) < 0);   // true if a and b have opposite signs
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
    MSGBuild();
  }
}


void MSGBuild() { /* Nachrichtenausgabe */
  if (MsgLen >= MsgLenMin) {
    uint8_t rssi = Chip_readReg(0x34, 0xC0);  // nicht konvertiert
    digitalWriteFast(LED, HIGH);              // LED on
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
    Chip_readRSSI();
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
    digitalWriteFast(LED, LOW);  // LED off
  }
  PatReset();
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
      MSGBuild();
      break;
    }
  }
}


void PatReset() { /* Zurücksetzen nach Nachrichtenbau oder max. Länge */
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


void NO_Chip() {
#ifdef CODE_AVR
  Serial.println(
#elif CODE_ESP
  MSG_OUTPUTLN(
#endif  // END - CODE_AVR || CODE_ESP
    F("Operation not executable (no CHIP found)"));
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
