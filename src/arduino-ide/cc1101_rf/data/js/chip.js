var js = document.createElement("script");
js.src = '/js/all.js';
document.head.appendChild(js);
const ts = document.title;

document.onreadystatechange = function () {
 if (document.readyState == 'complete') {
  const v = document.getElementById('cV').textContent;
  let c = v;
  if (ts === 'cc1101_rf_Gateway') {
    switch (v) {
      case '03':
        c += ' = CC1100';
        break;
      case '04':
      case '14':
        c += ' = CC1101';
        break;
      case '05':
        c += ' = CC1100E';
        break;
      case '07':
      case '17':
        c += ' = CC110L';
        break;
      case '08':
      case '18':
        c += ' = CC113L';
        break;
      case '15':
        c += ' = CC115L';
        break;
    }
  } else if (ts === 'rfm69_rf_Gateway') {
    switch (v) {
      case '21':
      case '22':
        c += ' = SX1231';
        break;
      case '23':
        c += ' = SX1231, RFM69W or RFM69CW';
        break;
      case '24':
        c += ' = SX1231H, RFM69HW or RFM69HCW';
        break;
    }
  }
  document.getElementById('cV').textContent=c;
 }
}

function WebSocket_MSG(event) {
 console.log('received message: ' + event.data);

 if(event.data == 'Connected'){
  websocket.send('chip');
 }else{
  const obj=event.data.split(',');
  if(ts === 'cc1101_rf_Gateway') {
   document.getElementById('MS').innerHTML = C_MS[obj[1]];
  } else if (ts === 'rfm69_rf_Gateway') {
   document.getElementById('MS').innerHTML = SX_Mode[obj[1]];
  }
  document.getElementById('MODE').innerHTML = obj[0];
  document.getElementById('toB').innerHTML = obj[2];
  document.getElementById('Time').innerHTML = obj[3];
 }
}

const C_MS = [
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

const SX_Mode = [
'00 = Sleep mode (SLEEP)',
'01 = Standby mode (STDBY)',
'02 = Frequency Synthesizer mode (FS)',
'03 = Transmitter mode (TX)',
'04 = Receiver mode (RX)'
];
