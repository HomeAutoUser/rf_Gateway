#ifndef WEBSITES_H
#define WEBSITES_H

#include <Arduino.h>
#include <EEPROM.h>
#include <WebSocketsServer.h>
#include "config.h"
#include "functions.h"
#include "wlan.h"

#ifdef CC110x
#include "cc110x.h"
#elif RFM69
#include "rfm69.h"
#endif

#ifdef ESP32_core_v1
#include <LITTLEFS.h>
#define LittleFS LITTLEFS
#else
#include <LittleFS.h>
#endif

// external integration: cc1101_rf.ino
extern byte RegBeforeChange[];
extern uint32_t msgCount;
extern void InputCommand(String input);
extern uint8_t freqAfc;
extern void appendLogFile(String logText);
extern unsigned long uptimeReset;
extern unsigned long uptime;
extern const char compile_date[];
extern const char TXT_VERSION[];
extern uint8_t msgRepeats;
extern uint32_t msgSendInterval;
extern IPAddress eip;
extern IPAddress esnm;
extern IPAddress esgw;
extern IPAddress edns;
extern WebSocketsServer webSocket;
extern String webSocketSite[WEBSOCKETS_SERVER_CLIENT_MAX];
extern int8_t freqErr;
extern struct Data Registers[];

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

#endif  // END - WEBSITES_H
