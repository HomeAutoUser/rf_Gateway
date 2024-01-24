#pragma once

#ifdef RFM69

#include <Arduino.h>
#include <digitalWriteFast.h>             // https://github.com/ArminJo/digitalWriteFast
#include <EEPROM.h>
#include <SPI.h>

//#define FXOSC_28 // SX1231 FXOSC 28,375 MHz, sonst RFM69 FXOSC 32,000 MHz
#define fxOsc 32000000                          // Crystal oscillator frequency
const float fStep = fxOsc / pow(2, 19);         // Frequency synthesizer step
#define CHIP_NAME         "RFM69"               // name Chip
#define CHIP_RFNAME       "rfm69_rf_Gateway"    // name web interface
#define REGISTER_MAX      84                    // register count

#define READ_BURST        0x00                  // for compatibility with the CC110x
#define CHIP_VERSION      0x10                  // RegVersion - Semtech ID relating the silicon revision
#define CHIP_RXFIFO       0x00                  // RX FIFO address
#define CHIP_PKTLEN       0x06                  // Packet Length address
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
void SX1231_start_idle(); // SX1231 start idle mode
uint8_t SX1231_setOperatingMode(uint8_t ModeNew, uint8_t Mode, uint8_t RegOpMode);
void SX1231_afc(uint8_t freqAfc); // AfcAutoOn, 0  AFC is performed each time AfcStart is set, 1  AFC is performed each time Rx mode is entered
void SX1231_read_reg_all();  // SX1231 read all 112 register
void SX1231_Deviation_Set(float deviation, byte * arr); // calculate register values (RegFdevMsb 0x5, RegFdevLsb 0x06) for frequency deviation
  
extern boolean ChipFound;
extern int RSSI_dez;
extern uint8_t freqAfc;
extern byte ReceiveModePKTLEN;
extern byte ReceiveModeNr;                   // activated protocol in flash
extern byte ToggleValues;
extern byte ToggleOrder[4];
extern unsigned long ToggleTime;
#endif
