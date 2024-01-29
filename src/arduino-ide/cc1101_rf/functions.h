#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <Arduino.h>
#include <SPI.h>

extern float Freq_offset;

String EEPROMread_string(int address);
String onlyDecToHex2Digit(byte Dec);
boolean isNumeric(String str);
boolean str2ip(char *string, byte * IP);
byte hex2int(byte hex);
int freeRam();
uint16_t hexToDec(String hexString);
uint32_t EEPROMread_long(int address);
uint8_t * EEPROMread_ipaddress(int address);
uint8_t EEPROMread(int adr);
void EEPROMclear();
void EEPROMread_table();
void EEPROMwrite(int adr, byte val);
void EEPROMwrite_ipaddress(int address, String ip);
void EEPROMwrite_long(int address, long value);
void EEPROMwrite_string(int address, String str);
#endif  // END - #ifndef FUNCTIONS_H
