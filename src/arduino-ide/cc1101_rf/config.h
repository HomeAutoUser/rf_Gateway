#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

/* **********************************************************
   Receiving hardware and some Definitions for program code */

//#define CC110x            1     /* https://wiki.fhem.de/w/images/3/3f/Selbstbau_cul_Schaltplan_1.png | SIGNALduino */
//#define RFM69             1     /* https://wiki.fhem.de/wiki/Datei:Lgw_Schaltplan_Devkit_full.png */

//#define debug             1     // to debug other
//#define debug_cc110x_MU   1     // to debug CC110x decoder OOK
//#define debug_cc110x_ms   1     // to debug CC110x Marcstate
//#define debug_chip        1     // to debug chip routines
//#define debug_eeprom      1     // to debug all EEPROM
//#define debug_html        1     // to debug HTML handling
//#define debug_telnet      1     // to debug telnet
//#define debug_websocket   1     // to debug websocket handling
//#define debug_wifi        1     // to debug wifi
//#define debug_mbus        1     // to debug mbus

//#define COUNT_LOOP        1     // loop-benchmark (höher=besser, delay(1) = ESP8266 RFM69 ca. 850, ESP32 CC1101 ca. 500, Nano CC1101 ca. 14500)
//                                   loop-benchmark (höher=besser, yield    = ESP8266 CC1101 WMBUS ca. 6000, Bresser/LaCrosse ca. 9500, Empfang schlechter?)
//                                   loop-benchmark (höher=besser, yield    = ESP8266 RFM69 WMBUS ca. 5800

#define FWVer             "V 2.1.2pre"
#define FWVerDate         "2024-03-20"

/* SIGNALduino compatibility (please comment out for no compatibility) */
#define SIGNALduino_comp  1     // for compatibility in FHEM

/* some backward compatibility */
//#define ESP32_core_v1     1     // to compatible for ESP32 core v1.0.6

/* Pins and other specific controller settings */
#if defined(ARDUINO_ARCH_ESP8266) && defined(CC110x)
#define GDO0          4           // GDO0     => ESP8266 (Pin TX out - PIN_SEND)
#define GDO2          5           // GDO2     => ESP8266 (Pin RX in  - PIN_RECEIVE)
#define LED           16          // LED      => ESP8266 (OK msg & WIFI)
#define CODE_ESP      1           // https://arduino-esp8266.readthedocs.io/en/3.1.2/reference.html#progmem
#elif defined(ARDUINO_ARCH_ESP8266) && defined(RFM69)
#define LED           16          // LED      => ESP8266 (OK msg & WIFI)
#define CODE_ESP      1           // https://arduino-esp8266.readthedocs.io/en/3.1.2/reference.html#progmem
#elif defined(ARDUINO_ARCH_ESP32) && defined(CC110x)
#define GDO0          4           // GDO0     => ESP32 (Pin TX out - PIN_SEND)
#define GDO2          13          // GDO2     => ESP32 (Pin RX in  - PIN_RECEIVE)
#define LED           2           // LED      => ESP32 (OK msg & WIFI)
#define CODE_ESP      1           // https://arduino-esp8266.readthedocs.io/en/3.1.2/reference.html#progmem
#elif defined(ARDUINO_ARCH_ESP32) && defined(RFM69)
#define LED           2           // LED      => ESP32 (OK msg & WIFI)
#define CODE_ESP      1           // https://arduino-esp8266.readthedocs.io/en/3.1.2/reference.html#progmem
#elif defined(ARDUINO_RADINOCC1101) && defined(CC110x)
#define GDO0          9           // GDO0     => Radino (Pin TX out - PIN_SEND)
#define GDO2          7           // GDO2     => Radino (Pin RX in  - PIN_RECEIVE)
#define LED           13          // LED      => Radino (OK msg)
#define digitalPinToInterrupt(p) ((p) == 0 ? 2 : ((p) == 1 ? 3 : ((p) == 2 ? 1 : ((p) == 3 ? 0 : ((p) == 7 ? 4 : NOT_AN_INTERRUPT)))))
#define PIN_MARK433   4
#define SS            8
#define CODE_AVR      1           // https://www.tutorialspoint.com/how-to-use-progmem-in-arduino-to-store-large-immutable-data & other code behave
#elif defined(ARDUINO_AVR_PRO) && defined(CC110x)
#define GDO0          3           // GDO0     => Arduino Pro Mini (Pin TX out - PIN_SEND)
#define GDO2          2           // GDO2     => Arduino Pro Mini (Pin RX in  - PIN_RECEIVE)
#define LED           9           // LED      => Arduino Pro Mini (OK msg)
#define CODE_AVR      1           // https://www.tutorialspoint.com/how-to-use-progmem-in-arduino-to-store-large-immutable-data & other code behave
#elif defined(ARDUINO_AVR_PRO) && defined(RFM69)
#define LED           9           // LED      => Arduino Pro Mini (OK msg)
#define CODE_AVR      1           // https://www.tutorialspoint.com/how-to-use-progmem-in-arduino-to-store-large-immutable-data & other code behave
#elif defined(ARDUINO_AVR_NANO) && defined(CC110x)
#define GDO0          3           // GDO0     => Arduino Nano (Pin TX out - PIN_SEND)
#define GDO2          2           // GDO2     => Arduino Nano (Pin RX in  - PIN_RECEIVE)
#define LED           9           // LED      => Arduino Nano (OK msg)
#define CODE_AVR      1           // https://www.tutorialspoint.com/how-to-use-progmem-in-arduino-to-store-large-immutable-data & other code behave
#elif defined(ARDUINO_AVR_NANO) && defined(RFM69)
#define LED           9           // LED      => Arduino Nano (OK msg)
#define CODE_AVR      1           // https://www.tutorialspoint.com/how-to-use-progmem-in-arduino-to-store-large-immutable-data & other code behave
#else

#endif

/* EEPROM size | all AVR board = fix SIZE | other variable size */
#define EEPROM_SIZE   512

/* Definitions for flexible cc1101 configuration */
#define TOGGLE_TIME_MIN   1     // seconds
#define TOGGLE_TIME_MAX   255   // seconds

/* Selection of available registers to compile into firmware
    NOTE 1: Please comment in or out to select !!!
    NOTE 2: Don't forget to adjust variable NUMBER_OF_MODES manually in cc110x.h or rfm69.h !!!
*/
#if defined (ARDUINO_ARCH_ESP8266) || defined (ARDUINO_ARCH_ESP32)
#define Bresser_5in1            1
#define Fine_Offset_WH57_868    1
#define Inkbird_IBS_P01R        1
#define Lacrosse_mode1          1
#define Lacrosse_mode2          1
#define WMBus_S                 1
#define WMBus_T                 1

#ifdef CC110x
#define Avantek                 1
#define Bresser_6in1            1
#define Bresser_7in1            1
#define Fine_Offset_WH51_434    1
#define Fine_Offset_WH51_868    1
#define Fine_Offset_WH57_434    1
#define KOPP_FC                 1
#define OOK_MU_433              1
#define PCA301                  1
#define Rojaflex                1
#define X_Sense                 1
#endif

/* under development */
//#define HomeMatic               1   // only CC110x inside
//#define Lacrosse_mode3          1   // only CC110x inside
//#define Max                     1   // only CC110x inside

/* Configuration for WLAN devices */
#define TELNET_CLIENTS_MAX      3                       // maximum number of Telnet clients
#define TELNET_PORT             23                      // Telnet Port
#define WLAN_password_ap        "config-gateway"        // AP - Passwort

#ifdef CC110x
#define WLAN_hostname           "cc110x-rf-Gateway"     // Hostname !!! maximum-length of 25 !!!
#elif RFM69
#define WLAN_hostname           "rfm69-rf-Gateway"      // Hostname !!! maximum-length of 25 !!!
#else
#define WLAN_hostname           "rf-Gateway"            // Hostname !!! maximum-length of 25 !!!
#endif

#else
/* all other devices without WLAN
    NOTE: Don't forget to adjust variable NUMBER_OF_MODES manually in cc110x.h or rfm69.h !!!
*/
#define Bresser_5in1            1
#define Lacrosse_mode1          1
#define Lacrosse_mode2          1
//#define OOK_MU_433              1     // OOK_MU_433 or WMBus, both is not enough RAM
//#define WMBus_S                 1     // OOK_MU_433 or WMBus, both is not enough RAM | Minimum requirement unknown - only test
#define WMBus_T                 1     // OOK_MU_433 or WMBus, both is not enough RAM | Minimum requirement unknown - only test
//#define X_Sense                 1
#endif

/* varible´s for Serial & TelNet TimeOut´s | sets the maximum milliseconds to wait for data. */
#define Timeout_Serial          125
#define Timeout_Telnet          125

/* varible´s for EEPROM */
#define Prog_Ident1             0xDE    /* fiktive Kennung zur Identifizierung von FW */
#define Prog_Ident2             0x22

/* varible´s for SerialSpeed´s */
#if defined (ARDUINO_ARCH_ESP8266) || defined (ARDUINO_ARCH_ESP32)
#define SerialSpeed             115200
#else
#define SerialSpeed             57600
#endif

/* EEPROM adresses */
#define EEPROM_ADDR_MIN         0       /* size */
#define EEPROM_ADDR_MAX         512

/* Start - END Register ADDR is fixed !!! */
/* TODO Register Start | END, Toggle max 8 */
#define EEPROM_ADDR_IP          156     /* IPAddress (4 Bytes) */
#define EEPROM_ADDR_GATEWAY     160
#define EEPROM_ADDR_DNS         164
#define EEPROM_ADDR_NETMASK     168
#define EEPROM_ADDR_DHCP        172     /* Flag´s */
#define EEPROM_ADDR_AP          173
#define EEPROM_ADDR_PATABLE     174     // 8 Byte 
#define EEPROM_ADDR_CHK         183     /* FW – Prüfsumme über PKTLEN 183–184 */
#define EEPROM_ADDR_FOFFSET     185     /* cc110x freq offset | 185 - 188 */
#define EEPROM_ADDR_AFC         189     /* cc110x afc */
#define EEPROM_ADDR_Prot        200     // mit command 'm' gesetzter Modus für start ohne toggle | TODO prüfen
#define EEPROM_ADDR_FW1         209
#define EEPROM_ADDR_FW2         210
#define EEPROM_ADDR_SSID        256     /* Strings (max 32)*/
#define EEPROM_ADDR_PASS        288     /* Strings (max 32)*/
#define EEPROM_ADDR_ToggleTime  384     /* Byte ToggleTimeMode (max 64 free in EEPROM, size set in NUMBER_OF_MODES cc110x.h / rfm69.h) */
#define EEPROM_ADDR_ToggleMode  448     /* Byte ToggleArray (max 64 free in EEPROM, size set in NUMBER_OF_MODES cc110x.h / rfm69.h) */

struct Data {
  const uint8_t* reg_val;
  byte length;
  const char* name;
  byte PKTLEN;
};

#endif  // END - #ifndef CONFIG_H
