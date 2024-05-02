var store = document.querySelector(':root');  /* Get the CSS root element */
var value = getComputedStyle(store);          /* Get the styles (properties and values) for the root */
var color1 = value.getPropertyValue('--rssi_good');
var color2 = value.getPropertyValue('--rssi_bad');

var js = document.createElement("script");
js.src = '/js/all.js';
document.head.appendChild(js);

document.onreadystatechange = function () {
 if (document.readyState == 'complete') {
   // var test = localStorage.getItem("someVarKey");
   // console.log('test');
   // console.log(test);
  const sd = document.getElementsByName('sd')[0];
  sd.maxLength = 128;
  sd.placeholder = 'Hex data to send (max 128 characters).';
  sd.title = 'Input: only hexadecimal characters.';
  sd.size = 60;  
  const rep = document.getElementsByName('rep')[0];
  rep.max = 99;
  rep.min = 0;
  rep.size = 4;
  rep.value = 0;
  const rept = document.getElementsByName('rept')[0];
  rept.max = 300000; // 999999
  rept.min = 2;
  rept.size = 7;
  rept.value = 100;
 }
}

function WebSocket_MSG(event) {
 console.log('received message: ' + event.data);
 if (event.data.includes('RAW,')) {
  var time = new Date().toLocaleTimeString();
  const obj=event.data.split(',');
  document.getElementById('MODE').innerHTML = obj[1];
  var table = document.getElementById('dataTable');
  var row = table.insertRow(1);
  var cell1 = row.insertCell(0);
  var cell2 = row.insertCell(1);
  var cell3 = row.insertCell(2);
  var cell4 = row.insertCell(3);
  TableHandlers();
  cell1.innerHTML = time;
  cell1.style.textAlign = "center";
  cell2.innerHTML = obj[2];
  cell2.style.fontFamily = "Courier New,Lucida Console";
  cell3.innerHTML = obj[3];
  if (obj[3] <= -80) {
   cell3.style.color = color2;
  } else if(obj[3] > -50) {
   cell3.style.color = color1;
  }
  cell3.style.textAlign = "right";
	cell3.style.paddingRight = "12px";
  cell4.innerHTML = obj[4];
  cell4.style.textAlign = "right";
	cell4.style.paddingRight = "12px";
 } else if (event.data.includes('MODE,')) {
  const obj=event.data.split(',');
  document.getElementById('MODE').innerHTML = obj[1];
 } else if (event.data.includes('TX,')) {
  const obj=event.data.split(',');
	if (obj[1]==0) {
   var today = new Date();
   document.getElementById('val').innerHTML = 'All messages has been sent, finished at ' + today.toLocaleTimeString('de-DE');
	} else {
   const rep = obj[1];
   const pause = obj[2];
	 const dur = rep*pause/1000;
   const hh = ~~((dur/60/60)%24);
   const mm = ~~((dur/60)%60);
   const ss = ~~(dur%60);
   document.getElementById('val').innerHTML = `Sending process active, ${rep} more repeats, finished in ${hh}h ${mm}m ${ss}s`;
	}
 }
}

function msgSend() {
 let sd = document.getElementsByName('sd')[0].value;
 let rep = document.getElementsByName('rep')[0].value;
 let rept = document.getElementsByName('rept')[0].value;
 if (sd === '' || rep === '' || rept === '') {
  document.getElementById('val').innerHTML = 'Please input all data, will not be sent!';
  document.getElementById('val').style.color = color2;
 } else if (!sd.match('^[\\da-fA-F]{1,128}$')) {
  document.getElementById('val').innerHTML = 'Data part is not hexadecimal, will not be sent!';
  document.getElementById('val').style.color = color2;
 } else if (sd.length % 2 != 0) {
  document.getElementById('val').innerHTML = 'Number of nibbles in data is odd, will not be sent!';
  document.getElementById('val').style.color = color2;
 } else {
  websocket.send(`send,${sd},${rep},${rept}`);
// var someVarName = rep;
// localStorage.setItem("someVarKey", someVarName); }
}

function TableHandlers() {
  const tbody = document.querySelector('#dataTable tbody');
  tbody.addEventListener('dblclick', function (e) {
    const cell = e.target.closest('td');
    if (!cell) {return;} // Quit, not clicked on a cell
    const row = cell.parentElement;
    if(cell.cellIndex == 1) {
      console.log('copyToClipboard ' + cell.innerHTML);

      const el = document.createElement('textarea');
      el.value = cell.innerHTML;
      document.body.appendChild(el);
      el.select();
      document.execCommand('copy');
      document.body.removeChild(el);
    }
  });
}