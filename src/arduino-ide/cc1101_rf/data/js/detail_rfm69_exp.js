var js = document.createElement("script");
js.src = '/js/all.js';
document.head.appendChild(js);
var js2 = document.createElement("script");
js2.src = '/js/functions.js';
document.head.appendChild(js2);

const tab = '	';
const SX1231_RegAddrTrans = ['0x58','0x59','0x5F','0x6F','0x71'];
var obj;
var fileName;

let stateCheck = setInterval(() => {
  if (document.readyState === 'complete' && websocket.readyState == 1) {
    websocket.send('detail');
    clearInterval(stateCheck);  // document ready
  }
}, 50);

function WebSocket_MSG(event) {
 console.log('received message: ' + event.data);

 if(event.data.includes(',detail,')) {
  obj=event.data.split(',');
  fileName=obj[obj.length - 2].replaceAll(' ','_');

  let element = document.getElementById("REGs");
  var txt = "'0001','012E','022E',";

  // 0x38 RegPayloadLength
  var FIFO_THR = parseInt(obj[56], 16) / 4;
  FIFO_THR = parseInt(FIFO_THR);
  txt += "'034" + FIFO_THR.toString(16) + "',";

  // 0x02 RegDataModul | TODO
  var ModType = (parseInt(obj[2], 16) & 0b00011000) >> 3;
  // cc1101 MDMCFG2 (0x12)
  var r02 = (ModType === 0) ? "02" : "32";

  // 0x05 RegFdevMsb 0x06 RegFdevLsb | TODO
  var r05 = parseInt(obj[5], 16);
  var r06 = parseInt(obj[6], 16);
  var Fdev = ( Fstep_SX * (r06 + r05 * 256) ) / 1000;
  // cc1101 DEVIATN (0x15)
  if (Fdev > 380.859375) Fdev = 380.859375;
  if (Fdev < 1.586914) Fdev = 1.586914;
  Fdev*=2;

  var Fdev_val;
  var devlast = 0;
  var bitlast = 0;

  for (var DEVIATION_E = 0; DEVIATION_E < 8; DEVIATION_E++) {
    for (var DEVIATION_M = 0; DEVIATION_M < 8; DEVIATION_M++) {
      Fdev_val = (8 + DEVIATION_M) * (2 ** DEVIATION_E) * 26000.0 / (2 ** 17);
      var bits = DEVIATION_M + (DEVIATION_E << 4);
      if (Fdev > Fdev_val || ((Fdev_val - Fdev) < (Fdev - devlast))) {
        devlast = Fdev_val;
        bitlast = bits;
      }
    }
  }

  // 0x2F RegSyncValue1 0x30 RegSyncValue2 0x31 RegSyncValue2 ...
  const Sync = [obj[47], obj[48], obj[49]];
  var SyncCnt = 0;
  // cc1101 SYNC0 (0x05) | SYNC1 (0x04)
  for (let i = 0; i < Sync.length; i++) {
   if(Sync[i] !== 'AA') {
     SyncCnt ++;
     if(SyncCnt == 1) {
      txt += "'04" + Sync[i] + "',";
     } else if (SyncCnt == 2) {
      txt += "'05" + Sync[i] +"',";
     }
   }
  }

  // 0x38 RegPayloadLength
  txt += "'06" + obj[56] + "',";
  // non-existent on RFM
  txt += "'0780',";

  // 0x37 RegPacketConfig1
  var r37_7 = obj[55] >> 7;
  txt += "'080" + r37_7 + "',";

  // 0x07 RegFrfMsb 0x08 RegFrfMid 0x09 RegFrfLsb
  var r07to09 = SX_FREQread(obj[7], obj[8], obj[9]);
  // cc1101 FREQ2 (0x0D) | FREQ1 (0x0E) | FREQ0 (0x0F)
  var arr = C_FREQset(r07to09);
  txt += `'0D${arr[0]}','0E${arr[1]}','0F${arr[2]}',`;

  // 0x19 RegRxBw
  var RxBwMant = (parseInt(obj[25], 16) & 0b00011000) >> 3;
  var RxBwExp = (parseInt(obj[25], 16) & 0b00000111);
  var RxBw = (2 * (FXOSC_SX / ((16 + RxBwMant * 4) * (2 ** (RxBwExp + 2 + ModType)) ))).toFixed(3);
  // 0x03 RegBitrateMsb 0x04 RegBitrateLsb
  var rBitRate = (FXOSC_SX / ((parseInt(obj[3], 16) * 256) + parseInt(obj[4], 16)));
  arr = C_DRATEset(rBitRate,RxBw);
  txt += `'10${arr[0].toString(16)}','11${arr[1].toString(16)}','12${r02}','1322','14F8','15${bitlast.toString(16)}','1916','1B43','1C68'`;
  txt = txt.toUpperCase();
  element.innerText = txt;
 }
}

function saveData(variant){
 var fns;
 var txt;
 if (variant == 'SX') {
  txt = '#Type	Register Name	Address[Hex]	Value[Hex]\n';
  for (i=0; i<= 84; i++) {
   if (i>=80) {
    txt += 'REG	' + SX_RegN[i] + tab + SX1231_RegAddrTrans[i-80] + tab + '0x' + obj[i] + '\n';
   } else {
    txt += 'REG	' + SX_RegN[i] + tab + '0x' + dec2hex(i) + tab + '0x' + obj[i] + '\n';
   }
  }
  txt += 'PKT\tFalse;False;0;0;\n';
  fns = 'SX1231SKB_' + fileName + '.cfg';
 } else if (variant == 'h') {
  txt = 'const uint8_t ' + fileName + '[] PROGMEM = {\n';
  txt += '  // SX1231 register values for ' + obj[obj.length - 2] + '\n';
  for (i=0; i<= 84; i++) {
   if (i>=80) {
    txt += '  0x' + obj[i] + ', // ' + SX1231_RegAddrTrans[i-80] + ' - ' + SX_RegN[i] + '\n';
   } else {
    txt += '  0x' + obj[i] + ', // ' + '0x' + dec2hex(i) + ' - ' + SX_RegN[i] + '\n';
   }
  }
  txt += '}; // END SX1231 ' +  fileName + ' register values\n';
  fns = 'SX1231_' + fileName + '.h';
 } else if (variant == 'C') {
  txt = '<dcpanelconfiguration>\n';
  txt += '    <registersettings>\n';

  txt += '    </registersettings>\n';
  txt += '</dcpanelconfiguration>\n';
  fns = 'SmarfRF_' + fileName + '.xml';
 } else {
  alert('development');
  return;
 }

 saveFile(txt,fns); 
}
