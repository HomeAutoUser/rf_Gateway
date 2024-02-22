var store = document.querySelector(':root');  /* Get the CSS root element */
var value = getComputedStyle(store);          /* Get the styles (properties and values) for the root */
var color1 = value.getPropertyValue('--bt_reception_active');
var color2 = value.getPropertyValue('--bt_reception_enable');
var status_html = false;
var onlyOne = false;

var js = document.createElement("script");
js.src = '/js/all.js';
document.head.appendChild(js);

function onMessage(event) {
 console.log('received message: ' + event.data);

 if (event.data.includes('Connected') ) {
  status_html = 1;
  check();
 } else if (event.data.includes('MODE,') ) {
  const obj=event.data.split(',');
  for (var c = 3; c < obj.length; c++) { // msg count
   var id = 'c';
   id += c - 3;
   document.getElementById(id).innerHTML = obj[c];
   if (!onlyOne){
    var name = 't';
    let element = document.getElementsByName(name)[0];
    name += c - 3;
    element.onkeypress = validateNumber;
    element.placeholder = element.value;
   }
  }
  if (!onlyOne){
   document.getElementsByName('ta')[0].onkeypress = validateNumber;
   onlyOne = true;
  }
  /* erste Spalte | automatischer Farbwechsel aktiver Modus
    Zählweise Tabellen ID
    0,1,2 
    ...
    33,34,35
    ...
  */
  const cell = document.getElementById("rec_mod").getElementsByTagName("td");
  for (var i = 0; i < cell.length; i++) {
   if (cell[i].innerHTML.indexOf(' - ' + obj[1]) >= 0) {
    cell[i].style.backgroundColor = color1;
    cell[i+1].style.backgroundColor = color1;
    cell[i+2].style.backgroundColor = color1;
    cell[i+3].style.backgroundColor = color1; /* count */
    i = i + 3;
   } else {
    cell[i].style.backgroundColor = color2;
   }
  }
  var today = new Date();
  document.getElementById('stat').innerHTML = 'last action: ' + today.toLocaleTimeString('de-DE');
 }
}

function check(){
 if (status_html && !onlyOne) {
  websocket.send('modes');
 }
}

function validateNumber(e) {
 const pattern = /^[0-9]$/;
 return pattern.test(e.key)
}