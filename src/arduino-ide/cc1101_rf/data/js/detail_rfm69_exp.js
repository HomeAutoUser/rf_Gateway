var js = document.createElement("script");
js.src = '/js/all.js';
document.head.appendChild(js);

let SX1231SKB_file;
const tab = '	';
const txt_hex = '0x';
const txt_ln = '\n';
var FileName;
const FXOSC = 32000000;
const Fstep = FXOSC / 524288;


function onMessage(event) {
 console.log('received message: ' + event.data);

 if(event.data == 'Connected') {
  websocket.send('detail');
 }

 if(event.data.includes(',detail,')) {
  const obj=event.data.split(',');

  SX1231SKB_file = '#Type	Register Name	Address[Hex]	Value[Hex]\n';

  for (i=0; i<= 84; i++) {
   if (i==80) {
    SX1231SKB_file += 'REG	' + RegName[i] + tab + '0x58' + tab + txt_hex + obj[i] + txt_ln;
   } else if (i==81) {
    SX1231SKB_file += 'REG	' + RegName[i] + tab + '0x59' + tab + txt_hex + obj[i] + txt_ln;
   } else if (i==82) {
    SX1231SKB_file += 'REG	' + RegName[i] + tab + '0x5F' + tab + txt_hex + obj[i] + txt_ln;
   } else if (i==83) {
    SX1231SKB_file += 'REG	' + RegName[i] + tab + '0x6F' + tab + txt_hex + obj[i] + txt_ln;
   } else if (i==84) {
    SX1231SKB_file += 'REG	' + RegName[i] + tab + '0x71' + tab + txt_hex + obj[i] + txt_ln;
   } else {
    SX1231SKB_file += 'REG	' + RegName[i] + tab + txt_hex + dec2hex(i) + tab + txt_hex + obj[i] + txt_ln;
   }
  }
  SX1231SKB_file += 'PKT	False;False;0;0;' + txt_ln;
  FileName = 'SX1231SKB_' + obj[obj.length - 2] + '.cfg';	   // The file to save the data.

  let element = document.getElementById("REGs");
  var txt = "";
  var txt_freq = "";

  // 0x07 RegFrfMsb 0x08 RegFrfMid 0x09 RegFrfLsb
  var txt_freq_val = parseInt(obj[7], 16) * 256;
  txt_freq_val = (txt_freq_val + parseInt(obj[8], 16) ) * 256;
  txt_freq_val = (txt_freq_val + parseInt(obj[9], 16) );
  txt_freq_val = (Fstep * txt_freq_val) / 1000000;
  // cc1101 FREQ2 (0x0D) | FREQ1 (0x0E) | FREQ0 (0x0F)
  txt_freq_val = (txt_freq_val * 1000) / 26000 * 65536;
  var freq0 = parseInt( txt_freq_val / 65536 );
  txt_freq += "'0D" + freq0.toString(16) +"',";
  var freq1 = parseInt((txt_freq_val % 65536) / 256 );
  txt_freq += "'0E" + freq1.toString(16) + "',";
  var freq2 = parseInt( txt_freq_val % 256 );
  txt_freq += "'0F" + freq2.toString(16) + "'";
  txt += txt_freq;
  txt += " -in development " + obj[obj.length - 2] + "- ";
  txt = txt.toUpperCase();

  element.textContent = txt;
 }
}

let saveFile = () => {
 // This variable stores all the data.
 let data = SX1231SKB_file;

 // Convert the text to BLOB.
 const textToBLOB = new Blob([data], { type: 'text/plain' });

 let newLink = document.createElement("a");
 newLink.download = FileName;

 if (window.webkitURL != null) {
  newLink.href = window.webkitURL.createObjectURL(textToBLOB);
 } else {
  newLink.href = window.URL.createObjectURL(textToBLOB);
  newLink.style.display = "none";
  document.body.appendChild(newLink);
 }

 newLink.click(); 
}

function dec2hex(i) {  // fix two places
 return (i+0x100).toString(16).substr(-2).toUpperCase();
}

const RegName = [
'RegFifo',
'RegOpMode',
'RegDataModul',
'RegBitrateMsb',
'RegBitrateLsb',
'RegFdevMsb',
'RegFdevLsb',
'RegFrfMsb',
'RegFrfMid',
'RegFrfLsb',
'RegOsc1',
'RegAfcCtrl',
'RegLowBat',
'RegListen1',
'RegListen2',
'RegListen3',
'RegVersion',
'RegPaLevel',
'RegPaRamp',
'RegOcp',
'Reserved14',
'Reserved15',
'Reserved16',
'Reserved17',
'RegLna',
'RegRxBw',
'RegAfcBw',
'RegOokPeak',
'RegOokAvg',
'RegOokFix',
'RegAfcFei',
'RegAfcMsb',
'RegAfcLsb',
'RegFeiMsb',
'RegFeiLsb',
'RegRssiConfig',
'RegRssiValue',
'RegDioMapping1',
'RegDioMapping2',
'RegIrqFlags1',
'RegIrqFlags2',
'RegRssiThresh',
'RegRxTimeout1',
'RegRxTimeout2',
'RegPreambleMsb',
'RegPreambleLsb',
'RegSyncConfig',
'RegSyncValue1',
'RegSyncValue2',
'RegSyncValue3',
'RegSyncValue4',
'RegSyncValue5',
'RegSyncValue6',
'RegSyncValue7',
'RegSyncValue8',
'RegPacketConfig1',
'RegPayloadLength',
'RegNodeAdrs',
'RegBroadcastAdrs',
'RegAutoModes',
'RegFifoThresh',
'RegPacketConfig2',
'RegAesKey1',
'RegAesKey2',
'RegAesKey3',
'RegAesKey4',
'RegAesKey5',
'RegAesKey6',
'RegAesKey7',
'RegAesKey8',
'RegAesKey9',
'RegAesKey10',
'RegAesKey11',
'RegAesKey12',
'RegAesKey13',
'RegAesKey14',
'RegAesKey15',
'RegAesKey16',
'RegTemp1',
'RegTemp2',
'RegTestLna',
'RegTestPa1',
'RegTestPa2',
'RegTestDagc',
'RegTestAfc'
];
