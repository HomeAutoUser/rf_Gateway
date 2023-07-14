var js = document.createElement("script");
js.src = '/all.js';
document.head.appendChild(js);

function onMessage(event) {
 console.log('received message: ' + event.data);

 if(event.data == 'Connected') {
  websocket.send('cc110x');
 }

 if(event.data.includes(',{') && event.data.includes('},')) {
  const obj=event.data.split(',');
  document.getElementById("MS").innerHTML = MARCSTATE[obj[1]];
  document.getElementById("MODE").innerHTML = obj[0];
  document.getElementById("ToggleBank").innerHTML = obj[2].replace(/\s/g, '&emsp;');
  document.getElementById("Time").innerHTML = obj[3];
 }
}

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
