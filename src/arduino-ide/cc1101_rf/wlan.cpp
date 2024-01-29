#include "wlan.h"

#ifdef ARDUINO_ARCH_ESP32
static esp_wps_config_t config;

void ESP32_wpsInitConfig() {
  //config.crypto_funcs = &g_wifi_default_wps_crypto_funcs;   /* not absolutely necessary for function with core v1 | no longer works on core v2 */
  config.wps_type = ESP_WPS_MODE;
  char buf[33];
  OwnStationHostname.toCharArray(buf, OwnStationHostname.length());
  strcpy(config.factory_info.device_name, buf);
}


String ESP32_wpspin2string(uint8_t a[]) {
  char wps_pin[9];
  for (int i = 0; i < 8; i++) {
    wps_pin[i] = a[i];
  }
  wps_pin[8] = '\0';
  return (String)wps_pin;
}


void ESP32_wpsStop() {
  if (esp_wifi_wps_disable()) {
#ifdef debug_wifi
    Serial.println("[DB] WPS Disable Failed");
#endif
  }
}


void ESP32_WiFiEvent(WiFiEvent_t event) {
  /* https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/src/WiFi.cpp
     https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/src/WiFiGeneric.h
     https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/esp_event.html */
#ifdef debug_wifi
  Serial.printf("[DB] WIFI event: %d - ", event);
#endif

  wifiEventId = event;
  switch (event) {
#ifdef debug_wifi
    case ARDUINO_EVENT_WIFI_READY:                                            // Id: 0
      Serial.println("WiFi interface ready");
      break;
    case ARDUINO_EVENT_WIFI_SCAN_DONE:                                        // Id: 1
      Serial.println("Completed scan for access points");
      break;
    case ARDUINO_EVENT_WIFI_STA_START:                                        // Id: 2
      Serial.println("WiFi client started");
      break;
    case ARDUINO_EVENT_WIFI_STA_STOP:
      Serial.println("WiFi clients stopped");
      break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:                                    // Id: 4
      Serial.print("Connected to access point "); Serial.print(WiFi.SSID());
      Serial.print(", MAC: "); Serial.println(WiFi.BSSIDstr());
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:                                 // Id: 5
      Serial.println("Disconnected from WiFi access point");
      break;
    case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
      Serial.println("Authentication mode of access point has changed");
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:                                       // Id: 7
      Serial.print("Obtained IP address: "); Serial.println(WiFi.localIP());
      break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
      Serial.println("Lost IP address and IP address is reset to 0");
      break;
#endif
    case ARDUINO_EVENT_WPS_ER_SUCCESS:                                        // Id: 24
#ifdef debug_wifi
      Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
#endif
      ESP32_wpsStop();
      delay(10);
      WiFi.begin();
      break;
    case ARDUINO_EVENT_WPS_ER_FAILED:
#ifdef debug_wifi
      Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
#endif
      ESP32_wpsStop();
      break;
    case ARDUINO_EVENT_WPS_ER_TIMEOUT:                                        // Id: 26
#ifdef debug_wifi
      Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
#endif
      ESP32_wpsStop();
      break;
#ifdef debug_wifi
    case ARDUINO_EVENT_WPS_ER_PIN:
      Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
      break;
    case ARDUINO_EVENT_WIFI_AP_START:
      Serial.println("WiFi access point started");
      break;
    case ARDUINO_EVENT_WIFI_AP_STOP:
      Serial.println("WiFi access point stopped");
      break;
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:                                  // Id: 12
      Serial.println("Client connected");
      break;
    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
      Serial.println("Client disconnected");
      break;
    case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:                                 // Id: 14
      Serial.println("Assigned IP address to client");
      break;
    case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
      Serial.println("Received probe request");
      break;
    case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
      Serial.println("AP IPv6 is preferred");
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
      Serial.println("STA IPv6 is preferred");
      break;
    case ARDUINO_EVENT_ETH_GOT_IP6:
      Serial.println("Ethernet IPv6 is preferred");
      break;
    case ARDUINO_EVENT_ETH_START:
      Serial.println("Ethernet started");
      break;
    case ARDUINO_EVENT_ETH_STOP:
      Serial.println("Ethernet stopped");
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      Serial.println("Ethernet connected");
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("Ethernet disconnected");
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      Serial.println("Obtained IP address");
      break;
#endif
    default: break;
  }
}
#endif // Ende #ifdef ARDUINO_ARCH_ESP32


void start_WLAN_AP(String ssid_ap, String password_ap) {
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);                /* WIFI set mode */

#ifdef ARDUINO_ARCH_ESP8266
  WiFi.hostname(OwnStationHostname); /* WIFI set hostname */
#elif ARDUINO_ARCH_ESP32
  WiFi.setHostname(OwnStationHostname.c_str()); /* WIFI set hostname */
#endif

  if (WiFi.softAP(ssid_ap.c_str(), password_ap.c_str(), 1, false, 3) == true) {
#ifdef debug_wifi
    Serial.println(F("[DB] WIFI AP Ready"));
    Serial.print(F("[DB] WIFI SSID        ")); Serial.println(ssid_ap);
    Serial.print(F("[DB] WIFI IP address  ")); Serial.println(WiFi.softAPIP());
    Serial.print(F("[DB] WIFI HOSTNAME    ")); Serial.println(OwnStationHostname);
#endif
    WLAN_AP = true;
    appendLogFile(F("WIFI Access point active"));
  } else {
#ifdef debug_wifi
    Serial.println(F("[DB] WIFI AP Failed!"));
#endif
    used_dhcp = 1;
    WLAN_AP = false;
  }
}


void start_WLAN_STATION(String qssid, String qpass) {
#ifdef debug_html
  Serial.print(F("[DB] start_WLAN_STATION, EEPROM DHCP       ")); Serial.println(EEPROMread(EEPROM_ADDR_DHCP) == 1 ? F("1 (on)") : F("0 (off)"));
  Serial.print(F("[DB] start_WLAN_STATION, EEPROM IP         ")); Serial.println(eip);
  Serial.print(F("[DB] start_WLAN_STATION, EEPROM Gateway    ")); Serial.println(esgw);
  Serial.print(F("[DB] start_WLAN_STATION, EEPROM DNS        ")); Serial.println(edns);
  Serial.print(F("[DB] start_WLAN_STATION, EEPROM SUBNETMASK ")); Serial.println(esnm);
#endif

  WiFi.disconnect();
#ifdef ARDUINO_ARCH_ESP8266
  WiFi.mode(WIFI_STA);               /* WIFI set mode */
#elif ARDUINO_ARCH_ESP32
  WiFi.mode(WIFI_MODE_STA);
#endif

#ifdef debug_wifi
  if (EEPROMread(EEPROM_ADDR_DHCP) == 0) { /* WIFI config static IP */
    if (!WiFi.config(eip, esnm, esgw, edns)) {
      Serial.println("[DB] WIFI Stationsmode WIFI_STA Failed to configure");
    } else {
      Serial.println(F("[DB] WIFI Stationsmode WIFI_STA configured with static IP"));
    }
  } else {
    Serial.println(F("[DB] WIFI Stationsmode WIFI_STA configured DHCP"));
  }
#endif

  int16_t WifiNetworks = WiFi.scanNetworks(); // scan network
  uint8_t* usedMAC = {};                      // Gets the MAC address of the router
  uint8_t usedChannel = 0;
  int16_t rssi = -999;
  int16_t bestRssi = -999;
#ifdef debug_wifi
  uint8_t* MAC;                               // Gets the MAC address of the router
  char str[18];                               // for sprintf IP-Address and MAC
  uint8_t foundChannel = 0;
#endif
  for (int8_t i = 0; i < WifiNetworks; ++i) {
    String wifiSSID = WiFi.SSID(i);
    if (wifiSSID == qssid) {
      rssi = WiFi.RSSI(i);
#ifdef debug_wifi
      MAC = WiFi.BSSID(i);                    // Gets the MAC address of the router
      foundChannel = WiFi.channel(i);         // Gets the Wifi channel
      Serial.print(F("[DB] ")); Serial.print(wifiSSID); Serial.print(F(", "));
      sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X", MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]);
      Serial.print(str); Serial.print(F(", ")); Serial.print(foundChannel); Serial.print(F(", ")); Serial.println(rssi);
#endif
      if (rssi > bestRssi) {
        bestRssi = rssi;
        usedMAC = WiFi.BSSID(i);        // Gets the MAC address of the router
        usedChannel = WiFi.channel(i);  // Gets the Wifi channel
      }
    }
  }

  WiFi.begin(qssid.c_str(), qpass.c_str(), usedChannel, usedMAC);

#ifdef debug_wifi
  Serial.print(F("[DB] WIFI try connect to ")); Serial.print(qssid); Serial.print(F(" with ")); Serial.println(qpass);
#endif
  delay(50);

  unsigned long startTime = millis(); /* ... Give ESP 60 seconds to connect to station. */
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 60000) {
    digitalWriteFast(LED, !digitalRead(LED));  // LED toggle
    delay(500);
#ifdef debug_wifi
    Serial.println('.');
#endif
  }

  if (WiFi.status() == WL_CONNECTED) {
    digitalWriteFast(LED, LOW);  // LED off
#ifdef debug_wifi
    Serial.print(F("[DB] WIFI IP address ")); Serial.println(WiFi.localIP());
#endif
    if (qssid != EEPROMread_string(EEPROM_ADDR_SSID) || qpass != EEPROMread_string(EEPROM_ADDR_PASS)) {
      EEPROMwrite_string(EEPROM_ADDR_SSID, qssid);
      EEPROMwrite_string(EEPROM_ADDR_PASS, qpass);
      EEPROMwrite(EEPROM_ADDR_AP, 0);
#ifdef debug_wifi
      Serial.println(F("[DB] WIFI Settings to EEPROM"));
    } else {
      Serial.println(F("[DB] WIFI Settings from EEPROM"));
#endif
    }
    String logText = F("WIFI connected to ");
    logText += qssid;
    appendLogFile(logText);

    WLAN_AP = false;
    WLAN_reco_cnt = 0;
  } else {
    WLAN_reco_cnt++;
    if (WLAN_reco_cnt <= 1) {
#ifdef debug_wifi
      Serial.print(F("[DB] WIFI connection failed, use WEB settings, attempt ")); Serial.println(WLAN_reco_cnt);
#endif
      start_WLAN_STATION(qssid, qpass);
    } else if (WLAN_reco_cnt <= 3) {
#ifdef debug_wifi
      Serial.print(F("[DB] WIFI connection failed, use EEPROM settings, attempt ")); Serial.println(WLAN_reco_cnt);
#endif
      start_WLAN_STATION(EEPROMread_string(EEPROM_ADDR_SSID), EEPROMread_string(EEPROM_ADDR_PASS));
    } else {
#ifdef debug_wifi
      Serial.print(F("[DB] WIFI connection failed, start AP ")); Serial.println(OwnStationHostname);
#endif
      start_WLAN_AP(OwnStationHostname, WLAN_password_ap);
      WLAN_reco_cnt = 0;
    }
  }
}


bool start_WLAN_WPS() { /* WPS works in STA (Station mode) only. */
#ifdef debug_wifi
  Serial.println(F("[DB] WIFI WPS started"));
#endif
  bool wpsSuccess;
#ifndef ARDUINO_ARCH_ESP32           /* ESP32 andere WPS Einbindung !!! */
  WiFi.hostname(OwnStationHostname); /* WIFI set hostname */
  WiFi.mode(WIFI_STA);               /* WIFI set mode */

  wpsSuccess = WiFi.beginWPSConfig();
  //WiFi.waitForConnectResult();
  if (wpsSuccess) {
    String qssid = WiFi.SSID();
    if (qssid.length() > 0 && WiFi.psk().length() > 0) {
      /* WPSConfig has already connected in STA mode successfully to the new station. */
      char* chrOwnHostname = &OwnStationHostname[0];
      wifi_station_set_hostname(chrOwnHostname);

#ifdef debug_wifi
      Serial.print(F("[DB] WIFI WPS connected to: ")); Serial.print(qssid); Serial.print(F(" with secret key ")); Serial.println(WiFi.psk());
#endif
      used_dhcp = 1;
      WLAN_AP = false;
      EEPROMwrite_string(EEPROM_ADDR_SSID, qssid);       // write String to EEPROM
      EEPROMwrite_string(EEPROM_ADDR_PASS, WiFi.psk());  // write String to EEPROM
      EEPROMwrite(EEPROM_ADDR_AP, 0);
      String logText = F("WIFI WPS connected to ");
      logText += qssid;
      appendLogFile(logText);
#ifdef debug_wifi
      Serial.println(logText);
#endif
    } else {
      wpsSuccess = false;
#ifdef debug_wifi
      Serial.println(F("[DB] WIFI WPS connection failed!"));
#endif
    }
  }
  return wpsSuccess;
#else // ESP32
  /*
    ESP32 anderes WPS Handling
    https://github.com/espressif/ESP31_RTOS_SDK/blob/master/include/esp_wps.h
    https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WPS/WPS.ino
    https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/
  */
  WiFi.onEvent(ESP32_WiFiEvent);
  WiFi.mode(WIFI_MODE_STA);
  ESP32_wpsInitConfig();
  if (esp_wifi_wps_enable(&config)) {
#ifdef debug_wifi
    Serial.println(F("[DB] WIFI WPS ERROR: enable failed"));
#endif
    WLAN_AP = 1;
    return false;
  } else if (esp_wifi_wps_start(0)) {
#ifdef debug_wifi
    Serial.println(F("[DB] WIFI WPS ERROR: start failed"));
#endif
    WLAN_AP = 1;
    return false;
  }

  uint16_t timeOut = 0;
  while (wifiEventId != 7) { // wait for event ARDUINO_EVENT_WIFI_STA_GOT_IP
    timeOut++;
    if (wifiEventId == 25 || wifiEventId == 26 || timeOut > 1800) { // ARDUINO_EVENT_WPS_ER_FAILED || ARDUINO_EVENT_WPS_ER_TIMEOUT || timeOut > 3 Minuten
#ifdef debug_wifi
      Serial.println(F("[DB] WIFI WPS ERROR: failed or timeout"));
#endif
      WLAN_AP = 1;
      return false;
    }
    delay(100);
  }

  Serial.println("WPS Successful, stopping WPS and connecting to: " + String(WiFi.SSID()));
  EEPROMwrite_string(EEPROM_ADDR_SSID, WiFi.SSID());       // write String to EEPROM
  EEPROMwrite_string(EEPROM_ADDR_PASS, WiFi.psk());        // write String to EEPROM
  EEPROMwrite(EEPROM_ADDR_AP, 0);

  wpsSuccess = true;
#endif // #ifndef ARDUINO_ARCH_ESP32           /* ESP32 andere WPS Einbindung !!! */
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
      return F("Open");
      break;
    case 8:
      return F("AUTO");
      break;
    default:
      return F("unknown");
      break;
#elif ARDUINO_ARCH_ESP32
#ifdef ESP32_core_v1
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
#else
    case WIFI_AUTH_OPEN:
      return F("Open");
      break;
    case WIFI_AUTH_WEP:
      return F("WEP");
      break;
    case WIFI_AUTH_WPA_PSK:
      return F("WPA");
      break;
    case WIFI_AUTH_WPA2_PSK:
      return F("WPA2");
      break;
    case WIFI_AUTH_WPA_WPA2_PSK:
      return F("WPA+WPA2");
      break;
    case WIFI_AUTH_WPA2_ENTERPRISE:
      return F("WPA2-EAP");
      break;
    case WIFI_AUTH_WPA3_PSK:
      return F("WPA3");
      break;
    case WIFI_AUTH_WPA2_WPA3_PSK:
      return F("WPA2+WPA3");
      break;
    case WIFI_AUTH_WAPI_PSK:
      return F("WAPI");
      break;
    default:
      return F("unknown");
      break;
#endif
#endif
  }
}
