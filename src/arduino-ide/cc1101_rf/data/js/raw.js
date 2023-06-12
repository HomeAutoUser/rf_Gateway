/*
  JavaScript for website RAW data /raw - cc110x_rf_Gateway
  Copyright (c) 2022 <HomeAutoUser & elektron-bbs>
  URL: https://github.com/HomeAutoUser/cc1101_rf_Gateway

  --- Data update via Websocket ---

*/

var js = document.createElement("script");
js.src = '/all.js';
document.head.appendChild(js);

// Show the received message 
function onMessage(event) {
  console.log('received message: ' + event.data);

  /*  {"RAW":"9006596A28", "RAW_rssi":"-52", "RAW_afc":"-14", "RAW_MODE":"Lacrosse_mode1"} */
  if(event.data.includes('RAW_rssi') ) {
    // Push the data in array
    var time = new Date().toLocaleTimeString();
    var obj = JSON.parse(event.data);
    var keyCount = Object.keys(obj).length;   // count JSON objects
    document.getElementById("RAW_MODE").innerHTML = obj.RAW_MODE;

    if(keyCount == 4) {   // if data received, object contains 4 elements | no received, object contains 1 element
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
      if(obj.RAW_rssi <= -80) {
        cell3.style.color = "#ff0000";
        cell3.style.textAlign = "right";
      }else if(obj.RAW_rssi > -50) {
        cell3.style.color = "#28a428";
        cell3.style.textAlign = "right";
      }else{
        cell3.style.textAlign = "right";
      }
      cell4.innerHTML = obj.RAW_afc;
      cell4.style.textAlign = "right";
    }
  }
}
