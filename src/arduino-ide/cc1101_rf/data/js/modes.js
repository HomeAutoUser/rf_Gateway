var store = document.querySelector(':root');  /* Get the CSS root element */
var value = getComputedStyle(store);          /* Get the styles (properties and values) for the root */
var color1 = value.getPropertyValue('--bt_reception_active');
var color2 = value.getPropertyValue('--bt_reception_enable');
var status_html = false;
var onlyOne = false;
var changed = 0;
var wsMsg = 0;

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
  const time=obj[3].split('_');
  const msgcnt=obj[4].split('_');
	wsMsg++;
  for (var c = 0; c < time.length; c++) { // msg count
   var id = 'c';
   id += c;
   var name = 't';
   name += c;
   document.getElementById(id).innerHTML = msgcnt[c];
   if(changed == 0){
    document.getElementsByName(name)[0].value = time[c];
   };

   if (!onlyOne){
    document.getElementsByName(name)[0].onkeypress = validNumber;
    document.getElementsByName(name)[0].placeholder = document.getElementsByName(name)[0].value;
   }
  }
  if (!onlyOne){
   document.getElementsByName('ta')[0].onkeypress = validNumber;
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
  if (document.getElementById('stat') && wsMsg > 1) {
    var today = new Date();
    document.getElementById('stat').innerHTML = 'last action: ' + today.toLocaleTimeString('de-DE');
  }
 }
}

function check(){
 if (status_html && !onlyOne) {
  websocket.send('modes');
 }
}

function validNumber(e) {
 const pattern = /^[0-9]$/;
 return pattern.test(e.key)
}

document.addEventListener('focusin', function(event) {
  if (document.activeElement.tagName == "INPUT" && document.activeElement.type == "number") {
    changed++;
  }
});