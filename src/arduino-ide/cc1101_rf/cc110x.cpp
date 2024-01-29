#include "config.h"

#ifdef CC110x

#include "cc110x.h"
#include "functions.h"
#include "macros.h"
#include "cc110x_register.h"


/*******************
  macros for CC110x
 *******************/

#define wait_Miso() while (digitalRead(MISO) > 0)       // Wait until SPI MISO line goes low

/**********************
  variables for CC110x
 **********************/
byte ReceiveModeNr = 0;                     // activated protocol in flash
byte ReceiveModePKTLEN;
boolean ChipFound = false;                  // against not clearly defined entrances (if flicker)


/***********
  functions
 ***********/
void ChipInit() { /* Init CC110x - Set default´s */
#ifdef debug_chip
  Serial.println(F("[DB] CC110x_init starting"));
#endif

  pinMode(SS, OUTPUT);
  SPI.begin();
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));  // SCLK frequency, burst access max. 6,5 MHz

  ChipDeselect();  // HIGH
  delayMicroseconds(10);
  ChipSelect();    // LOW
  delayMicroseconds(10);
  ChipDeselect();  // Hold CSn high for at least 40 µS ìs relative to pulling CSn low
  delayMicroseconds(40);
  ChipSelect();                // LOW
  wait_Miso();                    // Wait until SPI MISO line goes low
  CC110x_CmdStrobe(CC110x_SRES);  // Reset CC110x chip
  delay(10);

  uint8_t chipVersion = Chip_readReg(CHIP_VERSION, READ_BURST);
  if (chipVersion == 0x14 || chipVersion == 0x04 || chipVersion == 0x03 || chipVersion == 0x05 || chipVersion == 0x07 || chipVersion == 0x17) {  // found CC110x
    CC110x_CmdStrobe(CC110x_SIDLE);                                                                                                              /* Exit RX / TX, turn off frequency synthesizer and exit Wake-On-Radio mode if applicable */
    ChipFound = true;
#ifdef debug_chip
    Serial.print(F("[DB] CC110x_PARTNUM                  ")); Serial.println(Chip_readReg(CC110x_PARTNUM, READ_BURST), HEX);        // PARTNUM – Chip ID
    Serial.print(F("[DB] CHIP_VERSION                    ")); Serial.println(chipVersion, HEX);                                       // VERSION – Chip ID
    // Serial.print(F("[DB] CC110x_FREQEST                  ")); Serial.println(Chip_readReg(CC110x_FREQEST, READ_BURST), HEX);        // FREQEST – Frequency Offset Estimate from Demodulator
    // Serial.print(F("[DB] CC110x_LQI                      ")); Serial.println(Chip_readReg(CC110x_LQI, READ_BURST), HEX);            // LQI – Demodulator Estimate for Link Quality
    // Serial.print(F("[DB] CC110x_RSSI                     ")); Serial.println(Chip_readReg(CC110x_RSSI, READ_BURST), HEX);           // RSSI – Received Signal Strength Indication
    Serial.print(F("[DB] CC110x_MARCSTATE                ")); Serial.println(Chip_readReg(CC110x_MARCSTATE, READ_BURST), HEX);      // MARCSTATE – Main Radio Control State Machine State
    // Serial.print(F("[DB] CC110x_WORTIME1                 ")); Serial.println(Chip_readReg(CC110x_WORTIME1, READ_BURST), HEX);       // WORTIME1 – High Byte of WOR Time
    // Serial.print(F("[DB] CC110x_WORTIME0                 ")); Serial.println(Chip_readReg(CC110x_WORTIME0, READ_BURST), HEX);       // WORTIME0 – Low Byte of WOR Time
    // Serial.print(F("[DB] CC110x_PKTSTATUS                ")); Serial.println(Chip_readReg(CC110x_PKTSTATUS, READ_BURST), HEX);      // PKTSTATUS - Current GDOx Status and Packet Status
    Serial.print(F("[DB] CC110x_VCO_VC_DAC               ")); Serial.println(Chip_readReg(CC110x_VCO_VC_DAC, READ_BURST), HEX);     // VCO_VC_DAC - Current Setting from PLL Calibration Module
    // Serial.print(F("[DB] CC110x_TXBYTES                  ")); Serial.println(Chip_readReg(CC110x_TXBYTES, READ_BURST), HEX);        // TXBYTES - Underflow and Number of Bytes
    // Serial.print(F("[DB] CC110x_RXBYTES                  ")); Serial.println(Chip_readReg(CC110x_RXBYTES, READ_BURST), HEX);        // RXBYTES - Underflow and Number of Bytes
    // Serial.print(F("[DB] CC110x_RCCTRL1_STATUS           ")); Serial.println(Chip_readReg(CC110x_RCCTRL1_STATUS, READ_BURST), HEX); // CC110x_RCCTRL1_STATUS - Last RC Oscillator Calibration Result
    // Serial.print(F("[DB] CC110x_RCCTRL0_STATUS           ")); Serial.println(Chip_readReg(CC110x_RCCTRL0_STATUS, READ_BURST), HEX); // CC110x_RCCTRL0_STATUS - Last RC Oscillator Calibration Result
    Serial.print(F("[DB] CC110x_available_modes          ")); Serial.println(RegistersMaxCnt);                                        // Number of compiled register modes
    Serial.print(F("[DB] Prog_Ident Firmware1            ")); Serial.println(Prog_Ident1);
    Serial.print(F("[DB] Prog_Ident Firmware2            ")); Serial.println(Prog_Ident2);
#endif

    /* wenn Registerwerte geändert wurden beim compilieren */
    uint16_t chk = 0;
    uint16_t chk_comp = 0;
    for (byte i = 0; i < RegistersMaxCnt; i++) {
      chk += Registers[i].PKTLEN;
    }
    EEPROM.get(EEPROM_ADDR_CHK, chk_comp);

    /* normaler Start */
    if (EEPROMread(EEPROM_ADDR_FW1) == Prog_Ident1 && EEPROMread(EEPROM_ADDR_FW2) == Prog_Ident2 && chk == chk_comp) {  // EEPROM OK, chk OK
      /* cc110x - PATABLE */
      uint8_t uiBuffer[8]; // Array anlegen
      for (byte i = 0; i < 8; i++) {
        uiBuffer[i] = EEPROM.read(EEPROM_ADDR_PATABLE + i);
        if (uiBuffer[i] == 255) {   // EEPROM gelöscht
          if (i == 1) {
            uiBuffer[i] = 0x81;     // 5dB Default
          } else {
            uiBuffer[i] = 0x00;
          }
          EEPROMwrite(EEPROM_ADDR_PATABLE + i, uiBuffer[i]);
        }
        CC110x_writeBurstReg(uiBuffer, CC110x_PATABLE, 8);
      }
      EEPROM.get(EEPROM_ADDR_AFC, freqAfc);         /* cc110x - afc from EEPROM */
      if (freqAfc > 1) {
        freqAfc = 0;
        EEPROM.put(EEPROM_ADDR_AFC, freqAfc);
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
        EEPROM.commit();
#endif
      }
      EEPROM.get(EEPROM_ADDR_FOFFSET, Freq_offset); /* cc110x - freq offset from EEPROM */
      if (Freq_offset > 10.0 || Freq_offset < -10.0 || EEPROM.read(EEPROM_ADDR_FOFFSET) == 0xff) {
        Freq_offset = 0;
        EEPROM.put(EEPROM_ADDR_FOFFSET, Freq_offset);
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
        EEPROM.commit();
#endif
      }
      ReceiveModeNr = EEPROMread(EEPROM_ADDR_Prot);
#ifdef debug_chip
      Serial.print(F("[DB] CC110x_Freq.Offset              ")); Serial.print(Freq_offset, 3); Serial.println(F(" MHz"));
      Serial.print(F("[DB] read activated mode from EEPROM ")); Serial.println(ReceiveModeNr);
#endif
      ToggleTime = EEPROMread_long(EEPROM_ADDR_Toggle);
      if (ToggleTime > ToggleTimeMax) {
        ToggleTime = 0;
        EEPROMwrite_long(EEPROM_ADDR_Toggle, ToggleTime);
      }
#ifdef debug_chip
      Serial.print(F("[DB] read toggletime from EEPROM     ")); Serial.println(ToggleTime);
#endif
      if (ReceiveModeNr > 0 && ToggleTime == 0) {
#ifdef debug_chip
        ReceiveModeName = Registers[ReceiveModeNr].name;
        Serial.println(F("[DB] CC110x use config from EEPROM"));
        Serial.print(F("[DB] write ReceiveModeNr ")); Serial.print(ReceiveModeNr);
        Serial.print(F(", ")); Serial.println(ReceiveModeName);
#endif
        /* CC110x - configure CC110x registers from flash addresses */
        for (byte i = 0; i < 41; i++) {
#ifdef debug_chip
          Serial.print(F("[DB] CC110x_init, EEPROM   0x")); SerialPrintDecToHex(i); Serial.println("");
#endif
          Chip_writeReg(i, EEPROMread(i));
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
              ReceiveModeNr = ToggleOrder[ToggleValues - 1];
            }
#ifdef debug_chip
            Serial.print(F("[DB] ChipInit, EEPROM read toggle value ")); Serial.print(ToggleValues);
            Serial.print(F(" (")); Serial.print(ToggleOrder[ToggleValues - 1]);
            Serial.print(F(") and put it in array at ")); Serial.println(ToggleValues - 1);
#endif
          }
        }
        Chip_writeRegFor(Registers[ReceiveModeNr].reg_val, Registers[ReceiveModeNr].length, Registers[ReceiveModeNr].name);
      }
      ReceiveModeName = Registers[ReceiveModeNr].name;
      ReceiveModePKTLEN = Registers[ReceiveModeNr].PKTLEN;
      // Serial.println(Chip_readReg(CC110x_MARCSTATE, READ_BURST), HEX); // MARCSTATE – Main Radio Control State Machine State (1 = Idle)
#ifdef debug_chip
      Serial.print(F("[DB] CC110x_Frequency              ")); Serial.print(Chip_readFreq() / 1000, 3); Serial.println(F(" MHz"));
#endif
      Chip_setReceiveMode(); /* enable RX */
      // Serial.println(Chip_readReg(CC110x_MARCSTATE, READ_BURST), HEX); // MARCSTATE – Main Radio Control State Machine State (1 = Idle)
    } else { /* Ende normaler Start */
      /* ERROR EEPROM oder Registeranzahl geändert */
      EEPROMwrite(EEPROM_ADDR_Prot, 0);  // reset
      EEPROMwrite_long(EEPROM_ADDR_Toggle, 0);
      ToggleTime = 0;
      ToggleValues = 0;
      ReceiveModeNr = 0;
      ReceiveModePKTLEN = 0;
      /* wenn Registerwerte geändert wurden beim compilieren */
      if (chk != chk_comp) {  // checksum over Registers[].PKTLEN not OK
        Serial.println(F("ChipInit, reset togglebank and stop receiving (chk != chk registers.PKTLEN)"));
        EEPROM.put(EEPROM_ADDR_CHK, chk);  // reset
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
        EEPROM.commit();
#endif
      }
      /* EEPROM wurde gelöscht */
      if (EEPROMread(EEPROM_ADDR_FW1) != Prog_Ident1 || EEPROMread(EEPROM_ADDR_FW2) != Prog_Ident2) {  // EEPROM not OK
        Serial.println(F("ChipInit, EEPROM Init to defaults after ERROR"));
        EEPROMwrite(EEPROM_ADDR_FW1, Prog_Ident1);  // reset Prog_Ident1
        EEPROMwrite(EEPROM_ADDR_FW2, Prog_Ident2);  // reset Prog_Ident2
        EEPROMwrite_long(EEPROM_ADDR_Toggle, 0);    // reset Toggle time
        /* set adr. 48 - 52 to value 0 (active protocol and toggle array) */
        for (byte i = EEPROM_ADDR_Prot; i < (EEPROM_ADDR_ProtTo + 4); i++) {
          if (i == EEPROM_ADDR_Prot) {
            EEPROMwrite(i, 0);    // ReceiveModeNr
          } else {
            EEPROMwrite(i, 255);  // ToggleProtocols
          }
        }
#ifdef debug_chip
        Serial.println(F("[DB] reconfigure CC110x and write values to EEPROM"));
#endif
        for (byte i = 0; i < Registers[0].length; i++) {                 /* write register values ​​to flash */
          EEPROMwrite(i, pgm_read_byte_near(Registers[0].reg_val + i));  // Config_Default
        }
      }  // Ende EEPROM wurde gelöscht

      /* set cc110x to factory settings */
      Chip_writeRegFor(Registers[0].reg_val, Registers[0].length, Registers[0].name);
      ReceiveModeName = Registers[0].name;
      ReceiveModePKTLEN = Registers[0].PKTLEN;
#ifdef debug_chip
      Serial.println(F("[DB] set Chip to factory settings"));
      Serial.print(F("[DB] ReceiveModeNr = "));
      Serial.print(ReceiveModeNr); Serial.print(F(", ")); Serial.println(ReceiveModeName);
      Serial.println(F("[DB] Chip remains idle, please set receive mode!"));
#endif
      // Serial.println(Chip_readReg(CC110x_MARCSTATE, READ_BURST), HEX); // MARCSTATE – Main Radio Control State Machine State (1 = Idle)
    }      // Ende ERROR EEPROM oder Registeranzahl geändert
  } else { /* NO CC110x found */
    ChipFound = false;
#ifdef debug_chip
    Serial.println(F("[DB] Chip NOT recognized"));
#endif
  }
}


void Chip_writeReg(uint8_t regAddr, uint8_t value) {
  /*
    Write single register into the CC110x IC via SPI

    'regAddr' Register address
    'value'   Value to be writen
  */

#ifdef debug_chip
  Serial.print(F("[DB] Chip_writeReg       0x")); SerialPrintDecToHex(regAddr);
  Serial.print(F(" 0x")); SerialPrintDecToHex(value); Serial.println("");
#endif

  ChipSelect();           // Select CC110x
  wait_Miso();            // Wait until MISO goes low
  SPI.transfer(regAddr);  // Send register address
  SPI.transfer(value);    // Send value
  ChipDeselect();         // Deselect CC110x
}


void Chip_writeRegFor(const uint8_t *reg_name, uint8_t reg_length, String reg_modus) {
  /*
    variable loop to write register

    'reg_name'    Register name from register.cpp
    'reg_length'  Register name length from register.cpp
    'reg_modus'   Text for Modus from simplification.h
  */

  freqOffAcc = 0;                      // reset cc110x afc offset
  freqErrAvg = 0;                      // reset cc110x afc average
  Chip_writeReg(CC110x_FSCTRL0, 0);  // reset Register 0x0C: FSCTRL0 – Frequency Synthesizer Control

#ifdef debug_chip
  Serial.print(F("[DB] Chip_writeRegFor length = ")); Serial.println(reg_length);
  Serial.print(F("[DB] Chip_writeRegFor modus  = ")); Serial.println(reg_modus);
#endif

  for (byte i = 0; i < reg_length; i++) {
    Chip_writeReg(i, pgm_read_byte_near(reg_name + i)); /* write value to cc110x */
#ifdef debug_chip
    Serial.print(F("[DB] Chip_writeRegFor, i=")); Serial.print(i);
    Serial.print(F(" read ")); Serial.print(Chip_readReg(i, READ_BURST));
    Serial.print(F(" (0x")); SerialPrintDecToHex(Chip_readReg(i, READ_BURST));
    Serial.println(F(") from CC110x"));
#endif
    if (i == 15 && Freq_offset != 0.00) { /* 0D 0E 0F - attention to the frequency offset !!! */
      byte value[3];
      Chip_setFreq(Chip_readFreq(), value);
#ifdef debug_chip
      Serial.print(F("[DB] CC110x_Freq.Offset calculated "));
      Serial.print(((Chip_readFreq() + Freq_offset * 1000) / 1000), 3);
      Serial.println(F(" MHz an write new value"));
#endif
      for (byte i2 = 0; i2 < 3; i2++) { // write value to frequency register 0x07, 0x08, 0x09
        if (pgm_read_byte_near(reg_name + i2 + 13) != value[i2]) {
          Chip_writeReg(i2 + 13, value[i2]);  // write new values in SX1231
        }
      }
    }
  }
  ReceiveModeName = reg_modus;
}


uint8_t Chip_readReg(uint8_t regAddr, uint8_t regType) {
  /*
    Read CC110x register via SPI

    'regAddr'  Register address
    'regType'  Type of register: CC110x_CONFIG_REGISTER or CC110x_STATUS_REGISTER

    Return: Data byte returned by the CC110x IC
  */
  byte addr, val;

  addr = regAddr | regType;
  ChipSelect();             // Select CC110x
  wait_Miso();              // Wait until MISO goes low
  SPI.transfer(addr);       // Send register address
  val = SPI.transfer(0x00); // Read result
  ChipDeselect();           // Deselect CC110x

  return val;
}


void CC110x_setTransmitMode() {
  CC110x_CmdStrobe(CC110x_SIDLE);  // Idle mode 0x36
  CC110x_CmdStrobe(CC110x_SFTX);
  CC110x_CmdStrobe(CC110x_SIDLE);  // Idle mode 0x36
  delay(1);

  uint8_t maxloop = 0xff;
  while (maxloop-- && (CC110x_CmdStrobe(CC110x_STX) & CC110x_STATUS_STATE_BM) != CC110x_STATE_TX)  // TX enable
    delay(1);
  if (maxloop == 0) {
    Serial.println(F("ERROR - Setting TX failed"));
    return;
  }
  return;
}


void Chip_setReceiveMode() {
  CC110x_CmdStrobe(CC110x_SIDLE);  // Idle mode
  if (Chip_readReg(CC110x_MARCSTATE, READ_BURST) == 0x11) {   // RXFIFO_OVERFLOW
    CC110x_CmdStrobe(CC110x_SFRX);
  }
  uint8_t maxloop = 0xff;
  while (maxloop-- && (CC110x_CmdStrobe(CC110x_SRX) & CC110x_STATUS_STATE_BM) != CC110x_STATE_RX)  // RX enable
    delay(1);
  if (maxloop == 0) {
    Serial.println(F("ERROR - Setting RX failed"));
  }
}

uint8_t CC110x_CmdStrobe(byte cmd) {
  /* Send command strobe to the CC110x IC via SPI
     'cmd'  Command strobe  */

#ifdef debug_chip
  Serial.print(F("[DB] CC110x_cmdStrobe ")); SerialPrintDecToHex(cmd); Serial.println("");
#endif

  ChipSelect();                     // Select CC110x
  wait_Miso();                      // Wait until MISO goes low
  uint8_t ret = SPI.transfer(cmd);  // Send strobe command
  ChipDeselect();                   // Deselect CC110x
  return ret;                       // Chip Status Byte
}


void Chip_readBurstReg(byte *uiBuffer, byte regAddr, byte len) {
  /* Read burst data from CC110x via SPI

    'uiBuffer'  Buffer where to copy the result to
    'regAddr'  Register address
    'len'  Data length  */

  byte addr, i;

  addr = regAddr | READ_BURST;
  ChipSelect();                       // Select CC110x
  wait_Miso();                        // Wait until MISO goes low
  SPI.transfer(addr);                 // Send register address
  for (i = 0; i < len; i++)
    uiBuffer[i] = SPI.transfer(0x00); // Read result byte by byte
  ChipDeselect();                     // Deselect CC110x
}


void CC110x_writeBurstReg(byte *uiBuffer, byte regAddr, byte len) {
  /* Write burst data from CC110x via SPI

     'uiBuffer'  Buffer where to copy the result to
     'regAddr'  Register address
     'len'  Data length  */

  byte addr, i;

  addr = regAddr | WRITE_BURST;
  ChipSelect();                 // Select CC110x
  wait_Miso();                  // Wait until MISO goes low
  SPI.transfer(addr);           // Send register address
  for (i = 0; i < len; i++)
    SPI.transfer(uiBuffer[i]);  // Write result byte by byte
  ChipDeselect();               // Deselect CC110x
}


int Chip_readRSSI() {                                        /* Read RSSI value from Register */
  uint8_t RSSI_raw = Chip_readReg(CC110x_RSSI, READ_BURST);  // not converted
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


void Chip_sendFIFO(char *data) {
  CC110x_setTransmitMode();     // enable TX
#ifdef debug_chip
  Serial.println(F("[DB] Chip_sendFIFO"));
#endif

  ChipSelect();
  SPI.transfer(CC110x_TXFIFO | WRITE_BURST);

  uint8_t val;
  for (uint8_t i = 0; i < strlen(data); i += 2) {
    val = hex2int(data[i]) * 16;
    val += hex2int(data[i + 1]);
    SPI.transfer(val);  // fill FIFO with data
  }

  ChipDeselect();

  for (uint8_t i = 0; i < 200; ++i) {
    if (Chip_readReg(CC110x_MARCSTATE, READ_BURST) != 0x13)
      break;            // neither in RX nor TX, probably some error
    delay(1);
  }
}


float Chip_readFreq() {
  float Freq;
  Freq = Chip_readReg(0x0D, READ_BURST) * 256;
  Freq = (Freq + Chip_readReg(0x0E, READ_BURST)) * 256;
  Freq = Freq + Chip_readReg(0x0F, READ_BURST) ;
  Freq =  (26000 * Freq) / 65536 + 0.5; // frequency calculation - 0x0D 0x0E 0x0F | 26*(($r{"0D"}*256+$r{"0E"})*256+$r{"0F"})/65536
  return Freq;
}


void Chip_setFreq(long frequency, byte * arr) {   /* frequency set & calculation - 0x0D 0x0E 0x0F | function used in Chip_writeRegFor */
  int32_t f;
  f = (frequency + Freq_offset * 1000) / 26000 * 65536;
  arr[0] = f / 65536;
  arr[1] = (f % 65536) / 256;
  arr[2] = f % 256;
#if defined(debug) || defined(debug_html) || defined(debug_chip)
  Serial.print(F("[DB] web_Freq_Set, input  ")); Serial.println(frequency);
  Serial.print(F("[DB] web_Freq_Set, output ")); Serial.print(onlyDecToHex2Digit(arr[0]));
  Serial.print(' '); Serial.print(onlyDecToHex2Digit(arr[1]));
  Serial.print(' '); Serial.println(onlyDecToHex2Digit(arr[2]));
#endif
}

#if defined (ARDUINO_ARCH_ESP8266) || defined (ARDUINO_ARCH_ESP32)
byte Chip_Bandw_cal(float input) {   /* bandwidth calculation from web */
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
  Serial.print(F("[DB] Chip_Bandw_cal, Setting MDMCFG4 (10) to ")); Serial.println(onlyDecToHex2Digit((Chip_readReg(CHIP_RxBw, READ_BURST) & 0b00001111) + bits));
#endif
  return ((Chip_readReg(CHIP_RxBw, READ_BURST) & 0b00001111) + bits);
}

void Chip_Datarate_Set(long datarate, byte * arr) {    /* datarate set & calculation - 0x10 0x11 */
  if (datarate < 24.7955) {
    datarate = 24.7955;
  } else if (datarate > 1621830) {
    datarate = 1621830;
  }

  int ret = Chip_readReg(CC110x_MDMCFG4, READ_BURST);
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
  Serial.print(F("[DB] Chip_Datarate_Set, MDMCFG4..MDMCFG3 to ")); Serial.print(onlyDecToHex2Digit(arr[0])); Serial.print(onlyDecToHex2Digit(arr[1]));
  Serial.print(' '); Serial.print(F(" = ")); Serial.print(datarate); Serial.println(F(" Hz"));
#endif
}


byte CC110x_Deviation_Set(float deviation) {    /* Deviation set & calculation */
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
  Serial.print(F("[DB] CC110x_Deviation_Set, DEVIATN (15) to ")); Serial.print(bitlast, HEX); Serial.println(F(" (value set to next possible level)"));
#endif
  return bitlast;
}


byte web_Mod_set(byte input) {
#ifdef debug
  Serial.print(F("[DB] web_Mod_set, set new value to ")); Serial.println(input);
  Serial.print(F("[DB] web_Mod_set, MDMCFG2 (12) value is ")); Serial.println(onlyDecToHex2Digit(Chip_readReg(0x12, READ_BURST)));
#endif

  /* read all split values | example F1 -> 11110001 */
  byte reg12_6_4 = Chip_readReg(0x12, READ_BURST) & 0x8f ;
  byte output = input << 4;
  output = output | reg12_6_4;
  return output;
}
#endif // Ende #if defined (ARDUINO_ARCH_ESP8266) || defined (ARDUINO_ARCH_ESP32)

#endif
