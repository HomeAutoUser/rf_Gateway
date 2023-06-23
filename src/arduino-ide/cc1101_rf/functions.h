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
String web_Datarate_set(float input);
String web_Devi_set(float input);
String web_Freq_set(String input);
String web_Marcstate_read();
boolean isNumeric(String str);
boolean str2ip(char *string, byte * IP);
byte hex2int(byte hex);
byte web_Mod_set(String input);
float web_Freq_read(byte adr1, byte adr2, byte adr3);
int freeRam();
int web_Bandw_cal(int input, int input_split);
uint16_t hexToDec(String hexString);
uint32_t EEPROMread_long(int address);
uint8_t * EEPROMread_ipaddress(int address);
uint8_t EEPROMread(int adr);
unsigned long getUptime();
void EEPROMclear();
void EEPROMread_table();
void EEPROMwrite(int adr, byte val);
void EEPROMwrite_ipaddress(int address, String ip);
void EEPROMwrite_long(int address, long value);
void EEPROMwrite_string(int address, String str);
