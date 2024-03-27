#include "config.h"
#include "functions.h"
#include "macros.h"

#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>    // need for ESP8266 function system_get_free_heap_size (include failed, so separate)
#endif


uint8_t hexToDec(String hexString) {           // convert 2 hexdigits to 1 byte
  byte ret = 0;
  char ch;
  for (byte i = 0; i < 2; ++i) {
    ch = hexString[i];
    byte val = 0;
    if ('0' <= ch && ch <= '9')      val = ch - '0';
    else if ('a' <= ch && ch <= 'f') val = ch - 'a' + 10;
    else if ('A' <= ch && ch <= 'F') val = ch - 'A' + 10;
    ret = ret * 16 + val;
  }
  return ret;
}

byte hex2int(byte hex) {                      // convert 1 (char) hexdigit to 1 byte
  if (hex >= '0' && hex <= '9') hex = hex - '0';
  else if (hex >= 'a' && hex <= 'f') hex = hex - 'a' + 10;
  else if (hex >= 'A' && hex <= 'F') hex = hex - 'A' + 10;
  return hex;
}

String onlyDecToHex2Digit(byte Dec) {
  char ret[3];
  ret[0] = (Dec >> 4);
  ret[1] = (Dec & 0x0F);
  ret[0] = (ret[0] > 9 ? ret[0] + 55 : ret[0] + '0');
  ret[1] = (ret[1] > 9 ? ret[1] + 55 : ret[1] + '0');
  ret[2] = '\0';
  return ret;
}


boolean isNumeric(String str) {   /* Checks the value for numeric -> Return: 0 = nein / 1 = ja */
  unsigned int stringLength = str.length();

  if (stringLength == 0) {
    return false;
  }

  boolean seenDecimal = false;

  for (unsigned int i = 0; i < stringLength; ++i) {
    if (isDigit(str.charAt(i))) {
      continue;
    } else if ( i == 0 && str.charAt(0) == '-') {
      continue;
    }
    if (str.charAt(i) == '.') {
      if (seenDecimal) {
        return false;
      }
      seenDecimal = true;
      continue;
    }
    return false;
  }
  return true;
}


int freeRam() {    /* gibt den freien Speicher wieder | Arduino Nano (andere uC´s ggf andere Variante) */
#if defined (ARDUINO_AVR_NANO) || defined (ARDUINO_RADINOCC1101) || defined (ARDUINO_AVR_PRO)
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
#endif
#ifdef ARDUINO_ARCH_ESP8266
  return system_get_free_heap_size();
#endif
#ifdef ARDUINO_ARCH_ESP32
  return ESP.getFreeHeap();
#endif
}


uint8_t EEPROMread(int adr) {
  byte ret = 254;
#ifdef ARDUINO_AVR_NANO
  ret = EEPROM.read(adr);
#else
  EEPROM.get(adr, ret);
#endif
  //#ifdef debug_eeprom
  //  Serial.print(F("[DB] EEPROMread, address ")); Serial.print(adr); Serial.print(F(" - ")); Serial.println(ret);
  //#endif
  return ret;
}


// https://www.aranacorp.com/en/using-the-eeprom-with-the-esp8266/ //
// https://blog.hirnschall.net/esp8266-eeprom/ //
void EEPROMwrite(int adr, byte val) {
#ifdef debug_eeprom
  Serial.print(F("[DB] EEPROMwrite, at address ")); Serial.print(adr); Serial.print(F(" - ")); Serial.println(val);
#endif

#if defined (ARDUINO_AVR_NANO) || defined (ARDUINO_RADINOCC1101) || defined (ARDUINO_AVR_PRO)
  EEPROM.write(adr, val);
#else
  EEPROM.put(adr, val);       // Schreiben ab Adresse 0
  EEPROM.commit();            // Übernahme in den Flash
#endif
}


String EEPROMread_string(int address) {   /* read String from EEPROM (Address) */
  String str = "";
  for (int i = address; i < EEPROM_ADDR_MAX; ++i) {
    byte value = 0;
#ifdef ARDUINO_AVR_NANO
    value = EEPROM.read(i);
#else
    EEPROM.get(i, value);
#endif
    if (value == 0 || value == 255) {
      return str;
    }
    str += char(value);
  }
#ifdef debug_eeprom
  Serial.print(F("[DB] EEPROMread_string, at address ")); Serial.print(address); Serial.print(F(" - ")); Serial.print(str);
#endif
  return str;
}


void EEPROMwrite_string(int address, String str) {    /* write String to EEPROM */
#ifdef debug_eeprom
  Serial.print(F("[DB] EEPROMwrite_string, at address ")); Serial.print(address); Serial.print(F(" - "));
#endif

  for (unsigned int i = 0; i < str.length(); ++i) {
#ifndef ARDUINO_AVR_NANO
    EEPROM.put(address + i, str[i]);
#else
    EEPROM.write(address + i, str[i]);
#endif

#ifdef debug_eeprom
    Serial.print(str[i]);
#endif
  }

#if not defined (ARDUINO_AVR_NANO) && not defined (ARDUINO_RADINOCC1101) && not defined (ARDUINO_AVR_PRO)
  EEPROM.put(address + str.length(), 0);
  EEPROM.commit();
#else
  EEPROM.write(address + str.length(), 0);
#endif

#ifdef debug_eeprom
  Serial.println();
#endif
}


/* This function will write a 4 byte (32bit) long to the eeprom at
  the specified address to address + 3. */
void EEPROMwrite_long(int address, long value) {
  /* Decomposition from a long to 4 bytes by using bitshift.
     Byte1 = Most significant -> Byte4 = Least significant byte */
  byte byte4 = (value & 0xFF);
  byte byte3 = ((value >> 8) & 0xFF);
  byte byte2 = ((value >> 16) & 0xFF);
  byte byte1 = ((value >> 24) & 0xFF);
  /* Write the 4 bytes into the eeprom memory. */
#ifdef debug_eeprom
  Serial.print(F("[DB] EEPROMwrite_long, at address ")); Serial.print(address); Serial.print(F(" - ")); Serial.println(byte4);
  Serial.print(F("[DB] EEPROMwrite_long, at address ")); Serial.print(address + 1); Serial.print(F(" - ")); Serial.println(byte3);
  Serial.print(F("[DB] EEPROMwrite_long, at address ")); Serial.print(address + 2); Serial.print(F(" - ")); Serial.println(byte2);
  Serial.print(F("[DB] EEPROMwrite_long, at address ")); Serial.print(address + 3); Serial.print(F(" - ")); Serial.println(byte1);
#endif

#if defined (ARDUINO_ARCH_ESP8266) || defined (ARDUINO_ARCH_ESP32)
  EEPROM.put(address, byte4);
  EEPROM.put(address + 1, byte3);
  EEPROM.put(address + 2, byte2);
  EEPROM.put(address + 3, byte1);
  EEPROM.commit();
#else
  EEPROM.write(address, byte4);
  EEPROM.write(address + 1, byte3);
  EEPROM.write(address + 2, byte2);
  EEPROM.write(address + 3, byte1);
#endif
}


uint32_t EEPROMread_long(int address) {
  /* Read the 4 bytes from the eeprom memory. */
#ifndef ARDUINO_AVR_NANO
  byte byte4;
  EEPROM.get(address, byte4);
  byte byte3;
  EEPROM.get(address + 1, byte3);
  byte byte2;
  EEPROM.get(address + 2, byte2);
  byte byte1;
  EEPROM.get(address + 3, byte1);
#else
  byte byte4 = EEPROM.read(address);
  byte byte3 = EEPROM.read(address + 1);
  byte byte2 = EEPROM.read(address + 2);
  byte byte1 = EEPROM.read(address + 3);
#endif

#ifdef debug_eeprom
  Serial.print(F("[DB] EEPROMread_long, at address ")); Serial.print(address); Serial.print(F(" - ")); Serial.println(byte4);
  Serial.print(F("[DB] EEPROMread_long, at address ")); Serial.print(address + 1); Serial.print(F(" - ")); Serial.println(byte3);
  Serial.print(F("[DB] EEPROMread_long, at address ")); Serial.print(address + 2); Serial.print(F(" - ")); Serial.println(byte2);
  Serial.print(F("[DB] EEPROMread_long, at address ")); Serial.print(address + 3); Serial.print(F(" - ")); Serial.println(byte1);
  Serial.print(F("[DB] EEPROMread_long, result is "));
  Serial.println((byte4 & 0xFF) + ((byte3 << 8) & 0xFFFF) + ((byte2 << 16) & 0xFFFFFF) + ((byte1 << 24) & 0xFFFFFFFF));
#endif
  /* Return the recomposed long by using bitshift. */
  return ((uint32_t)byte4 & 0xFF) + (((uint32_t)byte3 << 8) & 0xFFFF) + (((uint32_t)byte2 << 16) & 0xFFFFFF) + (((uint32_t)byte1 << 24) & 0xFFFFFFFF);
}


boolean str2ip(char *string, byte * IP) {               /* Convert a char string to IP byte array */
  byte c;
  byte part = 0;
  int value = 0;

  for (unsigned int x = 0; x <= strlen(string); x++)
  {
    c = string[x];
    if (isdigit(c))
    {
      value *= 10;
      value += c - '0';
    }
    else if (c == '.' || c == 0) // next octet from IP address
    {
      if (value <= 255)
        IP[part++] = value;
      else
        return false;
      value = 0;
    }
    else if (c == ' ')          // ignore these
      ;
    else                        // invalid token
      return false;
  }
  if (part == 4)                // correct number of octets
    return true;
  return false;
}


#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
void EEPROMwrite_ipaddress(int address, String ip) {
  /* This function will write IPAddress (4 byte) to the eeprom at
     the specified address to address + 3.
  */
  byte ipa[4];
  str2ip((char*)ip.c_str(), ipa);

#ifdef debug_eeprom
  Serial.print(F("[DB] EEPROMwrite_ipaddress, at address ")); Serial.print(address); Serial.print(F(" - "));
  Serial.print(ipa[0]); Serial.print('.'); Serial.print(ipa[1]); Serial.print('.'); Serial.print(ipa[2]); Serial.print('.'); Serial.println(ipa[3]);
#endif

#if not defined (ARDUINO_AVR_NANO) && not defined (ARDUINO_RADINOCC1101)
  EEPROM.write(address, ipa[0]);
  EEPROM.write(address + 1, ipa[1]);
  EEPROM.write(address + 2, ipa[2]);
  EEPROM.write(address + 3, ipa[3]);
  EEPROM.commit();
#endif
}
#endif


uint8_t * EEPROMread_ipaddress(int address) {             /* read IP-Address from EEPROM */
  uint8_t * EEPROMret = NULL;
  EEPROMret = (uint8_t *) calloc(4, sizeof(uint8_t));
  EEPROMret[0] = EEPROM.read(address);
  EEPROMret[1] = EEPROM.read(address + 1);
  EEPROMret[2] = EEPROM.read(address + 2);
  EEPROMret[3] = EEPROM.read(address + 3);
  return (EEPROMret);
}


/* Speicherbereich auslesen
   gibt einen Dump des Speicherinhalts in tabellarischer Form über den seriellen Port aus. */
void EEPROMread_table() {
  uint8_t j = 0;
  Serial.println(F("-----------------------------------------------------"));
  Serial.println(F("EEPROM read all bytes"));
  Serial.print(F("Addr. "));
  for (uint8_t i = 0; i < 16; i++) {
    Serial.print(onlyDecToHex2Digit(i));
    Serial.print(' ');
  }
  Serial.println();
  Serial.println(F("-----------------------------------------------------"));
  for (uint16_t i = 0; i < EEPROM_SIZE; i++) {
    if (j == 0) {
      Serial.print(onlyDecToHex2Digit(i >> 8));
      Serial.print(onlyDecToHex2Digit(i & 0x00FF));
      Serial.print(": ");
    }
    uint8_t b = EEPROM.read(i);
    String hex = onlyDecToHex2Digit(b);
    hex += ' ';
    j++;
    if (j == 16) {
      j = 0;
      Serial.println(hex);
    } else {
      Serial.print(hex);
    }
  }
  Serial.println(F("-----------------------------------------------------"));
}


void EEPROMclear() {
  for (uint16_t i = 0; i < EEPROM_SIZE; EEPROM.write(i++, 255));
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  EEPROM.commit();
#endif
}
