/*
  JavaScript for website HOME / - cc110x_rf_Gateway
  Copyright (c) 2022 <HomeAutoUser & elektron-bbs>
  URL: https://github.com/HomeAutoUser/cc1101_rf_Gateway

  --- Data update via Websocket ---

*/

var js = document.createElement("script");
js.src = '/all.js';
document.head.appendChild(js);

var UptTxt = '';

// Show the received message 
function onMessage(event) {
  console.log('received message: ' + event.data);

  /* {"CC1101":"yes","RAM":"31024","Uptime":"7485","MSGcnt":"1330","WLANdB":"-61"} */
  if(event.data.includes('CC1101') ) {
    var obj = JSON.parse(event.data);
    document.getElementById("RAM").innerHTML = obj.RAM;
    document.getElementById("Uptime").innerHTML = obj.Uptime;
    var dd = ~~(obj.Uptime/60/60/24);
    var hh = ~~( (obj.Uptime/60/60) % 24 );
    var mm = ~~( (obj.Uptime/60) % 60 );
    var ss = (obj.Uptime % 60);
    UptTxt = dd + ' day(s)&emsp;' + hh + ' hour(s)&emsp;' + mm + ' minute(s)&emsp;' + ss + ' second(s)';
    document.getElementById("UptimeTxT").innerHTML = UptTxt;
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
