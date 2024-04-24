var js = document.createElement("script");
js.src = '/js/all.js';
document.head.appendChild(js);

function WebSocket_MSG(event) {
 console.log('received message: ' + event.data);

 if(event.data.includes('chip') ) {
  const obj=event.data.split(',');
  document.getElementById('RAM').innerHTML = obj[1];
  document.getElementById('Uptime').innerHTML = obj[2];
  var dd = ~~(obj[2]/60/60/24);
  var hh = ~~( (obj[2]/60/60) % 24 );
  var mm = ~~( (obj[2]/60) % 60 );
  var ss = (obj[2] % 60);
  var UptTxt = dd + ' day(s)&emsp;' + hh + ' hour(s)&emsp;' + mm + ' minute(s)&emsp;' + ss + ' second(s)';
  document.getElementById('UptimeTxT').innerHTML = UptTxt;
  var timeDiff=obj[2]-obj[3];
  if (timeDiff>=3600) {
   var MSGcntCal = obj[4] / (timeDiff / 3600);
   var MSGcntTXT = obj[4] + '&emsp;(' + Math.ceil(MSGcntCal) + ' per hour)';
   document.getElementById('MSGcnt').innerHTML = MSGcntTXT;
  } else if (timeDiff>=60) {
   var MSGcntCal = obj[4] / (timeDiff / 60);
   var MSGcntTXT = obj[4] + '&emsp;(' + Math.ceil(MSGcntCal) + ' per minute)';
   document.getElementById('MSGcnt').innerHTML = MSGcntTXT;
  } else {
   document.getElementById('MSGcnt').innerHTML = obj[4];
  }
  document.getElementById('WLANdB').innerHTML = obj[5];
 }
}

function refresh(event) {
 console.log('Restart ESP, try to reload the page in 30 seconds.');
 setTimeout(function(){
  location.reload();
 }, 30000);
}