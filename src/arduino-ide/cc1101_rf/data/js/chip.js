var js = document.createElement("script");
js.src = '/js/all.js';
document.head.appendChild(js);
const ts = document.title;

document.onreadystatechange = function () {
 if (document.readyState == 'complete') {
  const v=document.getElementById('cV').textContent;
  var c=v;
  if(ts=='cc1101_rf_Gateway'){
   if(v=='03'){
    c+=' = CC1100';
   }else if(v=='04'||v=='14'){
    c+=' = CC1101';
   }else if(v=='05'){
    c+=' = CC1100E';
   }else if(v=='07'||v=='17'){
    c+=' = CC110L';
   }else if(v=='08'||v=='18'){
    c+=' = CC113L';
   }else if(v=='15'){
    c+=' = CC115L';
   }
  } else if(ts=='rfm69_rf_Gateway') {
   if(v=='21'||v=='22'){
    c+=' = SX1231';
   }else if(v=='23'){
    c+=' = SX1231, RFM69W or RFM69CW';
   }else if(v=='24'){
    c+=' = SX1231H, RFM69HW or RFM69HCW';
   }
  }
  document.getElementById('cV').innerHTML=c;
 }
}

function WebSocket_MSG(event) {
 console.log('received message: ' + event.data);

 if(event.data == 'Connected'){
  websocket.send('chip');
 }else{
  const obj=event.data.split(',');
  if(ts == 'cc1101_rf_Gateway') {
   document.getElementById('MS').innerHTML = MARCSTATE[obj[1]];
  } else if (ts == 'rfm69_rf_Gateway') {
   document.getElementById('MS').innerHTML = RegOpMode[obj[1]];
  }
  document.getElementById('MODE').innerHTML = obj[0];
  document.getElementById('toB').innerHTML = obj[2];
  document.getElementById('Time').innerHTML = obj[3];
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

const RegOpMode = [
'00 = Sleep mode (SLEEP)',
'01 = Standby mode (STDBY)',
'02 = Frequency Synthesizer mode (FS)',
'03 = Transmitter mode (TX)',
'04 = Receiver mode (RX)'
];
