#pragma once

#ifdef CC110x
/*
    This file provides us with important information about the functional prototypes available to us,
    provides us with symbolic constants and keeps a list of all available macros.
*/

#include <Arduino.h>
#include <digitalWriteFast.h>           // https://github.com/ArminJo/digitalWriteFast
#include <EEPROM.h>
#include <SPI.h>

static const char PROGMEM RECEIVE_MODE_USER[] = "CC110x user configuration";

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
void CC110x_sendFIFO(char *startpos);
byte Chip_Bandw_cal(float input);
void Chip_Datarate_Set(long datarate, byte * arr);
byte CC110x_Deviation_Set(float deviation);
byte web_Mod_set(byte input);

extern void InputCommand(String input);
extern int RSSI_dez;
extern int8_t freqOffAcc;
extern float freqErrAvg;
extern boolean ChipFound;
extern uint8_t freqAfc;
extern byte ToggleValues;
extern byte ReceiveModeNr;                   // activated protocol in flash
extern byte ReceiveModePKTLEN;
extern unsigned long ToggleTime;
extern byte ToggleOrder[4];

#define CHIP_NAME                 "CC110x"              // name Chip
#define CHIP_RFNAME               "cc110x_rf_Gateway"   // name web interface
#define REGISTER_MAX              46                    // register count
#define CHIP_RxBw                0x10                   // Modem Configuration ... (BW & DRate)
#define CHIP_BitRate             0x10                   // first BitRate/DataRate address

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

#endif
