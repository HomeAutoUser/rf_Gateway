var store = document.querySelector(':root');  /* Get the CSS root element */
var value = getComputedStyle(store);          /* Get the styles (properties and values) for the root */
var color1 = value.getPropertyValue('--bt_reception_active');
var color2 = value.getPropertyValue('--bt_reception_enable');

var js = document.createElement("script");
js.src = '/all.js';
document.head.appendChild(js);

function onMessage(event) {
 console.log('received message: ' + event.data);

 if (event.data.includes('Connected') ) {
  document.getElementsByName('tgt')[0].maxLength = "7";
  document.getElementsByName('tgt')[0].pattern = "^[\\d]{1,7}$";
  document.getElementsByName('tgt')[0].title = "Input: 15000 ... 1800000 ms";
 } else if (event.data.includes('MODE,') ) {
  const obj=event.data.split(',');

  /* erste Spalte | automatischer Farbwechsel aktiver Modus
    Zählweise Tabellen ID
    0,1,2
    ...
    33,34,35
    ...
    ENDE
  */

  const cell = document.getElementById("rec_mod").getElementsByTagName("td");
  for (var i = 0; i < cell.length; i++) {
   if (i % 3 == 0) {
    btnr = i / 3;
    var bt = '#bt' + btnr;
    var myEle = document.querySelector(bt);   /* to check element of error: Cannot set properties of null (setting 'innerHTML') */

    if (myEle!=null && bt != 'null' && bt != 'NULL') {
     if (btnr == obj[2]) {        /* current button status */
      document.querySelector(bt).innerHTML = 'active reception';
      document.querySelector(bt).className = 'btn2';
     } else {
      document.querySelector(bt).innerHTML = 'enable reception';
      document.querySelector(bt).className = 'btn';
     }
    }
   }

   if (cell[i].innerHTML == obj[1]) {
    var btnr = i / 3;
    cell[i].style.backgroundColor = color1;     /* MODE */
    cell[i+1].style.backgroundColor = color1;   /* set to bank 0 1 2 3 */
    cell[i+2].style.backgroundColor = color1;   /* button enable(d) reception */
    i = i + 2;
   } else {
    cell[i].style.backgroundColor = color2;
   }
  }

  var today = new Date();
  document.getElementById('stat').innerHTML = 'last action: ' + today.toLocaleTimeString('de-DE');
 }
}
