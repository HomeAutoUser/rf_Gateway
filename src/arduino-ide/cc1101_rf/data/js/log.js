var js = document.createElement("script");
js.src = '/all.js';
document.head.appendChild(js);

function onMessage(event) {
 console.log('received message: ' + event.data);

 if(event.data == 'Connected') {
  var uptime = document.getElementById("uptime").textContent;
  
  const d = new Date();
  let now = d.getTime();
  var elms = document.querySelectorAll("[id='logtxt']");

  for(var i = 0; i < elms.length; i++) {
   var line = elms[i].textContent;
   let found = line.match(/(\d+)\s-\s/i);

   if (found) {
    var timeNew = now - (uptime * 1000) + (found[1]*1000);
    let res = line.replace(/\d+ - /g, getDateTime(timeNew) + " - ");
    elms[i].innerText = res;
    elms[i].hidden = false;
   }
  }
 }
}

function getDateTime(zeit) {
  var tzoffset=(new Date(zeit)).getTimezoneOffset()*60000;
  var localISOTime=(new Date(zeit-tzoffset)).toISOString().slice(0, 19).replace('T', ' ');
  return localISOTime;
}

