#include "config.h"
#include "register.h"

/* all available CC1101 registers to set */
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
    Packet Length = 22
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
  0x16,  // PKTLEN              Packet Length
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
    Base Frequency = 869.259949
    CRC Autoflush = false
    CRC Enable = false
    Carrier Frequency = 869.259949
    Channel Number = 0
    Channel Spacing = 199.951172
    Data Format = Normal mode
    Data Rate = 9.9926
    Deviation = 57.128906
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
  0x6E,  // FREQ1               Frequency Control Word, Middle Byte
  0xDE,  // FREQ0               Frequency Control Word, Low Byte
  0xB8,  // MDMCFG4             Modem Configuration
  0x93,  // MDMCFG3             Modem Configuration
  0x12,  // MDMCFG2             Modem Configuration
  0x22,  // MDMCFG1             Modem Configuration
  0xF8,  // MDMCFG0             Modem Configuration
  0x51,  // DEVIATN             Modem Deviation Setting
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


/*
  Register array with all assignments ( define the array of structs )

  [x] is de array pos of Registers

  access to reg value´s                     -> Registers[x].reg_val[x]
  access to reg length                      -> Registers[x].length
  access to reg name TXT                    -> Registers[x].name
  access to packet_length from protocol     -> Registers[x].packet_length

  Registername        Registerlength,   Name from           Packet
  to access values                      register to View    length

  reg_val             length            name                packet_length

*/

struct Data Registers[] = {
  { Config_Default, sizeof(Config_Default) / sizeof(Config_Default[0]), "CC110x Factory Default", 32  },
#ifdef Avantek
  { Config_Avantek, sizeof(Config_Avantek) / sizeof(Config_Avantek[0]), "Avantek",  8   },
#endif
#ifdef Bresser_5in1
  { Config_Bresser_5in1,  sizeof(Config_Bresser_5in1) / sizeof(Config_Bresser_5in1[0]), "Bresser_5in1", 26  },
#endif
#ifdef Bresser_6in1
  { Config_Bresser_6in1,  sizeof(Config_Bresser_6in1) / sizeof(Config_Bresser_6in1[0]), "Bresser_6in1", 18  },
#endif
#ifdef Bresser_7in1
  { Config_Bresser_7in1,  sizeof(Config_Bresser_7in1) / sizeof(Config_Bresser_7in1[0]), "Bresser_7in1", 22  },
#endif
#ifdef Fine_Offset_WH51_434
  { Config_Fine_Offset_WH51_434,  sizeof(Config_Fine_Offset_WH51_434) / sizeof(Config_Fine_Offset_WH51_434[0]), "Fine_Offset_WH51_434", 14  },
#endif
#ifdef Fine_Offset_WH51_868
  { Config_Fine_Offset_WH51_868,  sizeof(Config_Fine_Offset_WH51_868) / sizeof(Config_Fine_Offset_WH51_868[0]), "Fine_Offset_WH51_868", 14  },
#endif
#ifdef Fine_Offset_WH57_434
  { Config_Fine_Offset_WH57_434,  sizeof(Config_Fine_Offset_WH57_434) / sizeof(Config_Fine_Offset_WH57_434[0]), "Fine_Offset_WH57_434", 9  },
#endif
#ifdef Fine_Offset_WH57_868
  { Config_Fine_Offset_WH57_868,  sizeof(Config_Fine_Offset_WH57_868) / sizeof(Config_Fine_Offset_WH57_868[0]), "Fine_Offset_WH57_868", 9  },
#endif
#ifdef Inkbird_IBS_P01R
  { Config_Inkbird_IBS_P01R,  sizeof(Config_Inkbird_IBS_P01R) / sizeof(Config_Inkbird_IBS_P01R[0]), "Inkbird_IBS_P01R", 18  },
#endif
#ifdef KOPP_FC
  { Config_KOPP_FC,  sizeof(Config_KOPP_FC) / sizeof(Config_KOPP_FC[0]), "KOPP_FC", 15  },
#endif
#ifdef Lacrosse_mode1
  { Config_Lacrosse_mode1,  sizeof(Config_Lacrosse_mode1) / sizeof(Config_Lacrosse_mode1[0]), "Lacrosse_mode1", 5   },
#endif
#ifdef Lacrosse_mode2
  { Config_Lacrosse_mode2,  sizeof(Config_Lacrosse_mode2) / sizeof(Config_Lacrosse_mode2[0]), "Lacrosse_mode2", 5   },
#endif
#ifdef OOK_MU_433
  { Config_OOK_MU_433,  sizeof(Config_OOK_MU_433) / sizeof(Config_OOK_MU_433[0]), "OOK_MU_433", 5  },
#endif
#ifdef PCA301
  { Config_PCA301,  sizeof(Config_PCA301) / sizeof(Config_PCA301[0]), "PCA301", 32  },
#endif
#ifdef Rojaflex
  { Config_Rojaflex,  sizeof(Config_Rojaflex) / sizeof(Config_Rojaflex[0]), "Rojaflex", 12  },
#endif
#ifdef X_Sense
  { Config_X_Sense,  sizeof(Config_X_Sense) / sizeof(Config_X_Sense[0]), "X_Sense", 6  },
#endif

  /* under development */
#ifdef HomeMatic
  { Config_HomeMatic,  sizeof(Config_HomeMatic) / sizeof(Config_HomeMatic[0]), "under dev - HomeMatic", 12  },
#endif
#ifdef Lacrosse_mode3
  { Config_Lacrosse_mode3,  sizeof(Config_Lacrosse_mode3) / sizeof(Config_Lacrosse_mode3[0]), "Lacrosse_mode3", 5   },
#endif
#ifdef MAX
  { Config_MAX,  sizeof(Config_MAX) / sizeof(Config_MAX[0]), "under dev - MAX", 12  },
#endif
#ifdef WMBus_S
  { Config_WMBus_S,  sizeof(Config_WMBus_S) / sizeof(Config_WMBus_S[0]), "under dev - WMBus_S", 12  },
#endif
#ifdef WMBus_T
  { Config_WMBus_T,  sizeof(Config_WMBus_T) / sizeof(Config_WMBus_T[0]), "under dev - WMBus_T", 12  },
#endif
  /* under development END */
};

String activated_mode_name;                                           // name of active mode from array
float Freq_offset = 0;                                                /* cc110x - Frequency offset */
uint8_t RegistersCntMax = sizeof(Registers) / sizeof(Registers[0]);   // size of -> struct Data Registers array
uint8_t ToggleArray[4] = {255, 255, 255, 255};
