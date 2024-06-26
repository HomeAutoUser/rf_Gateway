"use strict";
var js = document.createElement("script");
js.src = '/js/all.js';
document.head.appendChild(js);
var js2 = document.createElement("script");
js2.src = '/js/functions.js';
document.head.appendChild(js2);

var store = document.querySelector(':root');
var value = getComputedStyle(store);
var color1 = value.getPropertyValue('--val_std');
var color2 = value.getPropertyValue('--val_diff');
var mod_list = false;
var bw_list = false;

document.onreadystatechange = function () {
if (document.readyState == 'complete') {
let obj = document.getElementById('p1');
obj.maxLength = 7;
obj.pattern = "^[\\d]{3}(\\.[\\d]{1,3})?$";
obj = document.getElementById('p2');
obj.maxLength = 6;
obj.pattern = "^-?[\\d]{1,3}(\\.[\\d]{1,3})?$";
obj = document.getElementById('p4');
obj.maxLength = 8;
obj.pattern = "^[\\d]{1,4}(\\.[\\d]{1,3})?$";
obj = document.getElementById('p5');
obj.maxLength = 7;
obj.pattern = "^[\\d]{1,3}(\\.[\\d]{1,3})?$";
var hex;
var lastmodal;
var element;
let l = C_Exp.length;
for (let i=0; i<l; i++) {
hex = (i <= 15 ? '0' : '') + i.toString(16);
element = document.getElementsByName('r' + i)[0];
element.maxLength = 2;
element.size=2;
element.title = `Input: 2 characters in hexadecimal`;
element.onkeypress = validHEX;
document.getElementById('s' + i).innerHTML = '0x' + hex.toUpperCase() + '&ensp;' + C_RegN[i];
document.getElementById('n' + i).innerHTML = C_Exp[i];
var id = 't' + i;
if (C_Add[i] !== '') {
document.getElementById(id).innerHTML = '&#128712;';
var div = document.createElement("div");
div.id = "Mo"+i;
div.className = "mod";
document.getElementById(id).appendChild(div);
var div2 = document.createElement("div");
div.appendChild(div2);
div2.className = "mod-cont";
var Span = document.createElement("span");
Span.className = "close";
Span.innerHTML = "&times;";
div2.appendChild(Span);
var p = document.createElement("p");
p.innerHTML = C_RegN[i];
div2.appendChild(p);
div2.insertAdjacentHTML( 'beforeend', C_Add[i] );
}
}
window.onclick = function(event) {
var id = event.target.id.match(/t\d+/i);
var nr = event.target.id.substr(1);
var cla = event.target.className;
var modal = document.getElementById("Mo" + nr); // Get the modal
if (C_Add[nr] !== ''){
if(id && modal) {
modal.style.display = "block"; // open the modal
lastmodal = modal;
}
if(cla && cla == 'close'){
lastmodal.style.display = "none"; // close the modal
}
}
}
}
}

function WebSocket_MSG(event) {
console.log(`received message: ` + event.data);
if(event.data.includes(',detail,')) {
const obj=event.data.split(',');
var REGISTER_MAX = 46;
for (let i=0; i<= REGISTER_MAX; i++) {
let element = document.getElementsByName('r' + i)[0];
element.placeholder = element.value.replace(element.value, obj[i]);
if (element.value != obj[i]) {
element.value = element.value.replace(element.value, obj[i]);
element.placeholder = obj[i];
element.style.color = color2;
} else {
element.style.color = color1;
}
}
// 0x06 0x08
var val = parseInt(obj[8], 16) & 0b00000011;
document.getElementById('PKTCTRL0').innerHTML = C_Length[val];
let span = document.getElementById("PKTCTRL0");
if (val === 0) {
let bytes = parseInt(obj[6], 16);
span.textContent = `current: bytes ${bytes}  |  nibbles ${bytes * 2}  |  bits ${bytes * 8}`;
} else {
span.textContent = `current: disabled`;
}
// 0x0C
if(document.getElementsByName('afc')[0].checked) {
document.getElementById('n12').innerHTML = `${C_Exp[12]} (Freq. offset ${(C_FREQOFFread(obj[12]) / 1000).toFixed(0)} kHz)`;	// ToDo
}
// 0x0D 0x0E 0x0F
var Freq = C_FREQread(obj[13], obj[14], obj[15]) / 1000000; // ToDO
document.getElementById('FREQis').innerHTML = Freq.toFixed(3);
Freq -= obj[REGISTER_MAX + 3] * 1;
document.getElementById('FREQ').innerHTML = Freq.toFixed(3);
document.getElementsByName('freq')[0].value = Freq.toFixed(3);
// 0x10 0x11
var RxBwComp = (C_BWread(obj[16]) / 1000).toFixed(3); // ToDO
var selectElement = document.getElementById('bandw');
if(!bw_list) {
bw_list = true;
const bwSteps = C_BWsteps();
for (let i = 0; i < bwSteps.length; i++) {
selectElement.add(new Option((bwSteps[i] / 1000).toFixed(3)));
}
}
selectElement.value = RxBwComp;
document.getElementById('CHANBW').innerHTML = `${RxBwComp} kHz`;
var DRATE = (C_DRATEread(obj[16], obj[17]) / 1000.0).toFixed(3); // ToDo
document.getElementById('DRATE').innerHTML = `${DRATE} kBaud`;
document.getElementsByName('datarate')[0].value = DRATE;
// 0x12
if (!mod_list) {
mod_list = true;
var selectElement = document.getElementById('modulation');
for (var j = 0; j<=7; j++) {
if (C_Mod[j] != '') {
selectElement.add(new Option(C_Mod[j]));
}
}
}
val = (parseInt(obj[18], 16) & 0b01110000) >> 4;
document.getElementById('MOD_FORMAT').innerHTML = C_Mod[val];
document.getElementById('modulation').value = C_Mod[val];
val = (parseInt(obj[18], 16) & 0b00000111);
document.getElementById('SYNC_MODE').innerHTML = C_Sync[val];
// 0x13
val = (parseInt(obj[19], 16) & 0b01110000) >> 4;
document.getElementById('MDMCFG1').innerHTML = `minimum ` + C_NumP[val] + ` preamble bytes to be transmitted configured in MDMCFG1 register`;
// 0x15
var DEVIATN = (C_DEVread(obj[21]) / 1000).toFixed(3); // ToDo
document.getElementById('DEVIATN').innerHTML = DEVIATN + ` kHz`;
document.getElementsByName('deviation')[0].value = DEVIATN;
document.getElementById('state').innerHTML = obj[obj.length - 2] + ' values readed &#10004;';
}
}

let stateCheck = setInterval(() => {
if (document.readyState === 'complete' && websocket.readyState == 1) {
websocket.send('detail');
clearInterval(stateCheck);
}
}, 50);

function validHEX(e) {
const pattern = /^[0-9a-fA-F]$/;
return pattern.test(e.key)
}

const a=` Output Pin Configuration`;
const b=`Packet Automation Control (`;
const c=`Frequency Synthesizer Control`;
const d=`Frequency Control Word, `;
const e=`Modem Configuration `;
const f=`Main Radio Control State Machine Configuration `;
const g=`AGC Control `;
const h=`Frequency Synthesizer Calibration `;
const i=`RC Oscillator Configuration `;
const j=`Various Test Settings `;

const C_Exp = [`GDO2${a}`,`GDO1${a}`,`GDO0${a}`,'RX FIFO and TX FIFO Thresholds','Sync Word, High Byte','Sync Word, Low Byte','Packet Length',`${b}Preamble quality, ...)`,`${b}Packet length config, ...)`,'Device Address','Channel Number',`${c}`,`${c}`,`${d}High Byte`,`${d}Middle Byte`,`${d}Low Byte`,`${e}4 (Datarate, Bandwidth)`,`${e}3 (Datarate)`,`${e}2 (Modulation, Sync_Mod, ...)`,`${e}1 (Num_Preamble, ...)`,`${e}0`,'Modem Deviation Setting',`${f}2`,`${f}1`,`${f}0`,'Frequency Offset Compensation Configuration','Bit Synchronization Configuration',`${g}2`,`${g}1`,`${g}0`,'High Byte Event0 Timeout','Low Byte Event0 Timeout','Wake On Radio Control','Front End RX Configuration','Front End TX Configuration',`${h}3`,`${h}2`,`${h}1`,`${h}0`,`${i}1`,`${i}0`,'Frequency Synthesizer Calibration Control','Production Test','AGC Test',`${j}2`,`${j}1`,`${j}0`];
const tabPre = '<table><tr><td>Bit No.</td><td>Name</td><td>Reset</td><td>R/W</td><td>Description</td></tr><tr><td>';
const tabEnd = '</td></tr></table>';

const C_Add = [
tabPre + '7</td><td>&nbsp;</td><td>0x00</td><td>R0</td><td>Not used</td></tr><tr><td>6</td><td>GDO2_INV</td><td>0x00</td><td>R/W</td><td>Invert output, i.e. select active low (1) / high (0)</td></tr><tr><td>5</td><td>GDO2_CFG[5:0]</td><td>0x29</td><td>R/W</td><td>Default is CHP_RDYn.' + tabEnd,
tabPre + '7</td><td>GDO_DS</td><td>0x00</td><td>R/W</td><td>Set high (1) or low (0) output drive strength on the GDO pins.</td></tr><tr><td>6</td><td>GDO1_INV</td><td>0x00</td><td>R/W</td><td>Invert output, i.e. select active low (1) / high (0)</td></tr><tr><td> 5:0</td><td>GDO1_CFG[5:0]</td><td>0x2e</td><td>R/W</td><td>Default is 3-state.' + tabEnd,
tabPre + '7</td><td>TEMP_SENSOR_ENABLE</td><td>0x00</td><td>R/W</td><td>Enable analog temperature sensor. Write 0 in all other register bits when using temperature sensor.</td></tr><tr><td>6</td><td>GDO0_INV</td><td>0x00</td><td>R/W</td><td>Invert output, i.e. select active low (1) / high (0)</td></tr><tr><td>5:0</td><td>GDO0_CFG[5:0]</td><td>0x3f</td><td>R/W</td><td>Default is CLK_XOSC/192. It is recommended to disable the clock output in initialization, in order to optimize RF performance.' + tabEnd,
tabPre + '7</td><td></td><td>0x00</td><td>R/W</td><td>Reserved , write 0 for compatibility with possible future extensions</td></tr><tr><td>6</td><td>ADC_RETENTION</td><td>0x00</td><td>R/W</td><td>...</td></tr><tr><td>5:4</td><td>CLOSE_IN_RX [1:0]</td><td>0x00</td><td>R/W</td><td>...</td></tr><tr><td>3:0</td><td>FIFO_THR[3:0]</td><td>0x07</td><td>R/W</td><td>Set the threshold for the TX FIFO and RX FIFO.<br>The threshold is exceeded when the number of bytes in the FIFO is equal to or higher than the threshold value. <table><tbody><tr><td>Setting </td><td>Bytes in TX FIFO</td><td>Bytes in RX FIFO</td></tr><tr><td>0 (0000)</td><td>61</td><td>4</td></tr><tr><td>1 (0001)</td><td>57</td><td>8</td></tr><tr><td>2 (0010)</td><td>53</td><td>12</td></tr><tr><td>3 (0011)</td><td>49</td><td>16</td></tr><tr><td>4 (0100)</td><td>45 </td><td>20</td></tr><tr><td>5 (0101)</td><td>41 </td><td>24 </td></tr><tr><td>6 (0110)</td><td>37</td><td>28</td></tr><tr><td>7 (0111)</td><td>33</td><td>32</td></tr><tr><td>8 (1000)</td><td>29</td><td>36</td></tr><tr><td>9 (1001)</td><td>25</td><td>40</td></tr><tr><td>10 (1010)</td><td>21</td><td>44</td></tr><tr><td>11 (1011)</td><td>17</td><td>48</td></tr><tr><td>12 (1100)</td><td>13</td><td>52</td></tr><tr><td>13 (1101)</td><td>9</td><td>56</td></tr><tr><td>14 (1110)</td><td>5</td><td>60</td></tr><tr><td>15 (1111)</td><td>1</td><td>64</td></tr></tbody></table>' + tabEnd,
'','','Indicates the packet length when fixed packet length mode is enabled.<br>If variable packet length mode is used, this value indicates the maximum packet length allowed.',
tabPre + '7:5</td><td>PQT[2:0]</td><td>0x00</td><td>R/W</td><td>Preamble quality estimator threshold. The preamble quality estimator increases an internal counter by one each time a bit is received that is different from the previous bit, and decreases the counter by 8 each time a bit is received that is the same as the last bit. A threshold of 4∙PQT for this counter is used to gate sync word detection. When PQT=0 a sync word is always accepted.</td></tr><tr><td>4</td><td></td><td>0x00</td><td>R0</td><td>Not Used.</td></tr><tr><td>3</td><td>CRC_AUTOFLUSH</td><td>0x00</td><td>R/W</td><td>Enable automatic flush of RX FIFO when CRC is not OK. This requires that only one packet is in the RXIFIFO and that packet length is limited to the RX FIFO size.</td></tr><tr><td>2</td><td>APPEND_STATUS</td><td>0x01</td><td>R/W</td><td>When enabled, two status bytes will be appended to the payload of the packet. The status bytes contain RSSI and LQI values, as well as CRC OK.</td></tr><tr><td>1:0</td><td>ADR_CHK[1:0]</td><td>0x00</td><td>R/W</td><td>Controls address check configuration of received packages.<br><br>Values:<br>0 : No address check<br>1 : Address check, no broadcast<br>2 : Address check and 0 (0x00) broadcast<br>3 : Address check and 0 (0x00) and 255 (0xFF) broadcast' + tabEnd,
tabPre + '7</td><td></td><td>0x00</td><td>R0</td><td>Not used</td></tr><tr><td>6</td><td>WHITE_DATA</td><td>0x01</td><td>R/W</td><td>Turn data whitening on / off.<br><br>Values:<br>0 : Whitening off<br>1 : Whitening on</td></tr><tr><td>5:4</td><td>PKT_FORMAT[1:0]</td><td>0x00</td><td>R/W</td><td>Format of RX and TX data<br><br>Values:<br>0 : Normal mode, use FIFOs for RX and TX<br>1 : Synchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins<br>2 : Random TX mode; sends random data using PN9 generator. Used for test. Works as normal mode, setting 0 (00), in RX<br>3 : Asynchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins</td></tr><tr><td>3</td> <td></td><td>0x00</td><td>R0</td><td>Not used</td></tr><tr><td>2</td><td>CRC_EN</td><td>0x01</td><td>R/W</td><td>Values:<br>1 : CRC calculation in TX and CRC check in RX enabled<br>0 : CRC disabled for TX and RX</td></tr><tr><td>1:0</td><td>LENGTH_CONFIG[1:0]</td><td>0x01</td><td>R/W</td><td>Configure the packet length<br><br>Values:<br>0 : Fixed packet length mode. Length configured in PKTLEN register<br>1 : Variable packet length mode. Packet length configured by the first byte after sync word<br>2: Infinite packet length mode<br>3: Reserved' + tabEnd,
'','','','','','','','','','',
tabPre + '7</td><td>FEC_EN</td><td>0x00</td><td>R/W</td><td>Enable Forward Error Correction (FEC) with interleaving for packet payload<br>(Only supported for fixed packet length mode, i.e. PKTCTRL0.LENGTH_CONFIG =0)<br><br>Values:<br>0 : Disable<br>1 : Enable</td></tr><tr><td>6:4</td><td>NUM_PREAMBLE[2:0]</td><td>0x02</td><td>R/W</td><td>Sets the minimum number of preamble bytes to be transmitted<br><br>Values:<br>0 : 2<br>1 : 3<br>2 : 4<br>3 : 6<br>4 : 8<br>5 : 12<br>6 : 16<br>7 : 24<br></td></tr><tr><td>3:2</td><td></td><td>0x00</td><td>R0</td><td>Not used</td></tr><tr><td>1:0</td><td>CHANSPC_E[1:0]</td><td>0x02</td><td>R/W</td><td>2 bit exponent of channel spacing' + tabEnd,
'','','','','','','','','','','','','','','','','','','','','','','','','','',''];

const C_Length = ['Fixed packet length. Length configured in PKTLEN register','Variable packet length. Length configured by the first byte after sync word','Infinite packet length','Reserved'];
const C_Mod = ['2-FSK', 'GFSK', '', 'ASK/OOK', '4-FSK', '', '', 'MSK'];
const C_NumP = [2, 3, 4, 6, 8, 12, 16, 24];
const C_Sync = ['No preamble/sync','15/16 sync word bits detected','16/16 sync word bits detected','30/32 sync word bits detected','No preamble/sync, carrier-sense above threshold','15/16 + carrier-sense above threshold','16/16 + carrier-sense above threshold','30/32 + carrier-sense above threshold'];
