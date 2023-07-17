#pragma once

/*
    This file provides us with important information about the functional prototypes available to us,
    provides us with symbolic constants and keeps a list of all available macros.
*/

#include <Arduino.h>
#include <SPI.h>

extern float Freq_offset;

String EEPROMread_string(int address);
String onlyDecToHex2Digit(byte Dec);
boolean isNumeric(String str);
boolean str2ip(char *string, byte * IP);
byte hex2int(byte hex);
byte web_Bandw_cal(float input, byte reg_split);
byte web_Devi_Set(float deviation);
byte web_Mod_set(byte input);
float web_Freq_read(byte adr1, byte adr2, byte adr3);
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
void web_Datarate_Set(long datarate, byte * arr);
void web_Freq_Set(long frequency, byte * arr);
