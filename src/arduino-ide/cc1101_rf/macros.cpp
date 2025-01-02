#include "config.h"
#include "macros.h"


/* output DEC to HEX with a leading zero to serial without ln */
void SerialPrintDecToHex(uint8_t dec) {
  if (dec < 16) {
    Serial.print('0');
  }
  Serial.print(dec, HEX);
}


/* --- all SETTINGS for Arduino Nano ---------------------------------------------------------------------------------------------------------- */
#if defined (ARDUINO_AVR_NANO) || defined (ARDUINO_RADINOCC1101) || defined (ARDUINO_AVR_PRO)
/* output DEC to HEX with a leading zero to serial without ln (all dec values < 16 obtain a zero in HEX value) */
void MSG_OUTPUT_DecToHEX_lz(uint8_t output) {
  if (output < 16) {
    Serial.print('0');
  }
  Serial.print(output, HEX);
}

#endif


/* --- all SETTINGS for the ESP8266 and ESP32 ------------------------------------------------------------------------------------------------- */
#if defined (ARDUINO_ARCH_ESP8266) || defined (ARDUINO_ARCH_ESP32)
/* output on the last client with input without ln */
void MSG_OUTPUT(const String & output) {
  if (TELNET_CLIENT_COUNTER == 0 || client_now == 255) {
    Serial.print(output);
  } else if (TELNET_CLIENT_COUNTER != 0) {
    TelnetClient[client_now].print(output);
  }
}

/* output on the last client with input with ln */
//void MSG_OUTPUTLN(String output) {
//  if (TELNET_CLIENT_COUNTER == 0 || client_now == 255) {
//    Serial.println(output);
//  } else if (TELNET_CLIENT_COUNTER != 0) {
//    TelnetClient[client_now].println(output);
//  }
//}

/* output to all interfaces and clients without ln */
void MSG_OUTPUTALL(const String & output) {
  for (uint8_t i = 0; i < TELNET_CLIENTS_MAX; i++) {
    if (TelnetClient[i] && TelnetClient[i].connected()) {
      TelnetClient[i].print(output);
      yield();
      //      delay(1);
    }
  }
  Serial.print(output);
}

/* output to all interfaces and clients with ln */
void MSG_OUTPUTALLLN(const String & output) {
  for (uint8_t i = 0; i < TELNET_CLIENTS_MAX; i++) {
    if (TelnetClient[i] && TelnetClient[i].connected()) {
      TelnetClient[i].println(output);
      yield();
      //      delay(1);
    }
  }
  Serial.println(output);
}

/* output DEC to HEX with a leading zero to the last client with input without ln (all dec values < 16 obtain a zero in HEX value) */
void MSG_OUTPUT_DecToHEX_lz(uint8_t output) {
  if (TELNET_CLIENT_COUNTER == 0 || client_now == 255) {
    if (output < 16) {
      Serial.print('0');
    }
    Serial.print(output, HEX);
  } else if (TELNET_CLIENT_COUNTER != 0) {
    if (output < 16) {
      TelnetClient[client_now].print('0');
    }
    TelnetClient[client_now].print(output, HEX);
  }
}

#endif
