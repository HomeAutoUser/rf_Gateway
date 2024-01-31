/*

  - Arduino Nano OHNE debug´s | FreeRam -> 993
  Der Sketch verwendet 22482 Bytes (73%) des Programmspeicherplatzes. Das Maximum sind 30720 Bytes.
  Globale Variablen verwenden 704 Bytes (34%) des dynamischen Speichers, 1344 Bytes für lokale Variablen verbleiben. Das Maximum sind 2048 Bytes.

  - Arduino Pro / Arduino Pro Mini OHNE debug´s | FreeRam -> 956
  Der Sketch verwendet 22574 Bytes (73%) des Programmspeicherplatzes. Das Maximum sind 30720 Bytes.
  Globale Variablen verwenden 704 Bytes (34%) des dynamischen Speichers, 1344 Bytes für lokale Variablen verbleiben. Das Maximum sind 2048 Bytes.

  - Arduino radino CC1101 OHNE debug´s | FreeRam -> ?
  Der Sketch verwendet 24890 Bytes (86%) des Programmspeicherplatzes. Das Maximum sind 28672 Bytes.
  Globale Variablen verwenden 675 Bytes des dynamischen Speichers.

  - ESP8266 OHNE debug´s (alle Protokolle) | FreeRam -> 35496
  . Variables and constants in RAM (global, static), used 39068 / 80192 bytes (48%)
  ║   SEGMENT  BYTES    DESCRIPTION
  ╠══ DATA     1812     initialized variables
  ╠══ RODATA   4528     constants
  ╚══ BSS      32728    zeroed variables
  . Instruction RAM (IRAM_ATTR, ICACHE_RAM_ATTR), used 61555 / 65536 bytes (93%)
  ║   SEGMENT  BYTES    DESCRIPTION
  ╠══ ICACHE   32768    reserved space for flash instruction cache
  ╚══ IRAM     28787    code in IRAM
  . Code in flash (default, ICACHE_FLASH_ATTR), used 421992 / 1048576 bytes (40%)
  ║   SEGMENT  BYTES    DESCRIPTION
  ╚══ IROM     421992   code in flash

  - ESP32 OHNE debug´s (alle Protokolle) | FreeRam -> 235052
  Der Sketch verwendet 951885 Bytes (72%) des Programmspeicherplatzes. Das Maximum sind 1310720 Bytes.
  Globale Variablen verwenden 51620 Bytes (15%) des dynamischen Speichers, 276060 Bytes für lokale Variablen verbleiben. Das Maximum sind 327680 Bytes.

  - !!! ein Register ca. 82 Bytes des Programmspeicherplatzes & 82 Bytes Globale Variablen !!!

  commands:
  ?                 - available commands
  e                 - set default registers
  fafc<n>           - frequency automatic control | valid value 0 (off) or 1 (on)
  foff<n>           - frequency offset
  ft                - frequency testsignal SN;R=99;D=FF;
  m<0-6>            - register´s in Firmware (Avantek, Lacrosse, Bresser ...)
  m<arraysizemax>   - "Developer gadget to test functions"
  t                 - get uptime
  tob<0-3><0-6>     - set togglebank (0-3) to value | <n> bank , <n> mode or 99 | 99 reset togglebank n (set to -)
  tob88             - scan all modes
  tob99             - reset togglebank              | set to { - | - | - | - }
  tos<n>            - toggletime in milliseconds (min defined in config.h)
  x                 - write cc110x_patable
  C                 - reads complete CC110x register
  C<n>              - reads one register adress (0-61)
  C99               - get ccreg99
  C3E               - get ccpatable
  C0DnF             - get ccconf
  CEA, CDA          - enable / disable automatic frequency control for FSK modulation
  E                 - reads complete EEPROM
  I                 - current status
  M                 - all available registers (mode´s)
  P                 - PING
  R                 - get FreeRam
  SN                - send data
  V                 - get Version
  W                 - write register
  WS                - write Strobes Command

  EEPROM addresses: ... see config.h ...

  notes ToDO:
  - ? wenn dev Protokolle aktiv, diese bei ScanAll überspringen
  - ? MQTT Weiterleitung

  https://www.ti.com/lit/ds/symlink/cc1101.pdf
  https://www.shotech.de/Datasheet/semtech/sx1231.pdf

*/

#ifndef NOTES_H
#define NOTES_H
#endif
