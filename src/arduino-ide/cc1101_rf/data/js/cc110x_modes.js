/*
  color theme style for all websites - cc1101_rf_Gateway
  Copyright (c) 2022 <HomeAutoUser>
  URL: https://github.com/
*/

 /* https://randomnerdtutorials.com/esp8266-web-server-spiffs-nodemcu/ */

setInterval(function() {
  getData();
}, 1000);    // milliseconds update rate

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {

    }
  };
  xhttp.open("GET", "/request_cc110x_modes", true);
  xhttp.send(null);
}
