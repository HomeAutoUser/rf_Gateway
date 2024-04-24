var store = document.querySelector(':root');  /* Get the CSS root element */
var value = getComputedStyle(store);          /* Get the styles (properties and values) for the root */
var color1 = value.getPropertyValue('--rssi_good');
var color2 = value.getPropertyValue('--rssi_bad');

var js = document.createElement("script");
js.src = '/js/all.js';
document.head.appendChild(js);

document.onreadystatechange = function () {
 if (document.readyState == 'complete') {
  document.getElementsByName('sd')[0].maxLength = 128;
  document.getElementsByName('sd')[0].pattern = "^[\\da-fA-F]{2,128}$";
  document.getElementsByName('sd')[0].placeholder = 'hex data to send (max 128 characters)';
  document.getElementsByName('sd')[0].title = 'Input: only hexadecimal characters';
  document.getElementsByName('sd')[0].setAttribute('size', '60');
  document.getElementsByName('rep')[0].max = 99;
  document.getElementsByName('rep')[0].min = 0;
  document.getElementsByName('rep')[0].setAttribute('size', '4');
  document.getElementsByName('rep')[0].value = 0;
  document.getElementsByName('rept')[0].max = 300000;
  document.getElementsByName('rept')[0].min = 2;
  document.getElementsByName('rept')[0].setAttribute('size', '7');
  document.getElementsByName('rept')[0].value = 2;
 }
}

function WebSocket_MSG(event) {
 console.log('received message: ' + event.data);

 if (event.data.includes('RAW,')) {
  var time = new Date().toLocaleTimeString();
  const obj=event.data.split(',');

  document.getElementById('MODE').innerHTML = obj[1];
  // Update Data Table
  var table = document.getElementById('dataTable');
  var row = table.insertRow(1);  // Add after headings
  var cell1 = row.insertCell(0);
  var cell2 = row.insertCell(1);
  var cell3 = row.insertCell(2);
  var cell4 = row.insertCell(3);

  TableHandlers();

  // Time
  cell1.innerHTML = time;
  cell1.style.textAlign = "center";
  // RAW
  cell2.innerHTML = obj[2];
  cell2.style.fontFamily = "Courier New,Lucida Console";
  // RSSI
  cell3.innerHTML = obj[3];
  if(obj[3] <= -80) {
   cell3.style.color = color2;
  }else if(obj[3] > -50) {
   cell3.style.color = color1;
  }
  cell3.style.textAlign = "right";
  cell4.innerHTML = obj[4];
  cell4.style.textAlign = "right";
 } else if (event.data.includes('MODE,')) {
  const obj=event.data.split(',');
  document.getElementById('MODE').innerHTML = obj[1];
 }else if (event.data.includes('TX_ready') ) {
  var today = new Date();
  document.getElementById('val').innerHTML = 'all data has been sent - ' + today.toLocaleTimeString('de-DE');
 }
}


function msgSend() {
 let sd = document.getElementsByName('sd')[0].value;
 let rep = document.getElementsByName('rep')[0].value;
 let rept = document.getElementsByName('rept')[0].value;

 if(sd == '' || rep == '' || rept == '') {
  document.getElementById('val').innerHTML = 'Please input all data &#128521;'
 } else {
  if(sd.length % 2 != 0) {
   document.getElementById('val').innerHTML = 'found odd number of nibbles, no send !!!'
   document.getElementById('val').style.color = color2;
  } else {
   if ((rep * rept / 1000) > 60) {
    document.getElementById('val').innerHTML = 'sending process active | finished in ' + ((rep * rept / 1000) / 60).toFixed(0) + ' minutes';
   } else {
    document.getElementById('val').innerHTML = 'sending process active | finished in ' + (rep * rept / 1000) + ' seconds';
   }
   websocket.send('send,' + sd + ',' + rep + ',' + rept);
  }
 }
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