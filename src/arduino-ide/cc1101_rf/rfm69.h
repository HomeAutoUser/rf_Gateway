#ifndef RFM69_H
#define RFM69_H

#ifdef RFM69
#include <Arduino.h>
#include <digitalWriteFast.h>             // https://github.com/ArminJo/digitalWriteFast
#include <SPI.h>

//#define FXOSC_28 // SX1231 FXOSC 28,375 MHz, sonst RFM69 FXOSC 32,000 MHz
#define fxOsc 32000000                          // Crystal oscillator frequency
const float fStep = fxOsc / pow(2, 19);         // Frequency synthesizer step
#define CHIP_NAME         "RFM69"               // name Chip
#define CHIP_RFNAME       "rfm69_rf_Gateway"    // name web interface
#define REGISTER_MAX      84                    // register count
#define REGISTER_STATUS_MAX   0x71              // register count (for compatibility with the CC110x)
#define CMD_W_REG_MAX     56                    // command W address max 0x80 (ASCII 56 = 8)

#define READ_BURST        0x00                  // for compatibility with the CC110x
#define CHIP_VERSION      0x10                  // RegVersion - Semtech ID relating the silicon revision
#define CHIP_RXFIFO       0x00                  // RX FIFO address
#define CHIP_PKTLEN       0x38                  // RegPayloadLength - payload length
#define CHIP_RxBw         0x19                  // Bandwidth address
#define CHIP_BitRate      0x03                  // first BitRate address

//const uint8_t SX1231_RegAddrTranslate[5] PROGMEM = {0x58, 0x59, 0x5F, 0x6F, 0x71};
const uint8_t SX1231_RegAddrTranslate[5] = {0x58, 0x59, 0x5F, 0x6F, 0x71};
static const char PROGMEM RECEIVE_MODE_USER[] = "RFM69 user configuration";

void ChipInit();
int Chip_readRSSI();
uint8_t Chip_readReg(uint8_t addr, uint8_t regType); // SX1231 read register (address) (regType for compatibility with the CC110x)
void Chip_readBurstReg(uint8_t * uiBuffer, uint8_t regAddr, uint8_t len);
void Chip_writeReg(uint8_t regAddr, uint8_t value); // SX1231 write register (address, value)
void Chip_writeRegFor(const uint8_t *reg_name, uint8_t reg_length, String reg_modus);
void Chip_setFreq(uint32_t frequency, byte * arr);    /* frequency set & calculation */
void Chip_setReceiveMode(); // SX1231 start receive mode
float Chip_readFreq();
byte Chip_Bandw_cal(float input);
void Chip_Datarate_Set(long datarate, byte * arr);
void Chip_sendFIFO(char *startpos);
void SX1231_setIdleMode(); // SX1231 start idle mode
uint8_t SX1231_setOperatingMode(uint8_t ModeNew, uint8_t Mode, uint8_t RegOpMode);
void SX1231_afc(uint8_t freqAfc); // AfcAutoOn, 0  AFC is performed each time AfcStart is set, 1  AFC is performed each time Rx mode is entered
void SX1231_read_reg_all();  // SX1231 read all 112 register
void SX1231_Deviation_Set(float deviation, byte * arr); // calculate register values (RegFdevMsb 0x5, RegFdevLsb 0x06) for frequency deviation
void SX1231_setTransmitMode(); // SX1231 start transmit mode

extern boolean ChipFound;
extern String ReceiveModeName;              // name of active mode from array
extern byte ReceiveModeNr;                  // activated protocol in flash
extern byte ReceiveModePKTLEN;

extern byte ToggleOrder[4];
extern byte ToggleValues;
extern uint8_t ToggleArray[4];
extern unsigned long ToggleTime;

extern float Freq_offset;
extern int RSSI_dez;
extern struct Data Registers[];
extern uint8_t RegistersMaxCnt;
extern uint8_t freqAfc;


struct Data {
  const uint8_t* reg_val;
  byte length;
  const char* name;
  byte PKTLEN;
};

// ############################## all available SX1231 registers ##############################
const uint8_t Config_Default[] PROGMEM = {
  // SX1231 default register values
  0x00, // Address 0x00 - RegFifo (FIFO data input/output)
  0x04, // Address 0x01 - RegOpMode (Operating modes of the transceiver)
  0x00, // Address 0x02 - RegDataModul (Data operation mode and Modulation settings)
  0x1A, // Address 0x03 - RegBitrateMsb (Bit Rate setting, Most Significant Bits)
  0x0B, // Address 0x04 - RegBitrateLsb (Bit Rate setting, Least Significant Bits)
  0x00, // Address 0x05 - RegFdevMsb (Frequency Deviation setting, Most Significant Bits)
  0x52, // Address 0x06 - RegFdevLsb (Frequency Deviation setting, Least Significant Bits)
  0xE4, // Address 0x07 - RegFrfMsb (RF Carrier Frequency, Most Significant Bits)
  0xC0, // Address 0x08 - RegFrfMid (RF Carrier Frequency, Intermediate Bits)
  0x00, // Address 0x09 - RegFrfLsb (RF Carrier Frequency, Least Significant Bits)
  0x41, // Address 0x0A - RegOsc1 (RC Oscillators Settings)
  0x00, // Address 0x0B - RegAfcCtrl (AFC control in low modulation index situations)
  0x02, // Address 0x0C - RegLowBat (Low Battery Indicator Settings)
  0x92, // Address 0x0D - RegListen1 (Listen Mode settings)
  0xF5, // Address 0x0E - RegListen2 (Listen Mode Idle duration)
  0x20, // Address 0x0F - RegListen3 (Listen Mode Rx duration)
  0x23, // Address 0x10 - RegVersion (Semtech ID relating the silicon revision)
  0x9F, // Address 0x11 - RegPaLevel (PA selection and Output Power control)
  0x09, // Address 0x12 - RegPaRamp (Control of the PA ramp time in FSK mode)
  0x1A, // Address 0x13 - RegOcp (Over Current Protection control)
  0x40, // Address 0x14 - Reserved14 (-)
  0xB0, // Address 0x15 - Reserved15 (-)
  0x7B, // Address 0x16 - Reserved16 (-)
  0x9B, // Address 0x17 - Reserved17 (-)
  0x88, // Address 0x18 - RegLna (LNA settings)
  0x55, // Address 0x19 - RegRxBw (Channel Filter BW Control)
  0x8B, // Address 0x1A - RegAfcBw (Channel Filter BW control during the AFC routine)
  0x40, // Address 0x1B - RegOokPeak (OOK demodulator selection and control in peak mode)
  0x80, // Address 0x1C - RegOokAvg (Average threshold control of the OOK demodulator)
  0x06, // Address 0x1D - RegOokFix (Fixed threshold control of the OOK demodulator)
  0x10, // Address 0x1E - RegAfcFei (AFC and FEI control and status)
  0x00, // Address 0x1F - RegAfcMsb (MSB of the frequency correction of the AFC)
  0x00, // Address 0x20 - RegAfcLsb (LSB of the frequency correction of the AFC)
  0x00, // Address 0x21 - RegFeiMsb (MSB of the calculated frequency error)
  0x00, // Address 0x22 - RegFeiLsb (LSB of the calculated frequency error)
  0x02, // Address 0x23 - RegRssiConfig (RSSI-related settings)
  0xFF, // Address 0x24 - RegRssiValue (RSSI value in dBm)
  0x00, // Address 0x25 - RegDioMapping1 (Mapping of pins DIO0 to DIO3)
  0x07, // Address 0x26 - RegDioMapping2 (Mapping of pins DIO4 and DIO5, ClkOut frequency)
  0x80, // Address 0x27 - RegIrqFlags1 (Status register: PLL Lock state, Timeout, RSSI > Threshold...)
  0x00, // Address 0x28 - RegIrqFlags2 (Status register: FIFO handling flags, Low Battery detection...)
  0xE4, // Address 0x29 - RegRssiThresh (RSSI Threshold control) -114 dB
  0x00, // Address 0x2A - RegRxTimeout1 (Timeout duration between Rx request and RSSI detection)
  0x00, // Address 0x2B - RegRxTimeout2 (Timeout duration between RSSI detection and PayloadReady)
  0x00, // Address 0x2C - RegPreambleMsb (Preamble length, MSB)
  0x03, // Address 0x2D - RegPreambleLsb (Preamble length, LSB)
  0x98, // Address 0x2E - RegSyncConfig (Sync Word Recognition control)
  0x01, // Address 0x2F - RegSyncValue1 (Sync Word bytes, 1 through 8)
  0x01, // Address 0x30 - RegSyncValue2 (Sync Word bytes, 1 through 8)
  0x01, // Address 0x31 - RegSyncValue3 (Sync Word bytes, 1 through 8)
  0x01, // Address 0x32 - RegSyncValue4 (Sync Word bytes, 1 through 8)
  0x01, // Address 0x33 - RegSyncValue5 (Sync Word bytes, 1 through 8)
  0x01, // Address 0x34 - RegSyncValue6 (Sync Word bytes, 1 through 8)
  0x01, // Address 0x35 - RegSyncValue7 (Sync Word bytes, 1 through 8)
  0x01, // Address 0x36 - RegSyncValue8 (Sync Word bytes, 1 through 8)
  0x10, // Address 0x37 - RegPacketConfig1 (Packet mode settings)
  0x40, // Address 0x38 - RegPayloadLength (Payload length setting)
  0x00, // Address 0x39 - RegNodeAdrs (Node address)
  0x00, // Address 0x3A - RegBroadcastAdrs (Broadcast address)
  0x00, // Address 0x3B - RegAutoModes (Auto modes settings)
  0x8F, // Address 0x3C - RegFifoThresh (Fifo threshold, Tx start condition)
  0x02, // Address 0x3D - RegPacketConfig2 (Packet mode settings)
  0x00, // Address 0x3E - RegAesKey1 (16 bytes of the cypher key)
  0x00, // Address 0x3F - RegAesKey2 (16 bytes of the cypher key)
  0x00, // Address 0x40 - RegAesKey3 (16 bytes of the cypher key)
  0x00, // Address 0x41 - RegAesKey4 (16 bytes of the cypher key)
  0x00, // Address 0x42 - RegAesKey5 (16 bytes of the cypher key)
  0x00, // Address 0x43 - RegAesKey6 (16 bytes of the cypher key)
  0x00, // Address 0x44 - RegAesKey7 (16 bytes of the cypher key)
  0x00, // Address 0x45 - RegAesKey8 (16 bytes of the cypher key)
  0x00, // Address 0x46 - RegAesKey9 (16 bytes of the cypher key)
  0x00, // Address 0x47 - RegAesKey10 (16 bytes of the cypher key)
  0x00, // Address 0x48 - RegAesKey11 (16 bytes of the cypher key)
  0x00, // Address 0x49 - RegAesKey12 (16 bytes of the cypher key)
  0x00, // Address 0x4A - RegAesKey13 (16 bytes of the cypher key)
  0x00, // Address 0x4B - RegAesKey14 (16 bytes of the cypher key)
  0x00, // Address 0x4C - RegAesKey15 (16 bytes of the cypher key)
  0x00, // Address 0x4D - RegAesKey16 (16 bytes of the cypher key)
  0x01, // Address 0x4E - RegTemp1 (Temperature Sensor control)
  0x00, // Address 0x4F - RegTemp2 (Temperature readout)
  0x1B, // Address 0x58 - RegTestLna (Sensitivity boost)
  0x09, // Address 0x59 - RegTestTcxo (XTAL or TCXO input selection)
  0x08, // Address 0x5F - RegTestllBw (PLL Bandwidth setting)
  0x30, // Address 0x6F - RegTestDagc (Fading Margin Improvement) DAGC on
  0x00, // Address 0x71 - RegTestAfc (AFC offset for low modulation index AFC)
}; // END SX1231 default register values

#ifdef Bresser_5in1
const uint8_t Config_Bresser_5in1[] PROGMEM = {
  // SX1231 register values for Bresser 5-in-1
  0x00, // Address 0x00 - RegFifo (FIFO data input/output)
  0x04, // Address 0x01 - RegOpMode - Standby mode (STDBY)
  0x00, // Address 0x02 - RegDataModul
  0x0F, // Address 0x03 - RegBitrateMsb
  0x2F, // Address 0x04 - RegBitrateLsb
  0x01, // Address 0x05 - RegFdevMsb
  0xEC, // Address 0x06 - RegFdevLsb
  0xD9, // Address 0x07 - RegFrfMsb
  0x13, // Address 0x08 - RegFrfMid
  0x33, // Address 0x09 - RegFrfLsb
  0x41, // Address 0x0A - RegOsc1
  0x00, // Address 0x0B - RegAfcCtrl
  0x02, // Address 0x0C - RegLowBat
  0x92, // Address 0x0D - RegListen1
  0xF5, // Address 0x0E - RegListen2
  0x20, // Address 0x0F - RegListen3
  0x24, // Address 0x10 - RegVersion
  0x9F, // Address 0x11 - RegPaLevel
  0x09, // Address 0x12 - RegPaRamp
  0x1A, // Address 0x13 - RegOcp
  0x40, // Address 0x14 - Reserved14
  0xB0, // Address 0x15 - Reserved15
  0x7B, // Address 0x16 - Reserved16
  0x9B, // Address 0x17 - Reserved17
  0x09, // Address 0x18 - RegLna
  0x4A, // Address 0x19 - RegRxBw
  0x82, // Address 0x1A - RegAfcBw
  0x40, // Address 0x1B - RegOokPeak
  0x80, // Address 0x1C - RegOokAvg
  0x06, // Address 0x1D - RegOokFix
  0x10, // Address 0x1E - RegAfcFei
  0x00, // Address 0x1F - RegAfcMsb
  0x00, // Address 0x20 - RegAfcLsb
  0x00, // Address 0x21 - RegFeiMsb
  0x00, // Address 0x22 - RegFeiLsb
  0x02, // Address 0x23 - RegRssiConfig
  0xFF, // Address 0x24 - RegRssiValue
  0x00, // Address 0x25 - RegDioMapping1
  0x07, // Address 0x26 - RegDioMapping2
  0x80, // Address 0x27 - RegIrqFlags1
  0x00, // Address 0x28 - RegIrqFlags2
  0xE4, // Address 0x29 - RegRssiThresh
  0x00, // Address 0x2A - RegRxTimeout1
  0x00, // Address 0x2B - RegRxTimeout2
  0x00, // Address 0x2C - RegPreambleMsb
  0x03, // Address 0x2D - RegPreambleLsb
  0x90, // Address 0x2E - RegSyncConfig
  0xAA, // Address 0x2F - RegSyncValue1
  0x2D, // Address 0x30 - RegSyncValue2
  0xD4, // Address 0x31 - RegSyncValue3
  0x00, // Address 0x32 - RegSyncValue4
  0x00, // Address 0x33 - RegSyncValue5
  0x00, // Address 0x34 - RegSyncValue6
  0x00, // Address 0x35 - RegSyncValue7
  0x00, // Address 0x36 - RegSyncValue8
  0x00, // Address 0x37 - RegPacketConfig1
  0x1A, // Address 0x38 - RegPayloadLength
  0x00, // Address 0x39 - RegNodeAdrs
  0x00, // Address 0x3A - RegBroadcastAdrs
  0x00, // Address 0x3B - RegAutoModes
  0x19, // Address 0x3C - RegFifoThresh
  0x02, // Address 0x3D - RegPacketConfig2
  0x00, // Address 0x3E - RegAesKey1
  0x00, // Address 0x3F - RegAesKey2
  0x00, // Address 0x40 - RegAesKey3
  0x00, // Address 0x41 - RegAesKey4
  0x00, // Address 0x42 - RegAesKey5
  0x00, // Address 0x43 - RegAesKey6
  0x00, // Address 0x44 - RegAesKey7
  0x00, // Address 0x45 - RegAesKey8
  0x00, // Address 0x46 - RegAesKey9
  0x00, // Address 0x47 - RegAesKey10
  0x00, // Address 0x48 - RegAesKey11
  0x00, // Address 0x49 - RegAesKey12
  0x00, // Address 0x4A - RegAesKey13
  0x00, // Address 0x4B - RegAesKey14
  0x00, // Address 0x4C - RegAesKey15
  0x00, // Address 0x4D - RegAesKey16
  0x01, // Address 0x4E - RegTemp1
  0x00, // Address 0x4F - RegTemp2
  0x2D, // Address 0x58 - RegTestLna (Sensitivity boost)
  0x09, // Address 0x59 - RegTestTcxo
  0x08, // Address 0x5F - RegTestllBw (PLL Bandwidth setting)
  0x30, // Address 0x6F - RegTestDagc
  0x00, // Address 0x71 - RegTestAfc
}; // END SX1231 Bresser_5in1 register values
#endif

#ifdef Fine_Offset_WH57_868
const uint8_t Config_Fine_Offset_WH57_868[] PROGMEM = {
  // SX1231 register values for FineOffset_WH57
  0x00, // Address 0x00 - RegFifo (FIFO data input/output)
  0x04, // Address 0x01 - RegOpMode
  0x00, // Address 0x02 - RegDataModul
  0x07, // Address 0x03 - RegBitrateMsb
  0x3E, // Address 0x04 - RegBitrateLsb
  0x02, // Address 0x05 - RegFdevMsb
  0x3E, // Address 0x06 - RegFdevLsb
  0xD9, // Address 0x07 - RegFrfMsb
  0x16, // Address 0x08 - RegFrfMid
  0x66, // Address 0x09 - RegFrfLsb
  0x41, // Address 0x0A - RegOsc1
  0x00, // Address 0x0B - RegAfcCtrl
  0x02, // Address 0x0C - RegLowBat
  0x92, // Address 0x0D - RegListen1
  0xF5, // Address 0x0E - RegListen2
  0x20, // Address 0x0F - RegListen3
  0x24, // Address 0x10 - RegVersion
  0x9F, // Address 0x11 - RegPaLevel
  0x09, // Address 0x12 - RegPaRamp
  0x1A, // Address 0x13 - RegOcp
  0x40, // Address 0x14 - Reserved14
  0xB0, // Address 0x15 - Reserved15
  0x7B, // Address 0x16 - Reserved16
  0x9B, // Address 0x17 - Reserved17
  0x89, // Address 0x18 - RegLna
  0x43, // Address 0x19 - RegRxBw
  0x92, // Address 0x1A - RegAfcBw
  0x40, // Address 0x1B - RegOokPeak
  0x80, // Address 0x1C - RegOokAvg
  0x06, // Address 0x1D - RegOokFix
  0x10, // Address 0x1E - RegAfcFei
  0x00, // Address 0x1F - RegAfcMsb
  0x00, // Address 0x20 - RegAfcLsb
  0x00, // Address 0x21 - RegFeiMsb
  0x00, // Address 0x22 - RegFeiLsb
  0x02, // Address 0x23 - RegRssiConfig
  0xFF, // Address 0x24 - RegRssiValue
  0x00, // Address 0x25 - RegDioMapping1
  0x07, // Address 0x26 - RegDioMapping2
  0x80, // Address 0x27 - RegIrqFlags1
  0x00, // Address 0x28 - RegIrqFlags2
  0xE4, // Address 0x29 - RegRssiThresh
  0x00, // Address 0x2A - RegRxTimeout1
  0x00, // Address 0x2B - RegRxTimeout2
  0x00, // Address 0x2C - RegPreambleMsb
  0x03, // Address 0x2D - RegPreambleLsb
  0x90, // Address 0x2E - RegSyncConfig
  0xAA, // Address 0x2F - RegSyncValue1
  0x2D, // Address 0x30 - RegSyncValue2
  0xD4, // Address 0x31 - RegSyncValue3
  0x00, // Address 0x32 - RegSyncValue4
  0x00, // Address 0x33 - RegSyncValue5
  0x00, // Address 0x34 - RegSyncValue6
  0x00, // Address 0x35 - RegSyncValue7
  0x00, // Address 0x36 - RegSyncValue8
  0x00, // Address 0x37 - RegPacketConfig1
  0x09, // Address 0x38 - RegPayloadLength
  0x00, // Address 0x39 - RegNodeAdrs
  0x00, // Address 0x3A - RegBroadcastAdrs
  0x00, // Address 0x3B - RegAutoModes
  0x08, // Address 0x3C - RegFifoThresh
  0x02, // Address 0x3D - RegPacketConfig2
  0x00, // Address 0x3E - RegAesKey1
  0x00, // Address 0x3F - RegAesKey2
  0x00, // Address 0x40 - RegAesKey3
  0x00, // Address 0x41 - RegAesKey4
  0x00, // Address 0x42 - RegAesKey5
  0x00, // Address 0x43 - RegAesKey6
  0x00, // Address 0x44 - RegAesKey7
  0x00, // Address 0x45 - RegAesKey8
  0x00, // Address 0x46 - RegAesKey9
  0x00, // Address 0x47 - RegAesKey10
  0x00, // Address 0x48 - RegAesKey11
  0x00, // Address 0x49 - RegAesKey12
  0x00, // Address 0x4A - RegAesKey13
  0x00, // Address 0x4B - RegAesKey14
  0x00, // Address 0x4C - RegAesKey15
  0x00, // Address 0x4D - RegAesKey16
  0x01, // Address 0x4E - RegTemp1
  0x00, // Address 0x4F - RegTemp2
  0x2D, // Address 0x58 - RegTestLna (Sensitivity boost)
  0x09, // Address 0x59 - RegTestTcxo
  0x08, // Address 0x5F - RegTestllBw (PLL Bandwidth setting)
  0x30, // Address 0x6F - RegTestDagc
  0x00, // Address 0x71 - RegTestAfc
}; // END SX1231 FineOffset_WH57 register values
#endif

#ifdef Inkbird_IBS_P01R
const uint8_t Config_Inkbird_IBS_P01R[] PROGMEM = {
  // SX1231 register values for Inkbird_IBS-P01R
  0x00, // Address 0x00 - RegFifo (FIFO data input/output)
  0x04, // Address 0x01 - RegOpMode
  0x00, // Address 0x02 - RegDataModul
  0x0C, // Address 0x03 - RegBitrateMsb
  0x80, // Address 0x04 - RegBitrateLsb
  0x01, // Address 0x05 - RegFdevMsb
  0x37, // Address 0x06 - RegFdevLsb
  0x6C, // Address 0x07 - RegFrfMsb
  0x7A, // Address 0x08 - RegFrfMid
  0xE1, // Address 0x09 - RegFrfLsb
  0x41, // Address 0x0A - RegOsc1
  0x00, // Address 0x0B - RegAfcCtrl
  0x02, // Address 0x0C - RegLowBat
  0x92, // Address 0x0D - RegListen1
  0xF5, // Address 0x0E - RegListen2
  0x20, // Address 0x0F - RegListen3
  0x24, // Address 0x10 - RegVersion
  0x9F, // Address 0x11 - RegPaLevel
  0x09, // Address 0x12 - RegPaRamp
  0x1A, // Address 0x13 - RegOcp
  0x40, // Address 0x14 - Reserved14
  0xB0, // Address 0x15 - Reserved15
  0x7B, // Address 0x16 - Reserved16
  0x9B, // Address 0x17 - Reserved17
  0x89, // Address 0x18 - RegLna
  0x53, // Address 0x19 - RegRxBw
  0x8B, // Address 0x1A - RegAfcBw
  0x40, // Address 0x1B - RegOokPeak
  0x80, // Address 0x1C - RegOokAvg
  0x06, // Address 0x1D - RegOokFix
  0x10, // Address 0x1E - RegAfcFei
  0x00, // Address 0x1F - RegAfcMsb
  0x00, // Address 0x20 - RegAfcLsb
  0x00, // Address 0x21 - RegFeiMsb
  0x00, // Address 0x22 - RegFeiLsb
  0x02, // Address 0x23 - RegRssiConfig
  0xFF, // Address 0x24 - RegRssiValue
  0x00, // Address 0x25 - RegDioMapping1
  0x07, // Address 0x26 - RegDioMapping2
  0x80, // Address 0x27 - RegIrqFlags1
  0x00, // Address 0x28 - RegIrqFlags2
  0xE4, // Address 0x29 - RegRssiThresh
  0x00, // Address 0x2A - RegRxTimeout1
  0x00, // Address 0x2B - RegRxTimeout2
  0x00, // Address 0x2C - RegPreambleMsb
  0x03, // Address 0x2D - RegPreambleLsb
  0x90, // Address 0x2E - RegSyncConfig
  0xAA, // Address 0x2F - RegSyncValue1
  0x2D, // Address 0x30 - RegSyncValue2
  0xD4, // Address 0x31 - RegSyncValue3
  0x00, // Address 0x32 - RegSyncValue4
  0x00, // Address 0x33 - RegSyncValue5
  0x00, // Address 0x34 - RegSyncValue6
  0x00, // Address 0x35 - RegSyncValue7
  0x00, // Address 0x36 - RegSyncValue8
  0x08, // Address 0x37 - RegPacketConfig1
  0x12, // Address 0x38 - RegPayloadLength
  0x00, // Address 0x39 - RegNodeAdrs
  0x00, // Address 0x3A - RegBroadcastAdrs
  0x00, // Address 0x3B - RegAutoModes
  0x11, // Address 0x3C - RegFifoThresh
  0x02, // Address 0x3D - RegPacketConfig2
  0x00, // Address 0x3E - RegAesKey1
  0x00, // Address 0x3F - RegAesKey2
  0x00, // Address 0x40 - RegAesKey3
  0x00, // Address 0x41 - RegAesKey4
  0x00, // Address 0x42 - RegAesKey5
  0x00, // Address 0x43 - RegAesKey6
  0x00, // Address 0x44 - RegAesKey7
  0x00, // Address 0x45 - RegAesKey8
  0x00, // Address 0x46 - RegAesKey9
  0x00, // Address 0x47 - RegAesKey10
  0x00, // Address 0x48 - RegAesKey11
  0x00, // Address 0x49 - RegAesKey12
  0x00, // Address 0x4A - RegAesKey13
  0x00, // Address 0x4B - RegAesKey14
  0x00, // Address 0x4C - RegAesKey15
  0x00, // Address 0x4D - RegAesKey16
  0x01, // Address 0x4E - RegTemp1
  0x00, // Address 0x4F - RegTemp2
  0x2D, // Address 0x58 - RegTestLna (Sensitivity boost)
  0x09, // Address 0x59 - RegTestTcxo
  0x08, // Address 0x5F - RegTestllBw (PLL Bandwidth setting)
  0x30, // Address 0x6F - RegTestDagc
  0x00, // Address 0x71 - RegTestAfc
}; // END SX1231  Inkbird_IBS-P01R register values
#endif

#ifdef Lacrosse_mode1
const uint8_t Config_Lacrosse_mode1[] PROGMEM = {
  // SX1231 register values for Bresser 5-in-1
  0x00, // Address 0x00 - RegFifo (FIFO data input/output)
  0x04, // Address 0x01 - RegOpMode
  0x00, // Address 0x02 - RegDataModul
  0x07, // Address 0x03 - RegBitrateMsb
  0x3E, // Address 0x04 - RegBitrateLsb
  0x03, // Address 0x05 - RegFdevMsb
  0xD8, // Address 0x06 - RegFdevLsb
  0xD9, // Address 0x07 - RegFrfMsb
  0x13, // Address 0x08 - RegFrfMid
  0x33, // Address 0x09 - RegFrfLsb
  0x41, // Address 0x0A - RegOsc1
  0x00, // Address 0x0B - RegAfcCtrl
  0x02, // Address 0x0C - RegLowBat
  0x92, // Address 0x0D - RegListen1
  0xF5, // Address 0x0E - RegListen2
  0x20, // Address 0x0F - RegListen3
  0x24, // Address 0x10 - RegVersion
  0x9F, // Address 0x11 - RegPaLevel
  0x09, // Address 0x12 - RegPaRamp
  0x1A, // Address 0x13 - RegOcp
  0x40, // Address 0x14 - Reserved14
  0xB0, // Address 0x15 - Reserved15
  0x7B, // Address 0x16 - Reserved16
  0x9B, // Address 0x17 - Reserved17
  0x88, // Address 0x18 - RegLna
  0x4A, // Address 0x19 - RegRxBw
  0x91, // Address 0x1A - RegAfcBw
  0x40, // Address 0x1B - RegOokPeak
  0x80, // Address 0x1C - RegOokAvg
  0x06, // Address 0x1D - RegOokFix
  0x10, // Address 0x1E - RegAfcFei
  0x00, // Address 0x1F - RegAfcMsb
  0x00, // Address 0x20 - RegAfcLsb
  0x00, // Address 0x21 - RegFeiMsb
  0x00, // Address 0x22 - RegFeiLsb
  0x02, // Address 0x23 - RegRssiConfig
  0xFF, // Address 0x24 - RegRssiValue
  0x00, // Address 0x25 - RegDioMapping1
  0x07, // Address 0x26 - RegDioMapping2
  0x80, // Address 0x27 - RegIrqFlags1
  0x00, // Address 0x28 - RegIrqFlags2
  0xE4, // Address 0x29 - RegRssiThresh
  0x00, // Address 0x2A - RegRxTimeout1
  0x00, // Address 0x2B - RegRxTimeout2
  0x00, // Address 0x2C - RegPreambleMsb
  0x03, // Address 0x2D - RegPreambleLsb
  0x90, // Address 0x2E - RegSyncConfig
  0xAA, // Address 0x2F - RegSyncValue1
  0x2D, // Address 0x30 - RegSyncValue2
  0xD4, // Address 0x31 - RegSyncValue3
  0x00, // Address 0x32 - RegSyncValue4
  0x00, // Address 0x33 - RegSyncValue5
  0x00, // Address 0x34 - RegSyncValue6
  0x00, // Address 0x35 - RegSyncValue7
  0x00, // Address 0x36 - RegSyncValue8
  0x00, // Address 0x37 - RegPacketConfig1
  0x05, // Address 0x38 - RegPayloadLength
  0x00, // Address 0x39 - RegNodeAdrs
  0x00, // Address 0x3A - RegBroadcastAdrs
  0x00, // Address 0x3B - RegAutoModes
  0x04, // Address 0x3C - RegFifoThresh
  0x02, // Address 0x3D - RegPacketConfig2
  0x00, // Address 0x3E - RegAesKey1
  0x00, // Address 0x3F - RegAesKey2
  0x00, // Address 0x40 - RegAesKey3
  0x00, // Address 0x41 - RegAesKey4
  0x00, // Address 0x42 - RegAesKey5
  0x00, // Address 0x43 - RegAesKey6
  0x00, // Address 0x44 - RegAesKey7
  0x00, // Address 0x45 - RegAesKey8
  0x00, // Address 0x46 - RegAesKey9
  0x00, // Address 0x47 - RegAesKey10
  0x00, // Address 0x48 - RegAesKey11
  0x00, // Address 0x49 - RegAesKey12
  0x00, // Address 0x4A - RegAesKey13
  0x00, // Address 0x4B - RegAesKey14
  0x00, // Address 0x4C - RegAesKey15
  0x00, // Address 0x4D - RegAesKey16
  0x01, // Address 0x4E - RegTemp1
  0x00, // Address 0x4F - RegTemp2
  0x2D, // Address 0x58 - RegTestLna (Sensitivity boost)
  0x09, // Address 0x59 - RegTestTcxo
  0x08, // Address 0x5F - RegTestllBw (PLL Bandwidth setting)
  0x30, // Address 0x6F - RegTestDagc
  0x00, // Address 0x71 - RegTestAfc
}; // END SX1231 Lacrosse_mode1 register values
#endif

#ifdef Lacrosse_mode2
const uint8_t Config_Lacrosse_mode2[] PROGMEM = {
  // SX1231 register values for Bresser 5-in-1
  0x00, // Address 0x00 - RegFifo (FIFO data input/output)
  0x04, // Address 0x01 - RegOpMode
  0x00, // Address 0x02 - RegDataModul
  0x0D, // Address 0x03 - RegBitrateMsb
  0x06, // Address 0x04 - RegBitrateLsb
  0x01, // Address 0x05 - RegFdevMsb
  0xEB, // Address 0x06 - RegFdevLsb
  0xD9, // Address 0x07 - RegFrfMsb
  0x13, // Address 0x08 - RegFrfMid
  0x33, // Address 0x09 - RegFrfLsb
  0x41, // Address 0x0A - RegOsc1
  0x00, // Address 0x0B - RegAfcCtrl
  0x02, // Address 0x0C - RegLowBat
  0x92, // Address 0x0D - RegListen1
  0xF5, // Address 0x0E - RegListen2
  0x20, // Address 0x0F - RegListen3
  0x24, // Address 0x10 - RegVersion
  0x9F, // Address 0x11 - RegPaLevel
  0x09, // Address 0x12 - RegPaRamp
  0x1A, // Address 0x13 - RegOcp
  0x40, // Address 0x14 - Reserved14
  0xB0, // Address 0x15 - Reserved15
  0x7B, // Address 0x16 - Reserved16
  0x9B, // Address 0x17 - Reserved17
  0x89, // Address 0x18 - RegLna
  //  0x53, // Address 0x19 - RegRxBw (41.667 kHz)
  //  0x4B, // Address 0x19 - RegRxBw (50.000 kHz)
  0x4B, // Address 0x19 - RegRxBw (62.500 kHz)
  0x53, // Address 0x1A - RegAfcBw
  0x40, // Address 0x1B - RegOokPeak
  0x80, // Address 0x1C - RegOokAvg
  0x06, // Address 0x1D - RegOokFix
  0x10, // Address 0x1E - RegAfcFei
  0x00, // Address 0x1F - RegAfcMsb
  0x00, // Address 0x20 - RegAfcLsb
  0x00, // Address 0x21 - RegFeiMsb
  0x00, // Address 0x22 - RegFeiLsb
  0x02, // Address 0x23 - RegRssiConfig
  0xFF, // Address 0x24 - RegRssiValue
  0x00, // Address 0x25 - RegDioMapping1
  0x07, // Address 0x26 - RegDioMapping2
  0x80, // Address 0x27 - RegIrqFlags1
  0x00, // Address 0x28 - RegIrqFlags2
  0xE4, // Address 0x29 - RegRssiThresh
  0x00, // Address 0x2A - RegRxTimeout1
  0x00, // Address 0x2B - RegRxTimeout2
  0x00, // Address 0x2C - RegPreambleMsb
  0x03, // Address 0x2D - RegPreambleLsb
  0x90, // Address 0x2E - RegSyncConfig
  0xAA, // Address 0x2F - RegSyncValue1
  0x2D, // Address 0x30 - RegSyncValue2
  0xD4, // Address 0x31 - RegSyncValue3
  0x00, // Address 0x32 - RegSyncValue4
  0x00, // Address 0x33 - RegSyncValue5
  0x00, // Address 0x34 - RegSyncValue6
  0x00, // Address 0x35 - RegSyncValue7
  0x00, // Address 0x36 - RegSyncValue8
  0x00, // Address 0x37 - RegPacketConfig1
  0x05, // Address 0x38 - RegPayloadLength
  0x00, // Address 0x39 - RegNodeAdrs
  0x00, // Address 0x3A - RegBroadcastAdrs
  0x00, // Address 0x3B - RegAutoModes
  0x04, // Address 0x3C - RegFifoThresh
  0x02, // Address 0x3D - RegPacketConfig2
  0x00, // Address 0x3E - RegAesKey1
  0x00, // Address 0x3F - RegAesKey2
  0x00, // Address 0x40 - RegAesKey3
  0x00, // Address 0x41 - RegAesKey4
  0x00, // Address 0x42 - RegAesKey5
  0x00, // Address 0x43 - RegAesKey6
  0x00, // Address 0x44 - RegAesKey7
  0x00, // Address 0x45 - RegAesKey8
  0x00, // Address 0x46 - RegAesKey9
  0x00, // Address 0x47 - RegAesKey10
  0x00, // Address 0x48 - RegAesKey11
  0x00, // Address 0x49 - RegAesKey12
  0x00, // Address 0x4A - RegAesKey13
  0x00, // Address 0x4B - RegAesKey14
  0x00, // Address 0x4C - RegAesKey15
  0x00, // Address 0x4D - RegAesKey16
  0x01, // Address 0x4E - RegTemp1
  0x00, // Address 0x4F - RegTemp2
  0x2D, // Address 0x58 - RegTestLna (Sensitivity boost)
  0x09, // Address 0x59 - RegTestTcxo
  0x08, // Address 0x5F - RegTestllBw (PLL Bandwidth setting)
  0x30, // Address 0x6F - RegTestDagc
  0x00, // Address 0x71 - RegTestAfc
}; // END SX1231 Lacrosse_mode2 register values
#endif

#endif  // END - #ifdef RFM69
#endif  // END - #ifndef RFM69_H
