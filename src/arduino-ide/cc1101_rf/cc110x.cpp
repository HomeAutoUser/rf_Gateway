#include "config.h"
#include "cc110x.h"
#include "functions.h"
#include "macros.h"
#include "register.h"


/**********
  macros
 * ********/

#define cc1101_Select() digitalWriteFast(SS, LOW)       // Select (SPI) CC1101
#define cc1101_Deselect() digitalWriteFast(SS, HIGH)    // Deselect (SPI) CC1101
#define wait_Miso() while (digitalRead(MISO) > 0)       // Wait until SPI MISO line goes low

/* output DEC to HEX with a leading zero to serial without ln */
#define SerialPrintDecToHex(dec) \
  { \
    uint8_t output = dec; \
    if (output < 16) { \
      Serial.print('0'); \
    } \
    Serial.print(output, HEX); \
  }


/**********
  variables
 * ********/
byte activated_mode_nr = 0;                     // activated protocol in flash
byte activated_mode_packet_length;
boolean CC1101_found = false;                   // against not clearly defined entrances (if flicker)

/**********
  functions
 * ********/

void CC1101_init() { /* CC1101 - Set default´s */
#ifdef debug_cc110x
  Serial.println(F("DB CC1101_init starting"));
#endif

  pinMode(SS, OUTPUT);
  SPI.begin();
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));  // SCLK frequency, burst access max. 6,5 MHz

  cc1101_Deselect();  // HIGH
  delayMicroseconds(10);
  cc1101_Select();    // LOW
  delayMicroseconds(10);
  cc1101_Deselect();  // Hold CSn high for at least 40 µS ìs relative to pulling CSn low
  delayMicroseconds(40);
  cc1101_Select();                // LOW
  wait_Miso();                    // Wait until SPI MISO line goes low
  CC1101_cmdStrobe(CC1101_SRES);  // Reset CC1101 chip
  delay(10);

  uint8_t chipVersion = CC1101_readReg(CC1101_VERSION, READ_BURST);
  if (chipVersion == 0x14 || chipVersion == 0x04 || chipVersion == 0x03 || chipVersion == 0x05 || chipVersion == 0x07 || chipVersion == 0x17) {  // found CC110x
    CC1101_cmdStrobe(CC1101_SIDLE);                                                                                                              /* Exit RX / TX, turn off frequency synthesizer and exit Wake-On-Radio mode if applicable */
    CC1101_found = true;
#ifdef debug_cc110x
    Serial.println(F("CC110x found"));
    Serial.print(F("CC110x_PARTNUM         ")); Serial.println(CC1101_readReg(CC1101_PARTNUM, READ_BURST), HEX);        // PARTNUM – Chip ID
    Serial.print(F("CC110x_VERSION         ")); Serial.println(chipVersion, HEX);                                       // VERSION – Chip ID
    Serial.print(F("CC110x_FREQEST         ")); Serial.println(CC1101_readReg(CC1101_FREQEST, READ_BURST), HEX);        // FREQEST – Frequency Offset Estimate from Demodulator
    Serial.print(F("CC110x_LQI             ")); Serial.println(CC1101_readReg(CC1101_LQI, READ_BURST), HEX);            // LQI – Demodulator Estimate for Link Quality
    Serial.print(F("CC110x_RSSI            ")); Serial.println(CC1101_readReg(CC1101_RSSI, READ_BURST), HEX);           // RSSI – Received Signal Strength Indication
    Serial.print(F("CC110x_MARCSTATE       ")); Serial.println(CC1101_readReg(CC1101_MARCSTATE, READ_BURST), HEX);      // MARCSTATE – Main Radio Control State Machine State
    Serial.print(F("CC110x_WORTIME1        ")); Serial.println(CC1101_readReg(CC1101_WORTIME1, READ_BURST), HEX);       // WORTIME1 – High Byte of WOR Time
    Serial.print(F("CC110x_WORTIME0        ")); Serial.println(CC1101_readReg(CC1101_WORTIME0, READ_BURST), HEX);       // WORTIME0 – Low Byte of WOR Time
    Serial.print(F("CC110x_PKTSTATUS       ")); Serial.println(CC1101_readReg(CC1101_PKTSTATUS, READ_BURST), HEX);      // PKTSTATUS - Current GDOx Status and Packet Status
    Serial.print(F("CC110x_VCO_VC_DAC      ")); Serial.println(CC1101_readReg(CC1101_VCO_VC_DAC, READ_BURST), HEX);     // VCO_VC_DAC - Current Setting from PLL Calibration Module
    Serial.print(F("CC110x_TXBYTES         ")); Serial.println(CC1101_readReg(CC1101_TXBYTES, READ_BURST), HEX);        // TXBYTES - Underflow and Number of Bytes
    Serial.print(F("CC110x_RXBYTES         ")); Serial.println(CC1101_readReg(CC1101_RXBYTES, READ_BURST), HEX);        // RXBYTES - Underflow and Number of Bytes
    Serial.print(F("CC110x_RCCTRL1_STATUS  ")); Serial.println(CC1101_readReg(CC1101_RCCTRL1_STATUS, READ_BURST), HEX); // CC1101_RCCTRL1_STATUS - Last RC Oscillator Calibration Result
    Serial.print(F("CC110x_RCCTRL0_STATUS  ")); Serial.println(CC1101_readReg(CC1101_RCCTRL0_STATUS, READ_BURST), HEX); // CC1101_RCCTRL0_STATUS - Last RC Oscillator Calibration Result
    Serial.print(F("CC110x_available_modes ")); Serial.println(RegistersCntMax);                                        // Number of compiled register modes
    Serial.print(F("DB CC1101_init, Prog_Ident1 - FW1 = ")); Serial.println(Prog_Ident1);
    Serial.print(F("DB CC1101_init, Prog_Ident2 - FW2 = ")); Serial.println(Prog_Ident2);
#endif

    /* wenn Registerwerte geändert wurden beim compilieren */
#ifdef debug_cc110x
    Serial.print(F("DB CC1101_Init, number of registers = ")); Serial.println(RegistersCntMax);
#endif
    uint16_t chk = 0;
    uint16_t chk_comp = 0;
    for (byte i = 0; i < RegistersCntMax; i++) {
      chk += Registers[i].packet_length;
    }
    EEPROM.get(EEPROM_ADDR_CHK, chk_comp);

    /* normaler Start */
    if (EEPROMread(EEPROM_ADDR_FW1) == Prog_Ident1 && EEPROMread(EEPROM_ADDR_FW2) == Prog_Ident2 && chk == chk_comp) {  // EEPROM OK, chk OK
      /* cc110x - PATABLE */
      uint8_t uiBuffer[8]; // Array anlegen
      for (byte i = 0; i < 8; i++) {
        uiBuffer[i] = EEPROM.read(EEPROM_ADDR_PATABLE + i);
        if (uiBuffer[i] == 255) {  // EEPROM gelöscht
          if (i == 1) {
            uiBuffer[i] = 0x81;  // 5dB Default
          } else {
            uiBuffer[i] = 0x00;
          }
          EEPROMwrite(EEPROM_ADDR_PATABLE + i, uiBuffer[i]);
        }
        CC1101_writeBurstReg(uiBuffer, CC1101_PATABLE, 8);
      }
      EEPROM.get(EEPROM_ADDR_AFC, freqAfc);         /* cc110x - afc from EEPROM */
      EEPROM.get(EEPROM_ADDR_FOFFSET, Freq_offset); /* cc110x - freq offset from EEPROM */
      if (Freq_offset > 10.0 || Freq_offset < -10.0 || EEPROM.read(EEPROM_ADDR_FOFFSET) == 0xff) {
        Freq_offset = 0;
        EEPROM.put(EEPROM_ADDR_FOFFSET, Freq_offset);
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
        EEPROM.commit();
#endif
      }
      activated_mode_nr = EEPROMread(EEPROM_ADDR_Prot);
#ifdef debug_cc110x
      Serial.print(F("CC110x_Freq.Offset     ")); Serial.print(Freq_offset, 3); Serial.println(F(" MHz"));
      Serial.print(F("DB CC1101_init, EEPROM read last activated mode ")); Serial.println(activated_mode_nr);
#endif
      ToggleTime = EEPROMread_long(EEPROM_ADDR_Toggle);
      if (ToggleTime > ToggleTimeMax) {
        ToggleTime = 0;
        EEPROMwrite_long(EEPROM_ADDR_Toggle, ToggleTime);
      }
#ifdef debug_cc110x
      Serial.print(F("DB CC1101_init, EEPROM read toggletime ")); Serial.println(ToggleTime);
#endif
      if (activated_mode_nr > 0 && ToggleTime == 0) {
#ifdef debug_cc110x
        activated_mode_name = Registers[activated_mode_nr].name;
        Serial.println(F("DB CC1101_Init, use CC1101 config from EEPROM"));
        Serial.print(F("DB CC1101_Init, write activated_mode_nr ")); Serial.print(activated_mode_nr);
        Serial.print(F(", ")); Serial.println(activated_mode_name);
#endif
        /* CC1101 - configure CC1101 registers from flash addresses */
        for (byte i = 0; i < 41; i++) {
#ifdef debug_cc110x
          Serial.print(F("DB CC1101_init, EEPROM   0x")); SerialPrintDecToHex(i); Serial.println("");
#endif
          CC1101_writeReg(i, EEPROMread(i));
        }
      }
      if (ToggleTime > 0) {
        ToggleValues = 0; /* counting Toggle values ​​and sorting into array */
        for (byte i = 0; i < 4; i++) {
          if (EEPROMread(i + EEPROM_ADDR_ProtTo) != 255) {
            ToggleValues++;
            ToggleOrder[ToggleValues - 1] = EEPROMread(i + EEPROM_ADDR_ProtTo);
            ToggleArray[i] = ToggleOrder[ToggleValues - 1];
            if (ToggleValues == 1) {
              activated_mode_nr = ToggleOrder[ToggleValues - 1];
            }
#ifdef debug_cc110x
            Serial.print(F("DB CC1101_init, EEPROM read toggle value ")); Serial.print(ToggleValues);
            Serial.print(F(" (")); Serial.print(ToggleOrder[ToggleValues - 1]);
            Serial.print(F(") and put it in array at ")); Serial.println(ToggleValues - 1);
#endif
          }
        }
        CC1101_writeRegFor(Registers[activated_mode_nr].reg_val, Registers[activated_mode_nr].length, Registers[activated_mode_nr].name);
      }
      activated_mode_name = Registers[activated_mode_nr].name;
      activated_mode_packet_length = Registers[activated_mode_nr].packet_length;
      // Serial.println(CC1101_readReg(CC1101_MARCSTATE, READ_BURST), HEX); // MARCSTATE – Main Radio Control State Machine State (1 = Idle)
      // CC1101_cmdStrobe(CC1101_SIDLE);                                    // Exit RX / TX, turn off frequency synthesizer and exit Wake-On-Radio mode if applicable
      CC1101_cmdStrobe(CC1101_SFRX);                                        // Flush the RX FIFO buffer. Only issue SFRX in IDLE or RXFIFO_OVERFLOW states
      CC1101_cmdStrobe(CC1101_SRX);                                         // Enable RX. Perform calibration first if coming from IDLE and MCSM0.FS_AUTOCAL=1
      for (uint8_t i = 0; i < 255; i++) {
        if (CC1101_readReg(CC1101_MARCSTATE, READ_BURST) == 0x0D) {         // RX
          break;
        }
        delay(1);
#ifdef debug_cc110x
        if (i == 254) {
          Serial.println("CC1101_init, ERROR read CC1101_MARCSTATE, READ_BURST !");
        }
#endif
      }
      // Serial.println(CC1101_readReg(CC1101_MARCSTATE, READ_BURST), HEX); // MARCSTATE – Main Radio Control State Machine State (1 = Idle)
    } else { /* Ende normaler Start */
      /* ERROR EEPROM oder Registeranzahl geändert */
      EEPROMwrite(EEPROM_ADDR_Prot, 0);  // reset
      EEPROMwrite_long(EEPROM_ADDR_Toggle, 0);
      ToggleTime = 0;
      ToggleValues = 0;
      activated_mode_nr = 0;
      activated_mode_packet_length = 0;
      /* wenn Registerwerte geändert wurden beim compilieren */
      if (chk != chk_comp) {  // checksum over Registers[].packet_length not OK
        Serial.println(F("CC1101_Init, reset togglebank and stop receiving (chk != chk registers.packet_length)"));
        EEPROM.put(EEPROM_ADDR_CHK, chk);  // reset
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
        EEPROM.commit();
#endif
      }
      /* EEPROM wurde gelöscht */
      if (EEPROMread(EEPROM_ADDR_FW1) != Prog_Ident1 || EEPROMread(EEPROM_ADDR_FW2) != Prog_Ident2) {  // EEPROM not OK
        Serial.println(F("CC1101_Init, EEPROM Init to defaults after ERROR"));
        EEPROMwrite(EEPROM_ADDR_FW1, Prog_Ident1);  // reset Prog_Ident1
        EEPROMwrite(EEPROM_ADDR_FW2, Prog_Ident2);  // reset Prog_Ident2
        EEPROMwrite_long(EEPROM_ADDR_Toggle, 0);    // reset Toggle time
        /* set adr. 48 - 52 to value 0 (active protocol and toggle array) */
        for (byte i = EEPROM_ADDR_Prot; i < (EEPROM_ADDR_ProtTo + 4); i++) {
          if (i == EEPROM_ADDR_Prot) {
            EEPROMwrite(i, 0);    // activated_mode_nr
          } else {
            EEPROMwrite(i, 255);  // ToggleProtocols
          }
        }
#ifdef debug_cc110x
        Serial.println(F("DB CC1101_init, reconfigure CC1101 and write values to EEPROM"));
#endif
        for (byte i = 0; i < Registers[0].length; i++) {                 /* write register values ​​to flash */
          EEPROMwrite(i, pgm_read_byte_near(Registers[0].reg_val + i));  // Config_Default
        }
      }  // Ende EEPROM wurde gelöscht

      /* set cc110x to factory settings */
      CC1101_writeRegFor(Registers[0].reg_val, Registers[0].length, Registers[0].name);
      activated_mode_name = Registers[0].name;
      activated_mode_packet_length = Registers[0].packet_length;
#ifdef debug_cc110x
      Serial.println(F("DB CC1101_init, set cc110x to factory settings"));
      Serial.print(F("DB CC1101_init, activated_mode_nr = "));
      Serial.print(activated_mode_nr); Serial.print(F(", ")); Serial.println(activated_mode_name);
      Serial.println(F("CC1101 remains idle, please set receive mode!"));
#endif
      // Serial.println(CC1101_readReg(CC1101_MARCSTATE, READ_BURST), HEX); // MARCSTATE – Main Radio Control State Machine State (1 = Idle)
    }      // Ende ERROR EEPROM oder Registeranzahl geändert
  } else { /* NO CC1101 found */
    CC1101_found = false;
#ifdef debug_cc110x
    Serial.println(F("DB CC1101_init, CC110x NOT recognized"));
#endif
  }
}


void CC1101_writeReg(uint8_t regAddr, uint8_t value) {
  /*
    Write single register into the CC1101 IC via SPI

    'regAddr' Register address
    'value'   Value to be writen
  */

#ifdef debug_cc110x
  Serial.print(F("DB CC1101_writeReg       0x")); SerialPrintDecToHex(regAddr);
  Serial.print(F(" 0x")); SerialPrintDecToHex(value); Serial.println("");
#endif

  cc1101_Select();        // Select CC1101
  wait_Miso();            // Wait until MISO goes low
  SPI.transfer(regAddr);  // Send register address
  SPI.transfer(value);    // Send value
  cc1101_Deselect();      // Deselect CC1101
}


void CC1101_writeRegFor(const uint8_t *reg_name, uint8_t reg_length, String reg_modus) {
  /*
    variable loop to write register

    'reg_name'    Register name from register.cpp
    'reg_length'  Register name length from register.cpp
    'reg_modus'   Text for Modus from simplification.h
  */

  freqOffAcc = 0;                      // reset cc110x afc offset
  freqErrAvg = 0;                      // reset cc110x afc average
  CC1101_writeReg(CC1101_FSCTRL0, 0);  // reset Register 0x0C: FSCTRL0 – Frequency Synthesizer Control

#ifdef debug_cc110x
  Serial.print(F("DB CC1101_writeRegFor length = ")); Serial.println(reg_length);
  Serial.print(F("DB CC1101_writeRegFor modus  = ")); Serial.println(reg_modus);
#endif

  for (byte i = 0; i < reg_length; i++) {
    CC1101_writeReg(i, pgm_read_byte_near(reg_name + i)); /* write value to cc110x */
#ifdef debug_cc110x
    Serial.print(F("DB CC1101_writeRegFor, i=")); Serial.print(i);
    Serial.print(F(" read ")); Serial.print(CC1101_readReg(i, READ_BURST));
    Serial.print(F(" (0x")); SerialPrintDecToHex(CC1101_readReg(i, READ_BURST));
    Serial.println(F(") from CC1101"));
#endif

    if (i == 15 && Freq_offset != 0.00) { /* 0D 0E 0F - attention to the frequency offset !!! */

      String ret = web_Freq_set(
                     String(web_Freq_read(CC1101_readReg(13, READ_BURST),
                                          CC1101_readReg(14, READ_BURST),
                                          CC1101_readReg(15, READ_BURST)),
                            3));

#ifdef debug_cc110x
      Serial.println(F("CC110x_Freq.Offset mod register 0x0D 0x0E 0x0F"));
      Serial.print(F("CC110x_Freq.Offset calculated "));
      Serial.print(String(web_Freq_read(CC1101_readReg(13, READ_BURST),
                                        CC1101_readReg(14, READ_BURST),
                                        CC1101_readReg(15, READ_BURST))
                          + Freq_offset,
                          3));
      Serial.println(F(" MHz an write new value"));
#endif
      for (byte i2 = 0; i2 < 3; i2++) { /* write value to register 0D,0E,0F */
        byte val = hexToDec(ret.substring(i2 * 2, i2 * 2 + 2));
        if (pgm_read_byte_near(reg_name + i2 + 13) != val) {
          CC1101_writeReg(i2 + 13, val);  // write in cc1101
        }
      }
    }
  }
  activated_mode_name = reg_modus;
}


uint8_t CC1101_readReg(uint8_t regAddr, uint8_t regType) {
  /*
    Read CC1101 register via SPI

    'regAddr'  Register address
    'regType'  Type of register: CC1101_CONFIG_REGISTER or CC1101_STATUS_REGISTER

    Return: Data byte returned by the CC1101 IC
  */
  byte addr, val;

  addr = regAddr | regType;
  cc1101_Select();           // Select CC1101
  wait_Miso();               // Wait until MISO goes low
  SPI.transfer(addr);        // Send register address
  val = SPI.transfer(0x00);  // Read result
  cc1101_Deselect();         // Deselect CC1101

  return val;
}


void CC1101_setTransmitMode() {
  CC1101_cmdStrobe(CC1101_SIDLE);  // Idle mode 0x36
  CC1101_cmdStrobe(CC1101_SFTX);
  CC1101_cmdStrobe(CC1101_SIDLE);  // Idle mode 0x36
  delay(1);

  uint8_t maxloop = 0xff;
  while (maxloop-- && (CC1101_cmdStrobe(CC1101_STX) & CC1101_STATUS_STATE_BM) != CC1101_STATE_TX)  // TX enable
    delay(1);
  if (maxloop == 0) {
    Serial.println(F("ERROR - Setting TX failed"));
    return;
  }
  return;
}


void CC1101_setReceiveMode() {
  CC1101_cmdStrobe(CC1101_SIDLE);  // Idle mode

  uint8_t maxloop = 0xff;
  while (maxloop-- && (CC1101_cmdStrobe(CC1101_SRX) & CC1101_STATUS_STATE_BM) != CC1101_STATE_RX)  // RX enable
    delay(1);
  if (maxloop == 0) {
    Serial.println(F("ERROR - Setting RX failed"));
  }
}


uint8_t CC1101_cmdStrobe(byte cmd) {
  /* Send command strobe to the CC1101 IC via SPI
     'cmd'  Command strobe  */

#ifdef debug_cc110x
  Serial.print(F("DB CC1101_cmdStrobe ")); SerialPrintDecToHex(cmd); Serial.println("");
#endif

  cc1101_Select();                  // Select CC1101
  wait_Miso();                      // Wait until MISO goes low
  uint8_t ret = SPI.transfer(cmd);  // Send strobe command
  cc1101_Deselect();                // Deselect CC1101
  return ret;                       // Chip Status Byte
}


void CC1101_readBurstReg(byte *uiBuffer, byte regAddr, byte len) {
  /* Read burst data from CC1101 via SPI

    'uiBuffer'  Buffer where to copy the result to
    'regAddr'  Register address
    'len'  Data length  */

  byte addr, i;

  addr = regAddr | READ_BURST;
  cc1101_Select();     // Select CC1101
  wait_Miso();         // Wait until MISO goes low
  SPI.transfer(addr);  // Send register address
  for (i = 0; i < len; i++)
    uiBuffer[i] = SPI.transfer(0x00);  // Read result byte by byte
  cc1101_Deselect();                 // Deselect CC1101
}


void CC1101_writeBurstReg(byte *uiBuffer, byte regAddr, byte len) {
  /* Write burst data from CC1101 via SPI

     'uiBuffer'  Buffer where to copy the result to
     'regAddr'  Register address
     'len'  Data length  */

  byte addr, i;

  addr = regAddr | WRITE_BURST;
  cc1101_Select();            // Select CC1101
  wait_Miso();                // Wait until MISO goes low
  SPI.transfer(addr);         // Send register address
  for (i = 0; i < len; i++)
    SPI.transfer(uiBuffer[i]);  // Write result byte by byte
  cc1101_Deselect();          // Deselect CC1101
}


int CC1101_readRSSI() {                                        /* Read RSSI value from Register */
  uint8_t RSSI_raw = CC1101_readReg(CC1101_RSSI, READ_BURST);  // not converted
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32) || !defined(SIGNALduino_comp)
  if (RSSI_raw >= 128) {
    RSSI_dez = (RSSI_raw - 256) / 2 - 74;
  } else if (RSSI_raw < 128) {
    RSSI_dez = (RSSI_raw / 2) - 74;
  }
#endif

#ifdef SIGNALduino_comp /* for FHEM */
  return RSSI_raw;  // not converted
#else
  return RSSI_dez;  // calculated - Telnet ???
#endif
}


void CC1101_sendFIFO(char *data) {
#ifdef debug_cc110x
  Serial.println(F("DB CC1101_sendFIFO"));
#endif

  cc1101_Select();
  SPI.transfer(CC1101_TXFIFO | WRITE_BURST);

  uint8_t val;
  for (uint8_t i = 0; i < strlen(data); i += 2) { /* start 2 -> in data D=1234 - besser D= vorher entfernen, Handhabung in Website dann einfacher */
    val = hex2int(data[i]) * 16;
    val += hex2int(data[i + 1]);
#ifdef debug_cc110x
    Serial.print(data[i]); Serial.print(data[i + 1]); Serial.print(' '); Serial.println(val);
#endif
    SPI.transfer(val);  // fill FIFO with data
  }

  cc1101_Deselect();

  for (uint8_t i = 0; i < 200; ++i) {
    if (CC1101_readReg(CC1101_MARCSTATE, READ_BURST) != 0x13)
      break;            // neither in RX nor TX, probably some error
    delay(1);
  }
}
