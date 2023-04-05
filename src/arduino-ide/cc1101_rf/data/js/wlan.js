/*
  color theme style for all websites - cc1101_rf_Gateway
  Copyright (c) 2022 <HomeAutoUser>
  URL: https://github.com/
*/

/* https://randomnerdtutorials.com/esp8266-web-server-spiffs-nodemcu/ */
function wlan_conn() {
  //alert("connect");
  document.getElementById("StatCon").innerHTML = "- new wlan settings -";
}

function wlan_wps() {
  //alert("WPS");
  document.getElementById("StatWPS").innerHTML = "- WPS starting -";
}