/*
  JavaScript for website cc110x / CC110x - cc110x_rf_Gateway
  Copyright (c) 2022 <HomeAutoUser & elektron-bbs>
  URL: https://github.com/HomeAutoUser/cc1101_rf_Gateway
*/

var js = document.createElement("script");
js.src = '/all.js';
document.head.appendChild(js);

// Show the received message 
function onMessage(event) {
  console.log('received message: ' + event.data);

  if(event.data.includes('MS') ) {
    var obj = JSON.parse(event.data);
    document.getElementById("MS").innerHTML = obj.MS;
    document.getElementById("MODE").innerHTML = obj.MODE;
    document.getElementById("ToggleBank").innerHTML = obj.ToggleBank.replace(/\s/g, '&emsp;');
    document.getElementById("Time").innerHTML = obj.Time;
  }
}
