var store = document.querySelector(':root');  /* Get the CSS root element */
var value = getComputedStyle(store);          /* Get the styles (properties and values) for the root */
var color1 = value.getPropertyValue('--bt_reception_active');
var color2 = value.getPropertyValue('--bt_reception_enable');

var js = document.createElement("script");
js.src = '/js/all.js';
document.head.appendChild(js);

function onMessage(event) {
 console.log('received message: ' + event.data);

 if (event.data.includes('Connected') ) {
  // document.getElementsByName('tgt')[0].maxLength = "7";
  // document.getElementsByName('tgt')[0].pattern = "^[\\d]{1,7}$";
  // document.getElementsByName('tgt')[0].title = "Input: 15000 ... 1800000 ms";

 } else if (event.data.includes('MODE,') ) {
  const obj=event.data.split(',');
  for (var c = 3; c < obj.length; c++) { // msg count
   var id = 'c';
   id += c - 3;
   document.getElementById(id).innerHTML = obj[c];
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
