#pragma once

/*
    This file provides us with important information about the functional prototypes available to us,
    provides us with symbolic constants and keeps a list of all available macros.
*/

#include <Arduino.h>
#include <EEPROM.h>
#include <SPI.h>

int CC1101_readRSSI();
uint8_t CC1101_readReg(uint8_t regAddr, uint8_t regType);
uint8_t CC1101_cmdStrobe(uint8_t cmd);
void CC1101_init();
void CC1101_readBurstReg(uint8_t * buffer, uint8_t regAddr, uint8_t len);
void CC1101_writeBurstReg(byte * buffer, byte regAddr, byte len);
void CC1101_writeReg(uint8_t regAddr, uint8_t value);
void CC1101_writeRegFor(const uint8_t *reg_name, uint8_t reg_length, String reg_modus);
void CC1101_setTransmitMode();
void CC1101_setReceiveMode();
void CC1101_sendFIFO(char *startpos);

extern void InputCommand(char* In);
extern int RSSI_dez;
extern int8_t freqOffAcc;
extern float freqErrAvg;
extern boolean CC1101_found;
extern boolean freqAfc;
extern byte ToggleValues;
extern byte activated_mode_nr;              // activated protocol in flash
extern byte activated_mode_packet_length;
extern unsigned long ToggleTime;
extern byte ToggleOrder[4];
extern uint8_t buffer[75];

/** Command strobes */
#define CC1101_SRES              0x30        // Reset CC1101 chip
#define CC1101_SFSTXON           0x31        // Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1). If in RX (with CCA):
// Go to a wait state where only the synthesizer is running (for quick RX / TX turnaround).
#define CC1101_SXOFF             0x32        // Turn off crystal oscillator
#define CC1101_SCAL              0x33        // Calibrate frequency synthesizer and turn it off. SCAL can be strobed from IDLE mode without
// setting manual calibration mode (MCSM0.FS_AUTOCAL=0)
#define CC1101_SRX               0x34        // Enable RX. Perform calibration first if coming from IDLE and MCSM0.FS_AUTOCAL=1
#define CC1101_STX               0x35        // In IDLE state: Enable TX. Perform calibration first if MCSM0.FS_AUTOCAL=1.
// If in RX state and CCA is enabled: Only go to TX if channel is clear
#define CC1101_SIDLE             0x36        // Exit RX / TX, turn off frequency synthesizer and exit Wake-On-Radio mode if applicable
#define CC1101_SWOR              0x38        // Start automatic RX polling sequence (Wake-on-Radio) as described in Section 19.5 if
// WORCTRL.RC_PD=0
#define CC1101_SPWD              0x39        // Enter power down mode when CSn goes high
#define CC1101_SFRX              0x3A        // Flush the RX FIFO buffer. Only issue SFRX in IDLE or RXFIFO_OVERFLOW states
#define CC1101_SFTX              0x3B        // Flush the TX FIFO buffer. Only issue SFTX in IDLE or TXFIFO_UNDERFLOW states
#define CC1101_SWORRST           0x3C        // Reset real time clock to Event1 value
#define CC1101_SNOP              0x3D        // No operation. May be used to get access to the chip status byte


/** Type of transfers (Table 45: SPI Address Space) */
#define WRITE_BURST              0x40
#define READ_SINGLE              0x80
#define READ_BURST               0xC0


/** PATABLE & FIFO's */
#define CC1101_PATABLE           0x3E        // PATABLE address
#define CC1101_TXFIFO            0x3F        // TX FIFO address
#define CC1101_RXFIFO            0x3F        // RX FIFO address


/** CC1101 configuration registers */
#define CC1101_IOCFG2            0x00        // GDO2 Output Pin Configuration
#define CC1101_IOCFG1            0x01        // GDO1 Output Pin Configuration
#define CC1101_IOCFG0            0x02        // GDO0 Output Pin Configuration
#define CC1101_FIFOTHR           0x03        // RX FIFO and TX FIFO Thresholds
#define CC1101_SYNC1             0x04        // Sync Word, High Byte
#define CC1101_SYNC0             0x05        // Sync Word, Low Byte
#define CC1101_PKTLEN            0x06        // Packet Length
#define CC1101_PKTCTRL1          0x07        // Packet Automation Control
#define CC1101_PKTCTRL0          0x08        // Packet Automation Control
#define CC1101_ADDR              0x09        // Device Address
#define CC1101_CHANNR            0x0A        // Channel Number
#define CC1101_FSCTRL1           0x0B        // Frequency Synthesizer Control
#define CC1101_FSCTRL0           0x0C        // Frequency Synthesizer Control
#define CC1101_FREQ2             0x0D        // Frequency Control Word, High Byte
#define CC1101_FREQ1             0x0E        // Frequency Control Word, Middle Byte
#define CC1101_FREQ0             0x0F        // Frequency Control Word, Low Byte
#define CC1101_MDMCFG4           0x10        // Modem Configuration
#define CC1101_MDMCFG3           0x11        // Modem Configuration
#define CC1101_MDMCFG2           0x12        // Modem Configuration
#define CC1101_MDMCFG1           0x13        // Modem Configuration
#define CC1101_MDMCFG0           0x14        // Modem Configuration
#define CC1101_DEVIATN           0x15        // Modem Deviation Setting
#define CC1101_MCSM2             0x16        // Main Radio Control State Machine Configuration
#define CC1101_MCSM1             0x17        // Main Radio Control State Machine Configuration
#define CC1101_MCSM0             0x18        // Main Radio Control State Machine Configuration
#define CC1101_FOCCFG            0x19        // Frequency Offset Compensation Configuration
#define CC1101_BSCFG             0x1A        // Bit Synchronization Configuration
#define CC1101_AGCCTRL2          0x1B        // AGC Control
#define CC1101_AGCCTRL1          0x1C        // AGC Control
#define CC1101_AGCCTRL0          0x1D        // AGC Control
#define CC1101_WOREVT1           0x1E        // High Byte Event0 Timeout
#define CC1101_WOREVT0           0x1F        // Low Byte Event0 Timeout
#define CC1101_WORCTRL           0x20        // Wake On Radio Control
#define CC1101_FREND1            0x21        // Front End RX Configuration
#define CC1101_FREND0            0x22        // Front End TX Configuration
#define CC1101_FSCAL3            0x23        // Frequency Synthesizer Calibration
#define CC1101_FSCAL2            0x24        // Frequency Synthesizer Calibration
#define CC1101_FSCAL1            0x25        // Frequency Synthesizer Calibration
#define CC1101_FSCAL0            0x26        // Frequency Synthesizer Calibration
#define CC1101_RCCTRL1           0x27        // RC Oscillator Configuration
#define CC1101_RCCTRL0           0x28        // RC Oscillator Configuration
#define CC1101_FSTEST            0x29        // Frequency Synthesizer Calibration Control
#define CC1101_PTEST             0x2A        // Production Test
#define CC1101_AGCTEST           0x2B        // AGC Test
#define CC1101_TEST2             0x2C        // Various Test Settings
#define CC1101_TEST1             0x2D        // Various Test Settings
#define CC1101_TEST0             0x2E        // Various Test Settings


/** Status registers */
#define CC1101_PARTNUM           0x30        // Chip ID
#define CC1101_VERSION           0x31        // Chip Version
#define CC1101_FREQEST           0x32        // Frequency Offset Estimate from Demodulator
#define CC1101_LQI               0x33        // Demodulator Estimate for Link Quality
#define CC1101_RSSI              0x34        // Received Signal Strength Indication
#define CC1101_MARCSTATE         0x35        // Main Radio Control State Machine State
#define CC1101_WORTIME1          0x36        // High Byte of WOR Time
#define CC1101_WORTIME0          0x37        // Low Byte of WOR Time
#define CC1101_PKTSTATUS         0x38        // Current GDOx Status and Packet Status
#define CC1101_VCO_VC_DAC        0x39        // Current Setting from PLL Calibration Module
#define CC1101_TXBYTES           0x3A        // Underflow and Number of Bytes
#define CC1101_RXBYTES           0x3B        // Overflow and Number of Bytes
#define CC1101_RCCTRL1_STATUS    0x3C        // Last RC Oscillator Calibration Result
#define CC1101_RCCTRL0_STATUS    0x3D        // Last RC Oscillator Calibration Result 


/** Chip States */
#define CC1101_STATE_IDLE                      0x00
#define CC1101_STATE_RX                        0x10
#define CC1101_STATE_TX                        0x20
#define CC1101_STATE_FSTXON                    0x30
#define CC1101_STATE_CALIBRATE                 0x40
#define CC1101_STATE_SETTLING                  0x50
#define CC1101_STATE_RX_OVERFLOW               0x60
#define CC1101_STATE_TX_UNDERFLOW              0x70


/* Bit fields in the Chip status byte */
#define CC1101_STATUS_CHIP_RDYn_BM             0x80
#define CC1101_STATUS_STATE_BM                 0x70
#define CC1101_STATUS_FIFO_BYTES_AVAILABLE_BM  0x0F


/**
   CC1101 configuration registers - Default values extracted from SmartRF Studio

   Configuration:
   Frequency: 868.299866         -> FREQ2, FREQ1, FREQ0
   Modulation format: 2-FSK      -> MDMCFG2
   Deviation: 88.867188          -> DEVIATN
   Data rate: 17257.69 Kbps      -> MDMCFG4, MDMCFG3
*/
#define CC1101_VAL_IOCFG2        0x01
#define CC1101_VAL_IOCFG1        0x2E
#define CC1101_VAL_IOCFG0        0x2E
#define CC1101_VAL_FIFOTHR       0x41
#define CC1101_VAL_SYNC1         0x2D
#define CC1101_VAL_SYNC0         0xD4
#define CC1101_VAL_PKTLEN        0x05
#define CC1101_VAL_PKTCTRL1      0x80
#define CC1101_VAL_PKTCTRL0      0x00
#define CC1101_VAL_ADDR          0x00
#define CC1101_VAL_CHANNR        0x00
#define CC1101_VAL_FSCTRL1       0x06
#define CC1101_VAL_FSCTRL0       0x00
#define CC1101_VAL_FREQ2         0x21
#define CC1101_VAL_FREQ1         0x65
#define CC1101_VAL_FREQ0         0x6A
#define CC1101_VAL_MDMCFG4       0x89
#define CC1101_VAL_MDMCFG3       0x5C
#define CC1101_VAL_MDMCFG2       0x02
#define CC1101_VAL_MDMCFG1       0x22
#define CC1101_VAL_MDMCFG0       0xF8
#define CC1101_VAL_DEVIATN       0x56
#define CC1101_VAL_MCSM2         0x07
#define CC1101_VAL_MCSM1         0x00
#define CC1101_VAL_MCSM0         0x18
#define CC1101_VAL_FOCCFG        0x16
#define CC1101_VAL_BSCFG         0x6C
#define CC1101_VAL_AGCCTRL2      0x43
#define CC1101_VAL_AGCCTRL1      0x68
#define CC1101_VAL_AGCCTRL0      0x91
#define CC1101_VAL_WOREVT1       0x87
#define CC1101_VAL_WOREVT0       0x6B
#define CC1101_VAL_WORCTRL       0xF8
#define CC1101_VAL_FREND1        0xB6
#define CC1101_VAL_FREND0        0x11
#define CC1101_VAL_FSCAL3        0xEC
#define CC1101_VAL_FSCAL2        0x2C
#define CC1101_VAL_FSCAL1        0x18
#define CC1101_VAL_FSCAL0        0x11
#define CC1101_VAL_RCCTRL1       0x41
#define CC1101_VAL_RCCTRL0       0x00
#define CC1101_VAL_FSTEST        0x59
#define CC1101_VAL_PTEST         0x7F
#define CC1101_VAL_AGCTEST       0x3E
#define CC1101_VAL_TEST2         0x88
#define CC1101_VAL_TEST1         0x31
#define CC1101_VAL_TEST0         0x0B
