var js = document.createElement("script");
js.src = '/js/all.js';
document.head.appendChild(js);
var js2 = document.createElement("script");
js2.src = '/js/functions.js';
document.head.appendChild(js2);

const tab = '	';
const SX1231_RegAddrTrans = ['0x58','0x59','0x5F','0x6F','0x71'];

/* const SX1231_RegAddrTrans = [
  ['0x58','0x59','0x5F','0x6F','0x71'], // v2.1 ???
  ['0x58','0x59','0x5F','0x6F','0x71'], // v2.2 ???
  ['0x58','0x59','0x5F','0x6F','0x71'], // v2.3 ???
  ['0x58','0x5A','0x5C','0x6F','0x71'], // v2.4
]; */

var obj;
var fileName;

let stateCheck = setInterval(() => {
  if (document.readyState === 'complete' && websocket.readyState == 1) {
   websocket.send('detail');
   clearInterval(stateCheck);  // document ready
  }
}, 50);

function WebSocket_MSG(event) {
 console.log(`received message: ` + event.data);

 if(event.data.includes(',detail,')) {
  obj=event.data.split(',');
  fileName=obj[obj.length - 2].replaceAll(' ','_');

  let element = document.getElementById("REGs");
  var txt = `'0001','012E','022E',`;

  // 0x38 RegPayloadLength
  var FIFO_THR = parseInt(obj[56], 16) / 4;
  FIFO_THR = parseInt(FIFO_THR);
  txt += `'034` + FIFO_THR.toString(16) + `',`;

  // 0x02 RegDataModul | TODO
  var ModType = (parseInt(obj[2], 16) & 0b00011000) >> 3;
  // cc1101 MDMCFG2 (0x12)
  var r02 = (ModType === 0) ? "02" : "32";

  // 0x05 RegFdevMsb 0x06 RegFdevLsb | TODO
  var r05 = parseInt(obj[5], 16);
  var r06 = parseInt(obj[6], 16);
  var Fdev = ( Fstep_SX * (r06 + r05 * 256) ) / 1000;
  // cc1101 DEVIATN (0x15)
  Fdev = C_DEVset(Fdev);

  // 0x2F RegSyncValue1 ... to 0x36 RegSyncValue8 | cc110x SYNC
  const out = SX_SyncToC(obj[47],obj[48],obj[49],obj[50],obj[51],obj[52],obj[53],obj[54]);
  txt += `'04` + out[0] + `',` + `'05` + out[1] + `',`;

  // 0x38 RegPayloadLength
  txt += `'06` + obj[56] + `',`;
  // non-existent on RFM
  txt += `'0780',`;

  // 0x37 RegPacketConfig1
  var r37_7 = obj[55] >> 7;
  txt += `'080` + r37_7 + `',`;

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
  txt += `'10${arr[0].toString(16)}','11${arr[1].toString(16)}','12${r02}','1322','14F8','15${Fdev}','1916','1B43','1C68'`;
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
   txt += 'REG	' + SX_RegN[i] + tab;
   i>=80 ? txt+=SX1231_RegAddrTrans[i-80] : txt+=`0x`+dec2hex(i);
   txt += tab + `0x` + obj[i] + `\n`;
  }
  txt += `PKT\tFalse;False;0;0;\n`;
  fns = `SX1231SKB_` + fileName + `.cfg`;
 } else if (variant == 'h') {
  txt = `const uint8_t ` + fileName + `[] PROGMEM = {\n`;
  txt += `  // SX1231 register values for ` + obj[obj.length - 2] + `\n`;
  for (i=0; i<= 84; i++) {
   txt += `  0x` + obj[i] + `, // `;
   i>=80 ? txt+=SX1231_RegAddrTrans[i-80] : txt+=`0x`+dec2hex(i);
   txt +=  ` - ` + SX_RegN[i] + `\n`;
  }
  txt += `}; // END SX1231 ` +  fileName + ` register values\n`;
  fns = `SX1231_` + fileName + `.h`;
 } else if (variant == 'C') {
  txt = `<dcpanelconfiguration>\n` + `    <registersettings>\n`;
  const sy = SX_SyncToC(obj[47],obj[48],obj[49],obj[50],obj[51],obj[52],obj[53],obj[54]);
  var fr = ['','',''];
  fr = C_FREQset( SX_FREQread(obj[7], obj[8], obj[9]) );
  var mdm = ['',''];
  mdm = C_DRATEset(SX_DRATEread(obj[3],obj[4]),(SX_BWread(obj[25], obj[2])*2));

  for(let r=0; r<=6; r++) {
   txt += C_XML[0] + `\n` + C_XML[2];
   if(r<=1) {  // Sync
    txt += C_RegN[r + 4] + C_XML[3] + `\n` + C_XML[4] + `0x` + sy[r];
   } else if (r>1 && r<=4) { // Frequency
    txt += C_RegN[r + 11] + C_XML[3] + `\n` + C_XML[4] + `0x` + fr[r - 2];
   } else if (r>4 && r<=6) { // DataRate
    txt += C_RegN[r + 11] + C_XML[3] + `\n` + C_XML[4] + `0x` + mdm[r - 5];
   }
   txt += C_XML[5] + `\n` + C_XML[1] + `\n`;
  }

  // Deviation
  let Fdev = SX_DEVread(obj[5], obj[6]) / 1000;	// ToDo
  txt += C_XML[0] + `\n` + C_XML[2] + C_RegN[21] + C_XML[3] + `\n`;
  txt += C_XML[4] + `0x` + C_DEVset(Fdev) + C_XML[5] + `\n` + C_XML[1] + `\n`;

  txt += C_XML[0] + `\n` + C_XML[2] + `PA_TABLE0` + C_XML[3] + `\n`;
  txt += C_XML[4] + `0x86` + C_XML[5] + `\n` + C_XML[1] + `\n`;
  txt += `    </registersettings>\n` + `</dcpanelconfiguration>\n`;
  fns = `SmarfRF_` + fileName + `.xml`;
 } else {
  alert(`development`);
  return;
 }

 saveFile(txt,fns);
}
