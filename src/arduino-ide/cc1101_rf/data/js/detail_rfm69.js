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
const SX1231_RegAddrTrans = ['58','59','5F','6F','71'];

document.onreadystatechange = function () {
 if (document.readyState == 'complete') {
  document.getElementById('p1').maxLength = 7;
  document.getElementById('p1').pattern = "^[\\d]{3}(\\.[\\d]{1,3})?$";
  document.getElementById('p2').maxLength = 6;
  document.getElementById('p2').pattern = "^-?[\\d]{1,3}(\\.[\\d]{1,3})?$";
  document.getElementById('p4').maxLength = 7;
  document.getElementById('p4').pattern = "^[\\d]{1,4}(\\.[\\d]{1,3})?$";
  document.getElementById('p5').maxLength = 7;
  document.getElementById('p5').pattern = "^[\\d]{1,3}(\\.[\\d]{1,3})?$";

  var hex;
  var txt;

  for ( i=0; i<Explan.length; i++ ) {
   if (i <= 15) {
    hex = '0' + i.toString(16)
   } else {
    hex = i.toString(16)
   }
   if (i>=80) {
    hex = SX1231_RegAddrTrans[i-80];
   }

   let element = document.getElementsByName('r' + i)[0];
   element.maxLength = 2;
   element.onkeypress = validHEX;
   element.setAttribute('size', '2');

   txt = '0x' + hex.toUpperCase() + '&ensp;access ';
   if(Explan_short[i] != '') {
    document.getElementById('s' + i).innerHTML = txt + Explan_short[i];
    element.disabled = true;
   } else {
    document.getElementById('s' + i).innerHTML = txt + '(rw)';
   }

   if(element.disabled == false) {
    element.title = "Input: two hex characters";
   } else {
    element.title = "read only";
   }
   
   document.getElementById('n' + i).innerHTML = Explan[i];
   var id = 't' + i;

   if (ExplanAdd[i] != '') {
    /* <!-- The Modal -->     https://www.w3schools.com/howto/tryit.asp?filename=tryhow_css_modal */
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
    p.innerHTML = Explan_short[i];

    div2.appendChild(p);
    div2.insertAdjacentHTML( 'beforeend', ExplanAdd[i] );
   } else {
    document.getElementById(id).innerHTML = '';
   }
  }
 }
}

function WebSocket_MSG(event) {
 console.log('received message: ' + event.data);

 if(event.data.includes(',detail,')) {
  const obj=event.data.split(',');
  var REGISTER_MAX = 84;

  for (i=0; i<= REGISTER_MAX; i++) {
   let element = document.getElementsByName('r' + i)[0];
   element.placeholder = element.value.replace(element.value, obj[i]);
   if (element.value != obj[i]) {
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
   selectElement.add(new Option(MOD[0]));
   selectElement.add(new Option(MOD[1]));

   document.getElementById('MOD_FORMAT').innerHTML = MOD[ModType];
   document.getElementById('modulation').value = MOD[ModType];
  }

  // 0x03 RegBitrateMsb 0x04 RegBitrateLsb
	var DRATE = SX_DRATEread(obj[3],obj[4])  / 1000;	// ToDo
  document.getElementById('DRATE').innerHTML = DRATE.toFixed(3) + ' kBaud';
  document.getElementsByName('datarate')[0].value = DRATE.toFixed(3);

  // 0x05 RegFdevMsb 0x06 RegFdevLsb
	var Fdev = SX_DEVread(obj[5], obj[6]) / 1000;	// ToDo
  document.getElementById('DEVIATN').innerHTML = Fdev.toFixed(3) + ' kHz';
  document.getElementsByName('deviation')[0].value = Fdev.toFixed(3);

  // 0x07 RegFrfMsb 0x08 RegFrfMid 0x09 RegFrfLsb
	var Freq = SX_FREQread(obj[7], obj[8], obj[9]) / 1000000;	// ToDO
  document.getElementById('FREQis').innerHTML = Freq.toFixed(3);
  Freq = Freq - (obj[REGISTER_MAX + 3] * 1);
  document.getElementById('FREQ').innerHTML = Freq.toFixed(3);
  document.getElementsByName('freq')[0].value = Freq.toFixed(3);

  // 0x19 RegRxBw
	var RxBwComp = (SX_BWread(obj[25], obj[2]) / 1000).toFixed(3);	// ToDo
  var selectElement = document.getElementById('bandw');
  if(!bw_list) {
   bw_list = true;
   for (let i = 0; i < SX_BWsteps().length; i++) {
    selectElement.add( new Option( (SX_BWsteps(obj[2])[i] / 1000).toFixed(3) ) ); // ToDo
   }
  }
  selectElement.value = RxBwComp;
  document.getElementById('CHANBW').innerHTML = RxBwComp + ' kHz';

  // 0x2c RegPreambleMsb 0x2d RegPreambleLsb 
  var PreambleSize = (parseInt(obj[44], 16) * 256) + parseInt(obj[45], 16);
  document.getElementById('MDMCFG1').innerHTML = 'minimum ' + PreambleSize + ' preamble bytes to be transmitted';

  // 0x2e RegSyncConfig
  var SyncSize = ((parseInt(obj[46], 16) & 0b00111000) >> 3) + 1;
  var SyncTol = (parseInt(obj[46], 16) & 0b00000111);
  document.getElementById('SYNC_MODE').innerHTML = 'Sync word size ' + SyncSize + ' with ' + SyncTol + ' tolerated bit errors';

  // 0x37 RegPacketConfig1 0x38 RegPayloadLength
  var PacketFormat = (parseInt(obj[55], 16) & 0b10000000) >> 7;
  var PayloadLength = parseInt(obj[56], 16);
  document.getElementById('PKTCTRL0').innerHTML = RegPacketConfig1[PacketFormat] +' with PayloadLength ' + PayloadLength;

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

const Explan = [
'RegFifo (FIFO read/write access)',
'RegOpMode (Operating modes transceiver)',
'RegDataModul (DataMode, Modulation)',
'RegBitrateMsb (BitRate)',
'RegBitrateLsb (BitRate)',
'RegFdevMsb (Deviation)',
'RegFdevLsb (Deviation)',
'RegFrfMsb (Frequency)',
'RegFrfMid (Frequency)',
'RegFrfLsb (Frequency)',
'RegOsc1 (Oscillators Settings)',
'RegAfcCtrl (AFC control)',
'RegLowBat',
'RegListen1 (RssiThreshold, ...)',
'RegListen2',
'RegListen3',
'RegVersion',
'RegPaLevel',
'RegPaRamp (Control PA ramp time in FSK mod)',
'RegOcp',
'Reserved14',
'Reserved15',
'Reserved16',
'Reserved17',
'RegLna',
'RegRxBw (Bandwidth -single side-)',
'RegAfcBw',
'RegOokPeak (OOK demodulator - selection and control in peak mode)',
'RegOokAvg (OOK demodulator - average threshold control)',
'RegOokFix (OOK demodulator - fixed threshold control)',
'RegAfcFei',
'RegAfcMsb (MSB of the frequency correction of the AFC)',
'RegAfcLsb (LSB of the frequency correction of the AFC)',
'RegFeiMsb (MSB of the calculated frequency error)',
'RegFeiLsb (LSB of the calculated frequency error)',
'RegRssiConfig',
'RegRssiValue (RSSI in dBm)',
'RegDioMapping1',
'RegDioMapping2',
'RegIrqFlags1 (Status register)',
'RegIrqFlags2 (Status register)',
'RegRssiThresh (RSSI trigger level)',
'RegRxTimeout1',
'RegRxTimeout2',
'RegPreambleMsb (Preamble length, MSB)',
'RegPreambleLsb (Preamble length, LSB)',
'RegSyncConfig (Sync word, FIFO condition)',
'RegSyncValue1 (1st byte of Sync word)',
'RegSyncValue2 (2st byte of Sync word)',
'RegSyncValue3 (3st byte of Sync word)',
'RegSyncValue4 (4st byte of Sync word)',
'RegSyncValue5 (5st byte of Sync word)',
'RegSyncValue6 (6st byte of Sync word)',
'RegSyncValue7 (7st byte of Sync word)',
'RegSyncValue8 (8st byte of Sync word)',
'RegPacketConfig1 (PacketFormat, CRC, ...)',
'RegPayloadLength (Payload length / Message length)',
'RegNodeAdrs',
'RegBroadcastAdrs',
'RegAutoModes (IntermediateMode)',
'RegFifoThresh (Fifo threshold, TX start condition)',
'RegPacketConfig2 (Packet mode settings)',
'RegAesKey1 (byte 1 of cipher key)',
'RegAesKey2 (byte 2 of cipher key)',
'RegAesKey3 (byte 3 of cipher key)',
'RegAesKey4 (byte 4 of cipher key)',
'RegAesKey5 (byte 5 of cipher key)',
'RegAesKey6 (byte 6 of cipher key)',
'RegAesKey7 (byte 7 of cipher key)',
'RegAesKey8 (byte 8 of cipher key)',
'RegAesKey9 (byte 9 of cipher key)',
'RegAesKey10 (byte 10 of cipher key)',
'RegAesKey11 (byte 11 of cipher key)',
'RegAesKey12 (byte 12 of cipher key)',
'RegAesKey13 (byte 13 of cipher key)',
'RegAesKey14 (byte 14 of cipher key)',
'RegAesKey15 (byte 15 of cipher key)',
'RegAesKey16 (byte 16 of cipher key)',
'RegTemp1 (Temperature Sensor control)',
'RegTemp2 (Temperature readout)',
'RegTestLna (Sensitivity boost)',
'RegTestTcxo',
'RegTestllBw (PLL Bandwidth setting)',
'RegTestDagc',
'RegTestAfc',
];

const Explan_short = [
'(nothing)',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'(r)',
'',
'',
'',
'(r)',
'(r)',
'(r)',
'(r)',
'',
'',
'',
'',
'',
'',
'',
'(r)',
'(r)',
'(r)',
'(r)',
'',
'(r)',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'(r)',
'',
'',
'',
'',
''
];

const ExplanAdd = [
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
];

const MOD = ['FSK', 'OOK', 'reserved', 'reserved'];
const RegPacketConfig1 = ['fixed', 'variable'];
