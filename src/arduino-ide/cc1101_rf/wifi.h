#pragma once

/*
    This file provides us with important information about the functional prototypes available to us,
    provides us with symbolic constants and keeps a list of all available macros.
*/

#include <Arduino.h>
#include <digitalWriteFast.h>           // https://github.com/ArminJo/digitalWriteFast

#ifdef ARDUINO_ARCH_ESP32
static esp_wps_config_t config;
#define ESP_WPS_MODE WPS_TYPE_PBC
//#define ESP_MANUFACTURER  "ESPRESSIF"
//#define ESP_MODEL_NUMBER  "ESP32"
//#define ESP_MODEL_NAME    "ESPRESSIF IOT"
#define ESP_DEVICE_NAME WLAN_hostname
#endif


String WLAN_MAC_String(uint8_t* MAC) { /* return MAC in format EC:FA:BC:C5:38:57 */
  char str[18];
  sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X", MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]);
  return str;
}


void start_WLAN_AP(String ssid_ap, String password_ap) {
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);                /* WIFI set mode */
  WiFi.hostname(OwnStationHostname); /* WIFI set hostname */

  if (WiFi.softAP(ssid_ap.c_str(), password_ap.c_str(), 1, false, 3) == true) {
    Serial.println(F("WIFI AP Ready"));
    Serial.print(F("WIFI SSID        ")); Serial.println(ssid_ap);
    Serial.print(F("WIFI IP address  ")); Serial.println(WiFi.softAPIP());
    Serial.print(F("WIFI HOSTNAME    ")); Serial.println(OwnStationHostname);
    WLAN_AP = true;
    WLAN_OK = true;
    File logfile = LittleFS.open("/files/log.txt", "a");         // Datei mit Schreibrechten öffnen
    if (logfile) {
      String logText = String(getUptime());
      logText += F(" - WIFI Access point active");
      logfile.println(logText);
      logfile.close(); /* Schließen der Datei */
    }
  } else {
    Serial.println(F("WIFI AP Failed!"));
    used_dhcp = 1;
    WLAN_AP = false;
    WLAN_OK = false;
  }
}


void start_WLAN_STATION(String qssid, String qpass) {
#ifdef debug_html
  Serial.print(F("DB start_WLAN_STATION, EEPROM DHCP       ")); Serial.println(EEPROMread(EEPROM_ADDR_DHCP) == 1 ? F("1 (on)") : F("0 (off)"));
  Serial.print(F("DB start_WLAN_STATION, EEPROM IP         ")); Serial.println(eip);
  Serial.print(F("DB start_WLAN_STATION, EEPROM Gateway    ")); Serial.println(esgw);
  Serial.print(F("DB start_WLAN_STATION, EEPROM DNS        ")); Serial.println(edns);
  Serial.print(F("DB start_WLAN_STATION, EEPROM SUBNETMASK ")); Serial.println(esnm);
#endif

  WiFi.disconnect();
  WiFi.mode(WIFI_STA);               /* WIFI set mode */
  WiFi.hostname(OwnStationHostname); /* WIFI set hostname */

  if (EEPROMread(EEPROM_ADDR_DHCP) == 0) { /* WIFI config static IP */
    if (!WiFi.config(eip, esnm, esgw, edns)) {
      Serial.println("WIFI Stationsmode WIFI_STA Failed to configure");
    } else {
      Serial.println(F("WIFI Stationsmode WIFI_STA configured with static IP"));
    }
  } else {
    Serial.println(F("WIFI Stationsmode WIFI_STA configured DHCP"));
  }

  WiFi.begin(qssid.c_str(), qpass.c_str()); /* WIFI connect to ssid with password */

  Serial.print(F("WIFI try connect to ")); Serial.print(qssid); Serial.print(F(" with ")); Serial.println(qpass);

  delay(50);

  unsigned long startTime = millis(); /* ... Give ESP 60 seconds to connect to station. */
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 60000) {
    digitalWriteFast(LED, !digitalRead(LED));  // LED toggle
    delay(500);
    Serial.print('.');
  }
  Serial.println("");

  if (WiFi.status() == WL_CONNECTED) {
    digitalWriteFast(LED, LOW);  // LED off
    Serial.print(F("WIFI IP address ")); Serial.println(WiFi.localIP());

    if (qssid != EEPROMread_string(EEPROM_ADDR_SSID) || qpass != EEPROMread_string(EEPROM_ADDR_PASS)) {
      EEPROMwrite_string(EEPROM_ADDR_SSID, qssid);
      EEPROMwrite_string(EEPROM_ADDR_PASS, qpass);
      EEPROMwrite(EEPROM_ADDR_AP, 0);
      Serial.println(F("WIFI Settings to EEPROM"));
    } else {
      Serial.println(F("WIFI Settings from EEPROM"));
    }

    File logfile = LittleFS.open("/files/log.txt", "a");  // Datei mit Schreibrechten öffnen
    if (logfile) {
      String logText = String(getUptime());
      logText += F(" - WIFI connected to ");
      logText += qssid;
      logfile.println(logText);
      logfile.close(); /* Schließen der Datei */
    }

    WLAN_AP = false;
    WLAN_OK = true;
    WLAN_reco_cnt = 0;
    used_ssid = qssid;
    used_ssid_mac = WLAN_MAC_String(WiFi.BSSID());
  } else {
    WLAN_reco_cnt++;
    if (WLAN_reco_cnt <= 1) {
      Serial.print(F("WIFI connection failed, old settings attempt ")); Serial.println(WLAN_reco_cnt);
      start_WLAN_STATION(EEPROMread_string(EEPROM_ADDR_SSID), EEPROMread_string(EEPROM_ADDR_PASS));
    } else {
      start_WLAN_AP(ssid_ap, password_ap);
      WLAN_reco_cnt = 0;
      used_ssid = "";
    }
  }
}


#ifdef ARDUINO_ARCH_ESP32
void ESP32_wpsInitConfig() {
  config.crypto_funcs = &g_wifi_default_wps_crypto_funcs;
  config.wps_type = ESP_WPS_MODE;
  //  strcpy(config.factory_info.manufacturer, ESP_MANUFACTURER);
  //  strcpy(config.factory_info.model_number, ESP_MODEL_NUMBER);
  //  strcpy(config.factory_info.model_name, ESP_MODEL_NAME);
  strcpy(config.factory_info.device_name, ESP_DEVICE_NAME);
}


String ESP32_wpspin2string(uint8_t a[]) {
  char wps_pin[9];
  for (int i = 0; i < 8; i++) {
    wps_pin[i] = a[i];
  }
  wps_pin[8] = '\0';
  return (String)wps_pin;
}


void ESP32_WiFiEvent(WiFiEvent_t event, system_event_info_t info) {
  switch (event) {
    case SYSTEM_EVENT_STA_START:
      Serial.println(F("Station Mode Started"));
      break;
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.println("Connected to :" + String(WiFi.SSID()));
      Serial.print(F("Got IP: ")); Serial.println(WiFi.localIP());
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println(F("Disconnected from station, attempting reconnection"));
      WiFi.reconnect();
      break;
    case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
      Serial.println("WPS Successful, stopping WPS and connecting to: " + String(WiFi.SSID()));
      used_ssid = WiFi.SSID();
      used_ssid_pass = WiFi.psk();
      EEPROMwrite_string(EEPROM_ADDR_SSID, used_ssid);       // write String to EEPROM
      EEPROMwrite_string(EEPROM_ADDR_PASS, used_ssid_pass);  // write String to EEPROM
      EEPROMwrite(EEPROM_ADDR_AP, 0);
      esp_wifi_wps_disable();
      delay(10);
      WiFi.begin();
      break;
    case SYSTEM_EVENT_STA_WPS_ER_FAILED:
      Serial.println(F("WPS Failed, retrying"));
      esp_wifi_wps_disable();
      esp_wifi_wps_enable(&config);
      esp_wifi_wps_start(0);
      break;
    case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
      Serial.println(F("WPS Timeout, retrying"));
      esp_wifi_wps_disable();
      esp_wifi_wps_enable(&config);
      esp_wifi_wps_start(0);
      break;
    case SYSTEM_EVENT_STA_WPS_ER_PIN:
      Serial.println("WPS_PIN = " + ESP32_wpspin2string(info.sta_er_pin.pin_code));
      break;
    default:
      break;
  }
}
#endif


bool start_WLAN_WPS() { /* WPS works in STA (Station mode) only. */
  Serial.println(F("WIFI WPS started"));
  bool wpsSuccess;
#ifndef ARDUINO_ARCH_ESP32           /* ESP32 andere WPS Einbindung !!! */
  WiFi.hostname(OwnStationHostname); /* WIFI set hostname */
  WiFi.mode(WIFI_STA);               /* WIFI set mode */

  wpsSuccess = WiFi.beginWPSConfig();
  if (wpsSuccess) {
    String qssid = WiFi.SSID();
    String qssidmac = WiFi.BSSIDstr();

    if (qssid.length() > 0 && WiFi.psk().length() > 0) {
      /* WPSConfig has already connected in STA mode successfully to the new station. */
      char* chrOwnHostname = &OwnStationHostname[0];
      wifi_station_set_hostname(chrOwnHostname);

#ifdef debug
      Serial.print(F("DB WIFI WPS connected to: ")); Serial.print(qssid); Serial.print(F(" with secret key ")); Serial.println(WiFi.psk());
#endif
      used_ssid = qssid;            // SSID übernehmen
      used_ssid_pass = WiFi.psk();  // Wifi Passwort übernehmen
      used_dhcp = 1;
      WLAN_AP = false;
      EEPROMwrite_string(EEPROM_ADDR_SSID, used_ssid);       // write String to EEPROM
      EEPROMwrite_string(EEPROM_ADDR_PASS, used_ssid_pass);  // write String to EEPROM
      EEPROMwrite(EEPROM_ADDR_AP, 0);
      Serial.println(F("WIFI WPS connection successful"));
    } else {
      Serial.println(F("WIFI WPS connection failed!"));
      wpsSuccess = false;
    }
  }
  return wpsSuccess;
#else
  /*
    ESP32 anderes WPS Handling
    https://github.com/espressif/ESP31_RTOS_SDK/blob/master/include/esp_wps.h
    https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WPS/WPS.ino
    https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/
  */
  WiFi.onEvent(ESP32_WiFiEvent);
  WiFi.mode(WIFI_MODE_STA);
  ESP32_wpsInitConfig();
  esp_wifi_wps_enable(&config);
  esp_wifi_wps_start(0);
  wpsSuccess = true;
#endif
  return wpsSuccess;
}


String WLAN_encryptionType(uint8_t i) {
  switch (i) {
#ifdef ARDUINO_ARCH_ESP8266
    case 2:
      return F("WPA (TKIP)");
      break;
    case 4:
      return F("WPA2 (CCMP)");
      break;
    case 5:
      return F("WEP");
      break;
    case 7:
      return F("NONE");
      break;
    case 8:
      return F("AUTO");
      break;
    default:
      return F("unknown");
      break;
#elif ARDUINO_ARCH_ESP32
    case 0:
      return F("Open");
      break;
    case 1:
      return F("WEP");
      break;
    case 2:
      return F("WPA_PSK");
      break;
    case 3:
      return F("WPA2_PSK");
      break;
    case 4:
      return F("WPA_WPA2_PSK");
      break;
    case 5:
      return F("WPA2_ENTERPRISE");
      break;
    default:
      return F("unknown");
      break;
#endif
  }
}
