/*
  JavaScript for website RAW data /raw - cc110x_rf_Gateway
  Copyright (c) 2022 <HomeAutoUser & elektron-bbs>
  URL: https://github.com/HomeAutoUser/cc1101_rf_Gateway

  --- Data update via Websocket ---

*/

var store = document.querySelector(':root');        /* Get the CSS root element */
var value = getComputedStyle(store);                /* Get the styles (properties and values) for the root */
var color1 = value.getPropertyValue('--rssi_good');
var color2 = value.getPropertyValue('--rssi_bad');

var js = document.createElement("script");
js.src = '/all.js';
document.head.appendChild(js);

// Show the received message 
function onMessage(event) {
  console.log('received message: ' + event.data);

  if (event.data.includes('RSSI') ) {
    // Push the data in array
    var time = new Date().toLocaleTimeString();
    var obj = JSON.parse(event.data);
    var keyCount = Object.keys(obj).length;   // count JSON objects
    document.getElementById("MODE").innerHTML = obj.MODE;

    if(keyCount == 4) {   // if data received, object contains 4 elements | security check
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
      cell3.innerHTML = obj.RSSI;
      if(obj.RSSI <= -80) {
        cell3.style.color = color2;
        cell3.style.textAlign = "right";
      }else if(obj.RSSI > -50) {
        cell3.style.color = color1;
        cell3.style.textAlign = "right";
      }else{
        cell3.style.textAlign = "right";
      }
      cell4.innerHTML = obj.AFC;
      cell4.style.textAlign = "right";
    }
  } else if (event.data.includes('MODE')) {
    var obj = JSON.parse(event.data);
    var keyCount = Object.keys(obj).length;
    if (keyCount == 1) {                       // if data received, object contains 1 elements | security check
      document.getElementById("MODE").innerHTML = obj.MODE;
    }
  }
}