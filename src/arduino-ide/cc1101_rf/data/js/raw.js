var store = document.querySelector(':root');  /* Get the CSS root element */
var value = getComputedStyle(store);          /* Get the styles (properties and values) for the root */
var color1 = value.getPropertyValue('--rssi_good');
var color2 = value.getPropertyValue('--rssi_bad');

var js = document.createElement("script");
js.src = '/js/all.js';
document.head.appendChild(js);

document.onreadystatechange = function () {
if (document.readyState == 'complete') {
const sd = document.getElementsByName('sd')[0];
sd.maxLength = 128;
sd.placeholder = `Hex data to send (max 128 characters).`;
sd.title = `Input: only hexadecimal characters.`;
sd.size = 60;
const rep = document.getElementsByName('rep')[0];
rep.max = 99;
rep.min = 0;
rep.size = 4;
rep.value = 0;
const rept = document.getElementsByName('rept')[0];
rept.max = 300000;
rept.min = 2;
rept.size = 7;
rept.value = 100;
}
}

function WebSocket_MSG(event) {
console.log(`received message: ` + event.data);
if (event.data.includes('RAW,')) {
var time = new Date().toLocaleTimeString();
const obj=event.data.split(',');
document.getElementById('MODE').innerHTML = obj[1];

var checkBox = document.getElementById("bits");
var table = document.getElementById('dataTable');
var row = table.insertRow(1);
var c1r1 = row.insertCell(0);
var c2r1 = row.insertCell(1);
var c3r1 = row.insertCell(2);
var c4r1 = row.insertCell(3);
c1r1.innerHTML = time;
c1r1.style.textAlign = "center";
c2r1.style.fontFamily = "Courier New,Lucida Console";

if (checkBox.checked == true && (!obj[2].match('^MU;'))){
var row2 = table.insertRow(2);
c2r1.innerHTML = obj[2];

var c1r2 = row2.insertCell(0);
var c2r2 = row2.insertCell(1);
c1r2.innerHTML = time;
c1r2.style.textAlign = "center";
var bits = '';
var raw = obj[2];
for (let i = 0; i < raw.length; i++) {
bits+=(parseInt(raw[i], 16).toString(2)).padStart(4, '0');
}
c2r2.colSpan = 3;
c2r2.innerHTML = bits;
c2r2.style.fontFamily = "Courier New,Lucida Console";
} else {
c2r1.innerHTML = obj[2];
}

c3r1.innerHTML = obj[3];
if (obj[3] <= -80) {
c3r1.style.color = color2;
} else if(obj[3] > -50) {
c3r1.style.color = color1;
}
c3r1.style.textAlign = "right";
c3r1.style.paddingRight = "12px";
c4r1.innerHTML = obj[4];
c4r1.style.textAlign = "right";
c4r1.style.paddingRight = "12px";

TableHandlers();
} else if (event.data.includes('MODE,')) {
const obj=event.data.split(',');
document.getElementById('MODE').innerHTML = obj[1];
} else if (event.data.includes('TX,')) {
const obj=event.data.split(',');
let ob = document.getElementById('val');
if (obj[1]==0) {
var today = new Date();
ob.innerHTML = `All messages has been sent, finished at ` + today.toLocaleTimeString('de-DE');
} else {
const rep = obj[1];
const pause = obj[2];
const dur = rep*pause/1000;
const hh = ~~((dur/60/60)%24);
const mm = ~~((dur/60)%60);
const ss = ~~(dur%60);
ob.innerHTML = `Sending process active, ${rep} more repeats, finished in ${hh}h ${mm}m ${ss}s`;
}
}
}

function msgSend() {
let sd = document.getElementsByName('sd')[0].value;
let rep = document.getElementsByName('rep')[0].value;
let rept = document.getElementsByName('rept')[0].value;
let obj = document.getElementById('val');
if (sd === '' || rep === '' || rept === '') {
obj.innerHTML = `Please input all data, will not be sent!`;
obj.style.color = color2;
} else if (!sd.match('^[\\da-fA-F]{1,128}$')) {
obj.innerHTML = `Data part is not hexadecimal, will not be sent!`;
obj.style.color = color2;
} else if (sd.length % 2 != 0) {
obj.innerHTML = `Number of nibbles in data is odd, will not be sent!`;
obj.style.color = color2;
} else {
websocket.send(`send,${sd},${rep},${rept}`);
// var someVarName = rep;
// localStorage.setItem("someVarKey", someVarName); }
}
}

function TableHandlers() {
const tbody = document.querySelector('#dataTable tbody');
tbody.addEventListener('dblclick', function (e) {
const cell = e.target.closest('td');
if (!cell) {return;} // Quit, not clicked on a cell
const row = cell.parentElement;
if(cell.cellIndex == 1) {
console.log(`copyToClipboard ` + cell.innerHTML);

const el = document.createElement('textarea');
el.value = cell.innerHTML;
document.body.appendChild(el);
el.select();
document.execCommand('copy');
document.body.removeChild(el);
}
});
}
