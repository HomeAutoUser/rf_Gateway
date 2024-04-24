var js = document.createElement("script");
js.src = '/js/all.js';
document.head.appendChild(js);
var js2 = document.createElement("script");
js2.src = '/js/functions.js';
document.head.appendChild(js2);

let SX1231SKB_file;
let CC110x_file;
const tab = '	';
var FileSX1231;
var FileCC110x;
const SX1231_RegAddrTrans = ['0x58','0x59','0x5F','0x6F','0x71'];

let stateCheck = setInterval(() => {
  if (document.readyState === 'complete' && websocket.readyState == 1) {
    websocket.send('detail');
    clearInterval(stateCheck);  // document ready
  }
}, 50);


function WebSocket_MSG(event) {
 console.log('received message: ' + event.data);

 if(event.data.includes(',detail,')) {
  const obj=event.data.split(',');

	const fileName=obj[obj.length - 2].replaceAll(' ','_');
  SX1231SKB_file = '#Type	Register Name	Address[Hex]	Value[Hex]\n';
  SX1231h_file = 'const uint8_t ' + fileName + '[] PROGMEM = {\n';
  SX1231h_file += '  // SX1231 register values for ' + obj[obj.length - 2] + '\n';
  for (i=0; i<= 84; i++) {
   if (i>=80) {
    SX1231SKB_file += 'REG	' + RegName[i] + tab + SX1231_RegAddrTrans[i-80] + tab + '0x' + obj[i] + '\n';
    SX1231h_file += '  0x' + obj[i] + ', // ' + SX1231_RegAddrTrans[i-80] + ' - ' + RegName[i] + '\n';
   } else {
    SX1231SKB_file += 'REG	' + RegName[i] + tab + '0x' + dec2hex(i) + tab + '0x' + obj[i] + '\n';
    SX1231h_file += '  0x' + obj[i] + ', // ' + '0x' + dec2hex(i) + ' - ' + RegName[i] + '\n';
   }
  }
  SX1231SKB_file += 'PKT	False;False;0;0;' + '\n';
  FileSX1231 = 'SX1231SKB_' + fileName + '.cfg';
  SX1231h_file += '}; // END SX1231 ' +  fileName + ' register values' + '\n';
  FileSX1231h = 'SX1231_' + fileName + '.h';

  CC110x_file = 'development\n';
  FileCC110x = 'SRF_' + obj[obj.length - 2] + '.xml';

  let element = document.getElementById("REGs");
  var txt = "'0001','012E','022E',";

  // 0x38 RegPayloadLength
  var FIFO_THR = parseInt(obj[56], 16) / 4;
  FIFO_THR = parseInt(FIFO_THR);
  txt += "'034" + FIFO_THR.toString(16) + "',";

  // 0x02 RegDataModul | TODO
  var ModType = (parseInt(obj[2], 16) & 0b00011000) >> 3;
  // cc1101 MDMCFG2 (0x12)
  var r02 = 0;
  if(ModType == 0) { // FSK
    r02 = "02";
  } else { // OOK
    r02 = "32";
  }

  // 0x05 RegFdevMsb 0x06 RegFdevLsb | TODO
  var r05 = parseInt(obj[5], 16);
  var r06 = parseInt(obj[6], 16);
  var Fdev = ( Fstep_SX * (r06 + r05 * 256) ) / 1000;
  // cc1101 DEVIATN (0x15)
  if (Fdev > 380.859375) {
   Fdev = 380.859375;
  }
  if (Fdev < 1.586914) {
   Fdev = 1.586914;
  }
  Fdev*=2;

  var Fdev_val;
  var bits = 0;
  var devlast = 0;
  var bitlast = 0;

  for (var DEVIATION_E = 0; DEVIATION_E < 8; DEVIATION_E++) {
    for (var DEVIATION_M = 0; DEVIATION_M < 8; DEVIATION_M++) {
      Fdev_val = (8 + DEVIATION_M) * (2 ** DEVIATION_E) * 26000.0 / (2 ** 17);
      bits = DEVIATION_M + (DEVIATION_E << 4);
      if (Fdev > Fdev_val) {
        devlast = Fdev_val;
        bitlast = bits;
      } else {
        if ((Fdev_val - Fdev) < (Fdev - devlast)) {
          devlast = Fdev_val;
          bitlast = bits;
        }
      }
    }
  }

  // 0x2F RegSyncValue1 0x30 RegSyncValue2 0x31 RegSyncValue2 ...
  const Sync = [obj[47], obj[48], obj[49]];
  var SyncCnt = 0;
  // cc1101 SYNC0 (0x05) | SYNC1 (0x04)
  for (let i = 0; i < Sync.length; i++) {
   if(Sync[i] != 'AA') {
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
  txt += "'0D" + arr[0];
  txt += "'0E" + arr[1];
  txt += "'0F" + arr[2];

  // 0x19 RegRxBw
  var RxBwMant = (parseInt(obj[25], 16) & 0b00011000) >> 3;
  var RxBwExp = (parseInt(obj[25], 16) & 0b00000111);
  var RxBw = (2 * (FXOSC_SX / ((16 + RxBwMant * 4) * (2 ** (RxBwExp + 2 + ModType))) / 1000)).toFixed(3);
  // cc1101 MDMCFG4 (0x10) CHANBW_E & CHANBW_M
  var bits1 = 0;
  var bits2 = 0;
  var bw1 = 0;
  var bw2 = 0;
  var DRATE_E = 0;
  var c10 = 0

  exit_loops:
  for (var e = 0; e < 4; e++) {
    for (var m = 0; m < 4; m++) {
      bits2 = bits1;
      bits1 = (e << 6) + (m << 4);
      bw2 = bw1;
      bw1 = 26000 / (8 * (4 + m) * (1 << e));
      if (RxBw >= bw1) {
        break exit_loops;
      }
    }
  }

  if((~(RxBw - bw1) + 1) > (~(RxBw - bw2) + 1)) {
   c10 = bits1;
  } else {
   c10 = bits2;
  }

  // 0x03 RegBitrateMsb 0x04 RegBitrateLsb
  var r03 = parseInt(obj[3], 16);
  var r04 = parseInt(obj[4], 16);
  var rBitRate = (FXOSC_SX / ((r03 * 256) + r04));
  // cc1101 MDMCFG4 (0x0010) DRATE_E | MDMCFG3 (0x0011) DRATE_M
  if (rBitRate < 24.7955) {
    rBitRate = 24.7955;
  } else if (rBitRate > 1621830) {
    rBitRate = 1621830;
  }

  var DRATE_E = rBitRate * ( 2**20 ) / 26000000.0;
  DRATE_E = Math.log(DRATE_E) / Math.log(2);
  DRATE_E = Math.trunc(DRATE_E);
  var DRATE_M = (rBitRate * (2**28) / (26000000.0 * (2**DRATE_E))) - 256;
  DRATE_M = Math.trunc(DRATE_M);
  var DRATE_Mr = Math.round(DRATE_M);
  var DRATE_M1 = DRATE_M + 1;
  var DRATE_E1 = DRATE_E;

  if (DRATE_M1 == 256) {
   DRATE_M1 = 0;
   DRATE_E1++;
  }

  if (DRATE_Mr != DRATE_M) {
   DRATE_M = DRATE_M1;
   DRATE_E = DRATE_E1;
  }
  c10 += DRATE_E;

  txt += "'10" + c10.toString(16) + "',";
  txt += "'11" + DRATE_M.toString(16) + "',";
  txt += "'12" + r02 + "',";
  txt += "'1322','14F8',";
  txt += "'15" + bitlast.toString(16) + "',";
  txt += "'1916','1B43','1C68'";
  txt = txt.toUpperCase();
  element.innerText = txt;
 }
}


function saveFile(variant) {
 let data;
 if (variant == 'SX') {
  data = SX1231SKB_file;
 } else if (variant == 'h') {
  data = SX1231h_file;
 } else {
  data = CC110x_file;
  alert('development');
  return;
 }

 // Convert the text to BLOB.
 const textToBLOB = new Blob([data], { type: 'text/plain' });

 let newLink = document.createElement("a");
 if (variant == 'SX') {
  newLink.download = FileSX1231;
 } else if (variant == 'h') {
  newLink.download = FileSX1231h;
 }

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
'RegTestTcxo',
'RegTestllBw',
'RegTestDagc',
'RegTestAfc'
];
