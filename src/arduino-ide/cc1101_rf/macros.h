#ifndef MACROS_H
#define MACROS_H

#include <Arduino.h>

#define ChipSelect() digitalWriteFast(SS, LOW)       // Select (SPI)
#define ChipDeselect() digitalWriteFast(SS, HIGH)    // Deselect (SPI)

void SerialPrintDecToHex(uint8_t dec);

/* --- all SETTINGS for Arduino Nano ---------------------------------------------------------------------------------------------------------- */
#if defined (ARDUINO_AVR_NANO) || defined (ARDUINO_RADINOCC1101) || defined (ARDUINO_AVR_PRO)
void MSG_OUTPUT_DecToHEX_lz(uint8_t output);
void MSG_OUTPUTALL_DecToHEX_lz(uint8_t output);
#endif


/* --- all SETTINGS for the ESP8266 and ESP32 ------------------------------------------------------------------------------------------------- */
#if defined (ARDUINO_ARCH_ESP8266) || defined (ARDUINO_ARCH_ESP32)
#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#elif ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#endif
extern WiFiClient TelnetClient[TELNET_CLIENTS_MAX];
extern byte TELNET_CLIENT_COUNTER;
extern byte client_now;

void MSG_OUTPUT(const char output);
void MSG_OUTPUTLN(String output); //917 1408
void MSG_OUTPUTALL(String output);
void MSG_OUTPUTALLLN(String output);
void MSG_OUTPUT_DecToHEX_lz(uint8_t output);
void MSG_OUTPUTALL_DecToHEX_lz(uint8_t output);

/* Useful Constants | macros from DateTime.h */
#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24L)
#endif

#endif // END - #ifndef MACROS_H
