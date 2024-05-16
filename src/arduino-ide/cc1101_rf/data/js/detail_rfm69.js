var js = document.createElement("script");
js.src = '/js/all.js';
document.head.appendChild(js);
var js2 = document.createElement("script");
js2.src = '/js/functions.js';
document.head.appendChild(js2);

var store = document.querySelector(':root');
var value = getComputedStyle(store);
var color2 = value.getPropertyValue('--val_diff');
var mod_list = false;
var bw_list = false;
const SX1231_RegAddrTrans = ['58','59','5F','6F','71'];

document.onreadystatechange = function () {
 if (document.readyState == 'complete') {
  let obj = document.getElementById('p1');
  obj.maxLength = 7;
  obj.pattern = "^[\\d]{3}(\\.[\\d]{1,3})?$";
  obj = document.getElementById('p2');
  obj.maxLength = 6;
  obj.pattern = "^-?[\\d]{1,3}(\\.[\\d]{1,3})?$";
  obj = document.getElementById('p4');
  obj.maxLength = 7;
  obj.pattern = "^[\\d]{1,4}(\\.[\\d]{1,3})?$";
  obj = document.getElementById('p5');
  obj.maxLength = 7;
  obj.pattern = "^[\\d]{1,3}(\\.[\\d]{1,3})?$";

  let l = SX_Exp1.length;
  for (let i=0; i<l; i++ ) {
   let txt;
   let hex = dec2hex(i);
   if (i>=80) {
    hex = SX1231_RegAddrTrans[i-80];
   }

   let element = document.getElementsByName('r' + i)[0];
   element.maxLength=2;
   element.onkeypress=validHEX;
   element.size=2;

   txt = '0x' + hex + '&ensp;access ';
   let el = document.getElementById('s' + i);
   if(SX_Exp2[i] != '') {
    el.innerHTML = txt + SX_Exp2[i];
    element.disabled = true;
   } else {
    el.innerHTML = txt + '(rw)';
   }

   if(element.disabled == false) {
    element.title = `Input: two hex characters`;
   } else {
    element.title = `read only`;
   }

   document.getElementById('n' + i).innerHTML = SX_Exp1[i];
   var id = 't' + i;

   if (SX_Add[i] != '') {
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
    p.innerHTML = SX_Exp2[i];

    div2.appendChild(p);
    div2.insertAdjacentHTML( 'beforeend', SX_Add[i] );
   } else {
    document.getElementById(id).innerHTML = '';
   }
  }
 }
};

function WebSocket_MSG(event) {
 console.log(`received message: ` + event.data);

 if(event.data.includes(',detail,')) {
  const obj=event.data.split(',');
  var REGISTER_MAX = 84;

  for (let i=0; i<= REGISTER_MAX; i++) {
   let element = document.getElementsByName('r' + i)[0];
   element.placeholder = element.value.replace(element.value, obj[i]);
   if (element.value !== obj[i]) {
    element.value = element.value.replace(element.value, obj[i]);
    element.placeholder = obj[i];
    element.style.color = color2;
   }
  }

  var ModType = (parseInt(obj[2], 16) & 0b00011000) >> 3;
  // 0x02 RegDataModul
  if (!mod_list) {
   mod_list = true;
   var selectElement = document.getElementById('modulation');
   selectElement.add(new Option(SX_Mod[0]));
   selectElement.add(new Option(SX_Mod[1]));

   document.getElementById('MOD_FORMAT').innerHTML = SX_Mod[ModType];
   document.getElementById('modulation').value = SX_Mod[ModType];
  }
  // 0x03 RegBitrateMsb 0x04 RegBitrateLsb
  var DRATE = SX_DRATEread(obj[3],obj[4])  / 1000;	// ToDo
  document.getElementById('DRATE').innerHTML = DRATE.toFixed(3) + ` kBaud`;
  document.getElementsByName('datarate')[0].value = DRATE.toFixed(3);
  // 0x05 RegFdevMsb 0x06 RegFdevLsb
  var Fdev = SX_DEVread(obj[5], obj[6]) / 1000;	// ToDo
  document.getElementById('DEVIATN').innerHTML = Fdev.toFixed(3) + ` kHz`;
  document.getElementsByName('deviation')[0].value = Fdev.toFixed(3);
  // 0x07 RegFrfMsb 0x08 RegFrfMid 0x09 RegFrfLsb
  var Freq = SX_FREQread(obj[7], obj[8], obj[9]) / 1000000;	// ToDO
  document.getElementById('FREQis').innerHTML = Freq.toFixed(3);
  Freq -= obj[REGISTER_MAX + 3] * 1;
  document.getElementById('FREQ').innerHTML = Freq.toFixed(3);
  document.getElementsByName('freq')[0].value = Freq.toFixed(3);
  // 0x19 RegRxBw
  var RxBwComp = (SX_BWread(obj[25], obj[2]) / 1000).toFixed(3);	// ToDo
  selectElement = document.getElementById('bandw');
  if(!bw_list) {
   bw_list = true;
   let size = SX_BWsteps().length;
   for (let i = 0; i < size; i++) {
    selectElement.add( new Option( (SX_BWsteps(obj[2])[i] / 1000).toFixed(3) ) ); // ToDo
   }
  }
  selectElement.value = RxBwComp;
  document.getElementById('CHANBW').innerHTML = RxBwComp + ` kHz`;

  // 0x2c RegPreambleMsb 0x2d RegPreambleLsb 
  var PreambleSize = (parseInt(obj[44], 16) * 256) + parseInt(obj[45], 16);
  document.getElementById('MDMCFG1').innerHTML = `minimum ` + PreambleSize + ` preamble bytes to be transmitted`;
  // 0x2e RegSyncConfig
  var SyncSize = ((parseInt(obj[46], 16) & 0b00111000) >> 3) + 1;
  var SyncTol = (parseInt(obj[46], 16) & 0b00000111);
  document.getElementById('SYNC_MODE').innerHTML = `Sync word size ` + SyncSize + ` with ` + SyncTol + ` tolerated bit errors`;
  // 0x37 RegPacketConfig1 0x38 RegPayloadLength
  var PacketFormat = (parseInt(obj[55], 16) & 0b10000000) >> 7;
  var PayloadLength = parseInt(obj[56], 16);
  document.getElementById('PKTCTRL0').innerHTML = SX_PacketC[PacketFormat] + ` with PayloadLength ` + PayloadLength;

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
 return pattern.test(e.key);
}

const A=`RegAesKey`;
const k=`of cipher key)`;
const S=`RegSyncValue`;
const w=`st byte of Sync word)`;
const SX_Exp1 = ['RegFifo (FIFO read/write access)','RegOpMode (Operating modes transceiver)','RegDataModul (DataMode, Modulation)','RegBitrateMsb (BitRate)','RegBitrateLsb (BitRate)','RegFdevMsb (Deviation)','RegFdevLsb (Deviation)','RegFrfMsb (Frequency)','RegFrfMid (Frequency)','RegFrfLsb (Frequency)','RegOsc1 (Oscillators Settings)','RegAfcCtrl (AFC control)','RegLowBat','RegListen1 (RssiThreshold, ...)','RegListen2','RegListen3','RegVersion','RegPaLevel','RegPaRamp (Control PA ramp time in FSK mod)','RegOcp','Reserved14','Reserved15','Reserved16','Reserved17','RegLna','RegRxBw (Bandwidth -single side-)','RegAfcBw','RegOokPeak (OOK demodulator - selection and control in peak mode)','RegOokAvg (OOK demodulator - average threshold control)','RegOokFix (OOK demodulator - fixed threshold control)','RegAfcFei','RegAfcMsb (MSB of the frequency correction of the AFC)','RegAfcLsb (LSB of the frequency correction of the AFC)','RegFeiMsb (MSB of the calculated frequency error)','RegFeiLsb (LSB of the calculated frequency error)','RegRssiConfig','RegRssiValue (RSSI in dBm)','RegDioMapping1','RegDioMapping2','RegIrqFlags1 (Status register)','RegIrqFlags2 (Status register)','RegRssiThresh (RSSI trigger level)','RegRxTimeout1','RegRxTimeout2','RegPreambleMsb (Preamble length, MSB)','RegPreambleLsb (Preamble length, LSB)','RegSyncConfig (Sync word, FIFO condition)',
`${S}1 (1${w}`,`${S}2 (2${w}`,`${S}3 (3${w}`,`${S}4 (4${w}`,`${S}5 (5${w}`,`${S}6 (6${w}`,`${S}7 (7${w}`,`${S}8 (8${w}`,
'RegPacketConfig1 (PacketFormat, CRC, ...)','RegPayloadLength (Payload length / Message length)','RegNodeAdrs','RegBroadcastAdrs','RegAutoModes (IntermediateMode)','RegFifoThresh (Fifo threshold, TX start condition)','RegPacketConfig2 (Packet mode settings)',
`${A}1 (byte 1 ${k}`,`${A}2 (byte 2 ${k}`,`${A}3 (byte 3 ${k}`,`${A}4 (byte 4 ${k}`,`${A}5 (byte 5 ${k}`,`${A}6 (byte 6 ${k}`,`${A}7 (byte 7 ${k}`,
`${A}8 (byte 8 ${k}`,`${A}9 (byte 9 ${k}`,`${A}10 (byte 10 ${k}`,`${A}11 (byte 11 ${k}`,`${A}12 (byte 12 ${k}`,`${A}13 (byte 13 ${k}`,
`${A}14 (byte 14 ${k}`,`${A}15 (byte 15 ${k}`,`${A}16 (byte 16 ${k}`,
'RegTemp1 (Temperature Sensor control)','RegTemp2 (Temperature readout)','RegTestLna (Sensitivity boost)','RegTestTcxo','RegTestllBw (PLL Bandwidth setting)','RegTestDagc','RegTestAfc'];

const SX_Exp2 = ['(nothing)','','','','','','','','','','','','','','','','(r)','','','','(r)','(r)','(r)','(r)','','','','','','','','(r)','(r)','(r)','(r)','','(r)','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','(r)','','','','',''];
const SX_Add = ['','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','',];
const SX_Mod = ['FSK', 'OOK', 'reserved', 'reserved'];
const SX_PacketC = ['fixed', 'variable'];
