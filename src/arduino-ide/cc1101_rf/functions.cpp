#include "config.h"
#include "cc110x.h"
#include "register.h"
#include "macros.h"

#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>    // need for ESP8266 function system_get_free_heap_size (include failed, so separate)
#endif


uint16_t hexToDec(String hexString) {       /* Passes the HEX value to Dec */
  byte ret = 0;
  char ch;

  for (byte i = 0, n = hexString.length(); i != n; ++i) {
    ch = hexString[i];
    int val = 0;
    if ('0' <= ch && ch <= '9')      val = ch - '0';
    else if ('a' <= ch && ch <= 'f') val = ch - 'a' + 10;
    else if ('A' <= ch && ch <= 'F') val = ch - 'A' + 10;
    else return 999;
    ret = ret * 16 + val;
  }
  return ret;
}


byte hex2int(byte hex) {                      /* convert a hexdigit to int (smallest variant, sketch is bigger with printf or scanf) */
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


float web_Freq_read(byte adr1, byte adr2, byte adr3) {    /* frequency calculation - 0x0D 0x0E 0x0F | 26*(($r{"0D"}*256+$r{"0E"})*256+$r{"0F"})/65536 */
  float Freq;
  Freq = adr1 * 256;
  Freq = (Freq + adr2) * 256;
  Freq = Freq + adr3 ;
  Freq = ( (26 * Freq) / 65536 ) * 1000;
  return Freq;
}


void web_Freq_Set(long frequency, byte * arr) {   /* frequency set & calculation - 0x0D 0x0E 0x0F | function used in CC1101_writeRegFor */
  int32_t f;
  f = (frequency + Freq_offset * 1000) / 26000 * 65536;
  arr[0] = f / 65536;
  arr[1] = (f % 65536) / 256;
  arr[2] = f % 256;

#ifdef debug
  Serial.print(F("DB web_Freq_Set, input ")); Serial.print(frequency); Serial.println(F(" MHz"));
  Serial.print(F("DB web_Freq_Set, FREQ2..0 (0D,0E,0F) to ")); Serial.print(onlyDecToHex2Digit(arr[0]));
  Serial.print(' '); Serial.print(onlyDecToHex2Digit(arr[1]));
  Serial.print(' '); Serial.println(onlyDecToHex2Digit(arr[2]));
#endif
}


#if defined (ARDUINO_ARCH_ESP8266) || defined (ARDUINO_ARCH_ESP32)
byte web_Bandw_cal(float input, byte reg_split) {   /* bandwidth calculation from web */
  int bits = 0;
  int bw = 0;
  for (int e = 0; e < 4; e++) {
    for (int m = 0; m < 4; m++) {
      bits = (e << 6) + (m << 4);
      bw = int(26000 / (8 * (4 + m) * (1 << e)));
      if (input >= bw) {
        goto END;
      }
    }
  }
END:
#ifdef debug
  Serial.print(F("DB web_Bandw_cal, Setting MDMCFG4 (10) to ")); Serial.println(onlyDecToHex2Digit(reg_split + bits));
#endif
  return (reg_split + bits);
}


void web_Datarate_Set(long datarate, byte * arr) {    /* datarate set & calculation - 0x10 0x11 */
  if (datarate < 24.7955) {
    datarate = 24.7955;
  } else if (datarate > 1621830) {
    datarate = 1621830;
  }

  int ret = CC1101_readReg(0x10, READ_BURST);
  ret = ret & 0xf0;

  float DRATE_E = datarate * ( pow(2, 20) ) / 26000000.0;
  DRATE_E = log(DRATE_E) / log(2);
  DRATE_E = int(DRATE_E);

  float DRATE_M = (datarate * (pow(2, 28)) / (26000000.0 * (pow(2, DRATE_E)))) - 256;
  DRATE_M = int(DRATE_M);
  int DRATE_Mr = round(DRATE_M);

  int DRATE_M1 = int(DRATE_M) + 1;
  int DRATE_E1 = DRATE_E;

  if (DRATE_M1 == 256) {
    DRATE_M1 = 0;
    DRATE_E1++;
  }

  if (DRATE_Mr != DRATE_M) {
    DRATE_M = DRATE_M1;
    DRATE_E = DRATE_E1;
  }

  arr[0] = ret + DRATE_E;
  arr[1] = DRATE_M;
#ifdef debug
  Serial.print(F("DB web_Datarate_Set, MDMCFG4..MDMCFG3 to ")); Serial.print(onlyDecToHex2Digit(arr[0])); Serial.print(onlyDecToHex2Digit(arr[1]));
  Serial.print(' '); Serial.print(F(" = ")); Serial.print(datarate); Serial.println(F(" Hz"));
#endif
}


byte web_Devi_Set(float deviation) {    /* Deviation set & calculation */
  if (deviation > 380.859375) {
    deviation = 380.859375;
  }
  if (deviation < 1.586914) {
    deviation = 1.586914;
  }

  float deviatn_val;
  int bits;
  int devlast = 0;
  int bitlast = 0;

  for (int DEVIATION_E = 0; DEVIATION_E < 8; DEVIATION_E++) {
    for (int DEVIATION_M = 0; DEVIATION_M < 8; DEVIATION_M++) {
      deviatn_val = (8 + DEVIATION_M) * (pow(2, DEVIATION_E)) * 26000.0 / (pow(2, 17));
      bits = DEVIATION_M + (DEVIATION_E << 4);
      if (deviation > deviatn_val) {
        devlast = deviatn_val;
        bitlast = bits;
      } else {
        if ((deviatn_val - deviation) < (deviation - devlast)) {
          devlast = deviatn_val;
          bitlast = bits;
        }
      }
    }
  }

#ifdef debug
  Serial.print(F("DB web_Devi_Set, DEVIATN (15) to ")); Serial.print(bitlast, HEX); Serial.println(F(" (value set to next possible level)"));
#endif
  return bitlast;
}


byte web_Mod_set(byte input) {
#ifdef debug
  Serial.print(F("DB web_Mod_set, set new value to ")); Serial.println(input);
  Serial.print(F("DB web_Mod_set, MDMCFG2 (12) value is ")); Serial.println(onlyDecToHex2Digit(CC1101_readReg(0x12, READ_BURST)));
#endif

  /* read all split values | example F1 -> 11110001 */
  byte reg12_6_4 = CC1101_readReg(0x12, READ_BURST) & 0x8f ;
  byte output = input << 4;
  output = output | reg12_6_4;
  return output;
}
#endif


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


int freeRam () {    /* gibt den freien Speicher wieder | Arduino Nano (andere uC´s ggf andere Variante) */
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
#ifdef debug_eeprom
  Serial.print(F("DB EEPROMread, address "));
  Serial.print(adr);
  Serial.print(F(" - "));
  Serial.println(ret);
#endif
  return ret;
}


// https://www.aranacorp.com/en/using-the-eeprom-with-the-esp8266/ //
// https://blog.hirnschall.net/esp8266-eeprom/ //
void EEPROMwrite(int adr, byte val) {
#ifdef debug_eeprom
  Serial.print(F("DB EEPROMwrite, at address ")); Serial.print(adr); Serial.print(F(" - ")); Serial.println(val);
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
  Serial.print(F("DB EEPROMread_string, at address ")); Serial.print(address); Serial.print(F(" - ")); Serial.print(str);
#endif
  return str;
}


void EEPROMwrite_string(int address, String str) {    /* write String to EEPROM */
#ifdef debug_eeprom
  Serial.print(F("DB EEPROMwrite_string, at address ")); Serial.print(address); Serial.print(F(" - "));
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
  Serial.print(F("DB EEPROMwrite_long, at address ")); Serial.print(address); Serial.print(F(" - ")); Serial.println(byte4);
  Serial.print(F("DB EEPROMwrite_long, at address ")); Serial.print(address + 1); Serial.print(F(" - ")); Serial.println(byte3);
  Serial.print(F("DB EEPROMwrite_long, at address ")); Serial.print(address + 2); Serial.print(F(" - ")); Serial.println(byte2);
  Serial.print(F("DB EEPROMwrite_long, at address ")); Serial.print(address + 3); Serial.print(F(" - ")); Serial.println(byte1);
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
  Serial.print(F("DB EEPROMread_long, at address ")); Serial.print(address); Serial.print(F(" - ")); Serial.println(byte4);
  Serial.print(F("DB EEPROMread_long, at address ")); Serial.print(address + 1); Serial.print(F(" - ")); Serial.println(byte3);
  Serial.print(F("DB EEPROMread_long, at address ")); Serial.print(address + 2); Serial.print(F(" - ")); Serial.println(byte2);
  Serial.print(F("DB EEPROMread_long, at address ")); Serial.print(address + 3); Serial.print(F(" - ")); Serial.println(byte1);
  Serial.print(F("DB EEPROMread_long, result is "));
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
  Serial.print(F("DB EEPROMwrite_ipaddress, at address ")); Serial.print(address); Serial.print(F(" - "));
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
