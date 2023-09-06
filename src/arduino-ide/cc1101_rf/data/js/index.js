var js = document.createElement("script");
js.src = '/all.js';
document.head.appendChild(js);

function onMessage(event) {
 console.log('received message: ' + event.data);

 if(event.data.includes('CC1101') ) {
  const obj=event.data.split(',');
  document.getElementById("RAM").innerHTML = obj[1];
  document.getElementById("Uptime").innerHTML = obj[2];
  var dd = ~~(obj[2]/60/60/24);
  var hh = ~~( (obj[2]/60/60) % 24 );
  var mm = ~~( (obj[2]/60) % 60 );
  var ss = (obj[2] % 60);
  var UptTxt = dd + ' day(s)&emsp;' + hh + ' hour(s)&emsp;' + mm + ' minute(s)&emsp;' + ss + ' second(s)';
  document.getElementById("UptimeTxT").innerHTML = UptTxt;
  if (obj[2]>= 3600) {
   var MSGcntCal = obj[4] / ((obj[2] - obj[3]) / 3600);
   var MSGcntTXT = obj[4] + '&emsp;(' + Math.ceil(MSGcntCal) + ' per hour)';
   document.getElementById("MSGcnt").innerHTML = MSGcntTXT;
  } else if (obj[2]>= 60) {
   var MSGcntCal = obj[4] / ((obj[2] - obj[3]) / 60);
   var MSGcntTXT = obj[4] + '&emsp;(' + Math.ceil(MSGcntCal) + ' per minute)';
   document.getElementById("MSGcnt").innerHTML = MSGcntTXT;
  } else {
   document.getElementById("MSGcnt").innerHTML = obj[4];
  }
  document.getElementById("WLANdB").innerHTML = obj[5];
 }
}

function refresh() {
 setTimeout(() => {
  document.location.reload();
 }, 2000);
}

/* 
obj[3]
*/