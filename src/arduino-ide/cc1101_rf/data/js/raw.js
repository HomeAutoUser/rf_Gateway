/*
  color theme style for all websites - cc1101_rf_Gateway
  Copyright (c) 2022 <HomeAutoUser>
  URL: https://github.com/
*/

 /* https://randomnerdtutorials.com/esp8266-web-server-spiffs-nodemcu/ */

setInterval(function() {
  getData();
}, 1500);    // milliseconds update rate

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      if(this.responseText != "{}") {
        // Push the data in array
        var time = new Date().toLocaleTimeString();
        var txt = this.responseText;
        var obj = JSON.parse(txt);
        // Update Data Table
        var table = document.getElementById("dataTable");
        var row = table.insertRow(1);   // Add after headings
        var cell1 = row.insertCell(0);
        var cell2 = row.insertCell(1);
        var cell3 = row.insertCell(2);
        var cell4 = row.insertCell(3);
        // Time & Style
        cell1.innerHTML = time;
        cell1.style.textAlign = "center";
        // RAW & Style
        cell2.innerHTML = obj.RAW;
        cell2.style.fontFamily = "Courier New,Lucida Console";
        // RSSI & Style
        cell3.innerHTML = obj.RAW_rssi;
        if(obj.RAW_rssi <= -60) {
          cell3.style.color = "#ff0000";
          cell3.style.textAlign = "right";
        }
        if(obj.RAW_rssi > -60) {
          cell3.style.color = "#28a428";
          cell3.style.textAlign = "right";
        }
        cell4.innerHTML = obj.RAW_afc;
        cell4.style.textAlign = "right";
      }
    }
  };
  xhttp.open("GET", "/request_raw", true);
  xhttp.send(null);
}
