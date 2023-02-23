/*
  color theme style for all websites - cc1101_rf_Gateway
  Copyright (c) 2022 <HomeAutoUser>
  URL: https://github.com/
*/

/* https://randomnerdtutorials.com/esp8266-web-server-spiffs-nodemcu/ */
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var txt = this.responseText;
      var obj = JSON.parse(txt);
      document.getElementById("RAM").innerHTML = obj.RAM;
      document.getElementById("Upt_Sec").innerHTML = obj.Upt_Sec;
      document.getElementById("Upt_TXT").innerHTML = obj.Upt_TXT;
    }
  };
  xhttp.open("GET", "/request_status", true);
  xhttp.send();
}, 10000 );
