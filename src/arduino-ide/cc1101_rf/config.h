#pragma once
#include <Arduino.h>

/*
   This file is part of the cc1101_Test project.
   all define pins
*/

/* Pins and other specific controller settings */
#if defined(ARDUINO_ARCH_ESP8266)
#define EEPROM_SIZE   512
#define GDO0          4           // GDO0     => ESP8266 (Pin TX out - PIN_SEND)
#define GDO2          5           // GDO2     => ESP8266 (Pin RX in  - PIN_RECEIVE)
#define LED           16          // LED      => ESP8266 (OK msg & WIFI)
#define CODE_ESP      1           // https://arduino-esp8266.readthedocs.io/en/3.1.2/reference.html#progmem
#elif defined(ARDUINO_ARCH_ESP32)
#define EEPROM_SIZE   512
#define GDO0          4           // GDO0     => ESP32 (Pin TX out - PIN_SEND)
#define GDO2          13          // GDO2     => ESP32 (Pin RX in  - PIN_RECEIVE)
#define LED           2           // LED      => ESP32 (OK msg & WIFI)
#define CODE_ESP      1           // https://arduino-esp8266.readthedocs.io/en/3.1.2/reference.html#progmem
#elif defined(ARDUINO_RADINOCC1101)
#define EEPROM_SIZE   512
#define GDO0          9           // GDO0     => Radino (Pin TX out - PIN_SEND)
#define GDO2          7           // GDO2     => Radino (Pin RX in  - PIN_RECEIVE)
#define LED           13          // LED      => Radino (OK msg)
#define digitalPinToInterrupt(p) ((p) == 0 ? 2 : ((p) == 1 ? 3 : ((p) == 2 ? 1 : ((p) == 3 ? 0 : ((p) == 7 ? 4 : NOT_AN_INTERRUPT)))))
#define PIN_MARK433   4
#define SS            8
#define CODE_AVR      1           // https://www.tutorialspoint.com/how-to-use-progmem-in-arduino-to-store-large-immutable-data & other code behave
#elif defined(ARDUINO_AVR_PRO)
#define EEPROM_SIZE   512
#define GDO0          3           // GDO0     => Arduino Pro Mini (Pin TX out - PIN_SEND)
#define GDO2          2           // GDO2     => Arduino Pro Mini (Pin RX in  - PIN_RECEIVE)
#define LED           9           // LED      => Arduino Pro Mini (OK msg)
#define CODE_AVR      1           // https://www.tutorialspoint.com/how-to-use-progmem-in-arduino-to-store-large-immutable-data & other code behave
#else
#define EEPROM_SIZE   512
#define GDO0          3           // GDO0     => Arduino Nano (Pin TX out - PIN_SEND)
#define GDO2          2           // GDO2     => Arduino Nano (Pin RX in  - PIN_RECEIVE)
#define LED           9           // LED      => Arduino Nano (OK msg)
#define CODE_AVR      1           // https://www.tutorialspoint.com/how-to-use-progmem-in-arduino-to-store-large-immutable-data & other code behave
#endif


/* Definitions for flexible cc1101 configuration */
#define ToggleTimeMin   15000     //   15000 milliseconds ->  15 seconds
#define ToggleTimeMax   1800000   // 3600000 milliseconds -> 0.5 hour

/* some backward compatibility */
//#define ESP32_core_v1     1    // to compatible for ESP32 core v1.0.6

/* Definitions for program code */
//#define debug             1    // to debug other
//#define debug_cc110x      1    // to debug CC1101 routines
//#define debug_cc110x_ms   1    // to debug CC1101 Marcstate
//#define debug_cc110x_MU   1    // to debug CC1101 decoder OOK
//#define debug_eeprom      1    // to debug all EEPROM
//#define debug_html        1    // to all HTML handling
//#define debug_websocket   1    // to all websocket handling
//#define debug_wifi        1    // to debug all wifi

/* SIGNALduino compatibility (please comment out for no compatibility) */
#define SIGNALduino_comp  1   // for compatibility in FHEM

/* Selection of available registers to compile into firmware
    Note: Please comment in or out to select !!!
*/
#if defined (ARDUINO_ARCH_ESP8266) || defined (ARDUINO_ARCH_ESP32)
#define Avantek                 1
#define Bresser_5in1            1
#define Bresser_6in1            1
#define Bresser_7in1            1
#define Fine_Offset_WH51_434    1
#define Fine_Offset_WH51_868    1
#define Fine_Offset_WH57_434    1
#define Fine_Offset_WH57_868    1
#define Inkbird_IBS_P01R        1
#define KOPP_FC                 1
#define Lacrosse_mode1          1
#define Lacrosse_mode2          1
#define OOK_MU_433              1
#define PCA301                  1
#define Rojaflex                1
#define X_Sense                 1

/* under development */
//#define HomeMatic               1
//#define Lacrosse_mode3          1
//#define MAX                     1
//#define WMBus_S                 1
//#define WMBus_T                 1

/* Configuration for WLAN devices */
#define TELNET_CLIENTS_MAX      3                       // maximum number of Telnet clients
#define TELNET_PORT             23                      // Telnet Port
#define WLAN_ssid_ap            "cc110x-rf-Gateway"     // AP - SSID
#define WLAN_password_ap        "config-gateway"        // AP - Passwort
#define WLAN_hostname           "cc110x-rf-Gateway"     // Hostname
#else
#define Bresser_5in1            1
#define Lacrosse_mode1          1
#define Lacrosse_mode2          1
#define OOK_MU_433              1
#define X_Sense                 1
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
#define EEPROM_ADDR_Prot        48
#define EEPROM_ADDR_ProtTo      49      /* ToggleProtocolls 49 - 52 */
#define EEPROM_ADDR_FW1         53
#define EEPROM_ADDR_FW2         54
#define EEPROM_ADDR_Toggle      56      /* ToggleTime 56 - 59 */

/* EEPROM - WIFI settings */
#define EEPROM_ADDR_SSID        60      /* Strings */
#define EEPROM_ADDR_PASS        92
#define EEPROM_ADDR_IP          156     /* IPAddress (4 Bytes) */
#define EEPROM_ADDR_GATEWAY     160
#define EEPROM_ADDR_DNS         164
#define EEPROM_ADDR_NETMASK     168
#define EEPROM_ADDR_DHCP        172     /* Flag´s */
#define EEPROM_ADDR_AP          173
#define EEPROM_ADDR_PATABLE     174
#define EEPROM_ADDR_CHK         183     /* FW – Prüfsumme über packet_length 183–184 */
#define EEPROM_ADDR_FOFFSET     185     /* cc110x freq offset | 185 - 188 */
#define EEPROM_ADDR_AFC         189     /* cc110x afc */
