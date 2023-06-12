/*
  JavaScript for website cc110x / CC110x - cc110x_rf_Gateway
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

  /* {"chip_MS":"0D = RX","chip_ReMo":"Lacrosse_mode1","ToggleBank":"{&emsp;11&emsp;12&emsp;13&emsp;-&emsp;}","ToggleTime":"30000"} */
  if(event.data.includes('chip_') ) {
    var obj = JSON.parse(event.data);
    document.getElementById("chip_MS").innerHTML = obj.chip_MS;
    document.getElementById("chip_ReMo").innerHTML = obj.chip_ReMo;
    document.getElementById("ToggleBank").innerHTML = obj.ToggleBank;
    document.getElementById("ToggleTime").innerHTML = obj.ToggleTime;
  }
}
