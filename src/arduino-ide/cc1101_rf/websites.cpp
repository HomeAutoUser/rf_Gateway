#include "config.h"

#ifdef CODE_ESP
#include "websites.h"

void web_index() {          // ########## web_index ##########
  String submit = HttpServer.arg("submit");               // welcher Button wurde betätigt
  if (submit == "MSG0") {
    msgCount = 0;
    for (uint8_t c = 0; c < NUMBER_OF_MODES; c++) {
      msgCountMode[c] = 0;
    }
    uptimeReset = uptime;
    appendLogFile(F("Message counter reset"));
  } else if (submit == "SWRESET") {
    ESP.restart();
  }
  String website = FPSTR(html_meta);
  website.reserve(2000);
  website += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/index.css\">"
               "<script src=\"/js/index.js\"></script>" // The type attribute is unnecessary for JavaScript resources.
               "</head>");
  website += FPSTR(html_head_table);
  HTML_mod(website);  // replace "RAW data" when no chip found
  website += F("<body>"
               "<form method=\"post\">" /* form method wichtig für Daten von Button´s !!! https://www.w3schools.com/tags/ref_httpmethods.asp */
               "<table><tr><th colspan=\"2\">Device - Information</th></tr>"
               "<tr><td class=\"tdf\">Firmware compiled</td><td>");
  website += FPSTR(compile_date);
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


void web_chip() {           // ########## web_chip ##########
  String website;
  website.reserve(2000);
  website += FPSTR(html_meta);
  website += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/chip.css\">"
               "<script src=\"/js/chip.js\"></script>"
               "</head>");
  website += FPSTR(html_head_table);
  HTML_mod(website);  // replace "RAW data" when no chip found
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
    website += (F("</span> RegIrqFlags1: "));
    website += String(Chip_readReg(0x27, 0x00), BIN);
    website += (F(" RegIrqFlags2: "));
    website += String(Chip_readReg(0x28, 0x00), BIN);
#endif
    website += F("</td></tr><tr><td>chip reception mode</td><td colspan=\"2\"><span id=\"MODE\">");
    website += ReceiveModeName;
    website += F("</span></td></tr><tr><td>Enabled mode(s)</td><td colspan=\"2\"><span id=\"ToggleBank\">");
    for (uint8_t modeNr = 0; modeNr < NUMBER_OF_MODES; modeNr++) {
      if (ToggleArray[modeNr]) {
        website += modeNr;
        website += ' ';
      }
    }
    website += F("</span></td></tr><tr><td>Toggle time in seconds</td><td colspan=\"2\"><span id=\"Time\">");
    website += ToggleTimeMode[ReceiveModeNr];
    website += F("</span></td></tr>");
  }
  website += F("</table></body></html>");
  sendHtml(website);
}


void web_modes() {          // ########## web_modes ##########
  if (!ChipFound) {
    web_chip();
  }
  uint8_t countargs = HttpServer.args();                  // Anzahl Argumente
  String web_status = F("<tr id=\"trLast\"><td class=\"ac\"><span id=\"stat\"></span></td>");
#ifdef debug_html
  Serial.print(F("[DB] web_modes, count ")); Serial.println(countargs);
#endif
  if (countargs != 0) {
    String submit = HttpServer.arg("s");        // Button
#ifdef debug_html
    Serial.print(F("[DB] web_modes, submit ")); Serial.println(submit);
#endif
    if (submit == F("all") || submit == F("none")) { // Button "all" or "none" - enable/disable all modes
      uint8_t enable = submit == F("all") ? 1 : 0;
#ifdef debug_html
      Serial.print(F("[DB] web_modes, submit ")); Serial.print(submit);
      Serial.print(F(", set all modes to ")); Serial.println(enable);
#endif
      for (uint8_t modeNr = 0; modeNr < NUMBER_OF_MODES; modeNr++) {
        ToggleArray[modeNr] = enable;
      }
    }
    if (submit == F("set")) { // Button "set" - set all scan times
      uint8_t setTimeAll = HttpServer.arg("ta").toInt();
#ifdef debug_html
      Serial.print(F("[DB] web_modes, submit ")); Serial.print(submit);
      Serial.print(F(", set all scan times to ")); Serial.println(setTimeAll);
#endif
      for (uint8_t modeNr = 0; modeNr < NUMBER_OF_MODES; modeNr++) {
        ToggleTimeMode[modeNr] = setTimeAll;
      }
    }
    if (submit == F("START")) { // Button "START"
      ToggleCnt = 0;
      for (uint8_t modeNr = 0; modeNr < NUMBER_OF_MODES; modeNr++) {
        String argument = "c"; argument += modeNr; // build argument for checkboxes
        if (HttpServer.hasArg(argument)) {
          ToggleCnt++;
          if (ToggleCnt == 1) { // save last active mode
            ReceiveModeNr = modeNr;
          }
        }
        ToggleArray[modeNr] = HttpServer.hasArg(argument); // set mode active/deactive
        EEPROM.write(EEPROM_ADDR_ToggleMode + modeNr, HttpServer.hasArg(argument));
        EEPROM.commit();
#ifdef debug_html
        Serial.print(F("[DB] web_modes, submit ")); Serial.print(submit);
        Serial.print(F(", mode ")); Serial.print(modeNr);
        Serial.print(F(", enable ")); Serial.print(HttpServer.hasArg(argument));
#endif
        argument = "t"; argument += modeNr; // build argument for time
        uint8_t setTimeMode = HttpServer.arg(argument).toInt();
        ToggleTimeMode[modeNr] = setTimeMode; // set mode toggletime
        EEPROM.write(EEPROM_ADDR_ToggleTime + modeNr, setTimeMode);
        EEPROM.commit();
#ifdef debug_html
        Serial.print(F(", time ")); Serial.println(setTimeMode);
#endif
      }
#ifdef debug_html
      Serial.print(F("[DB] web_modes, number of activated modes ")); Serial.println(ToggleCnt);
#endif
      if (ToggleCnt == 0) {
        web_status = F("<tr><td class=\"ac red\">no mode enabled!</td>");
        ReceiveModeNr = 255;
#ifdef CC110x
        CC110x_CmdStrobe(CC110x_SIDLE); // Exit RX / TX, turn off frequency synthesizer and exit Wake-On-Radio mode if applicable
#elif RFM69
        SX1231_setIdleMode();           // SX1231 start idle mode
#endif
      } else if (ToggleCnt == 1) {
        web_status = F("<tr><td class=\"ac grn\">");
        web_status += Registers[ReceiveModeNr].name;
        web_status += F(" &#10004; | toggle &#128721;</td>");
        Interupt_Variant(ReceiveModeNr);
      } else if (ToggleCnt > 1) {
        web_status = F("<tr><td class=\"ac grn\"><span id=\"stat\">toggle started &#10004;</span></td>");
        toggleTick = millis();
        Interupt_Variant(ReceiveModeNr); // start toggle
      }
    }
  }

  String website = FPSTR(html_meta);
  website.reserve(2080 + NUMBER_OF_MODES * 576);
  website += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/modes.css\">"
               "</head>");
  website += FPSTR(html_head_table);
  website += F("<body>"
               "<script src=\"/js/modes.js\"></script>"
               "<form method=\"get\">" /* form method wichtig für Daten von Button´s !!! https://www.w3schools.com/tags/ref_httpmethods.asp */
               "<table id=\"rec_mod\">"
               "<tr>"
               "<th class=\"thf1\"><a href=\"chip\">general information</a></th>"
               "<th class=\"thf1\"><a href=\"detail\">detail information</a></th>"
               "<th class=\"thf1\" colspan=\"2\">receive modes</th>"
               "</tr>"
               "<tr>"
               "<td class=\"acf\">available modes</td>"
               "<td class=\"acf\">enable modes</td>"
               "<td class=\"acf\">scan time (seconds)</td>"
               "<td class=\"acf\">msg count</td>"
               "</tr>");
  for (uint8_t modeNr = 0; modeNr < NUMBER_OF_MODES; modeNr++) {
    website += F("<tr><td"); // Spalte 1 - available modes
    website += ReceiveModeNr == modeNr ? F(" class=\"bggn\">") : F(">"); // background color light green
    if (modeNr <= 9) {
      website += F("0");
    }
    website += modeNr;
    website += F(" - ");
    website += Registers[modeNr].name;
    website += F("</td>");
    website += F("<td class=\"ac"); // Spalte 2 - enable modes
    website += ReceiveModeNr == modeNr ? F(" bggn") : F(""); // background color light green
    website += F("\"><input name=\"c"); // c0, c1, c2 ...
    website += modeNr;
    website += F("\" type=\"checkbox\" value=\"1\"");
    website += ToggleArray[modeNr] == 1 ? F(" checked") : F("");
    website += F("></td>");
    website += F("<td class=\"ac"); // Spalte 3 - scan time (seconds)
    website += ReceiveModeNr == modeNr ? F(" bggn") : F(""); // background color light green
    website += F("\"><input name=\"t"); // t0, t1, t2 ...
    website += modeNr;
    website += F("\" type=\"number\" value=\"");
    website += ToggleTimeMode[modeNr];
    website += F("\" min=\"");
    website += TOGGLE_TIME_MIN;
    website += F("\" max=\"");
    website += TOGGLE_TIME_MAX;
    website += F("\"></td><td class=\"ar"); // Spalte 4 - msg count
    website += ReceiveModeNr == modeNr ? F(" bggn") : F(""); // background color light green
    website += F("\" colspan=\"1\"><span id=\"c"); // msg count
    website += modeNr;
    website += F("\">");
    website += msgCountMode[modeNr];
    website += F("</span></td></tr>");
  }
  website += web_status; // Spalte 1
  website += F("<td class=\"ac\">select <input class=\"btn1\" name=\"s\" type=\"submit\" value=\"all\">" // Spalte 2
               "<input class=\"btn1\" name=\"s\" type=\"submit\" value=\"none\"></td>"
               "<td class=\"ac\">all times&nbsp;<input name=\"ta\" type=\"number\" value=\"60\" min=\"");
  website += TOGGLE_TIME_MIN;
  website += F("\" max=\"");
  website += TOGGLE_TIME_MAX;
  website += F("\"> <input class=\"btn1\" name=\"s\" type=\"submit\" value=\"set\"></td>"  // Spalte 3
               "<td class=\"ac\"><input class=\"btn\" type=\"submit\" name=\"s\" value=\"START\"></td>" // Spalte 4
               "</tr></table></form></body></html>");
  sendHtml(website);
}


void web_detail_export() {  // ########## web_detail_export ##########
  if (!ChipFound) {
    web_chip();
  }
  String website = FPSTR(html_meta);

#ifdef CC110x                 // #### web_detail_export - CC110x #### //
  website.reserve(1400);
  website += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/detail_exp.css\">"
               "<script src=\"/js/detail_cc110x_exp.js\"></script></head>"
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
  website += F("]<br><br>File to import on program SmartRF Studio 7<br>[development]<br>");

#elif RFM69                   // #### web_detail_export - RFM69 #### //
  website.reserve(950);
  website += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/detail_exp.css\">"
               "<script src=\"/js/detail_rfm69_exp.js\"></script></head>"
               "Export all current register values<br>Just copy and paste string into your application<br>"
               "<br>FHEM - SIGNALduino Format | SD_ProtocolData.pm, entry register =>"
               "<br>[<span id=\"REGs\"></span>]<br>"
               "<br>File to import on program SX1231SKB"
               "<br><button id=\"save-btn\" onclick=\"saveFile('SX')\">save current register in \"SX1231 Starter Kit\" format</button><br>"
               "<br>File to import on program SmartRF Studio 7"
               "<br><button id=\"save-btn\" onclick=\"saveFile('C')\">save a part of the registers in \"SmartRF Studio 7\" format</button><br>");
#endif

  website += F("<br><a class=\"back\" href=\"/detail\">&crarr; back to detail information</a>"
               "</body></html>");
  sendHtml(website);
}                             // #### web_detail_export - END #### //


void web_detail_import() {  // ########## web_detail_import ##########
  if (!ChipFound) {
    web_chip();
  }
#ifdef CC110x                 // #### web_detail_import - CC110x #### //
  String submit = HttpServer.arg("submit");   // welcher Button wurde betätigt
  String imp = HttpServer.arg("imp");         // String der Register inklusive Werte
  uint8_t countargs = HttpServer.args();      // Anzahl Argumente
  String website = FPSTR(html_meta);
  website.reserve(1024);
  website += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/detail_imp.css\"></head>"
               "<body><form method=\"post\">"); /* form method wichtig für Daten von Button´s !!! */

  if (countargs != 0) {
#ifdef debug_html
    Serial.print(F("[DB] web_detail_import, submit:    ")); Serial.println(submit);
    Serial.print(F("[DB] web_detail_import, countargs: ")); Serial.println(countargs);
    Serial.print(F("[DB] web_detail_import, import:    ")); Serial.println(imp);
#endif
    if (submit == "registers") {  // register processing from String imp ['01AB','11FB']
      ToggleCnt = 1; // stop toggle
      uint8_t Adr;
      uint8_t Val;
      detachInterrupt(digitalPinToInterrupt(GDO2));
      CC110x_CmdStrobe(CC110x_SIDLE); // Exit RX / TX, turn off frequency synthesizer and exit Wake-On-Radio mode if applicable
      CC110x_CmdStrobe(CC110x_SFRX);  // Flush the RX FIFO buffer. Only issue SFRX in IDLE or RXFIFO_OVERFLOW states
      for (uint16_t i = 2; i < imp.length(); i += 7) {
        Adr = hexToDec(imp.substring(i, i + 2));
        Val = hexToDec(imp.substring(i + 2, i + 4));
#ifdef debug_html
        Serial.print(F("[DB] web_detail_import, cc110x adr: 0x")); Serial.print(onlyDecToHex2Digit(Adr));
        Serial.print(F(" | val: 0x")); Serial.println(onlyDecToHex2Digit(Val));
#endif
        if (Adr > 0x2E) {
          break;
        }
        Chip_writeReg(Adr, Val);    // write in cc110x
        EEPROMwrite(Adr, Val);      // write in flash
      }
      if (Adr > 0x2E) {
        website += F("ERROR: wrong cc110x adress, writing aborted! - ");
        website += onlyDecToHex2Digit(Adr);
      } else {
        website += F("Import of the register values closed​​");
      }
      CC110x_CmdStrobe(CC110x_SFRX);  // Flush the RX FIFO buffer. Only issue SFRX in IDLE or RXFIFO_OVERFLOW states
      MOD_FORMAT = ( Chip_readReg(0x12, READ_SINGLE) & 0b01110000 ) >> 4;
      if (MOD_FORMAT != 3) {
        attachInterrupt(digitalPinToInterrupt(GDO2), Interupt, RISING);
      } else {
        attachInterrupt(digitalPinToInterrupt(GDO2), Interupt, CHANGE);
      }
      ReceiveModePKTLEN = Chip_readReg(CHIP_PKTLEN, READ_SINGLE); // only if fixed packet length
      ReceiveModeName = FPSTR(RECEIVE_MODE_USER);
      Chip_setReceiveMode();  // start receive mode
    }
  } else {
    website += F("Import current register values<br><br>"
                 "FHEM - SIGNALduino Format | SD_ProtocolData.pm, entry register =><br>"
                 "(Please paste the string in SIGNALduino format)<br><br>"
                 "<input size=\"100\" maxlength=\"288\" value=\"example ['01AB','11FB']\" name=\"imp\" pattern=\"^\\['[0-9a-fA-F]{4}'(,'[0-9a-fA-F]{4}')+\\]$\">"
                 "<br><br><button class=\"btn\" type=\"submit\" name=\"submit\" value=\"registers\">acceptance of the values ​​in the register</button><br>"
                 "<br>File to import from program SmartRF Studio 7<br>[development]");
  }
  website += F("<br><br><br><a class=\"back\" href=\"/detail\">&crarr; back to detail information</a>"
               "</form></body></html>");

#elif RFM69                 // #### web_detail_import - RFM69 #### //
  String website = FPSTR(html_meta);
  website.reserve(1024);
  website += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/detail_imp.css\">"
               "<script src=\"/js/detail_rfm69_imp.js\"></script></head>"
               "Import current register values<br>"
               "<br>FHEM - SIGNALduino Format | SD_ProtocolData.pm, entry register =><br>"
               "<input size=\"100\" maxlength=\"288\" value=\"example ['0104','2FAA']\" name=\"imp\" pattern=\"^\\['[0-9a-fA-F]{4}'(,'[0-9a-fA-F]{4}')+\\]$\">"
               "<br><br><button class=\"btn\" type=\"submit\" name=\"submit\" value=\"registers\">acceptance of the values ​​in the register</button><br>"
               "<br>File to import from program SX1231SKB<br>"
               "<button onclick=\"document.getElementById('inputfile').click()\">Choose a file</button>"
               "<input type=\"file\" id=\"inputfile\" style=\"display:none\" name=\"inputfile\" onClick=\"inputfile()\" accept=\".cfg\">"
               "<br><br><a class=\"back\" href=\"/detail\">&crarr; back to detail information</a>");
#endif
  sendHtml(website);
}                           // #### web_detail_import - END #### //


void web_detail() {         // ########## web_detail ##########
  if (!ChipFound) {
    web_chip();
  }
  // http://192.168.178.26/detail?freq=868.302&freq_off=0.001&bandw=203.2&submit=bbandw&datarate=8.23&deviation=57.13&modulation=2-FSK&r0=01&r1=2E&r2=2E&r3=46&r4=2D&r5=D4&r6=1A&r7=C0&r8=00&r9=00&r10=00&r11=06&r12=00&r13=21&r14=65&r15=6C&r16=88&r17=4C&r18=02&r19=22&r20=F8&r21=51&r22=07&r23=00&r24=18&r25=16&r26=6C&r27=43&r28=68&r29=91&r30=87&r31=6B&r32=FB&r33=56&r34=11&r35=EF&r36=0C&r37=3B&r38=1F&r39=41&r40=00&r41=59&r42=7F&r43=3E&r44=88&r45=31&r46=0B
  String afc = HttpServer.arg("afc");
  String mod = HttpServer.arg("modulation");
  String submit = HttpServer.arg("submit");  // welcher Button wurde betätigt
  String strArg;
  String web_stat;
  byte BrowserArgsReg[REGISTER_MAX + 1] = {};
  float bandw = (HttpServer.arg("bandw").toFloat());
  float deviation = (HttpServer.arg("deviation").toFloat());
  float freqOff = (HttpServer.arg("freq_off").toFloat());
  long datarate = (HttpServer.arg("datarate").toFloat()) * 1000;
  long freq = (HttpServer.arg("freq").toFloat()) * 1000;
  freq += 0;
  uint8_t countargs = HttpServer.args();    // Anzahl Argumente

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
      strArg = 'r';
      strArg += i;
      BrowserArgsReg[i] = hexToDec(HttpServer.arg(strArg));
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
        Chip_writeReg(i + CHIP_FREQMSB, value[i]);  // write Frequency Control Bytes to chip
        EEPROMwrite(i + CHIP_FREQMSB, value[i]);    // write Frequency Control Bytes in flash
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
        Chip_writeReg(CHIP_BitRate + i, value[i]);  // write in receiver
        EEPROMwrite(CHIP_BitRate + i, value[i]);    // write in flash
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
        Chip_writeReg(0x05 + i, value[i]);  // write in receiver
        EEPROMwrite(0x05 + i, value[i]);    // write in flash
      }
#endif
    } else if (submit == "bmod") {
#ifdef debug_html
      Serial.print(F("[DB] web_detail, set modulation to ")); Serial.println(mod);
#endif
      web_stat = F("Modulation set &#10004;");

#ifdef CC110x
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

      Chip_writeReg(18, web_Mod_set(value));      // write in cc110x
      EEPROMwrite(18, web_Mod_set(value));        // write in flash
#endif
      // Register
    } else if (submit == "breg") {
#ifdef debug_html
      Serial.println(F("[DB] web_detail, button set registers pushed"));
#endif
      web_stat = F("all registers set &#10004;");

      for (byte i = 0; i <= REGISTER_MAX; i++) { /* all registers */
        strArg = 'r';
        strArg += i;
        if (HttpServer.hasArg(strArg)) { // disabled inputs are not submitted
#ifdef debug_html
          Serial.print(F("[DB] web_detail, regData["));
          Serial.print(i); Serial.print(F("] = 0x")); Serial.println(onlyDecToHex2Digit(BrowserArgsReg[i]));
#endif
          /* compare value with value to be written */
          uint8_t addr = i;
#ifdef RFM69
          if (i >= 80) {
            addr = SX1231_RegAddrTranslate[i - 80];
          }
#endif
          BrowserArgsReg[i] = hexToDec(HttpServer.arg(strArg));
          //          byte regVal = Chip_readReg(addr, READ_BURST);
          //          if (BrowserArgsReg[i] != regVal) {
#ifdef debug_html
          /* i > 34 && <= 40    | CC110x automatic control register */
          /* i > 40             | CC110x test register */
          Serial.print(F("[DB] web_detail, regData[")); Serial.print(i);
          Serial.print(F("] value has changed 0x")); Serial.print(onlyDecToHex2Digit(regVal));
          Serial.print(F(" -> 0x")); Serial.println(onlyDecToHex2Digit(BrowserArgsReg[i]));
#endif
          if (i == CHIP_PKTLEN) { /* ToDo - CHIP_PKTLEN probably not optimal -> if register 6 is changed (dependencies) ??? */
            ReceiveModePKTLEN = BrowserArgsReg[i];
          }
          /* write value to register */
          Chip_writeReg(addr, BrowserArgsReg[i]);   // write in chip
          EEPROMwrite(addr, BrowserArgsReg[i]);     // write in flash
          //          }
        }
      }
    }
    ReceiveModeName = FPSTR(RECEIVE_MODE_USER);
    ReceiveModeNr = 1;
  }

  String website = FPSTR(html_meta);
#ifdef CC110x
  website.reserve(13000);
#elif RFM69
  website.reserve(20000);
#endif
  website += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/detail.css\">"
               "<script src=\"/js/"
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
               "<tr><td colspan=\"2\">Bandwidth"
#ifdef CC110x
               " (double side)"
#elif RFM69
               " (single side)"
#endif
               "</td>"
               "<td class=\"ce\" colspan=\"2\"><span id=\"CHANBW\"></span></td>"
               "<td class=\"ce\"><select id=\"bandw\" name=\"bandw\"></select><div class=\"txt\">&ensp;kHz</div></td>"
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
               "<tr><td colspan=\"2\">Deviation (no influence on RX)</td><td class=\"ce\" colspan=\"2\"><span id=\"DEVIATN\"></span></td>"
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
               "<button class=\"btn\" type=\"button\" onClick=\"location.href='""detail_export""'\">export all registers</button>&emsp;"
               "<button class=\"btn\" type=\"button\" onClick=\"location.href='""detail_import""'\">import registers</button></td></tr>"
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


void web_log() {            // ########## web_log ##########
  String website = FPSTR(html_meta);
  website.reserve(10000);
  website += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/log.css\">"
               "<script src=\"/js/log.js\"></script>"
               "</head>");
  website += FPSTR(html_head_table);
  HTML_mod(website);  // replace "RAW data" when no chip found

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


void web_raw() {            // ########## web_raw ##########
  if (!ChipFound) {
    web_chip();
  }
#ifdef CC110x
  uint8_t CC110x_PATABLE_VAL[8];
  Chip_readBurstReg(CC110x_PATABLE_VAL, 0x3E, 8);
  uint8_t OutputPower = CC110x_PATABLE_VAL[1];
#elif RFM69
  uint8_t RegPaLevel = Chip_readReg(0x11, READ_BURST);  // PA selection and Output Power control
  uint8_t OutputPower = RegPaLevel & 0b00011111;        // Output power setting with 1 dB steps, Pout = -18 + OutputPower [dBm] with PA0 or PA1, Pout = -14 + OutputPower [dBm] with PA1 and PA2, (limited to the 16 upper values of OutputPower)
#endif
  uint8_t countargs = HttpServer.args();

  if (countargs != 0) {
#ifdef CC110x
    uint8_t OutputPowerNew = HttpServer.arg("pow").toInt(); // PATABLE
    if (OutputPowerNew != OutputPower) {
      CC110x_PATABLE_VAL[1] = {OutputPowerNew};
      CC110x_writeBurstReg(CC110x_PATABLE_VAL, CC110x_PATABLE, 8);
#ifdef debug_chip
      Serial.print(F("[DB] old CC110x PATABLE val: 0x")); Serial.println(OutputPower, HEX);
      Serial.print(F("[DB] new CC110x PATABLE val: 0x")); Serial.println(OutputPowerNew, HEX);
#endif
      OutputPower = OutputPowerNew;
    }
#elif RFM69
    int8_t OutputPowerNew = HttpServer.arg("pow").toInt(); // Output power setting with 1 dB steps, Pout = -18 + OutputPower [dBm] with PA0 or PA1, Pout = -14 + OutputPower [dBm] with PA1 and PA2, (limited to the 16 upper values of OutputPower)
    if (OutputPowerNew != OutputPower) {
      RegPaLevel = (RegPaLevel & 0b11100000) | (OutputPowerNew);
      Chip_writeReg(0x11 , RegPaLevel); // PA selection and Output Power control
      OutputPower = OutputPowerNew;
#ifdef debug_chip
      Serial.print(F("[DB] SX1231 write RegPaLevel 0x11: 0x")); Serial.println(RegPaLevel, HEX);
      Serial.print(F("[DB] SX1231 new OutputPower: "));  Serial.println(OutputPower);
#endif
    }
#endif // END - #ifdef RFM69
  }

  String website = FPSTR(html_meta);
  website.reserve(3072);
  website += F("<body><form method=\"post\">" /* form method wichtig für Daten von Button´s !!! */
               "<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/raw.css\">"
               "<script src=\"/js/raw.js\"></script>"
               "</head>");
  website += FPSTR(html_head_table);
  website += F("<table>"
               "<tr><th colspan=\"4\">send data (with the current register settings)</th></tr>"
               "<tr>"
               "<td class=\"td1\"><input class=\"inp\" name=\"sd\" type=\"text\"></td>"
               "<td class=\"td1\">repeats <input aria-label=\"n1\" name=\"rep\"type=\"number\" onkeypress=\"if(this.value.length==2) return false;\"></td>"
               "<td class=\"td1\">pause (ms) <input aria-label=\"n2\" name=\"rept\" type=\"number\" onkeypress=\"if(this.value.length==6) return false;\"></td>"
               "<td class=\"td1\"><input class=\"btn\" type=\"button\" value=\"send\" onclick=\"msgSend()\"></td>"
               "</tr><tr><td class=\"td1\"><span id=\"val\">");
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

  website += F("</span></td>"
               "<td class=\"td1\" colspan=\"2\">" // Output Power setting
               "Output Power&ensp;<select id=\"pow\" name=\"pow\">");
#ifdef CC110x
  float freq = Chip_readFreq(); // kHz
  if (freq < 615000) {          // Half between 433 and 868 MHz
    memcpy_P(CC110x_PATABLE_VAL, CC110x_PATABLE_433, 8);
  } else {
    memcpy_P(CC110x_PATABLE_VAL, CC110x_PATABLE_868, 8);
  }
  for (int8_t x = 7; x >= 0; x--) {
    website += F("<option ");
    website += (OutputPower == CC110x_PATABLE_VAL[x] ? "selected " : "");
    website += F("value=\"");
    website += CC110x_PATABLE_VAL[x];
    website += F("\">");
    website += (int8_t)pgm_read_byte(&CC110x_PATABLE_POW[x]);
    website += F("</option>");
  }
#elif RFM69
  for (int8_t val = 0; val <= 31; val++) {
    website += F("<option ");
    website += (OutputPower == val ? "selected " : "");
    website += F("value=\"");
    website += val;
    website += F("\">");
    int8_t opt = val - 18;
    website += opt;
    website += F("</option>");
  }
#endif
  website += F("</select>&ensp;dBm</td>"
               "<td class=\"td1\"><input class=\"btn\" type=\"submit\" value=\"set\" id=\"set\"></td>" // Button set Output Power setting
               "</tr></table><br>"
               "<div><table id = \"dataTable\">"
               "<tr><th class=\"dd\">Time</th><th>current RAW, received data on mode &rarr;&nbsp;<span id=\"MODE\">");
  website += ReceiveModeName;
  website += F("</span></th><th class=\"dd\">RSSI<br>dB</th><th class=\"dd\">Offset<br>kHz</th></tr>"
               "</table></div>"
               "</form></body></html>");
  sendHtml(website);
}


void web_wlan() {           // ########## web_wlan ##########
  /* https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/scan-class.html
     https://links2004.github.io/Arduino/d4/d52/wl__definitions_8h_source.html
     https://blog.robberg.net/wifi-scanner-with-esp32/ */
  IPAddress ipadr;                              // IP addresse
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
  website += F("<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/wlan.css\">"
               "<script src=\"/js/wlan.js\"></script>"
               "</head>");
  website += FPSTR(html_head_table);
  HTML_mod(website);  // replace "RAW data" when no chip found
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
               "<td class=\"inp\" colspan=\"2\"><input name=\"hiddenssid\" type=\"text\" size=\"36\" placeholder=\"hidden ssid\"></td>"
               "<td class=\"inp\" colspan=\"3\"><input name=\"pw\" type=\"password\" size=\"28\" placeholder=\"current password\"></td>"
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
      String website = "";
      website.reserve(55);
      if (ReceiveModeNr < NUMBER_OF_MODES) {
        website += ReceiveModeName;
      }
      website += ',';
#ifdef CC110x
      website += Chip_readReg(CC110x_MARCSTATE, READ_BURST);
#elif RFM69
      website += (Chip_readReg(0x01, 0x00) & 0b00011100) >> 2;
#endif
      website += ',';
      for (uint8_t modeNr = 0; modeNr < NUMBER_OF_MODES; modeNr++) {
        if (ToggleArray[modeNr]) {
          website += modeNr;
          website += ' ';
        }
      }
      website += ',';
      website += ToggleTimeMode[ReceiveModeNr];
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
    Serial.println(F("old settings"));
  } else {
    Serial.println(F("new settings"));
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
      if (webSocketSite[num] == "/detail" || webSocketSite[num] == "/detail_export") {
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
    website.reserve(128);
    if (ReceiveModeNr < NUMBER_OF_MODES) {
      website += ReceiveModeName;
    }
    website += ',';
    website += ReceiveModeNr;
    website += ',';
    for (uint8_t c = 0; c < NUMBER_OF_MODES; c++) {
      website += ToggleTimeMode[c];
      if (c < NUMBER_OF_MODES - 1) {
        website += '_';
      }
    }
    website += ',';
    for (uint8_t c = 0; c < NUMBER_OF_MODES; c++) {
      website += msgCountMode[c];
      if (c < NUMBER_OF_MODES - 1) {
        website += '_';
      }
    }
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


void WebSocket_imp(const String values) {
#ifdef debug_websocket
  Serial.print(F("[DB] WebSocket_imp with string ")); Serial.println(values);
#endif
  if (values.substring(0, 7) == "imp,SX,") {
#ifdef RFM69
#ifdef debug_websocket
    Serial.println(F("[DB] WebSocket_imp, processing SX1231"));
#endif
    SX1231_setIdleMode();
    for (uint16_t i = 7; i < values.length(); i += 4) {
      uint8_t adr = hexToDec(values.substring(i, i + 2));
      uint8_t val = hexToDec(values.substring(i + 2, i + 4));
      EEPROMwrite(adr, val);
    }
#endif
  } else if (values.substring(0, 7) == "imp,CC,") {
#ifdef CC110x
#ifdef debug_websocket
    Serial.println(F("[DB] WebSocket_imp, processing CC110x"));
#endif
#endif
  }
  ReceiveModeNr = 1;
  ToggleArray[1] = 1;
  Interupt_Variant(1);
#ifdef debug_websocket
  Serial.println(F("[DB] WebSocket_imp END"));
#endif
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
  HttpServer.on("/detail_export", web_detail_export);
  HttpServer.on("/detail_import", web_detail_import);
  HttpServer.on("/modes", web_modes);
  HttpServer.on("/index.htm", web_index);
  HttpServer.on("/index.html", web_index);
  HttpServer.on("/log", web_log);
  HttpServer.on("/raw", web_raw);
  HttpServer.on("/wlan", web_wlan);
  HttpServer.onNotFound(handleUnknown);
}

void HTML_mod(String & str) { // replace "RAW data" when no chip found
  if (ChipFound != true) {
    str.replace(F("<a href=\"raw\" class=\"RAW\">RAW data</a>"), F(""));
  }
}

#endif  // END - CODE_ESP
