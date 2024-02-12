#ifndef CC110X_H
#define CC110X_H

#ifdef CC110x

#include <Arduino.h>
#include <digitalWriteFast.h>           // https://github.com/ArminJo/digitalWriteFast
#include <SPI.h>

static const char RECEIVE_MODE_USER[] PROGMEM = "CC110x user configuration";
const uint8_t CC110x_PATABLE_433[8] PROGMEM = {0xC0, 0xC8, 0x84, 0x60, 0x34, 0x1D, 0x0E, 0x12};
const uint8_t CC110x_PATABLE_868[8] PROGMEM = {0xC2, 0xCB, 0x81, 0x50, 0x27, 0x1E, 0x0F, 0x03};
const int8_t CC110x_PATABLE_POW[8] PROGMEM = {10, 7, 5, 0, -10, -15, -20, -30};

int Chip_readRSSI();
uint8_t Chip_readReg(uint8_t regAddr, uint8_t regType);
uint8_t CC110x_CmdStrobe(uint8_t cmd);
void ChipInit();
void Chip_readBurstReg(uint8_t * uiBuffer, uint8_t regAddr, uint8_t len);
void Chip_writeReg(uint8_t regAddr, uint8_t value);
void Chip_writeRegFor(const uint8_t *reg_name, uint8_t reg_length, String reg_modus);
void Chip_setReceiveMode();
void Chip_setFreq(long frequency, byte * arr);
float Chip_readFreq();
void CC110x_writeBurstReg(byte * uiBuffer, byte regAddr, byte len);
void CC110x_setTransmitMode();
void Chip_sendFIFO(char *startpos);
byte Chip_Bandw_cal(float input);
void Chip_Datarate_Set(long datarate, byte * arr);
byte CC110x_Deviation_Set(float deviation);
byte web_Mod_set(byte input);

extern boolean ChipFound;
extern String ReceiveModeName;              // name of active mode from array
extern byte ReceiveModeNr;                  // activated protocol in flash
extern byte ReceiveModePKTLEN;

extern byte ToggleOrder[4];
extern byte ToggleValues;
extern uint8_t ToggleArray[4];
extern unsigned long ToggleTime;

extern float Freq_offset;
extern int8_t freqOffAcc;
extern float freqErrAvg;
extern uint8_t freqAfc;
extern int RSSI_dez;
extern struct Data Registers[];
extern uint8_t RegistersMaxCnt;
extern unsigned long uptime;

struct Data {
  const uint8_t* reg_val;
  byte length;
  const char* name;
  byte PKTLEN;
};

// ############################## all available CC110x registers ##############################
const uint8_t Config_Default[] PROGMEM = {
  /*
    Address Config = No address check
    Base Frequency = 799.999878
    CRC Autoflush = false
    CRC Enable = true
    Carrier Frequency = 799.999878
    Channel Number = 0
    Channel Spacing = 199.951172
    Data Format = Normal mode
    Data Rate = 115.051
    Deviation = 47.607422
    Device Address = 0
    Manchester Enable = false
    Modulated = true
    Modulation Format = 2-FSK
    PA Ramping = false
    Packet Length = 255
    Packet Length Mode = Variable packet length mode. Packet length configured by the first byte after sync word
    Preamble Count = 4
    RX Filter BW = 203.125000
    Sync Word Qualifier Mode = 16/16 sync word bits detected
    TX Power = 0
    Whitening = true
  */

  0x07,  // IOCFG2              GDO2 Output Pin Configuration (default from TI tool 0x29)
  0x2E,  // IOCFG1              GDO1 Output Pin Configuration
  0x2E,  // IOCFG0              GDO0 Output Pin Configuration (default from TI tool 0x3F)
  0x47,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
  0xD3,  // SYNC1               Sync Word, High Byte
  0x91,  // SYNC0               Sync Word, Low Byte
  0xFF,  // PKTLEN              Packet Length
  0x04,  // PKTCTRL1            Packet Automation Control
  0x45,  // PKTCTRL0            Packet Automation Control
  0x00,  // ADDR                Device Address
  0x00,  // CHANNR              Channel Number
  0x0F,  // FSCTRL1             Frequency Synthesizer Control
  0x00,  // FSCTRL0             Frequency Synthesizer Control
  0x1E,  // FREQ2               Frequency Control Word, High Byte
  0xC4,  // FREQ1               Frequency Control Word, Middle Byte
  0xEC,  // FREQ0               Frequency Control Word, Low Byte
  0x8C,  // MDMCFG4             Modem Configuration
  0x22,  // MDMCFG3             Modem Configuration
  0x02,  // MDMCFG2             Modem Configuration
  0x22,  // MDMCFG1             Modem Configuration
  0xF8,  // MDMCFG0             Modem Configuration
  0x47,  // DEVIATN             Modem Deviation Setting
  0x07,  // MCSM2               Main Radio Control State Machine Configuration
  0x30,  // MCSM1               Main Radio Control State Machine Configuration
  0x04,  // MCSM0               Main Radio Control State Machine Configuration
  0x36,  // FOCCFG              Frequency Offset Compensation Configuration
  0x6C,  // BSCFG               Bit Synchronization Configuration
  0x03,  // AGCCTRL2            AGC Control
  0x40,  // AGCCTRL1            AGC Control
  0x91,  // AGCCTRL0            AGC Control
  0x87,  // WOREVT1             High Byte Event0 Timeout
  0x6B,  // WOREVT0             Low Byte Event0 Timeout
  0xF8,  // WORCTRL             Wake On Radio Control
  0x56,  // FREND1              Front End RX Configuration
  0x10,  // FREND0              Front End TX Configuration
  0xEA,  // FSCAL3              Frequency Synthesizer Calibration (x)
  0x2A,  // FSCAL2              Frequency Synthesizer Calibration (x)
  0x00,  // FSCAL1              Frequency Synthesizer Calibration (x)
  0x1F,  // FSCAL0              Frequency Synthesizer Calibration (x)
  0x41,  // RCCTRL1             RC Oscillator Configuration       (x)
  0x00,  // RCCTRL0             RC Oscillator Configuration       (x)
  //0x59,  // FSTEST              Frequency Synthesizer Calibration Control
  //0x7F,  // PTEST               Production Test
  //0x3F,  // AGCTEST             AGC Test
  //0x81,  // TEST2               Various Test Settings
  //0x35,  // TEST1               Various Test Settings
  //0x0B,  // TEST0               Various Test Settings
};

#ifdef Avantek
const uint8_t Config_Avantek[] PROGMEM = {
  /*
    Address Config = No address check
    Base Frequency = 433.299744
    CRC Autoflush = false
    CRC Enable = false
    Carrier Frequency = 433.299744
    Channel Number = 0
    Channel Spacing = 199.951172
    Data Format = Normal mode
    Data Rate = 49.9878
    Deviation = 57.128906
    Device Address = 0
    Manchester Enable = false
    Modulated = true
    Modulation Format = 2-FSK
    PA Ramping = true
    Packet Length = 255
    Packet Length Mode = Infinite packet length mode
    Preamble Count = 4
    RX Filter BW = 203.125000
    Sync Word Qualifier Mode = 16/16 sync word bits detected
    TX Power = unknown
    Whitening = false
  */

  0x01,  // IOCFG2              GDO2 Output Pin Configuration
  0x2E,  // IOCFG1              GDO1 Output Pin Configuration
  0x46,  // IOCFG0              GDO0 Output Pin Configuration
  0x01,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
  0x08,  // SYNC1               Sync Word, High Byte
  0x69,  // SYNC0               Sync Word, Low Byte
  0xFF,  // PKTLEN              Packet Length
  0x80,  // PKTCTRL1            Packet Automation Control
  0x02,  // PKTCTRL0            Packet Automation Control
  0x00,  // ADDR                Device Address
  0x00,  // CHANNR              Channel Number
  0x06,  // FSCTRL1             Frequency Synthesizer Control
  0x00,  // FSCTRL0             Frequency Synthesizer Control
  0x10,  // FREQ2               Frequency Control Word, High Byte
  0xAA,  // FREQ1               Frequency Control Word, Middle Byte
  0x56,  // FREQ0               Frequency Control Word, Low Byte
  0x8A,  // MDMCFG4             Modem Configuration
  0xF8,  // MDMCFG3             Modem Configuration
  0x02,  // MDMCFG2             Modem Configuration
  0x22,  // MDMCFG1             Modem Configuration
  0xF8,  // MDMCFG0             Modem Configuration
  0x51,  // DEVIATN             Modem Deviation Setting
  0x07,  // MCSM2               Main Radio Control State Machine Configuration
  0x00,  // MCSM1               Main Radio Control State Machine Configuration
  0x18,  // MCSM0               Main Radio Control State Machine Configuration
  0x16,  // FOCCFG              Frequency Offset Compensation Configuration
  0x6C,  // BSCFG               Bit Synchronization Configuration
  0x43,  // AGCCTRL2            AGC Control
  0x40,  // AGCCTRL1            AGC Control
  0x91,  // AGCCTRL0            AGC Control
  0x87,  // WOREVT1             High Byte Event0 Timeout
  0x6B,  // WOREVT0             Low Byte Event0 Timeout
  0xFB,  // WORCTRL             Wake On Radio Control
  0x56,  // FREND1              Front End RX Configuration
  0x11,  // FREND0              Front End TX Configuration
  0xEA,  // FSCAL3              Frequency Synthesizer Calibration (x)
  0x2A,  // FSCAL2              Frequency Synthesizer Calibration (x)
  0x00,  // FSCAL1              Frequency Synthesizer Calibration (x)
  0x1F,  // FSCAL0              Frequency Synthesizer Calibration (x)
  0x41,  // RCCTRL1             RC Oscillator Configuration       (x)
  0x00,  // RCCTRL0             RC Oscillator Configuration       (x)
  //0x59,  // FSTEST              Frequency Synthesizer Calibration Control
  //0x7F,  // PTEST               Production Test
  //0x3E,  // AGCTEST             AGC Test
  //0x88,  // TEST2               Various Test Settings
  //0x31,  // TEST1               Various Test Settings
  //0x0B,  // TEST0               Various Test Settings
};
#endif

#ifdef Bresser_5in1
const uint8_t Config_Bresser_5in1[] PROGMEM = {
  /*
    Address Config = No address check
    Base Frequency = 868.299866
    CRC Autoflush = false
    CRC Enable = false
    Carrier Frequency = 868.299866
    Channel Number = 0
    Channel Spacing = 199.951172
    Data Format = Normal mode
    Data Rate = 8.23212
    Deviation = 57.128906
    Device Address = 0
    Manchester Enable = false
    Modulated = true
    Modulation Format = 2-FSK
    PA Ramping = true
    Packet Length = 26
    Packet Length Mode = Fixed packet length mode. Length configured in PKTLEN register
    Preamble Count = 4
    RX Filter BW = 203.125000
    Sync Word Qualifier Mode = 16/16 sync word bits detected
    TX Power = unknown
    Whitening = false
  */

  0x01,  // IOCFG2              GDO2 Output Pin Configuration
  0x2E,  // IOCFG1              GDO1 Output Pin Configuration
  0x2E,  // IOCFG0              GDO0 Output Pin Configuration
  0x46,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
  0x2D,  // SYNC1               Sync Word, High Byte
  0xD4,  // SYNC0               Sync Word, Low Byte
  0x1A,  // PKTLEN              Packet Length
  0xC0,  // PKTCTRL1            Packet Automation Control
  0x00,  // PKTCTRL0            Packet Automation Control
  0x00,  // ADDR                Device Address
  0x00,  // CHANNR              Channel Number
  0x06,  // FSCTRL1             Frequency Synthesizer Control
  0x00,  // FSCTRL0             Frequency Synthesizer Control
  /*
    0x21,  // FREQ2               Frequency Control Word, High Byte (868.350 MHz)
    0x65,  // FREQ1               Frequency Control Word, Middle Byte (868.350 MHz)
    0xE8,  // FREQ0               Frequency Control Word, Low Byte (868.350 MHz)
  */
  0x21,  // FREQ2               Frequency Control Word, High Byte (868.300 MHz)
  0x65,  // FREQ1               Frequency Control Word, Middle Byte (868.300 MHz)
  0x6A,  // FREQ0               Frequency Control Word, Low Byte (868.300 MHz)
  0x88,  // MDMCFG4             Modem Configuration
  0x4C,  // MDMCFG3             Modem Configuration
  0x02,  // MDMCFG2             Modem Configuration
  0x22,  // MDMCFG1             Modem Configuration
  0xF8,  // MDMCFG0             Modem Configuration
  0x51,  // DEVIATN             Modem Deviation Setting
  0x07,  // MCSM2               Main Radio Control State Machine Configuration
  0x00,  // MCSM1               Main Radio Control State Machine Configuration
  0x18,  // MCSM0               Main Radio Control State Machine Configuration
  0x16,  // FOCCFG              Frequency Offset Compensation Configuration
  0x6C,  // BSCFG               Bit Synchronization Configuration
  0x43,  // AGCCTRL2            AGC Control
  0x68,  // AGCCTRL1            AGC Control
  0x91,  // AGCCTRL0            AGC Control
  0x87,  // WOREVT1             High Byte Event0 Timeout
  0x6B,  // WOREVT0             Low Byte Event0 Timeout
  0xFB,  // WORCTRL             Wake On Radio Control
  0x56,  // FREND1              Front End RX Configuration
  0x11,  // FREND0              Front End TX Configuration
  0xE9,  // FSCAL3              Frequency Synthesizer Calibration (from SmartRF Studio)
  0x2A,  // FSCAL2              Frequency Synthesizer Calibration (x)
  0x00,  // FSCAL1              Frequency Synthesizer Calibration (x)
  0x1F,  // FSCAL0              Frequency Synthesizer Calibration (x)
  0x41,  // RCCTRL1             RC Oscillator Configuration       (x)
  0x00,  // RCCTRL0             RC Oscillator Configuration       (x)
  //0x59,  // FSTEST              Frequency Synthesizer Calibration Control
  //0x7F,  // PTEST               Production Test
  //0xBE,  // AGCTEST             AGC Test
  //0x88,  // TEST2               Various Test Settings
  //0x31,  // TEST1               Various Test Settings
  //0x0B,  // TEST0               Various Test Settings
};
#endif

#ifdef Bresser_6in1
const uint8_t Config_Bresser_6in1[] PROGMEM = {
  /*
    Address Config = No address check
    Base Frequency = 868.299866
    CRC Autoflush = false
    CRC Enable = false
    Carrier Frequency = 868.299866
    Channel Number = 0
    Channel Spacing = 199.951172
    Data Format = Normal mode
    Data Rate = 8.23212
    Deviation = 57.128906
    Device Address = 0
    Manchester Enable = false
    Modulated = true
    Modulation Format = 2-FSK
    PA Ramping = true
    Packet Length = 18
    Packet Length Mode = Fixed packet length mode. Length configured in PKTLEN register
    Preamble Count = 4
    RX Filter BW = 203.125000
    Sync Word Qualifier Mode = 16/16 sync word bits detected
    TX Power = unknown
    Whitening = false
  */

  0x01,  // IOCFG2              GDO2 Output Pin Configuration
  0x2E,  // IOCFG1              GDO1 Output Pin Configuration
  0x2E,  // IOCFG0              GDO0 Output Pin Configuration
  0x44,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
  0x2D,  // SYNC1               Sync Word, High Byte
  0xD4,  // SYNC0               Sync Word, Low Byte
  0x12,  // PKTLEN              Packet Length
  0xC0,  // PKTCTRL1            Packet Automation Control
  0x00,  // PKTCTRL0            Packet Automation Control (0 : Fixed packet length mode)
  0x00,  // ADDR                Device Address
  0x00,  // CHANNR              Channel Number
  0x06,  // FSCTRL1             Frequency Synthesizer Control
  0x00,  // FSCTRL0             Frequency Synthesizer Control
  /*
    0x21,  // FREQ2               Frequency Control Word, High Byte (868.350 MHz)
    0x65,  // FREQ1               Frequency Control Word, Middle Byte (868.350 MHz)
    0xE8,  // FREQ0               Frequency Control Word, Low Byte (868.350 MHz)
  */
  0x21,  // FREQ2               Frequency Control Word, High Byte (868.300 MHz)
  0x65,  // FREQ1               Frequency Control Word, Middle Byte (868.300 MHz)
  0x6A,  // FREQ0               Frequency Control Word, Low Byte (868.300 MHz)
  0x88,  // MDMCFG4             Modem Configuration
  0x4C,  // MDMCFG3             Modem Configuration
  0x02,  // MDMCFG2             Modem Configuration
  0x22,  // MDMCFG1             Modem Configuration
  0xF8,  // MDMCFG0             Modem Configuration
  0x51,  // DEVIATN             Modem Deviation Setting
  0x07,  // MCSM2               Main Radio Control State Machine Configuration
  0x00,  // MCSM1               Main Radio Control State Machine Configuration
  0x18,  // MCSM0               Main Radio Control State Machine Configuration
  0x16,  // FOCCFG              Frequency Offset Compensation Configuration
  0x6C,  // BSCFG               Bit Synchronization Configuration
  0x43,  // AGCCTRL2            AGC Control
  0x68,  // AGCCTRL1            AGC Control
  0x91,  // AGCCTRL0            AGC Control
  0x87,  // WOREVT1             High Byte Event0 Timeout
  0x6B,  // WOREVT0             Low Byte Event0 Timeout
  0xFB,  // WORCTRL             Wake On Radio Control
  0x56,  // FREND1              Front End RX Configuration
  0x11,  // FREND0              Front End TX Configuration
  0xEA,  // FSCAL3              Frequency Synthesizer Calibration (x)
  0x2A,  // FSCAL2              Frequency Synthesizer Calibration (x)
  0x00,  // FSCAL1              Frequency Synthesizer Calibration (x)
  0x1F,  // FSCAL0              Frequency Synthesizer Calibration (x)
  0x41,  // RCCTRL1             RC Oscillator Configuration       (x)
  0x00,  // RCCTRL0             RC Oscillator Configuration       (x)
  //0x59,  // FSTEST              Frequency Synthesizer Calibration Control
  //0x7F,  // PTEST               Production Test
  //0x7E,  // AGCTEST             AGC Test
  //0x88,  // TEST2               Various Test Settings
  //0x31,  // TEST1               Various Test Settings
  //0x0B,  // TEST0               Various Test Settings
};
#endif

#ifdef Bresser_7in1
const uint8_t Config_Bresser_7in1[] PROGMEM = {
  /*
    Address Config = No address check
    Base Frequency = 868.299866
    CRC Autoflush = false
    CRC Enable = false
    Carrier Frequency = 868.299866
    Channel Number = 0
    Channel Spacing = 199.951172
    Data Format = Normal mode
    Data Rate = 8.23212
    Deviation = 57.128906
    Device Address = 0
    Manchester Enable = false
    Modulated = true
    Modulation Format = 2-FSK
    PA Ramping = true
    Packet Length = 23
    Packet Length Mode = Fixed packet length mode. Length configured in PKTLEN register
    Preamble Count = 4
    RX Filter BW = 203.125000
    Sync Word Qualifier Mode = 16/16 sync word bits detected
    TX Power = unknown
    Whitening = false
  */

  0x01,  // IOCFG2              GDO2 Output Pin Configuration
  0x2E,  // IOCFG1              GDO1 Output Pin Configuration
  0x2E,  // IOCFG0              GDO0 Output Pin Configuration
  0x45,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
  0x2D,  // SYNC1               Sync Word, High Byte
  0xD4,  // SYNC0               Sync Word, Low Byte
  0x17,  // PKTLEN              Packet Length
  0xC0,  // PKTCTRL1            Packet Automation Control
  0x00,  // PKTCTRL0            Packet Automation Control
  0x00,  // ADDR                Device Address
  0x00,  // CHANNR              Channel Number
  0x06,  // FSCTRL1             Frequency Synthesizer Control
  0x00,  // FSCTRL0             Frequency Synthesizer Control
  /*
    0x21,  // FREQ2               Frequency Control Word, High Byte (868.350 MHz)
    0x65,  // FREQ1               Frequency Control Word, Middle Byte (868.350 MHz)
    0xE8,  // FREQ0               Frequency Control Word, Low Byte (868.350 MHz)
  */
  0x21,  // FREQ2               Frequency Control Word, High Byte (868.300 MHz)
  0x65,  // FREQ1               Frequency Control Word, Middle Byte (868.300 MHz)
  0x6A,  // FREQ0               Frequency Control Word, Low Byte (868.300 MHz)
  0x88,  // MDMCFG4             Modem Configuration
  0x4C,  // MDMCFG3             Modem Configuration
  0x02,  // MDMCFG2             Modem Configuration
  0x22,  // MDMCFG1             Modem Configuration
  0xF8,  // MDMCFG0             Modem Configuration
  0x51,  // DEVIATN             Modem Deviation Setting
  0x07,  // MCSM2               Main Radio Control State Machine Configuration
  0x00,  // MCSM1               Main Radio Control State Machine Configuration
  0x18,  // MCSM0               Main Radio Control State Machine Configuration
  0x16,  // FOCCFG              Frequency Offset Compensation Configuration
  0x6C,  // BSCFG               Bit Synchronization Configuration
  0x43,  // AGCCTRL2            AGC Control
  0x68,  // AGCCTRL1            AGC Control
  0x91,  // AGCCTRL0            AGC Control
  0x87,  // WOREVT1             High Byte Event0 Timeout
  0x6B,  // WOREVT0             Low Byte Event0 Timeout
  0xFB,  // WORCTRL             Wake On Radio Control
  0x56,  // FREND1              Front End RX Configuration
  0x11,  // FREND0              Front End TX Configuration
  0xEA,  // FSCAL3              Frequency Synthesizer Calibration (x)
  0x2A,  // FSCAL2              Frequency Synthesizer Calibration (x)
  0x00,  // FSCAL1              Frequency Synthesizer Calibration (x)
  0x1F,  // FSCAL0              Frequency Synthesizer Calibration (x)
  0x41,  // RCCTRL1             RC Oscillator Configuration       (x)
  0x00,  // RCCTRL0             RC Oscillator Configuration       (x)
  //0x59,  // FSTEST              Frequency Synthesizer Calibration Control
  //0x7F,  // PTEST               Production Test
  //0xB6,  // AGCTEST             AGC Test
  //0x88,  // TEST2               Various Test Settings
  //0x31,  // TEST1               Various Test Settings
  //0x0B,  // TEST0               Various Test Settings
};
#endif

#ifdef Lacrosse_mode1
const uint8_t Config_Lacrosse_mode1[] PROGMEM = {
  /*
    Address Config = No address check
    Base Frequency = 868.299866
    CRC Autoflush = false
    CRC Enable = false
    Carrier Frequency = 868.299866
    Channel Number = 0
    Channel Spacing = 199.951172
    Data Format = Normal mode
    Data Rate = 17.2577
    Deviation = 88.867188
    Device Address = 0
    Manchester Enable = false
    Modulated = true
    Modulation Format = 2-FSK
    PA Ramping = true
    Packet Length = 5
    Packet Length Mode = Fixed packet length mode. Length configured in PKTLEN register
    Preamble Count = 4
    RX Filter BW = 203.125000
    Sync Word Qualifier Mode = 16/16 sync word bits detected
    TX Power = unknown
    Whitening = false
  */

  0x01,  // IOCFG2              GDO2 Output Pin Configuration
  0x2E,  // IOCFG1              GDO1 Output Pin Configuration
  0x2E,  // IOCFG0              GDO0 Output Pin Configuration
  0x41,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
  0x2D,  // SYNC1               Sync Word, High Byte
  0xD4,  // SYNC0               Sync Word, Low Byte
  0x05,  // PKTLEN              Packet Length
  0x80,  // PKTCTRL1            Packet Automation Control
  0x00,  // PKTCTRL0            Packet Automation Control
  0x00,  // ADDR                Device Address
  0x00,  // CHANNR              Channel Number
  0x06,  // FSCTRL1             Frequency Synthesizer Control
  0x00,  // FSCTRL0             Frequency Synthesizer Control
  0x21,  // FREQ2               Frequency Control Word, High Byte   (868.300 MHz)
  0x65,  // FREQ1               Frequency Control Word, Middle Byte (868.300 MHz)
  0x6A,  // FREQ0               Frequency Control Word, Low Byte    (868.300 MHz)
  0x89,  // MDMCFG4             Modem Configuration
  0x5C,  // MDMCFG3             Modem Configuration
  0x02,  // MDMCFG2             Modem Configuration
  0x22,  // MDMCFG1             Modem Configuration
  0xF8,  // MDMCFG0             Modem Configuration
  0x56,  // DEVIATN             Modem Deviation Setting
  0x07,  // MCSM2               Main Radio Control State Machine Configuration
  0x00,  // MCSM1               Main Radio Control State Machine Configuration
  0x18,  // MCSM0               Main Radio Control State Machine Configuration
  0x15,  // FOCCFG              Frequency Offset Compensation Configuration (SIGNALduino, 8 von 8 MSG in 30 Sekunden)
  //  0x0E,  // FOCCFG              Frequency Offset Compensation Configuration (SIGNALduino, Lacrosse_mode1 gut)
  0x6C,  // BSCFG               Bit Synchronization Configuration
  0x43,  // AGCCTRL2            AGC Control
  0x68,  // AGCCTRL1            AGC Control
  0x91,  // AGCCTRL0            AGC Control
  0x87,  // WOREVT1             High Byte Event0 Timeout
  0x6B,  // WOREVT0             Low Byte Event0 Timeout
  0xF8,  // WORCTRL             Wake On Radio Control
  0xB6,  // FREND1              Front End RX Configuration
  0x11,  // FREND0              Front End TX Configuration
  0xEA,  // FSCAL3              Frequency Synthesizer Calibration (x)
  0x2A,  // FSCAL2              Frequency Synthesizer Calibration (x)
  0x00,  // FSCAL1              Frequency Synthesizer Calibration (x)
  0x1F,  // FSCAL0              Frequency Synthesizer Calibration (x)
  0x41,  // RCCTRL1             RC Oscillator Configuration       (x)
  0x00,  // RCCTRL0             RC Oscillator Configuration       (x)
  //0x59,  // FSTEST              Frequency Synthesizer Calibration Control
  //0x7F,  // PTEST               Production Test
  //0x3E,  // AGCTEST             AGC Test
  //0x88,  // TEST2               Various Test Settings
  //0x31,  // TEST1               Various Test Settings
  //0x0B,  // TEST0               Various Test Settings
};
#endif

#ifdef Lacrosse_mode2
const uint8_t Config_Lacrosse_mode2[] PROGMEM = {
  /*
    Address Config = No address check
    Base Frequency = 868.299866
    CRC Autoflush = false
    CRC Enable = false
    Carrier Frequency = 868.299866
    Channel Number = 0
    Channel Spacing = 199.951172
    Data Format = Normal mode
    Data Rate = 9.59608
    Deviation = 31.738281
    Device Address = 0
    Manchester Enable = false
    Modulated = true
    Modulation Format = 2-FSK
    PA Ramping = true
    Packet Length = 5
    Packet Length Mode = Fixed packet length mode. Length configured in PKTLEN register
    Preamble Count = 4
    RX Filter BW = 101.562500
    Sync Word Qualifier Mode = 16/16 sync word bits detected
    TX Power = unknown
    Whitening = false
  */

  0x01,  // IOCFG2              GDO2 Output Pin Configuration
  0x2E,  // IOCFG1              GDO1 Output Pin Configuration
  0x2E,  // IOCFG0              GDO0 Output Pin Configuration
  0x41,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
  0x2D,  // SYNC1               Sync Word, High Byte
  0xD4,  // SYNC0               Sync Word, Low Byte
  0x05,  // PKTLEN              Packet Length
  0x80,  // PKTCTRL1            Packet Automation Control
  0x00,  // PKTCTRL0            Packet Automation Control
  0x00,  // ADDR                Device Address
  0x00,  // CHANNR              Channel Number
  0x06,  // FSCTRL1             Frequency Synthesizer Control
  0x00,  // FSCTRL0             Frequency Synthesizer Control
  0x21,  // FREQ2               Frequency Control Word, High Byte   (868.300 MHz)
  0x65,  // FREQ1               Frequency Control Word, Middle Byte (868.300 MHz)
  0x6A,  // FREQ0               Frequency Control Word, Low Byte    (868.300 MHz)
  0xC8,  // MDMCFG4             Modem Configuration
  0x83,  // MDMCFG3             Modem Configuration
  0x02,  // MDMCFG2             Modem Configuration
  0x22,  // MDMCFG1             Modem Configuration
  0xF8,  // MDMCFG0             Modem Configuration
  0x42,  // DEVIATN             Modem Deviation Setting
  0x07,  // MCSM2               Main Radio Control State Machine Configuration
  0x00,  // MCSM1               Main Radio Control State Machine Configuration
  0x18,  // MCSM0               Main Radio Control State Machine Configuration
  //0x0E,  // FOCCFG              Frequency Offset Compensation Configuration (SIGNALduino, Lacrosse_mode2 schlecht)
  //0x1D,  // FOCCFG              Frequency Offset Compensation Configuration (SIGNALduino, Lacrosse_mode2 gut) ++
  //0x15,  // FOCCFG              Frequency Offset Compensation Configuration (SIGNALduino, Lacrosse_mode2 gut) ++
  0x16,  // FOCCFG              Frequency Offset Compensation Configuration (SIGNALduino, Lacrosse_mode2 gut) ++
  0x6C,  // BSCFG               Bit Synchronization Configuration
  0x43,  // AGCCTRL2            AGC Control
  0x68,  // AGCCTRL1            AGC Control
  0x91,  // AGCCTRL0            AGC Control
  0x87,  // WOREVT1             High Byte Event0 Timeout
  0x6B,  // WOREVT0             Low Byte Event0 Timeout
  0xF8,  // WORCTRL             Wake On Radio Control
  0xB6,  // FREND1              Front End RX Configuration
  0x11,  // FREND0              Front End TX Configuration
  0xEA,  // FSCAL3              Frequency Synthesizer Calibration (x)
  0x2A,  // FSCAL2              Frequency Synthesizer Calibration (x)
  0x00,  // FSCAL1              Frequency Synthesizer Calibration (x)
  0x1F,  // FSCAL0              Frequency Synthesizer Calibration (x)
  0x41,  // RCCTRL1             RC Oscillator Configuration       (x)
  0x00,  // RCCTRL0             RC Oscillator Configuration       (x)
  //0x59,  // FSTEST              Frequency Synthesizer Calibration Control
  //0x7F,  // PTEST               Production Test
  //0x3E,  // AGCTEST             AGC Test
  //0x88,  // TEST2               Various Test Settings
  //0x31,  // TEST1               Various Test Settings
  //0x0B,  // TEST0               Various Test Settings
};
#endif

#ifdef Fine_Offset_WH51_434
const uint8_t Config_Fine_Offset_WH51_434[] PROGMEM = {
  /*
    Address Config = No address check
    Base Frequency = 433.919830
    CRC Autoflush = false
    CRC Enable = false
    Carrier Frequency = 433.919830
    Channel Number = 0
    Channel Spacing = 199.951172
    Data Format = Normal mode
    Data Rate = 17.2577
    Deviation = 34.912109
    Device Address = 0
    Manchester Enable = false
    Modulated = true
    Modulation Format = 2-FSK
    PA Ramping = true
    Packet Length = 14
    Packet Length Mode = Fixed packet length mode. Length configured in PKTLEN register
    Preamble Count = 4
    RX Filter BW = 135.416667
    Sync Word Qualifier Mode = 16/16 sync word bits detected
    TX Power = unknown
    Whitening = false
  */

  0x01,  // IOCFG2              GDO2 Output Pin Configuration
  0x2E,  // IOCFG1              GDO1 Output Pin Configuration
  0x2E,  // IOCFG0              GDO0 Output Pin Configuration
  0x43,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
  0x2D,  // SYNC1               Sync Word, High Byte
  0xD4,  // SYNC0               Sync Word, Low Byte
  0x0E,  // PKTLEN              Packet Length
  0x80,  // PKTCTRL1            Packet Automation Control
  0x00,  // PKTCTRL0            Packet Automation Control
  0x00,  // ADDR                Device Address
  0x00,  // CHANNR              Channel Number
  0x06,  // FSCTRL1             Frequency Synthesizer Control
  0x00,  // FSCTRL0             Frequency Synthesizer Control
  0x10,  // FREQ2               Frequency Control Word, High Byte
  0xB0,  // FREQ1               Frequency Control Word, Middle Byte
  0x71,  // FREQ0               Frequency Control Word, Low Byte
  0xA9,  // MDMCFG4             Modem Configuration
  0x5C,  // MDMCFG3             Modem Configuration
  0x02,  // MDMCFG2             Modem Configuration
  0x22,  // MDMCFG1             Modem Configuration
  0xF8,  // MDMCFG0             Modem Configuration
  0x43,  // DEVIATN             Modem Deviation Setting
  0x07,  // MCSM2               Main Radio Control State Machine Configuration
  0x00,  // MCSM1               Main Radio Control State Machine Configuration
  0x18,  // MCSM0               Main Radio Control State Machine Configuration
  0x16,  // FOCCFG              Frequency Offset Compensation Configuration
  0x6C,  // BSCFG               Bit Synchronization Configuration
  0x43,  // AGCCTRL2            AGC Control
  0x68,  // AGCCTRL1            AGC Control
  0x91,  // AGCCTRL0            AGC Control
  0x87,  // WOREVT1             High Byte Event0 Timeout
  0x6B,  // WOREVT0             Low Byte Event0 Timeout
  0xFB,  // WORCTRL             Wake On Radio Control
  0x56,  // FREND1              Front End RX Configuration
  0x11,  // FREND0              Front End TX Configuration
  0xEA,  // FSCAL3              Frequency Synthesizer Calibration (x)
  0x2A,  // FSCAL2              Frequency Synthesizer Calibration (x)
  0x00,  // FSCAL1              Frequency Synthesizer Calibration (x)
  0x1F,  // FSCAL0              Frequency Synthesizer Calibration (x)
  0x41,  // RCCTRL1             RC Oscillator Configuration       (x)
  0x00,  // RCCTRL0             RC Oscillator Configuration       (x)
  //0x59,  // FSTEST              Frequency Synthesizer Calibration Control
  //0x7F,  // PTEST               Production Test
  //0x3E,  // AGCTEST             AGC Test
  //0x88,  // TEST2               Various Test Settings
  //0x31,  // TEST1               Various Test Settings
  //0x0B,  // TEST0               Various Test Settings
};
#endif

#ifdef Fine_Offset_WH51_868
const uint8_t Config_Fine_Offset_WH51_868[] PROGMEM = {
  /*
    Address Config = No address check
    Base Frequency = 868.349854
    CRC Autoflush = false
    CRC Enable = false
    Carrier Frequency = 868.349854
    Channel Number = 0
    Channel Spacing = 199.951172
    Data Format = Normal mode
    Data Rate = 17.2577
    Deviation = 34.912109
    Device Address = 0
    Manchester Enable = false
    Modulated = true
    Modulation Format = 2-FSK
    PA Ramping = true
    Packet Length = 14
    Packet Length Mode = Fixed packet length mode. Length configured in PKTLEN register
    Preamble Count = 4
    RX Filter BW = 135.416667
    Sync Word Qualifier Mode = 16/16 sync word bits detected
    TX Power = unknown
    Whitening = false
  */

  0x01,  // IOCFG2              GDO2 Output Pin Configuration
  0x2E,  // IOCFG1              GDO1 Output Pin Configuration
  0x2E,  // IOCFG0              GDO0 Output Pin Configuration
  0x43,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
  0x2D,  // SYNC1               Sync Word, High Byte
  0xD4,  // SYNC0               Sync Word, Low Byte
  0x0E,  // PKTLEN              Packet Length
  0x80,  // PKTCTRL1            Packet Automation Control
  0x00,  // PKTCTRL0            Packet Automation Control
  0x00,  // ADDR                Device Address
  0x00,  // CHANNR              Channel Number
  0x06,  // FSCTRL1             Frequency Synthesizer Control
  0x00,  // FSCTRL0             Frequency Synthesizer Control
  0x21,  // FREQ2               Frequency Control Word, High Byte
  0x65,  // FREQ1               Frequency Control Word, Middle Byte
  0xE8,  // FREQ0               Frequency Control Word, Low Byte
  0xA9,  // MDMCFG4             Modem Configuration
  0x5C,  // MDMCFG3             Modem Configuration
  0x02,  // MDMCFG2             Modem Configuration
  0x22,  // MDMCFG1             Modem Configuration
  0xF8,  // MDMCFG0             Modem Configuration
  0x43,  // DEVIATN             Modem Deviation Setting
  0x07,  // MCSM2               Main Radio Control State Machine Configuration
  0x00,  // MCSM1               Main Radio Control State Machine Configuration
  0x18,  // MCSM0               Main Radio Control State Machine Configuration
  0x16,  // FOCCFG              Frequency Offset Compensation Configuration
  0x6C,  // BSCFG               Bit Synchronization Configuration
  0x43,  // AGCCTRL2            AGC Control
  0x68,  // AGCCTRL1            AGC Control
  0x91,  // AGCCTRL0            AGC Control
  0x87,  // WOREVT1             High Byte Event0 Timeout
  0x6B,  // WOREVT0             Low Byte Event0 Timeout
  0xFB,  // WORCTRL             Wake On Radio Control
  0x56,  // FREND1              Front End RX Configuration
  0x11,  // FREND0              Front End TX Configuration
  0xEA,  // FSCAL3              Frequency Synthesizer Calibration (x)
  0x2A,  // FSCAL2              Frequency Synthesizer Calibration (x)
  0x00,  // FSCAL1              Frequency Synthesizer Calibration (x)
  0x1F,  // FSCAL0              Frequency Synthesizer Calibration (x)
  0x41,  // RCCTRL1             RC Oscillator Configuration       (x)
  0x00,  // RCCTRL0             RC Oscillator Configuration       (x)
  //0x59,  // FSTEST              Frequency Synthesizer Calibration Control
  //0x7F,  // PTEST               Production Test
  //0x3E,  // AGCTEST             AGC Test
  //0x88,  // TEST2               Various Test Settings
  //0x31,  // TEST1               Various Test Settings
  //0x0B,  // TEST0               Various Test Settings
};
#endif

#ifdef Fine_Offset_WH57_434
const uint8_t Config_Fine_Offset_WH57_434[] PROGMEM = {
  /*
    Address Config = No address check
    Base Frequency = 433.919830
    CRC Autoflush = false
    CRC Enable = false
    Carrier Frequency = 433.919830
    Channel Number = 0
    Channel Spacing = 199.951172
    Data Format = Normal mode
    Data Rate = 17.2577
    Deviation = 34.912109
    Device Address = 0
    Manchester Enable = false
    Modulated = true
    Modulation Format = 2-FSK
    PA Ramping = true
    Packet Length = 9
    Packet Length Mode = Fixed packet length mode. Length configured in PKTLEN register
    Preamble Count = 4
    RX Filter BW = 135.416667
    Sync Word Qualifier Mode = 16/16 sync word bits detected
    TX Power = unknown
    Whitening = false
  */

  0x01,  // IOCFG2              GDO2 Output Pin Configuration
  0x2E,  // IOCFG1              GDO1 Output Pin Configuration
  0x2E,  // IOCFG0              GDO0 Output Pin Configuration
  0x43,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
  0x2D,  // SYNC1               Sync Word, High Byte
  0xD4,  // SYNC0               Sync Word, Low Byte
  0x09,  // PKTLEN              Packet Length
  0x80,  // PKTCTRL1            Packet Automation Control
  0x00,  // PKTCTRL0            Packet Automation Control
  0x00,  // ADDR                Device Address
  0x00,  // CHANNR              Channel Number
  0x06,  // FSCTRL1             Frequency Synthesizer Control
  0x00,  // FSCTRL0             Frequency Synthesizer Control
  0x10,  // FREQ2               Frequency Control Word, High Byte
  0xB0,  // FREQ1               Frequency Control Word, Middle Byte
  0x71,  // FREQ0               Frequency Control Word, Low Byte
  0xA9,  // MDMCFG4             Modem Configuration
  0x5C,  // MDMCFG3             Modem Configuration
  0x02,  // MDMCFG2             Modem Configuration
  0x22,  // MDMCFG1             Modem Configuration
  0xF8,  // MDMCFG0             Modem Configuration
  0x43,  // DEVIATN             Modem Deviation Setting
  0x07,  // MCSM2               Main Radio Control State Machine Configuration
  0x00,  // MCSM1               Main Radio Control State Machine Configuration
  0x18,  // MCSM0               Main Radio Control State Machine Configuration
  0x16,  // FOCCFG              Frequency Offset Compensation Configuration
  0x6C,  // BSCFG               Bit Synchronization Configuration
  0x43,  // AGCCTRL2            AGC Control
  0x68,  // AGCCTRL1            AGC Control
  0x91,  // AGCCTRL0            AGC Control
  0x87,  // WOREVT1             High Byte Event0 Timeout
  0x6B,  // WOREVT0             Low Byte Event0 Timeout
  0xFB,  // WORCTRL             Wake On Radio Control
  0x56,  // FREND1              Front End RX Configuration
  0x11,  // FREND0              Front End TX Configuration
  0xEA,  // FSCAL3              Frequency Synthesizer Calibration (x)
  0x2A,  // FSCAL2              Frequency Synthesizer Calibration (x)
  0x00,  // FSCAL1              Frequency Synthesizer Calibration (x)
  0x1F,  // FSCAL0              Frequency Synthesizer Calibration (x)
  0x41,  // RCCTRL1             RC Oscillator Configuration       (x)
  0x00,  // RCCTRL0             RC Oscillator Configuration       (x)
  //0x59,  // FSTEST              Frequency Synthesizer Calibration Control
  //0x7F,  // PTEST               Production Test
  //0x3E,  // AGCTEST             AGC Test
  //0x88,  // TEST2               Various Test Settings
  //0x31,  // TEST1               Various Test Settings
  //0x0B,  // TEST0               Various Test Settings
};
#endif

#ifdef Fine_Offset_WH57_868
const uint8_t Config_Fine_Offset_WH57_868[] PROGMEM = {
  /*
    Address Config = No address check
    Base Frequency = 868.349854
    CRC Autoflush = false
    CRC Enable = false
    Carrier Frequency = 868.349854
    Channel Number = 0
    Channel Spacing = 199.951172
    Data Format = Normal mode
    Data Rate = 17.2577
    Deviation = 34.912109
    Device Address = 0
    Manchester Enable = false
    Modulated = true
    Modulation Format = 2-FSK
    PA Ramping = true
    Packet Length = 9
    Packet Length Mode = Fixed packet length mode. Length configured in PKTLEN register
    Preamble Count = 4
    RX Filter BW = 135.416667
    Sync Word Qualifier Mode = 16/16 sync word bits detected
    TX Power = unknown
    Whitening = false
  */

  0x01,  // IOCFG2              GDO2 Output Pin Configuration
  0x2E,  // IOCFG1              GDO1 Output Pin Configuration
  0x2E,  // IOCFG0              GDO0 Output Pin Configuration
  0x43,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
  0x2D,  // SYNC1               Sync Word, High Byte
  0xD4,  // SYNC0               Sync Word, Low Byte
  0x09,  // PKTLEN              Packet Length
  0x80,  // PKTCTRL1            Packet Automation Control
  0x00,  // PKTCTRL0            Packet Automation Control
  0x00,  // ADDR                Device Address
  0x00,  // CHANNR              Channel Number
  0x06,  // FSCTRL1             Frequency Synthesizer Control
  0x00,  // FSCTRL0             Frequency Synthesizer Control
  0x21,  // FREQ2               Frequency Control Word, High Byte
  0x65,  // FREQ1               Frequency Control Word, Middle Byte
  0xE8,  // FREQ0               Frequency Control Word, Low Byte
  0xA9,  // MDMCFG4             Modem Configuration
  0x5C,  // MDMCFG3             Modem Configuration
  0x02,  // MDMCFG2             Modem Configuration
  0x22,  // MDMCFG1             Modem Configuration
  0xF8,  // MDMCFG0             Modem Configuration
  0x43,  // DEVIATN             Modem Deviation Setting
  0x07,  // MCSM2               Main Radio Control State Machine Configuration
  0x00,  // MCSM1               Main Radio Control State Machine Configuration
  0x18,  // MCSM0               Main Radio Control State Machine Configuration
  0x16,  // FOCCFG              Frequency Offset Compensation Configuration
  0x6C,  // BSCFG               Bit Synchronization Configuration
  0x43,  // AGCCTRL2            AGC Control
  0x68,  // AGCCTRL1            AGC Control
  0x91,  // AGCCTRL0            AGC Control
  0x87,  // WOREVT1             High Byte Event0 Timeout
  0x6B,  // WOREVT0             Low Byte Event0 Timeout
  0xFB,  // WORCTRL             Wake On Radio Control
  0x56,  // FREND1              Front End RX Configuration
  0x11,  // FREND0              Front End TX Configuration
  0xEA,  // FSCAL3              Frequency Synthesizer Calibration (x)
  0x2A,  // FSCAL2              Frequency Synthesizer Calibration (x)
  0x00,  // FSCAL1              Frequency Synthesizer Calibration (x)
  0x1F,  // FSCAL0              Frequency Synthesizer Calibration (x)
  0x41,  // RCCTRL1             RC Oscillator Configuration       (x)
  0x00,  // RCCTRL0             RC Oscillator Configuration       (x)
  //0x59,  // FSTEST              Frequency Synthesizer Calibration Control
  //0x7F,  // PTEST               Production Test
  //0x3E,  // AGCTEST             AGC Test
  //0x88,  // TEST2               Various Test Settings
  //0x31,  // TEST1               Various Test Settings
  //0x0B,  // TEST0               Various Test Settings
};
#endif

#ifdef Inkbird_IBS_P01R
const uint8_t Config_Inkbird_IBS_P01R[] PROGMEM = {
  /*
    Address Config = No address check
    Base Frequency = 433.919830
    CRC Autoflush = false
    CRC Enable = false
    Carrier Frequency = 433.919830
    Channel Number = 0
    Channel Spacing = 199.951172
    Data Format = Normal mode
    Data Rate = 9.9926
    Deviation = 19.042969
    Device Address = 0
    Manchester Enable = false
    Modulated = true
    Modulation Format = 2-FSK
    PA Ramping = true
    Packet Length = 18
    Packet Length Mode = Fixed packet length mode. Length configured in PKTLEN register
    Preamble Count = 4
    RX Filter BW = 101.562500
    Sync Word Qualifier Mode = 16/16 sync word bits detected
    TX Power = unknown
    Whitening = false
  */

  0x01,  // IOCFG2              GDO2 Output Pin Configuration
  0x2E,  // IOCFG1              GDO1 Output Pin Configuration
  0x2E,  // IOCFG0              GDO0 Output Pin Configuration
  0x44,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
  0x2D,  // SYNC1               Sync Word, High Byte
  0xD4,  // SYNC0               Sync Word, Low Byte
  0x12,  // PKTLEN              Packet Length
  0xC0,  // PKTCTRL1            Packet Automation Control
  0x00,  // PKTCTRL0            Packet Automation Control
  0x00,  // ADDR                Device Address
  0x00,  // CHANNR              Channel Number
  0x06,  // FSCTRL1             Frequency Synthesizer Control
  0x00,  // FSCTRL0             Frequency Synthesizer Control
  0x10,  // FREQ2               Frequency Control Word, High Byte
  0xB0,  // FREQ1               Frequency Control Word, Middle Byte
  0x71,  // FREQ0               Frequency Control Word, Low Byte
  0xC8,  // MDMCFG4             Modem Configuration
  0x93,  // MDMCFG3             Modem Configuration
  0x02,  // MDMCFG2             Modem Configuration
  0x22,  // MDMCFG1             Modem Configuration
  0xF8,  // MDMCFG0             Modem Configuration
  0x34,  // DEVIATN             Modem Deviation Setting
  0x07,  // MCSM2               Main Radio Control State Machine Configuration
  0x00,  // MCSM1               Main Radio Control State Machine Configuration
  0x18,  // MCSM0               Main Radio Control State Machine Configuration
  0x16,  // FOCCFG              Frequency Offset Compensation Configuration
  0x6C,  // BSCFG               Bit Synchronization Configuration
  0x43,  // AGCCTRL2            AGC Control
  0x48,  // AGCCTRL1            AGC Control
  0x91,  // AGCCTRL0            AGC Control
  0x87,  // WOREVT1             High Byte Event0 Timeout
  0x6B,  // WOREVT0             Low Byte Event0 Timeout
  0xF8,  // WORCTRL             Wake On Radio Control
  0xB6,  // FREND1              Front End RX Configuration
  0x11,  // FREND0              Front End TX Configuration
  0xEF,  // FSCAL3              Frequency Synthesizer Calibration (x)
  0x2C,  // FSCAL2              Frequency Synthesizer Calibration (x)
  0x18,  // FSCAL1              Frequency Synthesizer Calibration (x)
  0x1F,  // FSCAL0              Frequency Synthesizer Calibration (x)
  0x41,  // RCCTRL1             RC Oscillator Configuration       (x)
  0x00,  // RCCTRL0             RC Oscillator Configuration       (x)
  //0x59,  // FSTEST              Frequency Synthesizer Calibration Control
  //0x7F,  // PTEST               Production Test
  //0x3E,  // AGCTEST             AGC Test
  //0x88,  // TEST2               Various Test Settings
  //0x31,  // TEST1               Various Test Settings
  //0x0B,  // TEST0               Various Test Settings
};
#endif

#ifdef KOPP_FC
const uint8_t Config_KOPP_FC[] PROGMEM = {
  /*
    Address Config = No address check
    Base Frequency = 868.299866
    CRC Autoflush = false
    CRC Enable = false
    Carrier Frequency = 868.299866
    Channel Number = 0
    Channel Spacing = 349.914551
    Data Format = Normal mode
    Data Rate = 4.79794
    Deviation = 47.607422
    Device Address = 0
    Manchester Enable = false
    Modulated = true
    Modulation Format = GFSK
    PA Ramping = true
    Packet Length = 15
    Packet Length Mode = Fixed packet length mode. Length configured in PKTLEN register
    Preamble Count = 16
    RX Filter BW = 162.500000
    Sync Word Qualifier Mode = 16/16 + carrier-sense above threshold
    TX Power = unknown
    Whitening = false
  */

  0x01,  // IOCFG2              GDO2 Output Pin Configuration
  0x2E,  // IOCFG1              GDO1 Output Pin Configuration
  0x46,  // IOCFG0              GDO0 Output Pin Configuration
  0x04,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
  0xAA,  // SYNC1               Sync Word, High Byte
  0x54,  // SYNC0               Sync Word, Low Byte
  0x0F,  // PKTLEN              Packet Length
  0xE0,  // PKTCTRL1            Packet Automation Control
  0x00,  // PKTCTRL0            Packet Automation Control
  0x00,  // ADDR                Device Address
  0x00,  // CHANNR              Channel Number
  0x06,  // FSCTRL1             Frequency Synthesizer Control
  0x00,  // FSCTRL0             Frequency Synthesizer Control
  0x21,  // FREQ2               Frequency Control Word, High Byte
  0x65,  // FREQ1               Frequency Control Word, Middle Byte
  0x6A,  // FREQ0               Frequency Control Word, Low Byte
  0x97,  // MDMCFG4             Modem Configuration
  0x83,  // MDMCFG3             Modem Configuration
  0x16,  // MDMCFG2             Modem Configuration
  0x63,  // MDMCFG1             Modem Configuration
  0xB9,  // MDMCFG0             Modem Configuration
  0x47,  // DEVIATN             Modem Deviation Setting
  0x07,  // MCSM2               Main Radio Control State Machine Configuration
  0x0C,  // MCSM1               Main Radio Control State Machine Configuration
  0x29,  // MCSM0               Main Radio Control State Machine Configuration
  0x36,  // FOCCFG              Frequency Offset Compensation Configuration
  0x6C,  // BSCFG               Bit Synchronization Configuration
  0x07,  // AGCCTRL2            AGC Control
  0x40,  // AGCCTRL1            AGC Control
  0x91,  // AGCCTRL0            AGC Control
  0x87,  // WOREVT1             High Byte Event0 Timeout
  0x6B,  // WOREVT0             Low Byte Event0 Timeout
  0xF8,  // WORCTRL             Wake On Radio Control
  0x56,  // FREND1              Front End RX Configuration
  0x11,  // FREND0              Front End TX Configuration
  0xEA,  // FSCAL3              Frequency Synthesizer Calibration (x)
  0x2A,  // FSCAL2              Frequency Synthesizer Calibration (x)
  0x00,  // FSCAL1              Frequency Synthesizer Calibration (x)
  0x1F,  // FSCAL0              Frequency Synthesizer Calibration (x)
  0x41,  // RCCTRL1             RC Oscillator Configuration       (x)
  0x00,  // RCCTRL0             RC Oscillator Configuration       (x)
  //0x59,  // FSTEST              Frequency Synthesizer Calibration Control
  //0x7F,  // PTEST               Production Test
  //0x3F,  // AGCTEST             AGC Test
  //0x88,  // TEST2               Various Test Settings
  //0x31,  // TEST1               Various Test Settings
  //0x0B,  // TEST0               Various Test Settings
};
#endif

#ifdef OOK_MU_433
const uint8_t Config_OOK_MU_433[] PROGMEM = {
  /*
    Address Config = No address check
    Base Frequency = 433.919830
    CRC Autoflush = false
    CRC Enable = false
    Carrier Frequency = 433.919830
    Channel Number = 0
    Channel Spacing = 349.914551
    Data Format = Asynchronous serial mode
    Data Rate = 5.60379
    Deviation = 1.586914
    Device Address = 0
    Manchester Enable = false
    Modulation Format = ASK/OOK
    PA Ramping = false
    Packet Length = 61
    Packet Length Mode = Infinite packet length mode
    Preamble Count = 4
    RX Filter BW = 325.000000
    Sync Word Qualifier Mode = No preamble/sync
    TX Power = unknown
    Whitening = false
  */
  0x0D,  // IOCFG2              GDO2 Output Pin Configuration
  0x2E,  // IOCFG1              GDO1 Output Pin Configuration
  0x2D,  // IOCFG0              GDO0 Output Pin Configuration
  0x47,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
  0xD3,  // SYNC1               Sync Word, High Byte
  0x91,  // SYNC0               Sync Word, Low Byte
  0x3D,  // PKTLEN              Packet Length
  0x04,  // PKTCTRL1            Packet Automation Control
  0x32,  // PKTCTRL0            Packet Automation Control
  0x00,  // ADDR                Device Address
  0x00,  // CHANNR              Channel Number
  0x06,  // FSCTRL1             Frequency Synthesizer Control
  0x00,  // FSCTRL0             Frequency Synthesizer Control
  0x10,  // FREQ2               Frequency Control Word, High Byte
  0xB0,  // FREQ1               Frequency Control Word, Middle Byte
  0x71,  // FREQ0               Frequency Control Word, Low Byte
  0x57,  // MDMCFG4             Modem Configuration
  0xC4,  // MDMCFG3             Modem Configuration
  0x30,  // MDMCFG2             Modem Configuration
  0x23,  // MDMCFG1             Modem Configuration
  0xB9,  // MDMCFG0             Modem Configuration
  0x00,  // DEVIATN             Modem Deviation Setting
  0x07,  // MCSM2               Main Radio Control State Machine Configuration
  0x00,  // MCSM1               Main Radio Control State Machine Configuration
  0x18,  // MCSM0               Main Radio Control State Machine Configuration
  0x14,  // FOCCFG              Frequency Offset Compensation Configuration
  0x6C,  // BSCFG               Bit Synchronization Configuration
  0x00,  // AGCCTRL2            AGC Control // 24 dB
  0x00,  // AGCCTRL1            AGC Control
  0x91,  // AGCCTRL0            AGC Control
  0x87,  // WOREVT1             High Byte Event0 Timeout
  0x6B,  // WOREVT0             Low Byte Event0 Timeout
  0xF8,  // WORCTRL             Wake On Radio Control
  0xB6,  // FREND1              Front End RX Configuration
  0x11,  // FREND0              Front End TX Configuration
  0xE9,  // FSCAL3              Frequency Synthesizer Calibration (x)
  0x2A,  // FSCAL2              Frequency Synthesizer Calibration (x)
  0x00,  // FSCAL1              Frequency Synthesizer Calibration (x)
  0x1F,  // FSCAL0              Frequency Synthesizer Calibration (x)
  0x41,  // RCCTRL1             RC Oscillator Configuration       (x)
  0x00,  // RCCTRL0             RC Oscillator Configuration       (x)
  //0x59,  // FSTEST              Frequency Synthesizer Calibration Control
  //0x7F,  // PTEST               Production Test
  //0x3F,  // AGCTEST             AGC Test
  //0x88,  // TEST2               Various Test Settings
  //0x31,  // TEST1               Various Test Settings
  //0x09,  // TEST0               Various Test Settings
};
#endif

#ifdef PCA301
const uint8_t Config_PCA301[] PROGMEM = {
  /*
    Address Config = No address check
    Base Frequency = 868.949707
    CRC Autoflush = false
    CRC Enable = false
    Carrier Frequency = 868.949707
    Channel Number = 0
    Channel Spacing = 199.951172
    Data Format = Normal mode
    Data Rate = 6.62041
    Deviation = 69.824219
    Device Address = 0
    Manchester Enable = false
    Modulated = true
    Modulation Format = 2-FSK
    PA Ramping = true
    Packet Length = 255
    Packet Length Mode = Infinite packet length mode
    Preamble Count = 4
    RX Filter BW = 203.125000
    Sync Word Qualifier Mode = 16/16 + carrier-sense above threshold
    TX Power = unknown
    Whitening = false
  */

  0x01,  // IOCFG2              GDO2 Output Pin Configuration
  0x2E,  // IOCFG1              GDO1 Output Pin Configuration
  0x46,  // IOCFG0              GDO0 Output Pin Configuration
  0x07,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
  0x2D,  // SYNC1               Sync Word, High Byte
  0xD4,  // SYNC0               Sync Word, Low Byte
  0xFF,  // PKTLEN              Packet Length
  0x00,  // PKTCTRL1            Packet Automation Control
  0x02,  // PKTCTRL0            Packet Automation Control
  0x00,  // ADDR                Device Address
  0x00,  // CHANNR              Channel Number
  0x06,  // FSCTRL1             Frequency Synthesizer Control
  0x00,  // FSCTRL0             Frequency Synthesizer Control
  0x21,  // FREQ2               Frequency Control Word, High Byte
  0x6B,  // FREQ1               Frequency Control Word, Middle Byte
  0xD0,  // FREQ0               Frequency Control Word, Low Byte
  0x88,  // MDMCFG4             Modem Configuration
  0x0B,  // MDMCFG3             Modem Configuration
  0x06,  // MDMCFG2             Modem Configuration
  0x22,  // MDMCFG1             Modem Configuration
  0xF8,  // MDMCFG0             Modem Configuration
  0x53,  // DEVIATN             Modem Deviation Setting
  0x07,  // MCSM2               Main Radio Control State Machine Configuration
  0x00,  // MCSM1               Main Radio Control State Machine Configuration
  0x18,  // MCSM0               Main Radio Control State Machine Configuration
  0x16,  // FOCCFG              Frequency Offset Compensation Configuration
  0x6C,  // BSCFG               Bit Synchronization Configuration
  0x43,  // AGCCTRL2            AGC Control
  0x68,  // AGCCTRL1            AGC Control
  0x91,  // AGCCTRL0            AGC Control
  0x87,  // WOREVT1             High Byte Event0 Timeout
  0x6B,  // WOREVT0             Low Byte Event0 Timeout
  0xF8,  // WORCTRL             Wake On Radio Control
  0x56,  // FREND1              Front End RX Configuration
  0x11,  // FREND0              Front End TX Configuration
  0xEA,  // FSCAL3              Frequency Synthesizer Calibration (x)
  0x2A,  // FSCAL2              Frequency Synthesizer Calibration (x)
  0x00,  // FSCAL1              Frequency Synthesizer Calibration (x)
  0x1F,  // FSCAL0              Frequency Synthesizer Calibration (x)
  0x41,  // RCCTRL1             RC Oscillator Configuration       (x)
  0x00,  // RCCTRL0             RC Oscillator Configuration       (x)
  //0x59,  // FSTEST              Frequency Synthesizer Calibration Control
  //0x7F,  // PTEST               Production Test
  //0x3E,  // AGCTEST             AGC Test
  //0x88,  // TEST2               Various Test Settings
  //0x31,  // TEST1               Various Test Settings
  //0x0B,  // TEST0               Various Test Settings
};
#endif

#ifdef Rojaflex
const uint8_t Config_Rojaflex[] PROGMEM = {
  /*
    Address Config = No address check
    Base Frequency = 433.919830
    CRC Autoflush = true
    CRC Enable = true
    Carrier Frequency = 433.919830
    Channel Number = 0
    Channel Spacing = 199.951172
    Data Format = Normal mode
    Data Rate = 9.9926
    Deviation = 20.629883
    Device Address = 0
    Manchester Enable = false
    Modulated = true
    Modulation Format = GFSK
    PA Ramping = true
    Packet Length = 12
    Packet Length Mode = Variable packet length mode. Packet length configured by the first byte after sync word
    Preamble Count = 4
    RX Filter BW = 101.562500
    Sync Word Qualifier Mode = 30/32 sync word bits detected
    TX Power = unknown
    Whitening = false
  */

  0x07,  // IOCFG2              GDO2 Output Pin Configuration
  0x2E,  // IOCFG1              GDO1 Output Pin Configuration
  0x2E,  // IOCFG0              GDO0 Output Pin Configuration
  0x02,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
  0xD3,  // SYNC1               Sync Word, High Byte
  0x91,  // SYNC0               Sync Word, Low Byte
  0x0C,  // PKTLEN              Packet Length
  0x88,  // PKTCTRL1            Packet Automation Control
  0x05,  // PKTCTRL0            Packet Automation Control
  0x00,  // ADDR                Device Address
  0x00,  // CHANNR              Channel Number
  0x06,  // FSCTRL1             Frequency Synthesizer Control
  0x00,  // FSCTRL0             Frequency Synthesizer Control
  0x10,  // FREQ2               Frequency Control Word, High Byte
  0xB0,  // FREQ1               Frequency Control Word, Middle Byte
  0x71,  // FREQ0               Frequency Control Word, Low Byte
  0xC8,  // MDMCFG4             Modem Configuration
  0x93,  // MDMCFG3             Modem Configuration
  0x13,  // MDMCFG2             Modem Configuration
  0x22,  // MDMCFG1             Modem Configuration
  0xF8,  // MDMCFG0             Modem Configuration
  0x35,  // DEVIATN             Modem Deviation Setting
  0x07,  // MCSM2               Main Radio Control State Machine Configuration
  0x0F,  // MCSM1               Main Radio Control State Machine Configuration
  0x18,  // MCSM0               Main Radio Control State Machine Configuration
  0x16,  // FOCCFG              Frequency Offset Compensation Configuration
  0x6C,  // BSCFG               Bit Synchronization Configuration
  0x43,  // AGCCTRL2            AGC Control
  0x40,  // AGCCTRL1            AGC Control
  0x91,  // AGCCTRL0            AGC Control
  0x87,  // WOREVT1             High Byte Event0 Timeout
  0x6B,  // WOREVT0             Low Byte Event0 Timeout
  0xF8,  // WORCTRL             Wake On Radio Control
  0x56,  // FREND1              Front End RX Configuration
  0x11,  // FREND0              Front End TX Configuration
  0xEA,  // FSCAL3              Frequency Synthesizer Calibration (x)
  0x2A,  // FSCAL2              Frequency Synthesizer Calibration (x)
  0x00,  // FSCAL1              Frequency Synthesizer Calibration (x)
  0x1F,  // FSCAL0              Frequency Synthesizer Calibration (x)
  0x41,  // RCCTRL1             RC Oscillator Configuration       (x)
  0x00,  // RCCTRL0             RC Oscillator Configuration       (x)
  //0x59,  // FSTEST              Frequency Synthesizer Calibration Control
  //0x7F,  // PTEST               Production Test
  //0x3E,  // AGCTEST             AGC Test
  //0x88,  // TEST2               Various Test Settings
  //0x31,  // TEST1               Various Test Settings
  //0x0B,  // TEST0               Various Test Settings
};
#endif

#ifdef X_Sense
const uint8_t Config_X_Sense[] PROGMEM = {
  /*
    Address Config = No address check
    Base Frequency = 869.294861
    CRC Autoflush = false
    CRC Enable = false
    Carrier Frequency = 869.294861
    Channel Number = 0
    Channel Spacing = 199.951172
    Data Format = Normal mode
    Data Rate = 9.9926
    Deviation = 41.259766
    Device Address = 0
    Manchester Enable = false
    Modulated = true
    Modulation Format = GFSK
    PA Ramping = true
    Packet Length = 6
    Packet Length Mode = Fixed packet length mode. Length configured in PKTLEN register
    Preamble Count = 4
    RX Filter BW = 116.071429
    Sync Word Qualifier Mode = 16/16 sync word bits detected
    TX Power = unknown
    Whitening = false
  */

  0x01,  // IOCFG2              GDO2 Output Pin Configuration
  0x2E,  // IOCFG1              GDO1 Output Pin Configuration
  0x2E,  // IOCFG0              GDO0 Output Pin Configuration
  0x41,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
  0x2C,  // SYNC1               Sync Word, High Byte
  0x4C,  // SYNC0               Sync Word, Low Byte
  0x06,  // PKTLEN              Packet Length
  0x80,  // PKTCTRL1            Packet Automation Control
  0x00,  // PKTCTRL0            Packet Automation Control
  0x00,  // ADDR                Device Address
  0x00,  // CHANNR              Channel Number
  0x06,  // FSCTRL1             Frequency Synthesizer Control
  0x00,  // FSCTRL0             Frequency Synthesizer Control
  0x21,  // FREQ2               Frequency Control Word, High Byte
  0x6F,  // FREQ1               Frequency Control Word, Middle Byte
  0x36,  // FREQ0               Frequency Control Word, Low Byte
  0xB8,  // MDMCFG4             Modem Configuration
  0x93,  // MDMCFG3             Modem Configuration
  0x12,  // MDMCFG2             Modem Configuration
  0x22,  // MDMCFG1             Modem Configuration
  0xF8,  // MDMCFG0             Modem Configuration
  0x45,  // DEVIATN             Modem Deviation Setting
  0x07,  // MCSM2               Main Radio Control State Machine Configuration
  0x00,  // MCSM1               Main Radio Control State Machine Configuration
  0x18,  // MCSM0               Main Radio Control State Machine Configuration
  0x15,  // FOCCFG              Frequency Offset Compensation Configuration
  0x6C,  // BSCFG               Bit Synchronization Configuration
  0x43,  // AGCCTRL2            AGC Control
  0x68,  // AGCCTRL1            AGC Control
  0x91,  // AGCCTRL0            AGC Control
  0x87,  // WOREVT1             High Byte Event0 Timeout
  0x6B,  // WOREVT0             Low Byte Event0 Timeout
  0xF8,  // WORCTRL             Wake On Radio Control
  0xB6,  // FREND1              Front End RX Configuration
  0x11,  // FREND0              Front End TX Configuration
  0xEF,  // FSCAL3              Frequency Synthesizer Calibration (x)
  0x2B,  // FSCAL2              Frequency Synthesizer Calibration (x)
  0x14,  // FSCAL1              Frequency Synthesizer Calibration (x)
  0x1F,  // FSCAL0              Frequency Synthesizer Calibration (x)
  0x41,  // RCCTRL1             RC Oscillator Configuration       (x)
  0x00,  // RCCTRL0             RC Oscillator Configuration       (x)
  //0x59,  // FSTEST              Frequency Synthesizer Calibration Control
  //0x7F,  // PTEST               Production Test
  //0x3F,  // AGCTEST             AGC Test
  //0x81,  // TEST2               Various Test Settings
  //0x35,  // TEST1               Various Test Settings
  //0x09,  // TEST0               Various Test Settings
};
#endif


/* !!!!!!!!!!!!!!
   in development
   !!!!!!!!!!!!!! */

#ifdef HomeMatic
const uint8_t Config_HomeMatic[] PROGMEM = {
  /*
    Address Config = No address check
    Base Frequency = 868.299866
    CRC Autoflush = true
    CRC Enable = true
    Carrier Frequency = 868.299866
    Channel Number = 0
    Channel Spacing = 199.951172
    Data Format = Normal mode
    Data Rate = 9.9926
    Deviation = 19.042969
    Device Address = 0
    Manchester Enable = false
    Modulated = true
    Modulation Format = 2-FSK
    PA Ramping = false
    Packet Length = 255
    Packet Length Mode = Variable packet length mode. Packet length configured by the first byte after sync word
    Preamble Count = 4
    RX Filter BW = 101.562500
    Sync Word Qualifier Mode = 30/32 sync word bits detected
    TX Power = 0
    Whitening = true
  */
  0x07,  // IOCFG2              GDO2 Output Pin Configuration
  0x2E,  // IOCFG1              GDO1 Output Pin Configuration
  0x2E,  // IOCFG0              GDO0 Output Pin Configuration
  0x4D,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
  0xE9,  // SYNC1               Sync Word, High Byte
  0xCA,  // SYNC0               Sync Word, Low Byte
  0xFF,  // PKTLEN              Packet Length
  0x0C,  // PKTCTRL1            Packet Automation Control
  0x45,  // PKTCTRL0            Packet Automation Control
  0x00,  // ADDR                Device Address
  0x00,  // CHANNR              Channel Number
  0x06,  // FSCTRL1             Frequency Synthesizer Control
  0x00,  // FSCTRL0             Frequency Synthesizer Control
  0x21,  // FREQ2               Frequency Control Word, High Byte
  0x65,  // FREQ1               Frequency Control Word, Middle Byte
  0x6A,  // FREQ0               Frequency Control Word, Low Byte
  0xC8,  // MDMCFG4             Modem Configuration
  0x93,  // MDMCFG3             Modem Configuration
  0x03,  // MDMCFG2             Modem Configuration
  0x22,  // MDMCFG1             Modem Configuration
  0xF8,  // MDMCFG0             Modem Configuration
  0x34,  // DEVIATN             Modem Deviation Setting
  0x07,  // MCSM2               Main Radio Control State Machine Configuration
  0x33,  // MCSM1               Main Radio Control State Machine Configuration
  0x18,  // MCSM0               Main Radio Control State Machine Configuration
  0x16,  // FOCCFG              Frequency Offset Compensation Configuration
  0x6C,  // BSCFG               Bit Synchronization Configuration
  0x43,  // AGCCTRL2            AGC Control
  0x40,  // AGCCTRL1            AGC Control
  0x91,  // AGCCTRL0            AGC Control
  0x87,  // WOREVT1             High Byte Event0 Timeout
  0x6B,  // WOREVT0             Low Byte Event0 Timeout
  0xF8,  // WORCTRL             Wake On Radio Control
  0x56,  // FREND1              Front End RX Configuration
  0x10,  // FREND0              Front End TX Configuration
  0xEA,  // FSCAL3              Frequency Synthesizer Calibration (x)
  0x2A,  // FSCAL2              Frequency Synthesizer Calibration (x)
  0x00,  // FSCAL1              Frequency Synthesizer Calibration (x)
  0x1F,  // FSCAL0              Frequency Synthesizer Calibration (x)
  0x41,  // RCCTRL1             RC Oscillator Configuration       (x)
  0x00,  // RCCTRL0             RC Oscillator Configuration       (x)
  //0x59,  // FSTEST              Frequency Synthesizer Calibration Control
  //0x7F,  // PTEST               Production Test
  //0x3F,  // AGCTEST             AGC Test
  //0x81,  // TEST2               Various Test Settings
  //0x35,  // TEST1               Various Test Settings
  //0x09,  // TEST0               Various Test Settings
};
#endif

#ifdef Lacrosse_mode3
const uint8_t Config_Lacrosse_mode3[] PROGMEM = {
  /*
    Address Config = No address check
    Base Frequency = 915.299500
    CRC Autoflush = false
    CRC Enable = false
    Carrier Frequency = 915.299500
    Channel Number = 0
    Channel Spacing = 199.951172
    Data Format = Normal mode
    Data Rate = 9.57108
    Deviation = 31.738281
    Device Address = 0
    Manchester Enable = false
    Modulated = true
    Modulation Format = 2-FSK
    PA Ramping = true
    Packet Length = 5
    Packet Length Mode = Fixed packet length mode. Length configured in PKTLEN register
    Preamble Count = 4
    RX Filter BW = 101.562500
    Sync Word Qualifier Mode = 16/16 sync word bits detected
    TX Power = 0
    Whitening = false
  */

  0x01,  // IOCFG2              GDO2 Output Pin Configuration
  0x2E,  // IOCFG1              GDO1 Output Pin Configuration
  0x2E,  // IOCFG0              GDO0 Output Pin Configuration
  0x41,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
  0x2D,  // SYNC1               Sync Word, High Byte
  0xD4,  // SYNC0               Sync Word, Low Byte
  0x05,  // PKTLEN              Packet Length
  0x80,  // PKTCTRL1            Packet Automation Control
  0x00,  // PKTCTRL0            Packet Automation Control
  0x00,  // ADDR                Device Address
  0x00,  // CHANNR              Channel Number
  0x06,  // FSCTRL1             Frequency Synthesizer Control
  0x00,  // FSCTRL0             Frequency Synthesizer Control
  0x23,  // FREQ2               Frequency Control Word, High Byte   (868.300 MHz)
  0x34,  // FREQ1               Frequency Control Word, Middle Byte (868.300 MHz)
  0x2E,  // FREQ0               Frequency Control Word, Low Byte    (868.300 MHz)
  0xC8,  // MDMCFG4             Modem Configuration
  0x82,  // MDMCFG3             Modem Configuration
  0x02,  // MDMCFG2             Modem Configuration
  0x22,  // MDMCFG1             Modem Configuration
  0xF8,  // MDMCFG0             Modem Configuration
  0x42,  // DEVIATN             Modem Deviation Setting
  0x07,  // MCSM2               Main Radio Control State Machine Configuration
  0x00,  // MCSM1               Main Radio Control State Machine Configuration
  0x18,  // MCSM0               Main Radio Control State Machine Configuration
  0x16,  // FOCCFG              Frequency Offset Compensation Configuration (SIGNALduino, Lacrosse_mode2 gut) ++
  0x6C,  // BSCFG               Bit Synchronization Configuration
  0x43,  // AGCCTRL2            AGC Control
  0x68,  // AGCCTRL1            AGC Control
  0x91,  // AGCCTRL0            AGC Control
  0x87,  // WOREVT1             High Byte Event0 Timeout
  0x6B,  // WOREVT0             Low Byte Event0 Timeout
  0xF8,  // WORCTRL             Wake On Radio Control
  0xB6,  // FREND1              Front End RX Configuration
  0x11,  // FREND0              Front End TX Configuration
  0xEA,  // FSCAL3              Frequency Synthesizer Calibration (x)
  0x2A,  // FSCAL2              Frequency Synthesizer Calibration (x)
  0x00,  // FSCAL1              Frequency Synthesizer Calibration (x)
  0x1F,  // FSCAL0              Frequency Synthesizer Calibration (x)
  0x41,  // RCCTRL1             RC Oscillator Configuration       (x)
  0x00,  // RCCTRL0             RC Oscillator Configuration       (x)
  //0x59,  // FSTEST              Frequency Synthesizer Calibration Control
  //0x7F,  // PTEST               Production Test
  //0x3E,  // AGCTEST             AGC Test
  //0x88,  // TEST2               Various Test Settings
  //0x31,  // TEST1               Various Test Settings
  //0x0B,  // TEST0               Various Test Settings
};
#endif

#ifdef MAX
const uint8_t Config_MAX[] PROGMEM = {
  /*
    Address Config = No address check
    Base Frequency = 868.299866
    CRC Autoflush = true
    CRC Enable = true
    Carrier Frequency = 868.299866
    Channel Number = 0
    Channel Spacing = 199.951172
    Data Format = Normal mode
    Data Rate = 9.9926
    Deviation = 19.042969
    Device Address = 0
    Manchester Enable = false
    Modulated = true
    Modulation Format = 2-FSK
    PA Ramping = false
    Packet Length = 255
    Packet Length Mode = Variable packet length mode. Packet length configured by the first byte after sync word
    Preamble Count = 4
    RX Filter BW = 101.562500
    Sync Word Qualifier Mode = 30/32 sync word bits detected
    TX Power = unknown
    Whitening = true
  */
  0x07,  // IOCFG2              GDO2 Output Pin Configuration
  0x2E,  // IOCFG1              GDO1 Output Pin Configuration
  0x46,  // IOCFG0              GDO0 Output Pin Configuration
  0x47,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
  0xC6,  // SYNC1               Sync Word, High Byte
  0x26,  // SYNC0               Sync Word, Low Byte
  0xFF,  // PKTLEN              Packet Length
  0x0C,  // PKTCTRL1            Packet Automation Control
  0x45,  // PKTCTRL0            Packet Automation Control
  0x00,  // ADDR                Device Address
  0x00,  // CHANNR              Channel Number
  0x06,  // FSCTRL1             Frequency Synthesizer Control
  0x00,  // FSCTRL0             Frequency Synthesizer Control
  0x21,  // FREQ2               Frequency Control Word, High Byte
  0x65,  // FREQ1               Frequency Control Word, Middle Byte
  0x6A,  // FREQ0               Frequency Control Word, Low Byte
  0xC8,  // MDMCFG4             Modem Configuration
  0x93,  // MDMCFG3             Modem Configuration
  0x03,  // MDMCFG2             Modem Configuration
  0x22,  // MDMCFG1             Modem Configuration
  0xF8,  // MDMCFG0             Modem Configuration
  0x34,  // DEVIATN             Modem Deviation Setting
  0x07,  // MCSM2               Main Radio Control State Machine Configuration
  0x3F,  // MCSM1               Main Radio Control State Machine Configuration
  0x28,  // MCSM0               Main Radio Control State Machine Configuration
  0x16,  // FOCCFG              Frequency Offset Compensation Configuration
  0x6C,  // BSCFG               Bit Synchronization Configuration
  0x43,  // AGCCTRL2            AGC Control
  0x40,  // AGCCTRL1            AGC Control
  0x91,  // AGCCTRL0            AGC Control
  0x87,  // WOREVT1             High Byte Event0 Timeout
  0x6B,  // WOREVT0             Low Byte Event0 Timeout
  0xF8,  // WORCTRL             Wake On Radio Control
  0x56,  // FREND1              Front End RX Configuration
  0x10,  // FREND0              Front End TX Configuration
  0xEA,  // FSCAL3              Frequency Synthesizer Calibration (x)
  0x2A,  // FSCAL2              Frequency Synthesizer Calibration (x)
  0x00,  // FSCAL1              Frequency Synthesizer Calibration (x)
  0x1F,  // FSCAL0              Frequency Synthesizer Calibration (x)
  0x41,  // RCCTRL1             RC Oscillator Configuration       (x)
  0x00,  // RCCTRL0             RC Oscillator Configuration       (x)
  //0x59,  // FSTEST              Frequency Synthesizer Calibration Control
  //0x7F,  // PTEST               Production Test
  //0x3F,  // AGCTEST             AGC Test
  //0x81,  // TEST2               Various Test Settings
  //0x35,  // TEST1               Various Test Settings
  //0x09,  // TEST0               Various Test Settings
};
#endif

#ifdef WMBus_S
const uint8_t Config_WMBus_S[] PROGMEM = {
  /*
    Address Config = No address check
    Base Frequency = 868.299866
    CRC Autoflush = false
    CRC Enable = false
    Carrier Frequency = 868.299866
    Channel Number = 0
    Channel Spacing = 199.951172
    Data Format = Normal mode
    Data Rate = 32.7301
    Deviation = 47.607422
    Device Address = 0
    Manchester Enable = false
    Modulated = true
    Modulation Format = 2-FSK
    PA Ramping = false
    Packet Length = 255
    Packet Length Mode = Infinite packet length mode
    Preamble Count = 4
    RX Filter BW = 270.833333
    Sync Word Qualifier Mode = 16/16 + carrier-sense above threshold
    TX Power = 0
    Whitening = false
  */
  0x06,  // IOCFG2        GDO2 Output Pin Configuration
  0x2E,  // IOCFG1        GDO1 Output Pin Configuration
  0x00,  // IOCFG0        GDO0 Output Pin Configuration
  0x40,  // FIFOTHR       RX FIFO and TX FIFO Thresholds
  0x76,  // SYNC1         Sync Word, High Byte
  0x96,  // SYNC0         Sync Word, Low Byte
  0xFF,  // PKTLEN        Packet Length
  0x04,  // PKTCTRL1      Packet Automation Control
  0x02,  // PKTCTRL0      Packet Automation Control
  0x00,  // ADDR          Device Address
  0x00,  // CHANNR        Channel Number
  0x08,  // FSCTRL1       Frequency Synthesizer Control
  0x00,  // FSCTRL0       Frequency Synthesizer Control
  0x21,  // FREQ2         Frequency Control Word, High Byte
  0x65,  // FREQ1         Frequency Control Word, Middle Byte
  0x6A,  // FREQ0         Frequency Control Word, Low Byte
  0x6A,  // MDMCFG4       Modem Configuration
  0x4A,  // MDMCFG3       Modem Configuration
  0x06,  // MDMCFG2       Modem Configuration
  0x22,  // MDMCFG1       Modem Configuration
  0xF8,  // MDMCFG0       Modem Configuration
  0x47,  // DEVIATN       Modem Deviation Setting
  0x07,  // MCSM2         Main Radio Control State Machine Configuration
  0x00,  // MCSM1         Main Radio Control State Machine Configuration
  0x18,  // MCSM0         Main Radio Control State Machine Configuration
  0x2E,  // FOCCFG        Frequency Offset Compensation Configuration
  0x6D,  // BSCFG         Bit Synchronization Configuration
  0x04,  // AGCCTRL2      AGC Control
  0x09,  // AGCCTRL1      AGC Control
  0xB2,  // AGCCTRL0      AGC Control
  0x87,  // WOREVT1       High Byte Event0 Timeout
  0x6B,  // WOREVT0       Low Byte Event0 Timeout
  0xF8,  // WORCTRL       Wake On Radio Control
  0xB6,  // FREND1        Front End RX Configuration
  0x10,  // FREND0        Front End TX Configuration
  0xEA,  // FSCAL3        Frequency Synthesizer Calibration (x)
  0x2A,  // FSCAL2        Frequency Synthesizer Calibration (x)
  0x00,  // FSCAL1        Frequency Synthesizer Calibration (x)
  0x1F,  // FSCAL0        Frequency Synthesizer Calibration (x)
  0x41,  // RCCTRL1       RC Oscillator Configuration       (x)
  0x00,  // RCCTRL0       RC Oscillator Configuration       (x)
  //0x59,  // FSTEST        Frequency Synthesizer Calibration Control
  //0x7F,  // PTEST         Production Test
  //0x3F,  // AGCTEST       AGC Test
  //0x81,  // TEST2         Various Test Settings
  //0x35,  // TEST1         Various Test Settings
  //0x09,  // TEST0         Various Test Settings
};
#endif

#ifdef WMBus_T
const uint8_t Config_WMBus_T[] PROGMEM = {
  /*
    Address Config = No address check
    Base Frequency = 868.949707
    CRC Autoflush = false
    CRC Enable = false
    Carrier Frequency = 868.949707
    Channel Number = 0
    Channel Spacing = 199.951172
    Data Format = Normal mode
    Data Rate = 103.149
    Deviation = 38.085938
    Device Address = 0
    Manchester Enable = false
    Modulated = true
    Modulation Format = 2-FSK
    PA Ramping = false
    Packet Length = 255
    Packet Length Mode = Infinite packet length mode
    Preamble Count = 4
    RX Filter BW = 325.000000
    Sync Word Qualifier Mode = 16/16 + carrier-sense above threshold
    TX Power = 0
    Whitening = false
  */
  0x06,  // IOCFG2        GDO2 Output Pin Configuration
  0x2E,  // IOCFG1        GDO1 Output Pin Configuration
  0x00,  // IOCFG0        GDO0 Output Pin Configuration
  0x00,  // FIFOTHR       RX FIFO and TX FIFO Thresholds
  0x54,  // SYNC1         Sync Word, High Byte
  0x3D,  // SYNC0         Sync Word, Low Byte
  0xFF,  // PKTLEN        Packet Length
  0x04,  // PKTCTRL1      Packet Automation Control
  0x02,  // PKTCTRL0      Packet Automation Control
  0x00,  // ADDR          Device Address
  0x00,  // CHANNR        Channel Number
  0x08,  // FSCTRL1       Frequency Synthesizer Control
  0x00,  // FSCTRL0       Frequency Synthesizer Control
  0x21,  // FREQ2         Frequency Control Word, High Byte
  0x6B,  // FREQ1         Frequency Control Word, Middle Byte
  0xD0,  // FREQ0         Frequency Control Word, Low Byte
  0x5C,  // MDMCFG4       Modem Configuration
  0x04,  // MDMCFG3       Modem Configuration
  0x06,  // MDMCFG2       Modem Configuration
  0x22,  // MDMCFG1       Modem Configuration
  0xF8,  // MDMCFG0       Modem Configuration
  0x44,  // DEVIATN       Modem Deviation Setting
  0x07,  // MCSM2         Main Radio Control State Machine Configuration
  0x00,  // MCSM1         Main Radio Control State Machine Configuration
  0x18,  // MCSM0         Main Radio Control State Machine Configuration
  0x2E,  // FOCCFG        Frequency Offset Compensation Configuration
  0xBF,  // BSCFG         Bit Synchronization Configuration
  0x43,  // AGCCTRL2      AGC Control
  0x09,  // AGCCTRL1      AGC Control
  0xB5,  // AGCCTRL0      AGC Control
  0x87,  // WOREVT1       High Byte Event0 Timeout
  0x6B,  // WOREVT0       Low Byte Event0 Timeout
  0xF8,  // WORCTRL       Wake On Radio Control
  0xB6,  // FREND1        Front End RX Configuration
  0x10,  // FREND0        Front End TX Configuration
  0xEA,  // FSCAL3        Frequency Synthesizer Calibration (x)
  0x2A,  // FSCAL2        Frequency Synthesizer Calibration (x)
  0x00,  // FSCAL1        Frequency Synthesizer Calibration (x)
  0x1F,  // FSCAL0        Frequency Synthesizer Calibration (x)
  0x41,  // RCCTRL1       RC Oscillator Configuration       (x)
  0x00,  // RCCTRL0       RC Oscillator Configuration       (x)
  //0x59,  // FSTEST        Frequency Synthesizer Calibration Control
  //0x7F,  // PTEST         Production Test
  //0x3F,  // AGCTEST       AGC Test
  //0x88,  // TEST2         Various Test Settings
  //0x31,  // TEST1         Various Test Settings
  //0x09,  // TEST0         Various Test Settings
};
#endif

#define CHIP_NAME                 "CC110x"              // name Chip
#ifdef SIGNALduino_comp
#define CHIP_RFNAME               "cc1101_rf_Gateway"   // name web interface
#else
#define CHIP_RFNAME               "cc110x_rf_Gateway"   // name web interface
#endif

#define REGISTER_MAX              46                    // register count
#define REGISTER_STATUS_MAX     0x3D                    // register count inc. Status Registers
#define CMD_W_REG_MAX             52                    // command W address max 0x40 (ASCII 52 = 4)

#define CHIP_RxBw                0x10                   // Modem Configuration ... (BW & DRate)
#define CHIP_BitRate             0x10                   // first BitRate/DataRate address
#define CHIP_FREQMSB             0x0D                   // Frequency Control Word, High Byte

/** Command strobes */
#define CC110x_SRES              0x30        // Reset CC110x chip
#define CC110x_SFSTXON           0x31        // Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1). If in RX (with CCA):
// Go to a wait state where only the synthesizer is running (for quick RX / TX turnaround).
#define CC110x_SXOFF             0x32        // Turn off crystal oscillator
#define CC110x_SCAL              0x33        // Calibrate frequency synthesizer and turn it off. SCAL can be strobed from IDLE mode without
// setting manual calibration mode (MCSM0.FS_AUTOCAL=0)
#define CC110x_SRX               0x34        // Enable RX. Perform calibration first if coming from IDLE and MCSM0.FS_AUTOCAL=1
#define CC110x_STX               0x35        // In IDLE state: Enable TX. Perform calibration first if MCSM0.FS_AUTOCAL=1.
// If in RX state and CCA is enabled: Only go to TX if channel is clear
#define CC110x_SIDLE             0x36        // Exit RX / TX, turn off frequency synthesizer and exit Wake-On-Radio mode if applicable
#define CC110x_SWOR              0x38        // Start automatic RX polling sequence (Wake-on-Radio) as described in Section 19.5 if
// WORCTRL.RC_PD=0
#define CC110x_SPWD              0x39        // Enter power down mode when CSn goes high
#define CC110x_SFRX              0x3A        // Flush the RX FIFO buffer. Only issue SFRX in IDLE or RXFIFO_OVERFLOW states
#define CC110x_SFTX              0x3B        // Flush the TX FIFO buffer. Only issue SFTX in IDLE or TXFIFO_UNDERFLOW states
#define CC110x_SWORRST           0x3C        // Reset real time clock to Event1 value
#define CC110x_SNOP              0x3D        // No operation. May be used to get access to the chip status byte


/** Type of transfers (Table 45: SPI Address Space) */
#define WRITE_BURST              0x40
#define READ_SINGLE              0x80
#define READ_BURST               0xC0


/** PATABLE & FIFO's */
#define CC110x_PATABLE           0x3E        // PATABLE address
#define CC110x_TXFIFO            0x3F        // TX FIFO address
#define CHIP_RXFIFO              0x3F        // RX FIFO address


/** CC110x configuration registers */
#define CC110x_IOCFG2            0x00        // GDO2 Output Pin Configuration
#define CC110x_IOCFG1            0x01        // GDO1 Output Pin Configuration
#define CC110x_IOCFG0            0x02        // GDO0 Output Pin Configuration
#define CC110x_FIFOTHR           0x03        // RX FIFO and TX FIFO Thresholds
#define CC110x_SYNC1             0x04        // Sync Word, High Byte
#define CC110x_SYNC0             0x05        // Sync Word, Low Byte
#define CHIP_PKTLEN              0x06        // Packet Length
#define CC110x_PKTCTRL1          0x07        // Packet Automation Control
#define CC110x_PKTCTRL0          0x08        // Packet Automation Control
#define CC110x_ADDR              0x09        // Device Address
#define CC110x_CHANNR            0x0A        // Channel Number
#define CC110x_FSCTRL1           0x0B        // Frequency Synthesizer Control
#define CC110x_FSCTRL0           0x0C        // Frequency Synthesizer Control
#define CC110x_FREQ2             0x0D        // Frequency Control Word, High Byte
#define CC110x_FREQ1             0x0E        // Frequency Control Word, Middle Byte
#define CC110x_FREQ0             0x0F        // Frequency Control Word, Low Byte
#define CC110x_MDMCFG4           0x10        // Modem Configuration (duplex)
#define CC110x_MDMCFG3           0x11        // Modem Configuration
#define CC110x_MDMCFG2           0x12        // Modem Configuration
#define CC110x_MDMCFG1           0x13        // Modem Configuration
#define CC110x_MDMCFG0           0x14        // Modem Configuration
#define CC110x_DEVIATN           0x15        // Modem Deviation Setting
#define CC110x_MCSM2             0x16        // Main Radio Control State Machine Configuration
#define CC110x_MCSM1             0x17        // Main Radio Control State Machine Configuration
#define CC110x_MCSM0             0x18        // Main Radio Control State Machine Configuration
#define CC110x_FOCCFG            0x19        // Frequency Offset Compensation Configuration
#define CC110x_BSCFG             0x1A        // Bit Synchronization Configuration
#define CC110x_AGCCTRL2          0x1B        // AGC Control
#define CC110x_AGCCTRL1          0x1C        // AGC Control
#define CC110x_AGCCTRL0          0x1D        // AGC Control
#define CC110x_WOREVT1           0x1E        // High Byte Event0 Timeout
#define CC110x_WOREVT0           0x1F        // Low Byte Event0 Timeout
#define CC110x_WORCTRL           0x20        // Wake On Radio Control
#define CC110x_FREND1            0x21        // Front End RX Configuration
#define CC110x_FREND0            0x22        // Front End TX Configuration
#define CC110x_FSCAL3            0x23        // Frequency Synthesizer Calibration
#define CC110x_FSCAL2            0x24        // Frequency Synthesizer Calibration
#define CC110x_FSCAL1            0x25        // Frequency Synthesizer Calibration
#define CC110x_FSCAL0            0x26        // Frequency Synthesizer Calibration
#define CC110x_RCCTRL1           0x27        // RC Oscillator Configuration
#define CC110x_RCCTRL0           0x28        // RC Oscillator Configuration
#define CC110x_FSTEST            0x29        // Frequency Synthesizer Calibration Control
#define CC110x_PTEST             0x2A        // Production Test
#define CC110x_AGCTEST           0x2B        // AGC Test
#define CC110x_TEST2             0x2C        // Various Test Settings
#define CC110x_TEST1             0x2D        // Various Test Settings
#define CC110x_TEST0             0x2E        // Various Test Settings


/** Status registers */
#define CC110x_PARTNUM           0x30        // Chip ID
#define CHIP_VERSION             0x31        // Chip Version
#define CC110x_FREQEST           0x32        // Frequency Offset Estimate from Demodulator
#define CC110x_LQI               0x33        // Demodulator Estimate for Link Quality
#define CC110x_RSSI              0x34        // Received Signal Strength Indication
#define CC110x_MARCSTATE         0x35        // Main Radio Control State Machine State
#define CC110x_WORTIME1          0x36        // High Byte of WOR Time
#define CC110x_WORTIME0          0x37        // Low Byte of WOR Time
#define CC110x_PKTSTATUS         0x38        // Current GDOx Status and Packet Status
#define CC110x_VCO_VC_DAC        0x39        // Current Setting from PLL Calibration Module
#define CC110x_TXBYTES           0x3A        // Underflow and Number of Bytes
#define CC110x_RXBYTES           0x3B        // Overflow and Number of Bytes
#define CC110x_RCCTRL1_STATUS    0x3C        // Last RC Oscillator Calibration Result
#define CC110x_RCCTRL0_STATUS    0x3D        // Last RC Oscillator Calibration Result 


/** Chip States */
#define CC110x_STATE_IDLE                      0x00
#define CC110x_STATE_RX                        0x10
#define CC110x_STATE_TX                        0x20
#define CC110x_STATE_FSTXON                    0x30
#define CC110x_STATE_CALIBRATE                 0x40
#define CC110x_STATE_SETTLING                  0x50
#define CC110x_STATE_RX_OVERFLOW               0x60
#define CC110x_STATE_TX_UNDERFLOW              0x70


/* Bit fields in the Chip status byte */
#define CC110x_STATUS_CHIP_RDYn_BM             0x80
#define CC110x_STATUS_STATE_BM                 0x70
#define CC110x_STATUS_FIFO_BYTES_AVAILABLE_BM  0x0F

#endif  // END - #ifdef CC110x
#endif  // END - #ifndef CC110X_H
