#pragma once

/*
    This file provides us with important information about the functional prototypes available to us,
    provides us with symbolic constants and keeps a list of all available macros.
*/

#include <Arduino.h>
#include "wlan.h"

extern void InputCommand(String input);
extern String onlyDecToHex2Digit(byte Dec);
extern uint8_t freqAfc;
extern void appendLogFile(String logText);

/* predefinitions of the functions */
void WebSocket_chip();                        /* CC110x Factory Default,1,{ - - - - },0 */
void WebSocket_detail(byte variant);          /* 07,2E,2E,47,D3,91,FF,04,45,00,00,0F,00,1E,C4,EC,8C,22,02,22,F8,47,07,30,04,36,6C,03,40,91,87,6B,F8,56,10,EA,2A,00,1F,41,00,59,7F,3F,88,31,0B,detail,CC110x Factory Default,0.000 */
void WebSocket_help();                        /* ??? */
void WebSocket_index();                       /* CC110x,229556,277,0,0,-32 */
void WebSocket_modes();                       /* MODE,Lacrosse_mode1,11 */
void WebSocket_raw(const String & html_raw);  /* RAW,Lacrosse_mode1,9203816AB3,-98,-22 */
void routing_websites();
#ifdef CC110x
void web_detail_cc110x_export();
void web_detail_cc110x_import();
#elif RFM69
void web_detail_SX1231_export();
#endif
void web_chip();
void web_detail();
void web_index();
void web_log();
void web_modes();
void web_raw();
void web_wlan();
String HTML_mod(String txt);
void sendHtml(String & str);


/* Display of all websites from the web server
  https://wiki.selfhtml.org/wiki/HTML/Tutorials/Grundger%C3%BCst
  https://divtable.com/table-styler/
*/

byte RegBeforeChange[REGISTER_MAX + 1] = {};

const char html_meta[] PROGMEM = { "<!DOCTYPE html>"          /* added meta to html */
                                   "<html lang=\"de\">"
                                   "<head>"
                                   "<meta charset=\"utf-8\">"
                                   "<meta name=\"viewport\" content=\"width=device-width\">"
                                   "<link rel=\"stylesheet\" type=\"text/css\" href=\"css/all.css\">"
                                   "<title>" CHIP_RFNAME"</title>"
                                 };


const char html_head_table[] PROGMEM = { "<table>"          /* added table on head with all links */
                                         "<tr><th class=\"hth\" colspan=\"6\">" CHIP_RFNAME"</th></tr><tr>"
                                         "<td class=\"htd\"><a href=\"/\" class=\"HOME\">HOME</a></td>"
                                         "<td class=\"htd\"><a href=\"chip\" class=\"chip\">" CHIP_NAME"</a></td>"
                                         "<td class=\"htd\"><a href=\"html/"
#ifdef CC110x
                                         "help_cc110x.html"
#elif RFM69
                                         "help_rfm69.html"
#else
                                         "help_unknown.html"
#endif
                                         "\" class=\"Help\">Help</a></td>"
                                         "<td class=\"htd\"><a href=\"log\" class=\"Logfile\">Logfile</a></td>"
                                         "<td class=\"htd\"><a href=\"raw\" class=\"RAW\">RAW data</a></td>"
                                         "<td class=\"htd\"><a href=\"wlan\" class=\"WLAN\">WLAN</a></td></tr>"
                                         "</table><br>"
                                       };


void web_index() {
  String submit = HttpServer.arg("submit");               // welcher Button wurde betätigt
  String website = FPSTR(html_meta);
  website.reserve(2000);

  if (submit == "MSG0") {
    msgCount = 0;
    uptimeReset = uptime;
    appendLogFile(F("Message counter reset"));
  } else if (submit == "SWRESET") {
    ESP.restart();
  }

  website += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/index.css\">"
               "<script src=\"js/index.js\"></script>" // The type attribute is unnecessary for JavaScript resources.
               "</head>");
  website += FPSTR(html_head_table);
  website = HTML_mod(website);
  website += F("<body>"
               "<form method=\"post\">" /* form method wichtig für Daten von Button´s !!! https://www.w3schools.com/tags/ref_httpmethods.asp */
               "<table><tr><th colspan=2>Device - Information</th></tr>"
               "<tr><td class=\"tdf\">Firmware compiled</td><td>");
  website += compile_date;
  website += F("</td></tr><tr><td>Firmware mode</td><td>"
#ifdef SIGNALduino_comp
               "FHEM compatible"
#else
               "standalone"
#endif
               "</td></tr><tr>"
               "<td>Message count</td><td><span id=\"MSGcnt\">");
  website += msgCount;
  website += F("</span>"
               "<button class=\"btn\" type=\"submit\" name=\"submit\" value=\"MSG0\">RESET Counter</button></td></tr><tr><td>RAM (free heap)</td><td><span id=\"RAM\">");
  website += freeRam();
  website += F("</span></td></tr>"
               "<tr><td>RSSI (WLAN connection)</td><td><span id=\"WLANdB\">");
  website += WiFi.RSSI();

#ifdef ARDUINO_ARCH_ESP8266
  website += F("</span> dB</td></tr><tr><td>Supply voltage</td><td>");
  website += String(ESP.getVcc() / 1000.0, 2);  // using a float and 2 decimal places
  website += F(" Volt");
#elif ARDUINO_ARCH_ESP32
  website += F("</span> dB");
#endif
  website += F("</td></tr><tr><td>Uptime (Seconds)</td><td><span id=\"Uptime\">");
  website += uptime;
  website += F("</span>"
               "<button class=\"btn\" type=\"submit\" name=\"submit\" value=\"SWRESET\" onClick=\"refresh()\">RESTART ESP</button>"
               "</td></tr><tr><td>Uptime (Text)</td><td><span id=\"UptimeTxT\"></span></td></tr><tr><td>Version</td><td>");
  website += FPSTR(TXT_VERSION);
  website += F("</td></tr></table></form></body></html>");
  sendHtml(website);
}


void web_chip() {
  String website;
  website.reserve(2000);
  website += FPSTR(html_meta);
  website += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/chip.css\">"
               "<script src=\"js/chip.js\"></script>"
               "</head>");
  website += FPSTR(html_head_table);
  website = HTML_mod(website);
  website += F("<body>"
               "<table>"
               "<tr>");

  if (ChipFound) {
    website += F("<th class=\"thf\">general information</th>"
                 "<th class=\"thf\"><a href=\"detail\">detail information</a></th>"
                 "<th class=\"thf\"><a href=\"modes\">receive modes</a></th>");
  } else {
    website += F("<th colspan=\"3\">general information</th>");
  }

  website += F("</tr><tr><td>chip detected</td><td colspan=\"2\">");
  ChipFound ? website += F("yes") : website += F("no");
  website += F("</td></tr>");

  if (ChipFound) {
#ifdef CC110x
    website += F("<tr><td>chip PARTNUM</td><td colspan=\"2\">");
    website += onlyDecToHex2Digit(Chip_readReg(CC110x_PARTNUM, READ_BURST));
    website += F("</td></tr>");
#endif
    website += F("<tr><td>chip VERSION</td><td colspan=\"2\">");
    website += onlyDecToHex2Digit(Chip_readReg(CHIP_VERSION, READ_BURST));
    website += F("</td></tr><tr><td>chip STATE</td><td colspan=\"2\"><span id=\"MS\">");
#ifdef CC110x
    website += Chip_readReg(CC110x_MARCSTATE, READ_BURST);
#elif RFM69
    website += (Chip_readReg(0x01, 0x00) & 0b00011100) >> 2;
#endif
    website += F("</span></td></tr><tr><td>chip reception mode</td><td colspan=\"2\"><span id=\"MODE\">");
    website += ReceiveModeName;
    website += F("</span></td></tr><tr><td>ToggleBank 0-3</td><td colspan=\"2\"><span id=\"ToggleBank\">{&emsp;");

    for (byte i = 0; i < 4; i++) {
      if (ToggleArray[i] == 255) {
        website += '-';
      } else {
        website += ToggleArray[i];
      }
      website += F("&emsp;");
    }
    website += F("}</span></td></tr><tr><td>ToggleTime (ms)</td><td colspan=\"2\"><span id=\"Time\">");
    website += ToggleTime;
    website += F("</span></td></tr>");
  }

  website += F("</table></body></html>");
  sendHtml(website);
}


void web_modes() {
  if (!ChipFound) {
    HttpServer.send(404, "text/plain", F("Website not found !!!"));
  }
  String InputCmd;                                        // preparation for processing | control html InputCommand
  String submit = HttpServer.arg("submit");               // welcher Button wurde betätigt
  String tb = HttpServer.arg("tgb");
  unsigned long tgtime = HttpServer.arg("tgt").toInt();   // toggle time
  String web_status = F("<tr id=\"trLast\"><td class=\"ac\"><span id=\"stat\"></span></td>");
  uint8_t countargs = HttpServer.args();                  // Anzahl Argumente
  uint8_t tb_nr;                                          // togglebank nr
  uint8_t tb_val;                                         // togglebank value

#ifdef debug_html
  Serial.println(F("###########################"));
  Serial.print(F("[DB] web_modes, submit ")); Serial.println(submit);
  Serial.print(F("[DB] web_modes, tb ")); Serial.println(tb);
  Serial.print(F("[DB] web_modes, tgtime ")); Serial.println(tgtime);
  Serial.print(F("[DB] web_modes, count ")); Serial.println(countargs);
#endif

  if (countargs != 0) {
    if (submit != "" && submit != "time") {  // button "enable reception"
      InputCmd = "m";
      InputCmd += submit;

#ifdef debug_html
      Serial.print(F("[DB] web_modes, set reception ")); Serial.println(submit);
#endif
      web_status = F("<td class=\"in grn\">");
      web_status += Registers[submit.toInt()].name;
      web_status += F(" &#10004; | &#128721; toogle</td>");
    }

    if (submit != "" && submit == "time") {  // button "START"
      if (tgtime == 0) {
        tgtime = ToggleTime;
      }
      InputCmd = F("tos");
      InputCmd += tgtime;
#ifdef debug_html
      Serial.print(F("[DB] web_modes, set toggletime to ")); Serial.println(tgtime);
#endif
      if (tgtime >= ToggleTimeMin && tgtime <= ToggleTimeMax) {
        web_status = F("<tr><td class=\"in grn\"><span id=\"stat\">toggle started &#10004;</span></td>");
      } else {
        web_status = F("<tr><td class=\"in red\"><span id=\"stat\">toggle value ");
        if (tgtime < ToggleTimeMin) {
          web_status += F("< ");
          web_status += ToggleTimeMin;
        } else if (tgtime > ToggleTimeMax) {
          web_status += F("> ");
          web_status += ToggleTimeMax;
        }
        web_status += F(" &#10006;</span></td>");
      }
    }

    if (tb != "") {
      tb_nr = tb.substring(0, 1).toInt(); /* 0,1,2,3 for valid bank´s and 9 for reset all */
      tb_val = tb.substring(2).toInt();   /* 0 and 1 */

#ifdef debug_html
      Serial.print(F("[DB] web_modes, set togglebank ")); Serial.print(tb_nr);
      Serial.print(F(" to ")); Serial.println(tb_val);
#endif

      web_status = F("<tr><td class=\"in grn\">");
      if (tb_nr <= 3) {
#ifdef debug_html
        Serial.print(F("[DB] web_modes, togglebank value "));
        Serial.println(tb_val == ToggleArray[tb_nr] ? F("same") : F("differing"));
#endif
        InputCmd = F("tob");  // preparation for processing
        InputCmd += tb_nr;    // preparation for processing

        web_status += F("togglebank ");
        web_status += tb_nr;

        if (tb_val == ToggleArray[tb_nr]) {
          InputCmd += 99;       // preparation for processing
          web_status += F(" reset</td>");
        } else {
          InputCmd += tb_val;   // preparation for processing
          web_status += F(" &#10004;</td>");
        }
      } else {
        InputCmd = F("tob99");
        web_status += F("toggle reseted & stopped &#10004;</td>");
      }
    }
    InputCommand(InputCmd);
  }

  String website = FPSTR(html_meta);
  website.reserve(10000);
  website += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/modes.css\">"
               "</head>");
  website += FPSTR(html_head_table);
  website += F("<body>"
               "<script src=\"js/modes.js\"></script>"
               "<form method=\"post\">" /* form method wichtig für Daten von Button´s !!! https://www.w3schools.com/tags/ref_httpmethods.asp */
               "<table id=\"rec_mod\">"
               "<tr>"
               "<th class=\"thf1\"><a href=\"chip\">general information</a></th>"
               "<th class=\"thf1\"><a href=\"detail\">detail information</a></th>"
               "<th class=\"thf1\">receive modes</th></tr>");

  for (byte i = 0; i < RegistersMaxCnt; i++) {
    website += F("<tr><td>");
    if (i <= 9) {
      website += F("&nbsp;");
    }
    website += i;
    website += F(" - ");
    website += Registers[i].name;
    website += F("</td><td class=\"ac\">set to bank&ensp;");

    for (byte i2 = 0; i2 < 4; i2++) {
      website += F("<button class=");
      if (i == ToggleArray[i2]) {
        website += F("\"btn2\"");
      } else {
        website += F("\"btn\"");
      }
      website += F(" type=\"submit\" name=\"tgb\" value=\"");
      website += i2;
      website += '_';
      website += i;
      website += F("\">&#x3");
      website += i2;
      website += F(";</button>");
    }

    website += F("</td><td class=\"ac\"><button class=");

    if (ReceiveModeName == Registers[i].name) {
      website += F("\"btn2\"");
    } else {
      website += F("\"btn\"");
    }

    website += F(" type=\"submit\" name=\"submit\" id=\"bt");
    website += i;
    website += F("\" value=\"");
    website += i;
    website += F("\">enable reception</button></td></tr>");
  }

  website += web_status;
  website += F("<td class=\"ac\">toggletime&nbsp;"
               "<input name=\"tgt\" type=\"text\" size=\"4\" placeholder=\"");
  website += (ToggleTime != 0 ? String(ToggleTime) : F("(ms)"));
  website += F("\"><button class=\"btn\" type=\"submit\" name=\"submit\" value=\"time\">START</button></td>"
               "<td class=\"ac\"><button class=\"btn\" type=\"submit\" name=\"tgb\" id=\"btLast\" value=\"9_0\">reset togglebank & STOP</button>"
               "</td></tr></table></body></html>");
  sendHtml(website);
}

#ifdef CC110x
void web_detail_cc110x_export() {
  if (!ChipFound) {
    HttpServer.send(404, "text/plain", F("Website not found !!!"));
  }

  String website = FPSTR(html_meta);
  website.reserve(1500);
  website += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/detail_cc110x_exp.css\"></head>"
               "Export all current register values (without the registers 0x29 ... 0x2E)<br>Just copy and paste string into your application<br>"
               "<br>FHEM - SIGNALduino Format | set &lsaquo;name&rsaquo; cc1101_reg<br>");
  String website_p2 = F("<br>FHEM - SIGNALduino Format | SD_ProtocolData.pm, entry register => <br>");

  for (byte i = 0; i <= REGISTER_MAX; i++) { /* all registers */
    if (i == 0) {
      website_p2 += '[';
    }
    if (i < 41) {
      website += onlyDecToHex2Digit(i);
      website += onlyDecToHex2Digit(Chip_readReg(i, READ_BURST));
      website += ' ';
      website_p2 += '\'';
      website_p2 += onlyDecToHex2Digit(i);
      website_p2 += onlyDecToHex2Digit(Chip_readReg(i, READ_BURST));
      website_p2 += '\'';
      if (i != 40) {
        website_p2 += ',';
      }
    }
  }
  website += F("<br>");
  website += website_p2;
  website += F("]<br><br><a class=\"back\" href=\"/detail\">&crarr; back to detail information</a>"
               "</body></html>");
  sendHtml(website);
}

void web_detail_cc110x_import() {
  if (!ChipFound) {
    HttpServer.send(404, "text/plain", F("Website not found !!!"));
  }
  String submit = HttpServer.arg("submit");   // welcher Button wurde betätigt
  String imp = HttpServer.arg("imp");         // String der Register inklusive Werte
  uint8_t countargs = HttpServer.args();      // Anzahl Argumente
  String Part = "";
  String PartAdr = "";
  String PartVal = "";

  String website = FPSTR(html_meta);
  website.reserve(1000);
  website += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/detail_cc110x_imp.css\"></head>"
               "<body><form method=\"post\">"); /* form method wichtig für Daten von Button´s !!! */

  if (countargs != 0) {
#ifdef debug_html
    Serial.println(F("###########################"));
    Serial.print(F("[DB] web_detail_cc110x_import, submit     ")); Serial.println(submit);
    Serial.print(F("[DB] web_detail_cc110x_import, countargs  ")); Serial.println(countargs);
    Serial.print(F("[DB] web_detail_cc110x_import, imp        ")); Serial.println(imp);
#endif

    if (submit == "registers") {  // register processing from String imp ['01AB','11FB']
      uint8_t Adr;
      uint8_t Val;

      for (uint16_t i = 0; i < imp.length(); i++) {
        if (imp[i] == ',' || imp[i] == ']') {
          PartAdr = Part.substring(0, 2);
          PartVal = Part.substring(2, 4);
          Adr = hexToDec(PartAdr);
          Val = hexToDec(PartVal);
#ifdef debug_html
          Serial.print(F("[DB] web_detail_cc110x_import, cc110x adr: ")); Serial.print(PartAdr);
          Serial.print(' '); Serial.println(Adr);
          Serial.print(F("[DB] web_detail_cc110x_import, cc110x val: ")); Serial.print(PartVal);
          Serial.print(' '); Serial.println(Val);
#endif
          if (Adr > 0x2E) {
            break;
          } else {
            Chip_writeReg(Adr, Val);    // write in cc110x
            EEPROMwrite(Adr, Val);      // write in flash
          }
          Part = "";                    // reset String
          PartAdr = "";                 // reset String
          PartVal = "";                 // reset String
        }

        if (imp[i] != '[' && imp[i] != ']' && imp[i] != ',' && imp[i] != '\'') {
          Part += imp[i];
        }
      }
      if (Adr > 0x2E) {
        website += F("ERROR: wrong cc110x adress, nothing write!");
        website += PartAdr;
      } else {
        website += F("Import of the register values closed​​");
      }
    }
  } else {
    website += F("Import of the register values ​​(minimum 2 pieces)<br>(Please paste the string in SIGNALduino format)<br><br>"
                 "<input size=\"100\" maxlength=\"288\" value=\"example ['01AB','11FB']\" name=\"imp\" pattern=\"^\\['[0-9a-fA-F]{4}'(,'[0-9a-fA-F]{4}')+\\]$\">"
                 "<br><br><button class=\"btn\" type=\"submit\" name=\"submit\" value=\"registers\">acceptance of the values ​​in the register</button>");
  }
  website += F("<br><br><a class=\"back\" href=\"/detail\">&crarr; back to detail information</a>"
               "</form></body></html>");
  sendHtml(website);
}
#elif RFM69 // SX1231
void web_detail_SX1231_export() {
  if (!ChipFound) {
    HttpServer.send(404, "text/plain", F("Website not found !!!"));
  }

  String website = FPSTR(html_meta);
  website.reserve(1200);
  website += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/detail_rfm69_exp.css\">"
               "<script src=\"js/detail_rfm69_exp.js\"></script></head>"
               "Export all current register values<br>Just copy and paste string into your application<br>"
               "<br>FHEM - SIGNALduino Format | SD_ProtocolData.pm, entry register =>"
               "<br>-in development-<br>"
               "<br>File to import on program SX1231SKB"
               "<br><button id=\"save-btn\" onclick=\"saveFile()\">save current register in \"SX1231 Starter Kit\" format</button><br>"
               "<br><a class=\"back\" href=\"/detail\">&crarr; back to detail information</a>"
               "</body></html>");
  sendHtml(website);
};
#endif  // SX1231 - END


void web_detail() {
  if (!ChipFound) {
    HttpServer.send(404, "text/plain", F("Website not found !!!"));
  }
  // http://192.168.178.26/detail?freq=868.302&freq_off=0.001&bandw=203.2&submit=bbandw&datarate=8.23&deviation=57.13&modulation=2-FSK&r0=01&r1=2E&r2=2E&r3=46&r4=2D&r5=D4&r6=1A&r7=C0&r8=00&r9=00&r10=00&r11=06&r12=00&r13=21&r14=65&r15=6C&r16=88&r17=4C&r18=02&r19=22&r20=F8&r21=51&r22=07&r23=00&r24=18&r25=16&r26=6C&r27=43&r28=68&r29=91&r30=87&r31=6B&r32=FB&r33=56&r34=11&r35=EF&r36=0C&r37=3B&r38=1F&r39=41&r40=00&r41=59&r42=7F&r43=3E&r44=88&r45=31&r46=0B
  String afc = HttpServer.arg("afc");
  String mod = HttpServer.arg("modulation");
  String submit = HttpServer.arg("submit");  // welcher Button wurde betätigt
  String temp;
  String web_stat;
  byte BrowserArgsReg[REGISTER_MAX + 1] = {};
  float bandw = (HttpServer.arg("bandw").toFloat());
  float deviation = (HttpServer.arg("deviation").toFloat());
  float freqOff = (HttpServer.arg("freq_off").toFloat());
  long datarate = (HttpServer.arg("datarate").toFloat()) * 1000;
  long freq = (HttpServer.arg("freq").toFloat()) * 1000;
  freq += 0;
  uint8_t countargs = HttpServer.args();    // Anzahl Argumente

#ifdef RFM69
  uint8_t RegDataModul = Chip_readReg(0x02, 0); // Data operation mode and Modulation settings
  uint8_t ModulationType = (RegDataModul & 0b00011000) >> 3; // Modulation scheme (FSK / OOK)
  uint8_t RegRxBw = Chip_readReg(0x19, 0); // read register Channel Filter BW Control
  //  int8_t DccFreq = RegRxBw >> 5; // Cut-off frequency of the DC offset canceller (DCC)
  uint8_t RxBwMant = (RegRxBw & 0b00011000) >> 3; // Channel filter bandwidth control
  uint8_t RxBwExp = RegRxBw & 0b00000111; // Channel filter bandwidth control
  //  uint32_t RxDccFreq = round((4 * RxBw) / (2 * M_PI * pow(2, DccFreq + 2)));
#endif

  for (byte i = 0; i <= REGISTER_MAX; i++) {
#ifdef RFM69
    if (i == 0) { // SX1231 Register 0x00 FIFO nicht lesen
      RegBeforeChange[i] = 0;
    } else {
      uint8_t addr = i;
      if (i >= 80) {
        addr = SX1231_RegAddrTranslate[i - 80];
      }
      RegBeforeChange[i] = Chip_readReg(addr, READ_BURST);
    }
#else
    RegBeforeChange[i] = Chip_readReg(i, READ_BURST);
#endif
  }

  if (countargs != 0) {
#ifdef debug_html
    Serial.println(F("###########################"));
    Serial.print(F("[DB] web_detail, submit     ")); Serial.println(submit);
    Serial.print(F("[DB] web_detail, countargs  ")); Serial.println(countargs);
    Serial.print(F("[DB] web_detail, freq       ")); Serial.println(freq);
    Serial.print(F("[DB] web_detail, freqOff    ")); Serial.println(freqOff);
    Serial.print(F("[DB] web_detail, bandw      ")); Serial.println(bandw);
    Serial.print(F("[DB] web_detail, datarate   ")); Serial.println(datarate);
    Serial.print(F("[DB] web_detail, deviation  ")); Serial.println(deviation);
    Serial.print(F("[DB] web_detail, modulation ")); Serial.println(mod);
#endif

    for (byte i = 0; i <= REGISTER_MAX; i++) {      /* register values from browser | set registers button -> into array */
      temp = 'r';
      temp += i;
      BrowserArgsReg[i] = hexToDec(HttpServer.arg(temp));
#ifdef debug_html
      Serial.print(F("[DB] web_detail, BrowserArgsReg[")); Serial.print(i); Serial.print(F("] = ")); Serial.println(BrowserArgsReg[i]);
#endif
    }

    if (submit == "bfreq") {
#ifdef debug_html
      Serial.println(F("[DB] web_detail, submit set frequency, offset, AFC pushed"));
#endif
      // AFC
      if (afc == "1") {
        InputCommand(F("CEA")); // AFC - enable
      } else {
        InputCommand(F("CDA")); // AFC - disable
      }
      // Frequency Offset
      Freq_offset = freqOff;
#ifdef CC110x
      Chip_writeReg(CC110x_FSCTRL0, 0);  // 0x0C: FSCTRL0 – Frequency Synthesizer Control
#endif
      EEPROM.put(EEPROM_ADDR_FOFFSET, Freq_offset);
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
      EEPROM.commit();
#endif
      // Frequency
      byte value[3];
      Chip_setFreq(freq, value);
      for (byte i = 0; i < 3; i++) {    /* write value to register */
#ifdef CC110x
        Chip_writeReg(i + 13, value[i]);  // write to chip
        EEPROMwrite(i + 13, value[i]);    // write in flash
#elif RFM69
        Chip_writeReg(i + 7, value[i]);  // write to chip
        EEPROMwrite(i + 7, value[i]);    // write in flash
#endif
      }
      web_stat = F("Frequency, frequency offset and AFC set &#10004;");
      // bandwidth
    } else if (submit == "bbandw") {
      web_stat = F("Bandwidth set &#10004;");
      byte value = Chip_Bandw_cal(bandw); /* input complete | input split */
#ifdef debug_html
      Serial.print(F("[DB] web_detail, button set bandwidth pushed with ")); Serial.println(bandw);
      Serial.print(F("[DB] web_detail, register ")); Serial.print(CHIP_RxBw, HEX); Serial.print(F(" value is ")); Serial.println(value);
#endif
      Chip_writeReg(CHIP_RxBw, value);  // write in receiver
      EEPROMwrite(CHIP_RxBw, value);    // write in flash
      // Datarate
    } else if (submit == "bdatarate") {
#ifdef debug_html
      Serial.print(F("[DB] web_detail, button set datarate pushed with ")); Serial.println(datarate);
#endif
      web_stat = F("DataRate set &#10004;");
      byte value[2];
      Chip_Datarate_Set(datarate, value); // calculate register values for DataRate/BitRate
      for (byte i = 0; i < 2; i++) {
        Chip_writeReg(CHIP_BitRate + i, value[i]); // write in receiver
        EEPROMwrite(CHIP_BitRate + i, value[i]);  // write in flash
      }
      // Deviation
    } else if (submit == "bdev") {
#ifdef debug_html
      Serial.print(F("[DB] web_detail, button set deviation pushed with ")); Serial.println(deviation);
#endif
      web_stat = F("Deviation set &#10004;");
#ifdef CC110x
      byte value = CC110x_Deviation_Set(deviation);
      Chip_writeReg(21, value);   // write in receiver
      EEPROMwrite(21, value);     // write in flash
#elif RFM69
      byte value[2];
      SX1231_Deviation_Set(deviation, value); // calculate register values for frequency deviation
      for (byte i = 0; i < 2; i++) {
        Chip_writeReg(0x05 + i, value[i]); // write in receiver
        EEPROMwrite(0x05 + i, value[i]);  // write in flash
      }
#endif
    } else if (submit == "bmod") {
#ifdef debug_html
      Serial.print(F("[DB] web_detail, set modulation to ")); Serial.println(mod);
#endif
      web_stat = F("Modulation set &#10004;");

      byte value = 0;
      if (mod == F("2-FSK")) {
        value = 0;
      } else if (mod == F("GFSK")) {
        value = 1;
      } else if (mod == F("ASK/OOK")) {
        value = 3;
      } else if (mod == F("4-FSK")) {
        value = 4;
      } else if (mod == F("MSK")) {
        value = 7;
      }
#ifdef CC110x
      Chip_writeReg(18, web_Mod_set(value));      // write in cc110x
      EEPROMwrite(18, web_Mod_set(value));          // write in flash
#endif
    } else if (submit == "breg") { // TODO FIFO lesen !!!
#ifdef debug_html
      Serial.println(F("[DB] web_detail, button set registers pushed"));
#endif
      web_stat = F("all registers set &#10004;");

      for (byte i = 0; i <= REGISTER_MAX; i++) { /* all registers */
#ifdef debug_html
        Serial.print(F("[DB] web_detail, regData["));
        Serial.print(i); Serial.print(F("] = ")); Serial.println(BrowserArgsReg[i]);
#endif
        /* compare value with value to be written */
        byte temp = Chip_readReg(i, READ_BURST);
        if (BrowserArgsReg[i] != temp) {
#ifdef debug_html
          /* i > 34 && <= 40    | automatic control register */
          /* i > 40             | test register */
          Serial.print(F("[DB] web_detail, regData[")); Serial.print(i);
          Serial.print(F("] value has changed ")); Serial.print(temp);
          Serial.print(F(" -> ")); Serial.println(BrowserArgsReg[i]);
#endif
          if (i == 6) { /* ToDo - probably not optimal -> if register 6 is changed (dependencies) ??? */
            ReceiveModePKTLEN = BrowserArgsReg[i];
          }
          /* write value to register */
          Chip_writeReg(i, BrowserArgsReg[i]);   // write in cc110x
          EEPROMwrite(i, BrowserArgsReg[i]);       // write in flash
        }
      }
    }
    ReceiveModeName = FPSTR(RECEIVE_MODE_USER);
  }

  String website = FPSTR(html_meta);
#ifdef CC110x
  website.reserve(13000);
#elif RFM69
  website.reserve(20000);
#endif
  website += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/detail.css\">"
               "<script src=\"js/"
#ifdef CC110x
               "detail_cc110x.js"
#elif RFM69
               "detail_rfm69.js"
#endif
               "\"></script>"
               "</head>");
  website += FPSTR(html_head_table);
  website += F("<body><form method=\"get\">" /* form method wichtig für Daten von Button´s !!! */
               "<table><tr>"
               "<th class=\"f1\" colspan=\"2\"><a href=\"chip\">general information</a></th>"
               "<th class=\"f1\" colspan=\"2\">detail information</th>"
               "<th class=\"f1\" colspan=\"2\"><a href=\"modes\">receive modes</a></th></tr>"
               // Frequency (is | should)
               "<tr><td colspan=\"2\">Frequency (is | should)</td><td class=\"ce\" colspan=\"2\"><span id=\"FREQis\"></span> | <span id=\"FREQ\"></span>"
               " MHz</td><td class=\"ce\"><input aria-label=\"Fre\" size=\"6\" id=\"p1\" name=\"freq\" value=\""
               "\"><div class=\"txt\">&ensp;MHz</div></td><td class=\"ce\" rowspan=\"2\"><button class=\"btn\" type=\"submit\" name=\"submit\" value=\"bfreq\">set</button></td></tr>"
               // Frequency Offset
               "<tr><td colspan=\"2\">Frequency Offset</td><td class=\"f2 ce\">Afc: <input aria-label=\"FreO1\" name=\"afc\" type=\"checkbox\" value=\"1\"");
  website += (freqAfc == 1 ? F(" checked") : F(""));
  website += F("></td><td class=\"ce\">");
  website += String(Freq_offset, 3);
  website += F(" MHz</td><td class=\"ce\"><input aria-label=\"FreO2\" size=\"6\" id=\"p2\" name=\"freq_off\" value=\"");
  website += String(Freq_offset, 3);
  website += F("\"><div class=\"txt\">&ensp;MHz</div></td></tr>"
               // Bandwidth
               "<tr><td colspan=\"2\">"
#ifdef CC110x
               "Bandwidth (double side)"
#elif RFM69
               "Bandwidth (single side)"
#else
               "Bandwidth"
#endif
               "</td>"
               "<td class=\"ce\" colspan=\"2\"><span id=\"CHANBW\"></span></td>"
               //#ifdef CC110x
               //               "<td class=\"ce\"><input aria-label=\"Bw\" size=\"6\" id=\"p3\" name=\"bandw\" value=\"\"><div class=\"txt\">&ensp;kHz</div></td>"
               //#elif RFM69
               "<td class=\"ce\"><select id=\"bandw\" name=\"bandw\"></select><div class=\"txt\">&ensp;kHz</div></td>"
               //#endif
               "<td class=\"ce\"><button class=\"btn\" type=\"submit\" name=\"submit\" value=\"bbandw\">set</button></td></tr>"
               // DataRate
               "<tr><td colspan=\"2\">DataRate"
#ifdef RFM69
               " (BitRate)"
#endif
               "</td><td class=\"ce\" colspan=\"2\"><span id=\"DRATE\"></span></td>"
               "<td class=\"ce\"><input aria-label=\"datra\" size=\"6\" id=\"p4\" name=\"datarate\" value=\"\">"
               "<div class=\"txt\">&ensp;kBaud</div></td><td class=\"ce\"><button class=\"btn\" type=\"submit\" name=\"submit\" value=\"bdatarate\">set</button></td></tr>"
               // Deviation
               "<tr><td colspan=\"2\">Deviation</td><td class=\"ce\" colspan=\"2\"><span id=\"DEVIATN\"></span></td>"
               "<td class=\"ce\"><input aria-label=\"devi\" size=\"6\" id=\"p5\" name=\"deviation\" value=\"\">"
               "<div class=\"txt\">&ensp;kHz</div></td><td class=\"ce\"><button class=\"btn\" type=\"submit\" name=\"submit\" value=\"bdev\">set</button></td></tr>"
               // Modulation
               "<tr><td colspan=\"2\">Modulation</td><td class=\"ce\" colspan=\"2\"><span id=\"MOD_FORMAT\"></span></td>"
               "<td class=\"ce\"><select aria-label=\"mod\" id=\"modulation\" name=\"modulation\"></select></td>"
               "<td class=\"ce\"><button class=\"btn\" type=\"submit\" name=\"submit\" value=\"bmod\">set</button></td></tr>"
               // NUM_PREAMBLE
               "<tr><td colspan=\"2\">Number of preamble</td><td class=\"ce\" colspan=\"4\"><span id=\"MDMCFG1\"></span></td></tr>"
               // Packet length config
               "<tr><td colspan=\"2\">Packet length config</td><td class=\"ce\" colspan=\"4\"><span id=\"PKTCTRL0\"></span></td></tr>"
               // Sync-word qualifier mode
               "<tr><td colspan=\"2\">Sync-word qualifier mode</td><td class=\"ce\" colspan=\"4\"><span id=\"SYNC_MODE\"></span></td></tr>"
               // buttons
               "<tr><td class=\"ce\" colspan=\"6\"><button class=\"btn\" type=\"submit\" name=\"submit\" value=\"breg\">set all registers</button>&emsp;"
#ifdef CC110x
               "<button class=\"btn\" type=\"button\" onClick=\"location.href='detail_cc110x_export'\">export all registers</button>&emsp;"
               "<button class=\"btn\" type=\"button\" onClick=\"location.href='detail_cc110x_import'\">import registers</button></td></tr>"
#elif RFM69
               "<button class=\"btn\" type=\"button\" onClick=\"location.href='detail_rfm69_export'\">export all registers</button>&emsp;"
#endif
               // status line
               "<tr><td class=\"in\" colspan=\"6\"><span id=\"state\">");

  website += web_stat;
  website += F("</span></td></tr><tr><td>register</td><td class=\"ce\">value</td><td colspan=\"4\">notes</td></tr>");

  for (byte i = 0; i <= REGISTER_MAX; i++) {
    website += F("<tr><td class=\"f4\"><span id=\"s");
    website += i;
    website += F("\"></span></td><td class=\"ce\"><input name=\"r"); /* registername for GET / POST */
    website += i;
    website += F("\" value=\"");
#ifdef RFM69
    if (i == 0) { // SX1231 Register 0x00 FIFO nicht lesen
      website += "00";
    } else {
      uint8_t addr = i;
      if (i >= 80) {
        addr = SX1231_RegAddrTranslate[i - 80];
      }
      website += onlyDecToHex2Digit(Chip_readReg(addr, READ_BURST));
    }
#else
    website += onlyDecToHex2Digit(Chip_readReg(i, READ_BURST));
#endif
    website += F("\" type=\"text\"></td><td colspan=\"3\"><span id=\"n");
    website += i;
    website += F("\"></span></td><td id=\"t");
    website += i;
    website += F("\"></td></tr>");
  }

  website += F("</table></form></body></html>");
  sendHtml(website);
}


void web_log() {
  String website = FPSTR(html_meta);
  website.reserve(10000);
  website += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/log.css\">"
               "<script src=\"js/log.js\"></script>"
               "</head>");
  website += FPSTR(html_head_table);
  website = HTML_mod(website);

  File logfile = LittleFS.open("/files/log.txt", "r");
  if (!logfile) {
#ifdef debug_html
    Serial.println(F("[DB] File log.txt nothing found"));
#endif
    website += F("No file log.txt found. (File system incomplete)</html>");
  } else {
    uint8_t lineCnt = 0;

    while (logfile.available()) {
      website += F("<div id=\"logtxt\" hidden>");
      website += logfile.readStringUntil('\n');
      website += F("</div>");
      lineCnt++;
    }
    logfile.close();
    if (lineCnt == 0) {
      website += F("Log file is empty");
    }
    website += F("<p id=\"uptime\" hidden>");
    website += uptime;
    website += F("</p></html>");
  }
  sendHtml(website);
}


void web_raw() {
  if (!ChipFound) {
    HttpServer.send(404, "text/plain", F("Website not found !!!"));
  }

  String website = FPSTR(html_meta);
  website.reserve(2000);
  website += F("<body><form method=\"post\">" /* form method wichtig für Daten von Button´s !!! */
               "<link rel=\"stylesheet\" type=\"text/css\" href=\"css/raw.css\">"
               "<script src=\"js/raw.js\"></script>"
               "</head>");
  website += FPSTR(html_head_table);
  website += F("<table>"
               "<tr><th colspan=\"4\">send data (with the current register settings)</th></tr>"
               "<tr>"
               "<td class=\"td1\"><input class=\"inp\" name=\"sd\" type=\"text\"></td>"
               "<td class=\"td1\">repeats <input aria-label=\"n1\" name=\"rep\"type=\"number\" onkeypress=\"if(this.value.length==2) return false;\"></td>"
               "<td class=\"td1\">pause (ms) <input aria-label=\"n2\" name=\"rept\" type=\"number\" onkeypress=\"if(this.value.length==6) return false;\"></td>"
               "<td class=\"td1\"><input class=\"btn\" type=\"button\" value=\"send\" onclick=\"msgSend()\"></td>"
               "</tr><tr><td class=\"td1\" colspan=\"4\"><span id=\"val\">");
  if (msgRepeats != 0) {
    website += F("sending process active (");
    website += msgRepeats;
    website += F(" repeats with ");
    website += msgSendInterval;
    website += F(" ms pause) | finished in ");
    if ((msgRepeats * msgSendInterval / 1000) > 60) {
      website += (msgRepeats * msgSendInterval / 1000) / 60;
      website += F(" minutes");
    } else {
      website += (msgRepeats * msgSendInterval / 1000);
      website += F(" seconds");
    }
  } else {
    website += F("ready to broadcast");
  }

  website += F("</span></td></tr></table><br>"
               "<div><table id=\"dataTable\">"
               "<tr><th class=\"dd\">Time</th><th>current RAW, received data on mode &rarr;&nbsp;<span id=\"MODE\">");
  website += ReceiveModeName;
  website += F("</span></th><th class=\"dd\">RSSI<br>dB</th><th class=\"dd\">Offset<br>kHz</th></tr>"
               "</table></div>"
               "</body></html>");
  sendHtml(website);
}


void web_wlan() {
  /* https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/scan-class.html
     https://links2004.github.io/Arduino/d4/d52/wl__definitions_8h_source.html
     https://blog.robberg.net/wifi-scanner-with-esp32/ */
  IPAddress ipadr;  // IP addresse
  char ipbuffer[16];
  String WifiEncryptionType;
  String WifiMAC;
  String hssid = HttpServer.arg("hiddenssid");  // Auswahl versteckte SSID
  String qdhcp = HttpServer.arg("dhcp");        // 1-DHCP ein, 0-DHCP aus
  String qdns = HttpServer.arg("dns");          // Domain Name Server statisch
  String qip = HttpServer.arg("ip");            // IP-Adresse statisch
  String qpass = HttpServer.arg("pw");          // WLAN Passwort
  String qsgw = HttpServer.arg("gw");           // Gateway statisch
  String qsnm = HttpServer.arg("sn");           // Subnetzmaske  statisch
  String qssid = HttpServer.arg("setssid");     // Auswahl SSID
  String submit = HttpServer.arg("submit");     // welcher Button wurde betätigt
  uint8_t countargs = HttpServer.args();        // Anzahl Argumente
  String used_ssid_mac = WiFi.BSSIDstr();
  String used_ssid = WiFi.SSID();               // for the output on web server

#ifdef debug_html
  Serial.print(F("[DB] web_wlan, setssid    ")); Serial.println(qssid);
  Serial.print(F("[DB] web_wlan, hiddenssid ")); Serial.println(hssid);
  Serial.print(F("[DB] web_wlan, pw         ")); Serial.println(qpass);
  Serial.print(F("[DB] web_wlan, dhcp       ")); Serial.println(qdhcp);
  Serial.print(F("[DB] web_wlan, ip         ")); Serial.println(qip);
  Serial.print(F("[DB] web_wlan, dns        ")); Serial.println(qdns);
  Serial.print(F("[DB] web_wlan, sn         ")); Serial.println(qsnm);
  Serial.print(F("[DB] web_wlan, gw         ")); Serial.println(qsgw);
  Serial.print(F("[DB] web_wlan, submit     ")); Serial.println(submit);
  Serial.print(F("[DB] web_wlan, countargs  ")); Serial.println(countargs);
  Serial.println(F("[DB] web_wlan, scan networks ..."));
#endif

  String website = FPSTR(html_meta);
  website.reserve(10000); // MR | bei 16 WLAN´s --> website length 7092
  website += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/wlan.css\">"
               "<script src=\"js/wlan.js\"></script>"
               "</head>");
  website += FPSTR(html_head_table);
  website = HTML_mod(website);
  website += F("<body><form method=\"post\">" /* form method wichtig für Daten von Button´s !!! */
               "<table>"                      /* START Tabelle gesamt */
               "<tr><th colspan=\"6\">WLAN - Device status</th></tr>"
               "<tr><th colspan=\"4\" class=\"fw\">Mode</td><th colspan=\"2\">MAC</th></tr>"
               "<tr><td class=\"alig_c\" colspan=\"4\">");

  if (!WLAN_AP) {
    website += F("WIFI_STA (normal, station mode function)");
  } else {
    website += F("WIFI_AP (wireless access point)");
  }

  website += F("</td><td class=\"alig_c\" colspan=\"2\">");
  website += WiFi.macAddress();
  website += F("</td></tr>"
               "<tr><td class=\"b1\" colspan=\"6\"></td></tr>"
               "<tr><th colspan=\"6\">WLAN - available networks</th></tr>"
               "<tr><th></th><th>SSID</th><th>MAC</th><th>CH</th><th>RSSI</th><th>encryptionType</th></tr>");
  int WifiNetworks = WiFi.scanNetworks();  // Anzahl Netzwerke

  for (int i = 0; i < WifiNetworks; ++i) {
    WifiMAC = WiFi.BSSIDstr(i); // Returns a String from MAC address of a network discovered during the scan.
    WifiEncryptionType = WLAN_encryptionType(WiFi.encryptionType(i));
    /* Tabelle WLAN - available networks */
    website += F("<tr><td class=\"ra1\"><input aria-label=\"na1\" type=\"radio\" name=\"setssid\" value=\"");
    website += WiFi.SSID(i);

#ifdef debug_html
    Serial.print(F("[DB] web_wlan,"));
#endif

    if (used_ssid == WiFi.SSID(i) && used_ssid_mac == WifiMAC) {
#ifdef debug_html
      Serial.print('x');
#endif
      website += F("\" checked>");
    } else {
      website += F("\">");
    }

#if defined(debug_html) || defined(debug_wifi)
    Serial.print(' '); Serial.print(i + 1); Serial.print(' '); Serial.print(WiFi.SSID(i));
    Serial.print(' '); Serial.print(WifiMAC); Serial.print(' '); Serial.print(WiFi.channel(i));
    Serial.print(' '); Serial.print(WiFi.RSSI(i)); Serial.print(' '); Serial.println(WifiEncryptionType);
#endif

    website += F("</td><td class=\"ssid\">");
    website += WiFi.SSID(i);
    website += F("</td><td class=\"fb\">");
    website += WifiMAC;
    website += F("</td><td class=\"wch\">");
    website += WiFi.channel(i);
    website += F("</td><td class=\"wrs");
    if (WiFi.RSSI(i) > -40) {         // WLAN RSSI good
      website += F(" wrsgrn\">");
    } else if (WiFi.RSSI(i) < -65) {  // WLAN RSSI very bad
      website += F(" wrsred\">");
    } else {
      website += F("\">");            // WLAN RSSI middle
    }
    website += WiFi.RSSI(i);
    website += F(" db</td><td class=\"alig_c\">");
    website += WifiEncryptionType;
    website += F("</td></tr>");
  }

  /* Tabelle WLAN - available networks, letzte Zeile für SSID-Eingabe */
  website += F("<tr>"
               "<td class=\"ra1\"><input aria-label=\"n2\" name=\"setssid\" type=\"radio\" value=\"hidden\"></td>"
               "<td class=\"inp\" colspan=\"2\"><input name=\"hiddenssid\" type=\"text\" size=\"36\"></td>"
               "<td class=\"inp\" colspan=\"3\"><input name=\"pw\" type=\"password\" size=\"28\"></td>"
               "</tr>"
               "<tr>"
               "<td class=\"zcb\"></td><td class=\"inp\" colspan=\"2\"><button class=\"btn\" onclick=\"wlan_wps()\" type=\"submit\" name=\"submit\" value=\"wps\">Wi-Fi Protected (WPS)</button></td>"
               "<td class=\"inp\" colspan=\"3\"><button class=\"btn\" onclick=\"wlan_conn()\" type=\"submit\" name=\"submit\" value=\"connect\">connect</button></td>"
               "</tr>"
               "<tr><td colspan=\"6\" class=\"b1\"><span id=\"StatCon\"></span></td></tr>"
               /* Tabelle Setting IP - network */
               "<tr><th colspan=\"6\">Setting IP - network</th></tr>");

  if (WLAN_AP) {
    ipadr = WiFi.softAPIP();  // IP
  } else {
    ipadr = WiFi.localIP();   // IP
  }

  /* Tabelle Setting IP - network | IP DHCP */
  website += F("<tr><td class=\"ra1\"><input aria-label=\"n3\" type=\"radio\" name=\"dhcp\" value=\"1\"");
  website += (used_dhcp == 1 ? F(" checked") : F(""));
  website += F("></td><td colspan=\"2\">IP - adress automatically (DHCP)</td><td class=\"inp\" colspan=\"3\">");
  sprintf(ipbuffer, "%d.%d.%d.%d", ipadr[0], ipadr[1], ipadr[2], ipadr[3]);
  website += ipbuffer;
  website += F("</td></tr>");

  /* Tabelle Setting IP - network | IP statisch */
  website += F("<tr><td class=\"ra1\"><input aria-label=\"n4\" type=\"radio\" name=\"dhcp\" value=\"0\"");
  website += (used_dhcp == 0 ? F(" checked") : F(""));
  website += F("></td><td colspan=\"2\">IP - adress static</td>"
               "<td class=\"inp\" colspan=\"3\"><input aria-label=\"n4\" size=\"14\" name=\"ip\" type=\"text\" id=\"ip\" value=\"");
  sprintf(ipbuffer, "%d.%d.%d.%d", ipadr[0], ipadr[1], ipadr[2], ipadr[3]);
  website += ipbuffer;
  website += F("\"></td></tr>");

  ipadr = WiFi.gatewayIP(); /* Tabelle Setting IP - network | domain name server */
  website += F("<tr><td class=\"alig_r\" colspan=\"3\">domain name server</td><td class=\"inp\" colspan=\"3\"><input aria-label=\"n5\" size=\"14\" name=\"dns\" type=\"text\" id=\"dns\" value=\"");
  sprintf(ipbuffer, "%d.%d.%d.%d", ipadr[0], ipadr[1], ipadr[2], ipadr[3]);
  website += ipbuffer;
  website += F("\"></td></tr>");

  /* Tabelle Setting IP - network | Standard-Gateway */
  website += F("<tr><td class=\"alig_r\" colspan=\"3\">default gateway</td><td class=\"inp\" colspan=\"3\"><input aria-label=\"n6\" size=\"14\" name=\"gw\" type=\"text\" id=\"gw\" value=\"");
  sprintf(ipbuffer, "%d.%d.%d.%d", ipadr[0], ipadr[1], ipadr[2], ipadr[3]);
  website += ipbuffer;
  website += F("\"></td></tr>");

  ipadr = WiFi.subnetMask(); /* Tabelle Setting IP - network | Subnetzkaske */
  website += F("<tr><td class=\"alig_r\" colspan=\"3\">subnet mask</td><td class=\"inp\" colspan=\"3\"><input aria-label=\"n7\" size=\"14\" name=\"sn\" type=\"text\" id=\"sn\" value=\"");
  sprintf(ipbuffer, "%d.%d.%d.%d", ipadr[0], ipadr[1], ipadr[2], ipadr[3]);
  website += ipbuffer;
  website += F("\"></td></tr>");

  if (submit == "connect") {    /* Button connect | with WIFI Settings */
#ifdef debug_html
    Serial.println(qdhcp == "1" ? F("[DB] web_wlan, qdhcp 1 (on)") : F("[DB] web_wlan, qdhcp 0 (off)"));
#endif

    if (qdhcp == "1") { /* DHCP ein */
      used_dhcp = 1;
      EEPROMwrite(EEPROM_ADDR_DHCP, 1);
    } else { /* DHCP aus */
      EEPROMwrite(EEPROM_ADDR_DHCP, 0);
      used_dhcp = 0;

      byte ipaddress[4];
      /* statische IP übernehmen */
      if (str2ip((char*)qip.c_str(), ipaddress)) {  // convert String to Array of 4 Bytes
#ifdef debug_html
        Serial.print(F("[DB] web_wlan, static IP ")); Serial.println(qip);
#endif
        EEPROMwrite_ipaddress(EEPROM_ADDR_IP, qip);  // write IPAddress to EEPROM
        eip = EEPROMread_ipaddress(EEPROM_ADDR_IP);
      } else {
        /* Static IP address invalid! */
      }

      /* Standard-Gateway übernehmen */
      if (str2ip((char*)qsgw.c_str(), ipaddress)) {  // convert String to Array of 4 Bytes
#ifdef debug_html
        Serial.print(F("[DB] web_wlan, Standard-Gateway ")); Serial.println(qsgw);
#endif
        EEPROMwrite_ipaddress(EEPROM_ADDR_GATEWAY, qsgw);  // write IPAddress to EEPROM
        esgw = EEPROMread_ipaddress(EEPROM_ADDR_GATEWAY);
      } else {
        /* Default gateway invalid! */
      }

      /* statischen Domain Name Server übernehmen */
      if (str2ip((char*)qdns.c_str(), ipaddress)) {     // convert String to Array of 4 Bytes
#ifdef debug_html
        Serial.print(F("[DB] web_wlan, static Domain Name Server ")); Serial.println(qdns);
#endif
        EEPROMwrite_ipaddress(EEPROM_ADDR_DNS, qdns);   // write IPAddress to EEPROM
        edns = EEPROMread_ipaddress(EEPROM_ADDR_DNS);
      } else {
        /* Invalid domain name server! */
      }

      /* Subnetmask übernehmen */
      if (str2ip((char*)qsnm.c_str(), ipaddress)) {     // convert String to Array of 4 Bytes
#ifdef debug_html
        Serial.print(F("[DB] web_wlan, subnet mask ")); Serial.println(qsnm);
#endif
        EEPROMwrite_ipaddress(EEPROM_ADDR_NETMASK, qsnm);  // write IPAddress to EEPROM
        esnm = EEPROMread_ipaddress(EEPROM_ADDR_NETMASK);
      } else {
        /* Invalid subnet mask! */
      }
    }

    if (qssid.length() > 0 && qpass.length() > 0) {
      if (qssid == "hidden") { /* verstecktes Netzwerk */
        qssid = hssid;         /* versteckte SSID übernehmen */
      }
    }

#ifdef debug_html
    Serial.println(F("[DB] web_wlan, send HTML (connect)"));
#endif

    delay(250);
    start_WLAN_STATION(qssid, qpass);
    submit = "";
  }   /* Button connect - END  */

  website += F("</table></form></body></html>");

  if (submit == "wps") {    /* Button wps | wps methode for connection */
#ifdef debug_html
    Serial.println(WLAN_AP == 0 ? F("[DB] web_wlan, wps methode setting (call from station)") : F("[DB] web_wlan, wps methode setting (call from AP)"));
#endif
    delay(250);

    if (!start_WLAN_WPS()) {
      if (WLAN_AP == 1 && EEPROMread(EEPROM_ADDR_SSID) == 255) {
#ifdef debug_wifi
        Serial.println(F("[DB] WIFI WPS failed, start AP"));
#endif
        appendLogFile(F("WPS failed, start AP"));
        start_WLAN_AP(OwnStationHostname, WLAN_password_ap);
      } else {
#ifdef debug_wifi
        Serial.println(F("[DB] WIFI WPS failed, use old WiFi settings"));
#endif
        appendLogFile(F("WPS failed, use old WiFi settings"));
        start_WLAN_STATION(EEPROMread_string(EEPROM_ADDR_SSID), EEPROMread_string(EEPROM_ADDR_PASS));
      }
    } else {
#ifdef debug_wifi
      Serial.println(F("[DB] WIFI WPS worked, new WiFi settings"));
#endif
      start_WLAN_STATION(EEPROMread_string(EEPROM_ADDR_SSID), EEPROMread_string(EEPROM_ADDR_PASS));
    }
  }   /* Button wps - END */

  if (countargs == 0) {
#ifdef debug_html
    Serial.println(F("[DB] web_wlan, HTML send (args == 0)"));
#endif
    sendHtml(website);
  }
  WiFi.scanDelete(); // Delete the last scan result from memory. // TODO
}


void sendHtml(String & str) {
  HttpServer.sendHeader(F("cache-control"), F("private, no-cache"));       // A 'cache-control' header is missing or empty.
  HttpServer.sendHeader(F("x-content-type-options"), F("nosniff"));        // Response should include 'x-content-type-options' header.
  HttpServer.sendHeader(F("content-type"), F("text/html; charset=utf-8")); // Response should include 'content-type' header.
  HttpServer.send ( 200, "text/html; charset=utf-8", str );                // 'content-type' header charset value should be 'utf-8'.
#ifdef debug_wifi
  Serial.print(F("[DB] Website ")); Serial.print(HttpServer.uri()); Serial.print(F(" HTTP TX Bytes: ")); Serial.println(str.length());
#endif
}


void WebSocket_chip() {
#ifdef debug_websocket
  Serial.println(F("[DB] WebSocket_chip running"));
#endif
  if (ChipFound) {
    if (webSocket.connectedClients() > 0) {
      String website = ReceiveModeName;
      website.reserve(55);
      website += ',';
#ifdef CC110x
      website += Chip_readReg(CC110x_MARCSTATE, READ_BURST);
#elif RFM69
      website += (Chip_readReg(0x01, 0x00) & 0b00011100) >> 2;
#endif
      website += F(",{ ");

      for (byte i = 0; i < 4; i++) {
        if (ToggleArray[i] == 255) {
          website += '-';
        } else {
          website += ToggleArray[i];
        }
        if (i != 3) {
          website += ' ';
        }
      }

      website += F(" },");
      website += ToggleTime;

      for (uint8_t num = 0; num < WEBSOCKETS_SERVER_CLIENT_MAX; num++) {
        if (webSocketSite[num] == "/chip") {
          webSocket.sendTXT(num, website);
        }
      }
    }
  }
}


void WebSocket_detail(byte variant) {
#ifdef debug_websocket
  Serial.print(F("[DB] WebSocket_detail running with variant "));
  if (variant == 0) {
    Serial.println(F(" old settings"));
  } else {
    Serial.println(F(" new settings"));
  }
#endif
  if (webSocket.connectedClients() > 0) {
    String website = "";
    website.reserve(200);
    for (byte i = 0; i <= REGISTER_MAX; i++) { /* all registers | fastest variant */
#ifdef RFM69
      if (i == 0) { // SX1231 Register 0x00 FIFO nicht lesen
        website += "00";
      } else {
        uint8_t addr = i;
        if (i >= 80) {
          addr = SX1231_RegAddrTranslate[i - 80];
        }
        website += (variant == 0 ? onlyDecToHex2Digit(RegBeforeChange[i]) : onlyDecToHex2Digit(Chip_readReg(addr, READ_BURST)));
      }
#else // CC110x
      website += (variant == 0 ? onlyDecToHex2Digit(RegBeforeChange[i]) : onlyDecToHex2Digit(Chip_readReg(i, READ_BURST)));
#endif
      website += ',';
    }
    website += F("detail,");
    website += ReceiveModeName;
    website += ',';
    website += String(Freq_offset, 3);

    for (uint8_t num = 0; num < WEBSOCKETS_SERVER_CLIENT_MAX; num++) {
      if (webSocketSite[num] == "/detail" || webSocketSite[num] == "/detail_rfm69_export" || webSocketSite[num] == "/detail_cc110x_export") {
        webSocket.sendTXT(num, website);
      }
    }
  }
}


void WebSocket_modes() {
#ifdef debug_websocket
  Serial.println(F("[DB] WebSocket_modes running"));
#endif
  if (webSocket.connectedClients() > 0) {
    String website = F("MODE,");
    website.reserve(35);
    website += ReceiveModeName;
    website += ',';
    website += ReceiveModeNr;

    for (uint8_t num = 0; num < WEBSOCKETS_SERVER_CLIENT_MAX; num++) {
      if (webSocketSite[num] == "/modes" || webSocketSite[num] == F("/raw")) {
        webSocket.sendTXT(num, website);
      }
    }
  }
}


void WebSocket_help() {
#ifdef debug_websocket
  Serial.println(F("[DB] WebSocket_help running"));
#endif
  if (webSocket.connectedClients() > 0) {
    String website = CHIP_NAME;
    website += ',';
    website += ChipFound == true ? F("yes") : F("no");
    for (uint8_t num = 0; num < WEBSOCKETS_SERVER_CLIENT_MAX; num++) {
      if (webSocketSite[num] == "/html/rfm69_help.html" || webSocketSite[num] == "/html/rfm69_help.html" || webSocketSite[num] == "/html/rfm69_help.html") {
        webSocket.sendTXT(num, website);
      }
    }
  }
}


void WebSocket_index() {
#ifdef debug_websocket
  Serial.println(F("[DB] WebSocket_index running"));
#endif
  if (webSocket.connectedClients() > 0) {
    String website = F("chip,");
    website.reserve(48);
    website += freeRam();
    website += ',';
    website += uptime;
    website += ',';
    website += uptimeReset;
    website += ',';
    website += msgCount;
    website += ',';
    website += WiFi.RSSI();

    for (uint8_t num = 0; num < WEBSOCKETS_SERVER_CLIENT_MAX; num++) {
      if (webSocketSite[num] == "/") {
        webSocket.sendTXT(num, website);
      }
    }
  }
}


void WebSocket_raw(const String & html_raw) {
#ifdef debug_websocket
  Serial.println(F("[DB] WebSocket_raw running"));
#endif
  if (webSocket.connectedClients() > 0) {
    String website = F("RAW,");
    website.reserve(460);
    website += ReceiveModeName;
    website += ',';
    website += html_raw;
    website += ',';
    website += RSSI_dez;
    website += ',';
    website += int16_t( (Freq_offset / 1000) + (26000000 / 16384 * freqErr / 1000) );

    for (uint8_t num = 0; num < WEBSOCKETS_SERVER_CLIENT_MAX; num++) {
      if (webSocketSite[num] == "/raw") {
        webSocket.sendTXT(num, website);
      }
    }
  }
}


String getContentType(String filename) {
  if (HttpServer.hasArg("download")) return "application/octet-stream";
  //else if (filename.endsWith(".htm")) return "text/html; charset=utf-8";
  else if (filename.endsWith(".html")) return "text/html; charset=utf-8";
  else if (filename.endsWith(".css")) return "text/css; charset=utf-8";
  else if (filename.endsWith(".js")) return "application/javascript; charset=utf-8";
  //else if (filename.endsWith(".png")) return "image/png";
  //else if (filename.endsWith(".gif")) return "image/gif";
  //else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  //else if (filename.endsWith(".svg")) return "image/svg+xml; charset=utf-8";
  //else if (filename.endsWith(".xml")) return "text/xml";
  //else if (filename.endsWith(".pdf")) return "application/x-pdf";
  //else if (filename.endsWith(".zip")) return "application/x-zip";
  //else if (filename.endsWith(".gz")) return "application/x-gzip";
  //else if (filename.endsWith(".log")) return "application/octet-stream";
  //else if (filename.endsWith(".pdf")) return "application/octet-stream";
  return "text/plain";
}


void handleUnknown() {  /* Es wird versucht, die angegebene Datei aus dem LittleFS hochzuladen */
  digitalWriteFast(LED, HIGH);    /* LED on */
  String filename = HttpServer.uri();
#ifdef debug_wifi
  Serial.print(F("[DB] HTTP requested file: ")); Serial.println(filename);
#endif
  File pageFile = LittleFS.open(filename, "r");
  if (pageFile) {
#ifdef debug_wifi
    uint32_t fsizeDisk = pageFile.size();
    Serial.print(F("[DB] HTTP requested file size: ")); Serial.print(fsizeDisk); Serial.println(F(" bytes"));
#endif
    String contentTyp = getContentType(filename);
    if (filename.endsWith("html")) {
      HttpServer.sendHeader(F("cache-control"), F("private, no-cache"));            // A 'cache-control' header is missing or empty.
    } else {
      HttpServer.sendHeader(F("cache-control"), F("max-age=31536000, immutable"));  // Cache-Control: max-age=31536000, immutable
    }
    HttpServer.sendHeader(F("X-Content-Type-Options"), F("nosniff"));               // X-Content-Type-Options: nosniff
    size_t sent = HttpServer.streamFile(pageFile, contentTyp);
    sent = sent + 0; // !!! only Arduino Warning on this position | unused variable 'sent' [-Werror=unused-variable] !!!
    pageFile.close();
#ifdef debug_wifi
    Serial.print(F("[DB] HTTP TX file: ")); Serial.print(filename); Serial.print(F(", Bytes: "));
    Serial.print(sent); Serial.print(F(", Free Heap: "));
    uint32_t uis = ESP.getFreeHeap();
    Serial.println(uis);
#endif
  } else {
    HttpServer.sendHeader("Location", "/", true);                   // Redirect to our html
    HttpServer.send(404, "text/plain", F("Website not found !!!")); // Site404();
#ifdef debug_wifi
    Serial.println(F("[DB] HTTP requested file not found!"));
#endif
  }
  digitalWriteFast(LED, LOW); /* LED off */
}


void routing_websites() {
  HttpServer.on("/", web_index);
  HttpServer.on("/chip", web_chip);
  HttpServer.on("/detail", web_detail);
#ifdef CC110x
  HttpServer.on("/detail_cc110x_export", web_detail_cc110x_export);
  HttpServer.on("/detail_cc110x_import", web_detail_cc110x_import);
#elif RFM69
  HttpServer.on("/detail_rfm69_export", web_detail_SX1231_export);
#endif
  HttpServer.on("/modes", web_modes);
  HttpServer.on("/index.htm", web_index);
  HttpServer.on("/index.html", web_index);
  HttpServer.on("/log", web_log);
  HttpServer.on("/raw", web_raw);
  HttpServer.on("/wlan", web_wlan);
  HttpServer.onNotFound(handleUnknown);
}


String HTML_mod(String txt) {
  if (ChipFound != true) {
    txt.replace(F("<a href=\"raw\" class=\"RAW\">RAW data</a>"), F(""));
  }
  return txt;
}
