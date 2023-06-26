#include "config.h"
#include "cc110x.h"
#include "register.h"

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
  String ret = String(Dec, HEX);
  if (Dec < 16) {
    ret = "0" + ret;
  }
  ret.toUpperCase();
  return ret;
}


float web_Freq_read(byte adr1, byte adr2, byte adr3) {    /* frequency calculation - 0x0D 0x0E 0x0F | 26*(($r{"0D"}*256+$r{"0E"})*256+$r{"0F"})/65536 */
  float Freq;
  Freq = adr1 * 256;
  Freq = (Freq + adr2) * 256;
  Freq = Freq + adr3 ;
  Freq = (26 * Freq) / 65536;
  return Freq;
}


String web_Freq_set(String input) {   /* frequency set & calculation - 0x0D 0x0E 0x0F | function used in CC1101_writeRegFor */
  float f;
  int f2;
  int f1;
  int f0;
  f = (input.toFloat() + Freq_offset) / 26 * 65536;
  f2 = f / 65536;
  f1 = int(fmod(f, 65536)) / 256;
  f0 = fmod(f, 256);

#ifdef debug
  Serial.print(F("DB web_Freq_set, input ")); Serial.print(input); Serial.println(F(" MHz"));
  Serial.print(F("DB web_Freq_set, FREQ2..0 (0D,0E,0F) to ")); Serial.print(onlyDecToHex2Digit(f2));
  Serial.print(' '); Serial.print(onlyDecToHex2Digit(f1));
  Serial.print(' '); Serial.println(onlyDecToHex2Digit(f0));
#endif

  return String(onlyDecToHex2Digit(f2)) + String(onlyDecToHex2Digit(f1)) + String(onlyDecToHex2Digit(f0));
}

#if defined (ARDUINO_ARCH_ESP8266) || defined (ARDUINO_ARCH_ESP32)
int web_Bandw_cal(int input, int input_split) {   /* bandwidth calculation from web */
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
  Serial.print(F("DB web_Bandw_cal, Setting MDMCFG4 (10) to ")); Serial.println(onlyDecToHex2Digit(input_split + bits));
#endif
  return (input_split + bits);
}


String web_Datarate_set(float input) {    /* datarate set & calculation - 0x10 0x11 */
  if (input < 0.0247955) {
    input = 0.0247955;
  } else if (input > 1621.83) {
    input = 1621.83;
  }

  int ret = CC1101_readReg(0x10, READ_BURST);
  ret = ret & 0xf0;

  float DRATE_E = (input * 1000.0) * ( pow(2, 20) ) / 26000000.0;
  DRATE_E = log(DRATE_E) / log(2);
  DRATE_E = int(DRATE_E);

  float DRATE_M = ((input * 1000.0) * (pow(2, 28)) / (26000000.0 * (pow(2, DRATE_E)))) - 256;
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

  String reg10 = String( int(ret + DRATE_E), HEX );
  String reg11 = String( int(DRATE_M), HEX );

#ifdef debug
  Serial.print(F("DB web_Datarate_set, MDMCFG4..MDMCFG3 to ")); Serial.print(reg10);
  Serial.print(' '); Serial.print(reg11); Serial.print(F(" = ")); Serial.print(input); Serial.println(F(" kHz"));
#endif

  return reg10 + reg11;
}


String web_Devi_set(float input) {    /* Deviation set & calculation */
  if (input > 380.859375) {
    input = 380.859375;
  }
  if (input < 1.586914) {
    input = 1.586914;
  }

  float deviatn_val;
  int bits;
  int devlast = 0;
  int bitlast = 0;

  for (int DEVIATION_E = 0; DEVIATION_E < 8; DEVIATION_E++) {
    for (int DEVIATION_M = 0; DEVIATION_M < 8; DEVIATION_M++) {
      deviatn_val = (8 + DEVIATION_M) * (pow(2, DEVIATION_E)) * 26000.0 / (pow(2, 17));
      bits = DEVIATION_M + (DEVIATION_E << 4);
      if (input > deviatn_val) {
        devlast = deviatn_val;
        bitlast = bits;
      } else {
        if ((deviatn_val - input) < (input - devlast)) {
          devlast = deviatn_val;
          bitlast = bits;
        }
      }
    }
  }

#ifdef debug
  Serial.print(F("DB web_Devi_set, DEVIATN (15) to ")); Serial.print(bitlast, HEX); Serial.println(F(" (value set to next possible level)"));
#endif
  return String(bitlast, HEX);
}


String web_Marcstate_read() {
  byte reg35 = CC1101_readReg(CC1101_MARCSTATE, READ_BURST);
  switch (reg35) {
    case 0:
      return F("00 = SLEEP");
      break;
    case 1:
      return F("01 = IDLE");
      break;
    case 2:
      return F("02 = XOFF");
      break;
    case 3:
      return F("03 = VCOON_MC");
      break;
    case 4:
      return F("04 = REGON_MC");
      break;
    case 5:
      return F("05 = MANCAL");
      break;
    case 6:
      return F("06 = VCOON");
      break;
    case 7:
      return F("07 = REGON");
      break;
    case 8:
      return F("08 = STARTCAL");
      break;
    case 9:
      return F("09 = BWBOOST");
      break;
    case 10:
      return F("0A = FS_LOCK");
      break;
    case 11:
      return F("0B = IFADCON");
      break;
    case 12:
      return F("0C = ENDCAL");
      break;
    case 13:
      return F("0D = RX");
      break;
    case 14:
      return F("0E = RX_END");
      break;
    case 15:
      return F("0F = RX_RST");
      break;
    case 16:
      return F("10 = TXRX_SWITCH");
      break;
    case 17:
      return F("11 = RXFIFO_OVERFLOW");
      break;
    case 18:
      return F("12 = FSTXON");
      break;
    case 19:
      return F("13 = TX");
      break;
    case 20:
      return F("14 = TX_END");
      break;
    case 21:
      return F("15 = RXTX_SWITCH");
      break;
    case 22:
      return F("16 = TXFIFO_UNDERFLOW");
      break;
    default:
      return F("unknown");
      break;
  }
}


byte web_Mod_set(String input) {
#ifdef debug
  Serial.print(F("DB web_Mod_set, set new value to ")); Serial.println(input);
  Serial.print(F("DB web_Mod_set, MDMCFG2 (12) value is ")); Serial.println(onlyDecToHex2Digit(CC1101_readReg(0x12, READ_BURST)));
#endif

  /* read all split values | example F1 -> 11110001 */
  byte reg12_6_4 = CC1101_readReg(0x12, READ_BURST) & 0x8f ;
  byte output = input.toInt() << 4;
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
  Serial.println(F("-----------------------------------------------------"));
  Serial.println(F("EEPROM read all bytes"));
  int bytesPerRow = 16;
  int i;
  int j = 0;
  byte b;
  char buf[10];
  Serial.print(F("Addr. "));
  for (i = 0; i < bytesPerRow; i++) {
    sprintf(buf, "%02X ", i);
    Serial.print(buf);
  }
  Serial.println();
  Serial.println(F("-----------------------------------------------------"));
  for (i = 0; i < EEPROM_SIZE; i++) {
    if (j == 0) {
      sprintf(buf, "%04X: ", i);
      Serial.print(buf);
    }
    b = EEPROM.read(i);
    sprintf(buf, "%02X ", b);
    j++;
    if (j == bytesPerRow) {
      j = 0;
      Serial.println(buf);
    } else Serial.print(buf);
  }
  Serial.println(F("-----------------------------------------------------"));
}


void EEPROMclear() {
  for (uint16_t i = 0; i < EEPROM_SIZE; EEPROM.write(i++, 255));
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  EEPROM.commit();
#endif
}
