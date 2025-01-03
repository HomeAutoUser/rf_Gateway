﻿var store = document.querySelector(':root');
var value = getComputedStyle(store);
var color1 = value.getPropertyValue('--bt_reception_active');
var color2 = value.getPropertyValue('--bt_reception_enable');
var onlyOne = false;
var changed = 0;
var wsMsg = 0;
var js = document.createElement("script");
js.src = '/js/all.js';
document.head.appendChild(js);

function WebSocket_MSG(event) {
console.log(`received message: ` + event.data);
if (event.data.includes('MODE,')) {
const obj=event.data.split(',');
const time=obj[3].split('_');
const msgcnt=obj[4].split('_');
let l = time.length;
wsMsg++;
for (var c=0;c<l;c++) { // msg count
var id = 'c'+c;
var name = 't'+c;
let el = document.getElementsByName(name)[0];
document.getElementById(id).innerHTML = msgcnt[c];
if(changed == 0){
el.value = time[c];
}
if (!onlyOne){
el.onkeypress = validNumber;
el.placeholder = document.getElementsByName(name)[0].value;
}
}
if (!onlyOne){
document.getElementsByName('ta')[0].onkeypress = validNumber;
onlyOne = true;
}
const tab = document.getElementById("rec_mod");
let l2 = tab.rows.length;
for (var i=2;i<l2-1;i++){
if(tab.rows[i].cells[0].innerHTML.substring(5) === obj[1]){
for(let c=0;c<4;c++){
tab.rows[i].cells[c].style.backgroundColor = color1;
}
} else {
for(let c=0;c<4;c++){
tab.rows[i].cells[c].style.backgroundColor = color2;
}
}
}
if (document.getElementById('stat') && wsMsg > 1) {
var today = new Date();
document.getElementById('stat').innerHTML = `last action: ` + today.toLocaleTimeString('de-DE');
}
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


document.onreadystatechange = function () {
if (document.readyState == 'complete') {
CheckNotes();
}
};

function CheckNotes() {
const tbody = document.querySelector('#rec_mod tbody');
tbody.addEventListener('mouseover', function (e) {
const cell = e.target.closest('td');
if (!cell) {return;}
const row = cell.parentElement;
if(cell.cellIndex == 0) {
var value = cell.innerHTML;
if (value.match('^[\\d]{1,3}')) {
var mode = value.substring(5);
var txt = modeIndex(mode);
if (txt){
if (cell.hasAttribute('title') == false) {
cell.setAttribute('title', txt);
}
}
}
}
});
}

function modeIndex(Value){
for (var i = 0; i < notes.length; i++) {
if(Value == notes[i][0]){
return notes[i][1];
}
}
}

const a = `- firmware must support processing & only one register setting does not work!`;
const b = `- Fine Offset WH40, Ambient Weather WH40E, ecowitt WH40\n- Fine Offset WH57, Ambient Weather WH31L, Froggit DP60`;
const c = `- Fine Offset WH51, ecowitt WH51, Froggit DP100, MISOL/1`;
const r = `remotes:\n`;
const s = `sensors:\n`;

const notes = [
[`Chip factory default`,`factory settings chip from datasheet`],
[`Chip user setting`,`User settings after adjustment`],
//[`ASK/OOK 433 MHz`,``],
//[`Avantek DB-LE`,``],
//[`Bresser 5-in-1`,``],
[`Bresser 6-in-1`,`send interval: 12 secondes, two different messages`],
//[`Bresser 7-in-1`,``],
//[`Fine Offset WH31 868 MHz`,``],
[`Fine Offset WH40/WH57 434 MHz`,`${s}${b}`],
[`Fine Offset WH40/WH57 868 MHz`,`${s}${b}`],
[`Fine Offset WH51 434 MHz`,`${s}${c}`],
[`Fine Offset_WH51 868 MHz`,`${s}${c}`],
[`Inkbird IBS-P01R`,`${s}- Inkbird IBS-P01R, ITH-20R`],
[`Hoymiles Inverter HMS/HMT`,`${a}\n- settings of the base frequency DTU (different frequencies must be set manually !)\n- send interval: different`],
[`KOPP FC`,`- untested!`],
[`LaCrosse mode 1`,`send interval: 4 secondes`],
[`LaCrosse mode 2`,`send interval: 10 secondes`],
//[`PCA301`,``],
[`Rojaflex`,`${r}- HSR-15, HSTR-15`],
[`WMBus S`,`${a}`],
[`WMBus T`,`${a}`],
[`X-Sense XS01-WR`,`- only on SX1231 can receive & send | CC110X receiver not suitable (only receive)`]
];
