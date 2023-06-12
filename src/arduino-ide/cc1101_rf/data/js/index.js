/*
  JavaScript for website HOME / - cc110x_rf_Gateway
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

  /* {"CC1101":"yes","RAM":"31024","Uptime":"7485","dd":"0","hh":"2","mm":"4","ss":"45","MSGcnt":"1330","WLANdB":"-61"} */
  if(event.data.includes('CC1101') ) {
    var obj = JSON.parse(event.data);
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
}
