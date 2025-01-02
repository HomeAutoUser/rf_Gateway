#ifndef WLAN_H
#define WLAN_H

#include <Arduino.h>
#include <digitalWriteFast.h>           // https://github.com/ArminJo/digitalWriteFast
#include "config.h"

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
#include "functions.h"

// external integration: cc1101_rf.ino
extern void appendLogFile(const String & logText);
extern String OwnStationHostname;
extern boolean WLAN_AP;
extern byte used_dhcp;
extern byte WLAN_reco_cnt;

// internal preliminary definition
void start_WLAN_AP(String ssid_ap, String password_ap);
void start_WLAN_STATION(String qssid, String qpass);
bool start_WLAN_WPS();
String WLAN_encryptionType(uint8_t i);

#ifdef ARDUINO_ARCH_ESP8266
// external integration: cc1101_rf.ino
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
extern ESP8266WebServer HttpServer;
#endif  // END - #ifdef ARDUINO_ARCH_ESP8266

#ifdef ARDUINO_ARCH_ESP32
// external integration: cc1101_rf.ino
#include <WebServer.h>
#include <WiFi.h>
#include <esp_wps.h>
extern WebServer HttpServer;
extern int wifiEventId;
// internal preliminary definition
#define ESP_WPS_MODE WPS_TYPE_PBC
void ESP32_WiFiEvent(WiFiEvent_t event);
void ESP32_wpsInitConfig();
void ESP32_wpsStop();
String ESP32_wpspin2string(uint8_t a[]);
#endif  // END - #ifdef ARDUINO_ARCH_ESP32

extern IPAddress eip;
extern IPAddress esgw;
extern IPAddress edns;
extern IPAddress esnm;

#endif  // END - #if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
#endif  // END - #ifndef WLAN_H
