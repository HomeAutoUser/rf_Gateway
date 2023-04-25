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
      document.getElementById("Uptime").innerHTML = obj.Uptime;
      document.getElementById("dd").innerHTML = obj.dd;
      document.getElementById("hh").innerHTML = obj.hh;
      document.getElementById("mm").innerHTML = obj.mm;
      document.getElementById("ss").innerHTML = obj.ss;
      document.getElementById("WLANdB").innerHTML = obj.WLANdB;

      if ( (obj.Uptime / 60) >= 60) {
        var MSGcntCal = obj.MSGcnt / (obj.Uptime / 3600);
        var MSGcntTXT = obj.MSGcnt + '&emsp;(' + Math.ceil(MSGcntCal) + ' per hour)';
        document.getElementById("MSGcnt").innerHTML = MSGcntTXT;
      } else if ( (obj.Uptime / 60) >= 1) {
        var MSGcntCal = obj.MSGcnt / (obj.Uptime / 60);
        var MSGcntTXT = obj.MSGcnt + '&emsp;(' + Math.ceil(MSGcntCal) + ' per minute)';
        document.getElementById("MSGcnt").innerHTML = MSGcntTXT;
      } else {
        document.getElementById("MSGcnt").innerHTML = obj.MSGcnt;
      }
    }
  };
  xhttp.open("GET", "/request_status", true);
  xhttp.send();
}, 10000 );
