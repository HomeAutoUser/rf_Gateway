var js = document.createElement("script");
js.src = '/js/all.js';
document.head.appendChild(js);
const ts = document.title;

function onMessage(event) {
 console.log('received message: ' + event.data);
 if(event.data == 'Connected') {
  websocket.send('chip');
 }else{
  const obj=event.data.split(',');
  if(ts == 'cc1101_rf_Gateway') {
   document.getElementById("MS").innerHTML = MARCSTATE[obj[1]];
  } else if (ts == 'rfm69_rf_Gateway') {
   document.getElementById("MS").innerHTML = RegOpMode[obj[1]];
  }
  document.getElementById("MODE").innerHTML = obj[0];
  document.getElementById("ToggleBank").innerHTML = obj[2];
  document.getElementById("Time").innerHTML = obj[3];
 }
}

// CC110x //
const MARCSTATE = [
'00 = SLEEP',
'01 = IDLE',
'02 = XOFF',
'03 = VCOON_MC',
'04 = REGON_MC',
'05 = MANCAL',
'06 = VCOON',
'07 = REGON',
'08 = STARTCAL',
'09 = BWBOOST',
'0A = FS_LOCK',
'0B = IFADCON',
'0C = ENDCAL',
'0D = RX',
'0E = RX_END',
'0F = RX_RST',
'10 = TXRX_SWITCH',
'11 = RXFIFO_OVERFLOW',
'12 = FSTXON',
'13 = TX',
'14 = TX_END',
'15 = RXTX_SWITCH',
'16 = TXFIFO_UNDERFLOW'
];

// SX1231 //
const RegOpMode = [
'00 = Sleep mode (SLEEP)',
'01 = Standby mode (STDBY)',
'02 = Frequency Synthesizer mode (FS)',
'03 = Transmitter mode (TX)',
'04 = Receiver mode (RX)'
];
