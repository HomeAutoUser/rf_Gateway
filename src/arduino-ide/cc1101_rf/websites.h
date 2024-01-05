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
void WebSocket_cc110x();
/* {"MODE":"Lacrosse_mode2","MS":"8","ToggleBank":"{ 11 12 - - }","Time":"30000"} */
void WebSocket_cc110x_detail();
/* {"MODE":"Lacrosse_mode2", "MODE_id":"12", "Time":"30000", "FreOff":"0.000", "Config":"01,2E,2E,41,2D,D4,05,80,00,00,00,06,00,21,65,6A,C8,83,02,22,F8,42,07,00,18,16,6C,43,68,91,87,6B,F8,B6,11,EA,2A,00,1F,41,00,01,2E,2E,41,2D,D4"} */
void WebSocket_cc110x_modes();
/* {"MODE":"Lacrosse_mode2", "MODE_id":"12"} */
void WebSocket_help();
/* {"CC1101":"no"} */
void WebSocket_index();
/* {"CC1101":"yes","RAM":"29496","Uptime":"305","MSGcnt":"37","WLANdB":"-60"} */
void WebSocket_raw(const String & html_raw);
/* {"MODE":"Lacrosse_mode2", "RAW":"9706636AE6", "RSSI":"-52", "AFC":"-15"} */
void routing_websites();
void web_cc110x();
void web_cc110x_detail();
void web_cc110x_detail_export();
void web_cc110x_detail_import();
void web_cc110x_modes();
void web_index();
void web_log();
void web_raw();
void web_wlan();
String HTML_mod(String txt);
void sendHtml(String & str);

/* Display of all websites from the web server
  https://wiki.selfhtml.org/wiki/HTML/Tutorials/Grundger%C3%BCst
  https://divtable.com/table-styler/
*/


const char html_meta[] PROGMEM = { "<!DOCTYPE html>"          /* added meta to html */
                                   "<html lang=\"de\">"
                                   "<head>"
                                   "<meta charset=\"utf-8\">"
                                   "<meta name=\"viewport\" content=\"width=device-width\">"
                                   "<link rel=\"stylesheet\" href=\"css/all.css\" type=\"text/css\">"
                                   "<title>cc110x_rf_Gateway</title>"
                                 };


const char html_head_table[] PROGMEM = { "<table>"          /* added table on head with all links */
                                         "<tr><th class=\"hth\" colspan=\"6\">cc110x_rf_Gateway</th></tr>"
                                         "<tr>"
                                         "<td class=\"htd\"><a href=\"/\" class=\"HOME\">HOME</a></td>"
                                         "<td class=\"htd\"><a href=\"cc110x\" class=\"CC110x\">CC110x</a></td>"
                                         "<td class=\"htd\"><a href=\"html/help.html\" class=\"Help\">Help</a></td>"
                                         "<td class=\"htd\"><a href=\"log\" class=\"Logfile\">Logfile</a></td>"
                                         "<td class=\"htd\">"
                                         "<a href=\"raw\" class=\"RAW\">RAW data</a>"
                                         "</td><td class=\"htd\"><a href=\"wlan\" class=\"WLAN\">WLAN</a></td></tr>"
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


void web_cc110x() {
  String website;
  website.reserve(2000);
  website += FPSTR(html_meta);
  website += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/cc110x.css\">"
               "<script src=\"js/cc110x.js\"></script>"
               "</head>");
  website += FPSTR(html_head_table);
  website = HTML_mod(website);
  website += F("<body>"
               "<table>"
               "<tr>");

  if (CC1101_found) {
    website += F("<th class=\"thf\">general information</th>"
                 "<th class=\"thf\"><a href=\"cc110x_detail\">detail information</a></th>"
                 "<th class=\"thf\"><a href=\"cc110x_modes\">receive modes</a></th>");
  } else {
    website += F("<th colspan=\"3\">general information</th>");
  }

  website += F("</tr><tr><td>chip detected</td><td colspan=\"2\">");
  CC1101_found ? website += F("yes") : website += F("no");
  website += F("</td></tr>");

  if (CC1101_found) {
    website += F("<tr><td>chip PARTNUM</td><td colspan=\"2\">");
    website += onlyDecToHex2Digit(CC1101_readReg(CC1101_PARTNUM, READ_BURST));
    website += F("</td></tr><tr><td>chip VERSION</td><td colspan=\"2\">");
    website += onlyDecToHex2Digit(CC1101_readReg(CC1101_VERSION, READ_BURST));
    website += F("</td></tr><tr><td>chip MARCSTATE</td><td colspan=\"2\"><span id=\"MS\">");
    website += CC1101_readReg(CC1101_MARCSTATE, READ_BURST);
    website += F("</span></td></tr><tr><td>chip reception mode</td><td colspan=\"2\"><span id=\"MODE\">");
    website += activated_mode_name;
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


void web_cc110x_modes() {
  if (!CC1101_found) {
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
  Serial.print(F("DB web_cc1101_modes, submit ")); Serial.println(submit);
  Serial.print(F("DB web_cc1101_modes, tb ")); Serial.println(tb);
  Serial.print(F("DB web_cc1101_modes, tgtime ")); Serial.println(tgtime);
  Serial.print(F("DB web_cc1101_modes, count ")); Serial.println(countargs);
#endif

  if (countargs != 0) {
    if (submit != "" && submit != "time") {  // button "enable reception"
      InputCmd = "m";
      InputCmd += submit;

#ifdef debug_html
      Serial.print(F("DB web_cc1101_modes, set reception ")); Serial.println(submit);
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
      Serial.print(F("DB web_cc1101_modes, set toggletime to ")); Serial.println(tgtime);
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
      Serial.print(F("DB web_cc1101_modes, set togglebank ")); Serial.print(tb_nr);
      Serial.print(F(" to ")); Serial.println(tb_val);
#endif

      web_status = F("<tr><td class=\"in grn\">");
      if (tb_nr <= 3) {
#ifdef debug_html
        Serial.print(F("DB web_cc1101_modes, togglebank value "));
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
  website += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/cc110x_modes.css\">"
               "</head>");
  website += FPSTR(html_head_table);
  website += F("<body>"
               "<script src=\"js/cc110x_modes.js\"></script>"
               "<form method=\"post\">" /* form method wichtig für Daten von Button´s !!! https://www.w3schools.com/tags/ref_httpmethods.asp */
               "<table id=\"rec_mod\">"
               "<tr>"
               "<th class=\"thf1\"><a href=\"cc110x\">general information</a></th>"
               "<th class=\"thf1\"><a href=\"cc110x_detail\">detail information</a></th>"
               "<th class=\"thf1\">receive modes</th></tr>");

  for (byte i = 0; i < RegistersCntMax; i++) {
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

    if (activated_mode_name == Registers[i].name) {
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


void web_cc110x_detail_export() {
  if (!CC1101_found) {
    HttpServer.send(404, "text/plain", F("Website not found !!!"));
  }

  String website = FPSTR(html_meta);
  website.reserve(1500);
  website += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/cc110x_detail_exp.css\"></head>"
               "Export all current register values (without the registers 0x29 ... 0x2E)<br>Just copy and paste string into your application<br>"
               "<br>FHEM - SIGNALduino Format | set &lsaquo;name&rsaquo; cc1101_reg<br>");
  String website_p2 = F("<br>FHEM - SIGNALduino Format | SD_ProtocolData.pm, entry register => <br>");

  for (byte i = 0; i <= 46; i++) { /* all registers */
    if (i == 0) {
      website_p2 += '[';
    }
    if (i < 41) {
      website += onlyDecToHex2Digit(i);
      website += onlyDecToHex2Digit(CC1101_readReg(i, READ_BURST));
      website += ' ';
      website_p2 += '\'';
      website_p2 += onlyDecToHex2Digit(i);
      website_p2 += onlyDecToHex2Digit(CC1101_readReg(i, READ_BURST));
      website_p2 += '\'';
      if (i != 40) {
        website_p2 += ',';
      }
    }
  }
  website += F("<br>");
  website += website_p2;
  website += F("]<br><br><a class=\"back\" href=\"/cc110x_detail\">&crarr; back to detail information</a>"
               "</body></html>");
  sendHtml(website);
}


void web_cc110x_detail_import() {
  if (!CC1101_found) {
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
  website += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/cc110x_detail_imp.css\"></head>"
               "<body><form method=\"post\">"); /* form method wichtig für Daten von Button´s !!! */

  if (countargs != 0) {
#ifdef debug_html
    Serial.println(F("###########################"));
    Serial.print(F("DB web_cc110x_detail_import, submit     ")); Serial.println(submit);
    Serial.print(F("DB web_cc110x_detail_import, countargs  ")); Serial.println(countargs);
    Serial.print(F("DB web_cc110x_detail_import, imp        ")); Serial.println(imp);
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
          Serial.print(F("DB web_cc110x_detail_import, cc110x adr: ")); Serial.print(PartAdr);
          Serial.print(' '); Serial.println(Adr);
          Serial.print(F("DB web_cc110x_detail_import, cc110x val: ")); Serial.print(PartVal);
          Serial.print(' '); Serial.println(Val);
#endif
          if (Adr > 0x2E) {
            break;
          } else {
            CC1101_writeReg(Adr, Val);    // write in cc1101
            EEPROMwrite(Adr, Val);        // write in flash
          }
          Part = "";                      // reset String
          PartAdr = "";                   // reset String
          PartVal = "";                   // reset String
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
                 "<input size=\"100\" maxlength=\"288\" value=\"example ['01AB','11FB']\" name=\"imp\" pattern=\"^\\['[[0-9a-fA-F]{4}'(,'[[0-9a-fA-F]{4}')+\\]$\">"
                 "<br><br><button class=\"btn\" type=\"submit\" name=\"submit\" value=\"registers\">acceptance of the values ​​in the register</button>");
  }
  website += F("<br><br><a class=\"back\" href=\"/cc110x_detail\">&crarr; back to detail information</a>"
               "</form></body></html>");
  sendHtml(website);
}


void web_cc110x_detail() {
  if (!CC1101_found) {
    HttpServer.send(404, "text/plain", F("Website not found !!!"));
  }

  String afc = HttpServer.arg("afc");
  String mod = HttpServer.arg("modulation");
  String submit = HttpServer.arg("submit");  // welcher Button wurde betätigt
  String temp;
  String web_stat = F("<span id=\"state\"></span>");
  byte web_regData[47] = {};
  float bandw = (HttpServer.arg("bandw").toFloat());
  float deviation = (HttpServer.arg("deviation").toFloat());
  float freqOff = (HttpServer.arg("freq_off").toFloat());
  long datarate = (HttpServer.arg("datarate").toFloat()) * 1000;
  long freq = (HttpServer.arg("freq").toFloat()) * 1000;
  uint8_t countargs = HttpServer.args();    // Anzahl Argumente

  if (countargs != 0) {
#ifdef debug_html
    Serial.println(F("###########################"));
    Serial.print(F("DB web_cc1101_detail, submit     ")); Serial.println(submit);
    Serial.print(F("DB web_cc1101_detail, countargs  ")); Serial.println(countargs);
    Serial.print(F("DB web_cc1101_detail, freq       ")); Serial.println(freq);
    Serial.print(F("DB web_cc1101_detail, freqOff    ")); Serial.println(freqOff);
    Serial.print(F("DB web_cc1101_detail, bandw      ")); Serial.println(bandw);
    Serial.print(F("DB web_cc1101_detail, datarate   ")); Serial.println(datarate);
    Serial.print(F("DB web_cc1101_detail, deviation  ")); Serial.println(deviation);
    Serial.print(F("DB web_cc1101_detail, modulation ")); Serial.println(mod);
#endif

    if (countargs > 0) { /* register values from browser | set registers button -> into array */
      for (byte i = 0; i <= 46; i++) {
        temp = 'r';
        temp += i;
        web_regData[i] = hexToDec(HttpServer.arg(temp));
#ifdef debug_html
        Serial.print(F("DB web_cc1101_detail, web_regData[")); Serial.print(i); Serial.print(F("] = ")); Serial.println(web_regData[i]);
#endif
      }
    }

    if (submit == "bfreq") {
#ifdef debug_html
      Serial.println(F("DB web_cc1101_detail, submit set frequency, offset, AFC pushed"));
#endif
      // AFC
      if (afc == "1") {
        InputCommand(F("CEA")); // enable AFC
      } else {
        InputCommand(F("CDA")); // disable AFC
      }
      // Frequency Offset
      Freq_offset = freqOff;
      CC1101_writeReg(CC1101_FSCTRL0, 0);  // 0x0C: FSCTRL0 – Frequency Synthesizer Control
      EEPROM.put(EEPROM_ADDR_FOFFSET, Freq_offset);
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
      EEPROM.commit();
#endif
      // Frequency
      byte value[3];
      web_Freq_Set(freq, value);
      for (byte i = 0; i < 3; i++) {    /* write value to register 0D,0E,0F */
        CC1101_writeReg(i + 13, value[i]); // write in cc1101
        EEPROMwrite(i + 13, value[i]);     // write in flash
      }
      web_stat = F("Frequency, frequency offset and AFC set &#10004;");
    } else if (submit == "bbandw") {
#ifdef debug_html
      Serial.print(F("DB web_cc1101_detail, button set bandwidth pushed with ")); Serial.println(bandw);
      Serial.print(F("DB web_cc1101_detail, register 0x10 value is ")); Serial.println(web_regData[16]);
#endif
      web_stat = F("Bandwidth set &#10004;");
      byte value = web_Bandw_cal(bandw, (web_regData[16] & 0x0f)); /* input complete | input split */
      CC1101_writeReg(16, value);                                                   // write in cc1101
      EEPROMwrite(16, value);                                                       // write in flash
    } else if (submit == "bdatarate") {
#ifdef debug_html
      Serial.print(F("DB web_cc1101_detail, button set datarate pushed with ")); Serial.println(datarate);
#endif
      web_stat = F("DataRate set &#10004;");

      byte value[2];
      web_Datarate_Set(datarate, value);

      for (byte i = 0; i < 2; i++) {    /* write value to register 0D,0E,0F */
        CC1101_writeReg(i + 16, value[i]); // write in cc1101
        EEPROMwrite(i + 16, value[i]);     // write in flash
      }
    } else if (submit == "bdev") {
#ifdef debug_html
      Serial.print(F("DB web_cc1101_detail, button set deviation pushed with ")); Serial.println(deviation);
#endif
      web_stat = F("Deviation set &#10004;");

      byte value = web_Devi_Set(deviation);
      CC1101_writeReg(21, value);  // write in cc1101
      EEPROMwrite(21, value);      // write in flash
    } else if (submit == "bmod") {
#ifdef debug_html
      Serial.print(F("DB web_cc1101_detail, set modulation to ")); Serial.println(mod);
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

      CC1101_writeReg(18, web_Mod_set(value));      // write in cc1101
      EEPROMwrite(18, web_Mod_set(value));          // write in flash
    } else if (submit == "breg") {
#ifdef debug_html
      Serial.println(F("DB web_cc1101_detail, button set registers pushed"));
#endif
      web_stat = F("all registers set &#10004;");

      for (byte i = 0; i <= 46; i++) { /* all registers */
#ifdef debug_html
        Serial.print(F("DB web_cc1101_detail, regData["));
        Serial.print(i); Serial.print(F("] = ")); Serial.println(web_regData[i]);
#endif
        /* compare value with value to be written */
        byte temp = CC1101_readReg(i, READ_BURST);
        if (web_regData[i] != temp) {
#ifdef debug_html
          /* i > 34 && <= 40    | automatic control register */
          /* i > 40             | test register */
          Serial.print(F("DB web_cc1101_detail, regData[")); Serial.print(i);
          Serial.print(F("] value has changed ")); Serial.print(temp);
          Serial.print(F(" -> ")); Serial.println(web_regData[i]);
#endif
          if (i == 6) { /* ToDo - probably not optimal -> if register 6 is changed (dependencies) ??? */
            activated_mode_packet_length = web_regData[i];
          }
          /* write value to registe */
          CC1101_writeReg(i, web_regData[i]);   // write in cc1101
          EEPROMwrite(i, web_regData[i]);       // write in flash
        }
      }
    }
    activated_mode_name = F("CC110x user configuration");
  }

  String website = FPSTR(html_meta);
  website.reserve(13000);
  website += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/cc110x_detail.css\">"
               "<script src=\"js/cc110x_detail.js\"></script>"
               "</head>");
  website += FPSTR(html_head_table);
  website += F("<body><form method=\"post\">" /* form method wichtig für Daten von Button´s !!! */
               "<table><tr>"
               "<th class=\"f1\" colspan=\"2\"><a href=\"cc110x\">general information</a></th>"
               "<th class=\"f1\" colspan=\"2\">detail information</th>"
               "<th class=\"f1\" colspan=\"2\"><a href=\"cc110x_modes\">receive modes</a></th></tr>"
               // Frequency (should | is)
               "<tr><td colspan=\"2\">Frequency (should | is)</td><td class=\"ce\" colspan=\"2\"><span id=\"FREQis\"></span> | <span id=\"FREQ\"></span>"
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
               "<tr><td colspan=\"2\">Bandwidth</td><td class=\"ce\" colspan=\"2\"><span id=\"CHANBW\"></span></td>"
               "<td class=\"ce\"><input aria-label=\"Bw\" size=\"6\" id=\"p3\" name=\"bandw\" value=\"\">"
               "<div class=\"txt\">&ensp;kHz</div></td><td class=\"ce\"><button class=\"btn\" type=\"submit\" name=\"submit\" value=\"bbandw\">set</button></td></tr>"
               // DataRate
               "<tr><td colspan=\"2\">DataRate</td><td class=\"ce\" colspan=\"2\"><span id=\"DRATE\"></span></td>"
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
               "<button class=\"btn\" type=\"button\" onClick=\"location.href='cc110x_detail_export'\">export all registers</button>&emsp;"
               "<button class=\"btn\" type=\"button\" onClick=\"location.href='cc110x_detail_import'\">import registers</button></td></tr>"
               // status line
               "<tr><td class=\"in\" colspan=\"6\">");
  website += web_stat;
  website += F("</td></tr><tr><td>register</td><td class=\"ce\">value</td><td colspan=\"4\">notes</td></tr>");

  for (byte i = 0; i <= 46; i++) {
    website += F("<tr><td class=\"f4\"><span id=\"s");
    website += i;
    website += F("\"></span></td><td class=\"ce\"><input name=\"r"); /* registername for GET / POST */
    website += i;
    website += F("\" value=\"");
    website += onlyDecToHex2Digit(CC1101_readReg(i, READ_BURST));                 /* value */
    website += F("\" type=\"text\"></td><td colspan=\"3\"><span id=\"n");
    website += i;
    website += F("\"></span></td><td id=\"t");
    website += i;
    website += F("\">i</td></tr>");
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
    Serial.println(F("File log.txt nothing found"));
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
  if (!CC1101_found) {
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
  website += activated_mode_name;
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
  Serial.print(F("DB web_wlan, setssid    ")); Serial.println(qssid);
  Serial.print(F("DB web_wlan, hiddenssid ")); Serial.println(hssid);
  Serial.print(F("DB web_wlan, pw         ")); Serial.println(qpass);
  Serial.print(F("DB web_wlan, dhcp       ")); Serial.println(qdhcp);
  Serial.print(F("DB web_wlan, ip         ")); Serial.println(qip);
  Serial.print(F("DB web_wlan, dns        ")); Serial.println(qdns);
  Serial.print(F("DB web_wlan, sn         ")); Serial.println(qsnm);
  Serial.print(F("DB web_wlan, gw         ")); Serial.println(qsgw);
  Serial.print(F("DB web_wlan, submit     ")); Serial.println(submit);
  Serial.print(F("DB web_wlan, countargs  ")); Serial.println(countargs);
  Serial.println(F("DB web_wlan, scan networks ..."));
#endif

  String website = FPSTR(html_meta);
  website.reserve(10000);
  website += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/wlan.css\">"
               "<script src=\"js/wlan.js\"></script>"
               "</head>");
  website += FPSTR(html_head_table);
  website = HTML_mod(website);
  website += F("<body><form method=\"post\">" /* form method wichtig für Daten von Button´s !!! */
               "<table>"                     /* START Tabelle gesamt */
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
    Serial.print(F("DB web_wlan,"));
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

  /* Button connect | with WIFI Settings */
  if (submit == "connect") {
#ifdef debug_html
    Serial.println(qdhcp == "1" ? F("DB web_wlan, qdhcp 1 (on)") : F("DB web_wlan, qdhcp 0 (off)"));
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
        Serial.print(F("DB web_wlan, static IP ")); Serial.println(qip);
#endif
        EEPROMwrite_ipaddress(EEPROM_ADDR_IP, qip);  // write IPAddress to EEPROM
        eip = EEPROMread_ipaddress(EEPROM_ADDR_IP);
      } else {
        /* Static IP address invalid! */
      }

      /* Standard-Gateway übernehmen */
      if (str2ip((char*)qsgw.c_str(), ipaddress)) {  // convert String to Array of 4 Bytes
#ifdef debug_html
        Serial.print(F("DB web_wlan, Standard-Gateway ")); Serial.println(qsgw);
#endif
        EEPROMwrite_ipaddress(EEPROM_ADDR_GATEWAY, qsgw);  // write IPAddress to EEPROM
        esgw = EEPROMread_ipaddress(EEPROM_ADDR_GATEWAY);
      } else {
        /* Default gateway invalid! */
      }

      /* statischen Domain Name Server übernehmen */
      if (str2ip((char*)qdns.c_str(), ipaddress)) {     // convert String to Array of 4 Bytes
#ifdef debug_html
        Serial.print(F("DB web_wlan, static Domain Name Server ")); Serial.println(qdns);
#endif
        EEPROMwrite_ipaddress(EEPROM_ADDR_DNS, qdns);   // write IPAddress to EEPROM
        edns = EEPROMread_ipaddress(EEPROM_ADDR_DNS);
      } else {
        /* Invalid domain name server! */
      }

      /* Subnetmask übernehmen */
      if (str2ip((char*)qsnm.c_str(), ipaddress)) {     // convert String to Array of 4 Bytes
#ifdef debug_html
        Serial.print(F("DB web_wlan, subnet mask ")); Serial.println(qsnm);
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
    Serial.println(F("DB web_wlan, send HTML (connect)"));
#endif

    delay(250);
    start_WLAN_STATION(qssid, qpass);
    submit = "";
  }
  /* Button connect - END  */

  website += F("</table></form></body></html>");

  /* Button wps | wps methode for connection */
  if (submit == "wps") {
#ifdef debug_html
    Serial.println(WLAN_AP == 0 ? F("DB web_wlan, wps methode setting (call from station)") : F("DB web_wlan, wps methode setting (call from AP)"));
#endif
    delay(250);

    if (!start_WLAN_WPS()) {
      if (WLAN_AP == 1) {
        appendLogFile(F("WPS failed, start AP"));
        start_WLAN_AP(OwnStationHostname, WLAN_password_ap);
      } else {
        appendLogFile(F("WPS failed, use old WiFi settings"));
        start_WLAN_STATION(EEPROMread_string(EEPROM_ADDR_SSID), EEPROMread_string(EEPROM_ADDR_PASS));
      }
    }
  }
  /* Button wps - END */

  if (countargs == 0) {
#ifdef debug_html
    Serial.println(F("DB web_wlan, send HTML (args == 0)"));
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
  Serial.print(F("Website "));
  Serial.print(HttpServer.uri());
  Serial.print(F(" HTTP TX Bytes: "));
  Serial.print(str.length());
  Serial.print(F(", Free Heap: "));
  Serial.println(freeRam());
#endif
}

void WebSocket_cc110x() {
#ifdef debug_websocket
  Serial.println(F("DB WebSocket_cc110x running"));
#endif
  if (CC1101_found) {
    if (webSocket.connectedClients() > 0) {
      String website = activated_mode_name;
      website.reserve(55);
      website += ',';
      website += CC1101_readReg(CC1101_MARCSTATE, READ_BURST);
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
        if (webSocketSite[num] == "/cc110x") {
          webSocket.sendTXT(num, website);
        }
      }
    }
  }
}


void WebSocket_cc110x_detail() {
#ifdef debug_websocket
  Serial.println(F("DB WebSocket_cc110x_detail running"));
#endif
  if (webSocket.connectedClients() > 0) {
    String website = "";
    website.reserve(200);
    for (byte i = 0; i <= 46; i++) { /* all registers | fastest variant */
      if (ToggleTime != 0) {
        website += onlyDecToHex2Digit(pgm_read_byte_near(Registers[activated_mode_nr].reg_val + i));
      } else {
        website += onlyDecToHex2Digit(CC1101_readReg(i, READ_BURST));
      }
      if (i == 46) {
        website += F(",detail,");
      } else {
        website += ',';
      }
    }

    website += activated_mode_name;
    website += ',';
    website += String(Freq_offset, 3);

    for (uint8_t num = 0; num < WEBSOCKETS_SERVER_CLIENT_MAX; num++) {
      if (webSocketSite[num] == "/cc110x_detail") {
        webSocket.sendTXT(num, website);
      }
    }
  }
}


void WebSocket_cc110x_modes() {
#ifdef debug_websocket
  Serial.println(F("DB WebSocket_cc110x_modes running"));
#endif
  if (webSocket.connectedClients() > 0) {
    String website = F("MODE,");
    website.reserve(35);
    website += activated_mode_name;
    website += ',';
    website += activated_mode_nr;

    for (uint8_t num = 0; num < WEBSOCKETS_SERVER_CLIENT_MAX; num++) {
      if (webSocketSite[num] == "/cc110x_modes" || webSocketSite[num] == F("/raw")) {
        webSocket.sendTXT(num, website);
      }
    }
  }
}


void WebSocket_help() {
#ifdef debug_websocket
  Serial.println(F("DB WebSocket_help running"));
#endif
  if (webSocket.connectedClients() > 0) {
    String website = F("CC1101,");
    website += CC1101_found == true ? F("yes") : F("no");
    for (uint8_t num = 0; num < WEBSOCKETS_SERVER_CLIENT_MAX; num++) {
      if (webSocketSite[num] == "/help") {
        webSocket.sendTXT(num, website);
      }
    }
  }
}


void WebSocket_index() {
#ifdef debug_websocket
  Serial.println(F("DB WebSocket_index running"));
#endif
  if (webSocket.connectedClients() > 0) {
    String website = F("CC1101,");
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
  Serial.println(F("DB WebSocket_raw running"));
#endif
  if (webSocket.connectedClients() > 0) {
    String website = F("RAW,");
    website.reserve(460);
    website += activated_mode_name;
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

// Es wird versucht, die angegebene Datei aus dem LittleFS hochzuladen
void handleUnknown() {
  digitalWriteFast(LED, HIGH);    /* LED on */
  String filename = HttpServer.uri();
#ifdef debug_wifi
  Serial.print(F("HTTP requested file: "));
  Serial.println(filename);
#endif
  File pageFile = LittleFS.open(filename, "r");
  if (pageFile) {
#ifdef debug_wifi
    uint32_t fsizeDisk = pageFile.size();
    Serial.print(F("HTTP requested file size: "));
    Serial.print(fsizeDisk);
    Serial.println(F(" bytes"));
#endif
    String contentTyp = getContentType(filename);
    if (filename.endsWith("html")) {
      HttpServer.sendHeader(F("cache-control"), F("private, no-cache")); // A 'cache-control' header is missing or empty.
    } else {
      HttpServer.sendHeader(F("cache-control"), F("max-age=31536000, immutable")); // Cache-Control: max-age=31536000, immutable
    }
    HttpServer.sendHeader(F("X-Content-Type-Options"), F("nosniff")); // X-Content-Type-Options: nosniff
    size_t sent = HttpServer.streamFile(pageFile, contentTyp);
    sent = sent + 0; // !!! only Arduino Warning on this position | unused variable 'sent' [-Werror=unused-variable] !!!
    pageFile.close();
#ifdef debug_wifi
    Serial.print(F("HTTP TX file: "));
    Serial.print(filename);
    Serial.print(F(", Bytes: "));
    Serial.print(sent);
    Serial.print(F(", Free Heap: "));
    uint32_t uis = ESP.getFreeHeap();
    Serial.println(uis);
#endif
  } else {
    HttpServer.sendHeader("Location", "/", true);  // Redirect to our html
    HttpServer.send(404, "text/plain", F("Website not found !!!"));
    //    Site404();
#ifdef debug_wifi
    Serial.println(F("HTTP requested file not found!"));
#endif
  }
  digitalWriteFast(LED, LOW); /* LED off */
}

void routing_websites() {
  HttpServer.on("/", web_index);
  HttpServer.on("/cc110x", web_cc110x);
  HttpServer.on("/cc110x_detail", web_cc110x_detail);
  HttpServer.on("/cc110x_detail_export", web_cc110x_detail_export);
  HttpServer.on("/cc110x_detail_import", web_cc110x_detail_import);
  HttpServer.on("/cc110x_modes", web_cc110x_modes);
  HttpServer.on("/index.htm", web_index);
  HttpServer.on("/index.html", web_index);
  HttpServer.on("/log", web_log);
  HttpServer.on("/raw", web_raw);
  HttpServer.on("/wlan", web_wlan);
  HttpServer.onNotFound(handleUnknown);
}

String HTML_mod(String txt) {
  if (CC1101_found != true) {
    txt.replace(F("<a href=\"raw\" class=\"RAW\">RAW data</a>"), F(""));
  }
  return txt;
}
