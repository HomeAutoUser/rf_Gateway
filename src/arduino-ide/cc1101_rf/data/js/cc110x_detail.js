var js = document.createElement("script");
js.src = '/all.js';
document.head.appendChild(js);

var store = document.querySelector(':root');
var value = getComputedStyle(store);
var color1 = value.getPropertyValue('--val_std');
var color2 = value.getPropertyValue('--val_diff');
var status_html = false;
var status_websocket = false;
var mod_list = false;

function onMessage(event) {
 console.log('received message: ' + event.data);

 if(event.data == 'Connected') {
  status_websocket = 1;
  check();
 }

 if(event.data.includes(',detail,') ) {
  const obj=event.data.split(',');

 var name;
 var name_s;
 var hex;
 var Freq;
 var DRATE1;
 var DRATE2;

 for (i=0; i<= 46; i++) {
  name = 'r' + i;
  name_s = 's' + i;
  if (i <= 15) {
   hex = '0' + i.toString(16)
  } else {
   hex = i.toString(16)
  }
  document.getElementById(name_s).innerHTML = '0x' + hex.toUpperCase() + '&ensp;' + regExplanation_short[i];
  document.getElementsByName(name)[0].pattern = "^[\\da-fA-F]{1,2}$";
  document.getElementsByName(name)[0].maxLength = "2";
  if (document.getElementsByName(name)[0].value != obj[i]) {
   document.getElementsByName(name)[0].value = document.getElementsByName(name)[0].value.replace(document.getElementsByName(name)[0].value, obj[i]);
   document.getElementsByName(name)[0].placeholder = obj[i];
   document.getElementsByName(name)[0].style.color = color2;
  } else {
   document.getElementsByName(name)[0].style.color = color1;
  }
  if (i == 8) {
   var val = ( parseInt(obj[i], 16) & "00000011" ) >> 4;
   document.getElementById('PKTCTRL0').innerHTML = LENGTH_CONFIG[val];
  }
  if (i == 12) {
   if(document.getElementsByName('afc')[0].checked) {
   var off = ( parseInt(obj[i], 16) );
   if (off > 127) {
    off -= 255;
   }
   off = (26000000 / 16384 * off / 1000);
   document.getElementById('n' + i).innerHTML = regExplanation[i] + ' (Freq. offset ' + off.toFixed(0) + ' kHz)';
   }
  }
  if (i == 13 || i == 14 || i == 15) {
   if(i == 13){
   Freq = parseInt(obj[i], 16) * 256;
   }
   if(i == 14){
   Freq = (Freq + parseInt(obj[i], 16) ) * 256;
   }
   if(i == 15){
   Freq = (Freq + parseInt(obj[i], 16) );
   Freq = (26 * Freq) / 65536;
   document.getElementById('FREQis').innerHTML = Freq.toFixed(3);
   document.getElementById('p2').pattern = "^-?[\\d]{1,3}(\\.[\\d]{1,3})?$";
   Freq = Freq - (obj[49] * 1);
   document.getElementById('FREQ').innerHTML = Freq.toFixed(3);
   document.getElementById('p1').pattern = "^[\\d]{3}(\\.[\\d]{1,3})?$";
   document.getElementsByName('freq')[0].value = Freq.toFixed(3);
   }
  }
  if (i == 16 || i == 17) {
   if(i == 16) {
   var CHANBW;
   var CHANBW1 = ( parseInt(obj[i], 16) ) >> 4;
   CHANBW1 = 4 + ( CHANBW1 & 3 );
   var CHANBW2 = ( parseInt(obj[i], 16) ) >> 6;
   CHANBW2 = ( CHANBW2 & 3 );
   CHANBW2 = 1 << CHANBW2;
   CHANBW = 26000.0 / (8.0 * CHANBW1 * CHANBW2);
   document.getElementById('CHANBW').innerHTML = CHANBW.toFixed(1) + ' kHz';
   document.getElementsByName('bandw')[0].value = CHANBW.toFixed(1);
   document.getElementById('p3').pattern = "^[\\d]{2,3}(\\.[\\d]{1,2})?$";
   DRATE1 = parseInt(obj[i], 16);
   }
   if(i == 17) {
   var DRATE;
   DRATE2 = parseInt(obj[i], 16);
   DRATE = ( ( (256 + DRATE2) * (2 ** (DRATE1 & 15)) ) * 26000000.0 / (2 ** 28) / 1000.0 );
   document.getElementById('DRATE').innerHTML = DRATE.toFixed(2) + ' kBaud';
   document.getElementsByName('datarate')[0].value = DRATE.toFixed(2);
   document.getElementById('p4').pattern = "^[\\d]{1,4}(\\.[\\d]{1,2})?$";
   }
  }
  if (i == 18) {
   if (!mod_list) {
   mod_list = true;
   var selectElement = document.getElementById('modulation');
   for (var j = 0; j<=7; j++) {
    if (MOD_FORMAT[j] != '') {
    selectElement.add(new Option(MOD_FORMAT[j]));
    }
   }
   }
   var val = (parseInt(obj[i], 16) & "01110000") >> 4;
   document.getElementById('MOD_FORMAT').innerHTML = MOD_FORMAT[val];
   document.getElementById('modulation').value = MOD_FORMAT[val];
   var val = (parseInt(obj[i], 16) & "00000111") >> 4;
   document.getElementById('SYNC_MODE').innerHTML = SYNC_MODE[val];
  }
  if (i == 21) {
   var DEVIATN = parseInt(obj[i], 16);
   DEVIATN = (8 + (DEVIATN & 7)) * ( 2 ** ((DEVIATN >> 4) & 7) ) * 26000.0 / ( 2 ** 17);
   document.getElementById('DEVIATN').innerHTML = DEVIATN.toFixed(2) + ' kHz';
   document.getElementsByName('deviation')[0].value = DEVIATN.toFixed(2);
   document.getElementById('p5').pattern = "^[\\d]{1,3}(\\.[\\d]{1,2})?$";
  }
  }
  if (document.getElementById('state')) {
  document.getElementById('state').innerHTML = obj[48] + ' values readed &#10004;';
  }
 }
}

document.onreadystatechange = function () {
 if (document.readyState == 'complete') {
 for ( i=0; i<regExplanation.length; i++ ) {
  document.getElementById('n' + i).innerHTML = regExplanation[i];
 }
 status_html = 1;
 check();
 }
}

function check(){
 if (status_html && status_websocket) {
 websocket.send('cc110x_detail');
 }
}

const regExplanation = [
'GDO2 Output Pin Configuration',
'GDO1 Output Pin Configuration',
'GDO0 Output Pin Configuration',
'RX FIFO and TX FIFO Thresholds',
'Sync Word, High Byte',
'Sync Word, Low Byte',
'Packet Length',
'Packet Automation Control (Preamble quality, ...)',
'Packet Automation Control (Packet length config, ...)',
'Device Address',
'Channel Number',
'Frequency Synthesizer Control',
'Frequency Synthesizer Control',
'Frequency Control Word, High Byte',
'Frequency Control Word, Middle Byte',
'Frequency Control Word, Low Byte',
'Modem Configuration 4 (Datarate, Bandwidth)',
'Modem Configuration 3 (Datarate)',
'Modem Configuration 2 (Modulation,Sync_Mod, ...)',
'Modem Configuration 1 (Num_Preamble, ...)',
'Modem Configuration 0',
'Modem Deviation Setting',
'Main Radio Control State Machine Configuration 2',
'Main Radio Control State Machine Configuration 1',
'Main Radio Control State Machine Configuration 0',
'Frequency Offset Compensation Configuration',
'Bit Synchronization Configuration',
'AGC Control 2',
'AGC Control 1',
'AGC Control 0',
'High Byte Event0 Timeout',
'Low Byte Event0 Timeout',
'Wake On Radio Control',
'Front End RX Configuration',
'Front End TX Configuration',
'Frequency Synthesizer Calibration 3',
'Frequency Synthesizer Calibration 2',
'Frequency Synthesizer Calibration 1',
'Frequency Synthesizer Calibration 0',
'RC Oscillator Configuration 1',
'RC Oscillator Configuration 0',
'Frequency Synthesizer Calibration Control',
'Production Test',
'AGC Test',
'Various Test Settings 2',
'Various Test Settings 1',
'Various Test Settings 0'
];

const regExplanation_short = [
'IOCFG2',
'IOCFG1',
'IOCFG0',
'FIFOTHR',
'SYNC1',
'SYNC0',
'PKTLEN',
'PKTCTRL1',
'PKTCTRL0',
'ADDR',
'CHANNR',
'FSCTRL1',
'FSCTRL0',
'FREQ2',
'FREQ1',
'FREQ0',
'MDMCFG4',
'MDMCFG3',
'MDMCFG2',
'MDMCFG1',
'MDMCFG0',
'DEVIATN',
'MCSM2',
'MCSM1',
'MCSM0',
'FOCCFG',
'BSCFG',
'AGCCTRL2',
'AGCCTRL1',
'AGCCTRL0',
'WOREVT1',
'WOREVT0',
'WORCTRL',
'FREND1',
'FREND0',
'FSCAL3',
'FSCAL2',
'FSCAL1',
'FSCAL0',
'RCCTRL1',
'RCCTRL0',
'FSTEST',
'PTEST',
'AGCTEST',
'TEST2',
'TEST1',
'TEST0',
];

const LENGTH_CONFIG = [
'Fixed packet length. Length configured in PKTLEN register',
'Variable packet length. Length configured by the first byte after sync word',
'Infinite packet length',
'Reserved'
];

const MOD_FORMAT = ['2-FSK', 'GFSK', '', 'ASK/OOK', '4-FSK', '', '', 'MSK'];

const SYNC_MODE = [
'No preamble/sync',
'15/16 sync word bits detected',
'16/16 sync word bits detected',
'30/32 sync word bits detected',
'No preamble/sync, carrier-sense above threshold',
'15/16 + carrier-sense above threshold',
'16/16 + carrier-sense above threshold',
'30/32 + carrier-sense above threshold'
];
