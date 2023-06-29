/*
  JavaScript for website cc110x / CC110x - cc110x_rf_Gateway
  Copyright (c) 2022 <HomeAutoUser & elektron-bbs>
  URL: https://github.com/HomeAutoUser/cc1101_rf_Gateway
*/

var js = document.createElement("script");
js.src = '/all.js';
document.head.appendChild(js);

// Show the received message 
function onMessage(event) {
  console.log('received message: ' + event.data);

  if(event.data == 'Connected') {
    websocket.send('cc110x');
  }

  if(event.data.includes('MS') ) {
    var obj = JSON.parse(event.data);
    document.getElementById("MS").innerHTML = MARCSTATE[obj.MS];
    document.getElementById("MODE").innerHTML = obj.MODE;
    document.getElementById("ToggleBank").innerHTML = obj.ToggleBank.replace(/\s/g, '&emsp;');
    document.getElementById("Time").innerHTML = obj.Time;
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