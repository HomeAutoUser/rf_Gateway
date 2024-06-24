function C_FREQread(Msb,Mid,Lsb) {
var r=parseInt(Msb,16)*65536;
r+=parseInt(Mid,16)*256;
r+=parseInt(Lsb,16);
r=parseInt(FXOSC_C*r/ 65536);
return r;
}

function SX_FREQread(Msb,Mid,Lsb) {
var r=parseInt(Msb,16)*65536;
r+=parseInt(Mid,16)*256;
r+=parseInt(Lsb,16);
r=parseInt(Fstep_SX*r);
return r;
}

function C_FREQset(freq) {
var r=['','',''];
var f=freq/FXOSC_C*65536;
r[0]=dec2hex(parseInt(f/65536));
r[1]=dec2hex(parseInt((f%65536)/256));
r[2]=dec2hex(parseInt(f%256));
return r;
}

function SX_FREQset(freq) {
var r=['','',''];
var f=freq/Fstep_SX;
r[0]=dec2hex(parseInt(f/65536));
r[1]=dec2hex(parseInt((f%65536)/256));
r[2]=dec2hex(parseInt(f%256));
return r;
}

function C_BWread(MDMCFG4) {
var CHANBW_M = (parseInt(MDMCFG4, 16) & 0b00110000) >> 4;
var CHANBW_E = (parseInt(MDMCFG4, 16) >> 6);
var RxBwComp = FXOSC_C / (8 * (4 + CHANBW_M) * (2 ** CHANBW_E));
return RxBwComp;
}

function SX_BWread(RegRxBw, RegDataModul) {
var RxBwMant = (parseInt(RegRxBw, 16) & 0b00011000) >> 3;
var RxBwExp = (parseInt(RegRxBw, 16) & 0b00000111);
var RxBwComp = FXOSC_SX / ((16 + RxBwMant * 4) * (2 ** (RxBwExp + 2 + ((parseInt(RegDataModul, 16) & 0b00011000) >> 3))));
return RxBwComp;
}

function C_BWsteps() {
var r = [];
for (var bwExp = 3; bwExp >= 0; bwExp--) {
for (var bwMant = 3; bwMant >= 0; bwMant--) {
var RxBw = (FXOSC_C / (8 * (4 + bwMant) * (2 ** bwExp)));
r.push(RxBw);
}
}
return r;
}

function SX_BWsteps(RegDataModul) {
var r = [];
for (var bwExp = 7; bwExp >= 0; bwExp--) {
for (var bwMant = 2; bwMant >= 0; bwMant--) {
var RxBw = FXOSC_SX / ((16 + bwMant * 4) * (2 ** (bwExp + 2 + ((parseInt(RegDataModul, 16) & 0b00011000) >> 3) )));
r.push(RxBw);
}
}
return r;
}

function C_DRATEread(MDMCFG4, MDMCFG3) {
var DRATE_E = parseInt(MDMCFG4, 16);
var DRATE_M = parseInt(MDMCFG3, 16);
var DRATE = ( (256 + DRATE_M) * (2 ** (DRATE_E & 15)) ) * FXOSC_C / (2 ** 28);
return DRATE;
}

function SX_DRATEread(RegBitrateMsb, RegBitrateLsb) {
var DRATE1 = parseInt(RegBitrateMsb, 16);
var DRATE2 = parseInt(RegBitrateLsb, 16);
var DRATE = FXOSC_SX / ((DRATE1 * 256) + DRATE2);
return DRATE;
}

function C_DRATEset(DRATE,RxBw) {
var r = [];
var bits1=0;
var bits2=0;
var bw1=0;
var bw2=0;
var c10=0;

if(RxBw == undefined) {
return r;
}
exit_loops:
for (var e = 0; e < 4; e++) {
for (var m = 0; m < 4; m++) {
bits2 = bits1;
bits1 = (e << 6) + (m << 4);
bw2 = bw1;
bw1 = FXOSC_C / (8 * (4 + m) * (1 << e));
if (RxBw >= bw1) {
break exit_loops;
}
}
}
if(Math.abs(RxBw - bw1) > Math.abs(RxBw - bw2)) {
c10 = bits2;
} else {
c10 = bits1;
}

if (DRATE < 24.7955) {
DRATE = 24.7955;
} else if (DRATE > 1621830) {
DRATE = 1621830;
}
var DRATE_E = DRATE * ( 2**20 ) / FXOSC_C;
DRATE_E = Math.log(DRATE_E) / Math.log(2);
DRATE_E = Math.trunc(DRATE_E);
var DRATE_M = (DRATE * (2**28) / (FXOSC_C * (2**DRATE_E))) - 256;
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
var val1 = C_DRATEread(DRATE_E.toString(16),DRATE_M.toString(16));
var DRATE_E2 = DRATE_E;
var DRATE_M2 = DRATE_M + 1;
if (DRATE_M2 == 256) {
DRATE_M2 = 0;
DRATE_E2++;
}
var val2 = C_DRATEread(DRATE_E2.toString(16),DRATE_M2.toString(16));
if(Math.abs(DRATE - val1) < Math.abs(DRATE - val2)) {
c10 += DRATE_E;
r.push(c10.toString(16));
r.push(DRATE_M.toString(16));
} else {
c10 += DRATE_E2;
r.push(c10.toString(16));
r.push(DRATE_M2.toString(16));
}
return r;
}

function C_DEVread(DEVIATN) {
DEVIATN = parseInt(DEVIATN, 16);
DEVIATN = (8 + (DEVIATN & 7)) * ( 2 ** ((DEVIATN >> 4) & 7) ) * FXOSC_C / ( 2 ** 17);
return DEVIATN;
}

function SX_DEVread(RegFdevMsb, RegFdevLsb) {
RegFdevMsb = parseInt(RegFdevMsb, 16);
RegFdevLsb = parseInt(RegFdevLsb, 16);
var Fdev = Fstep_SX * (RegFdevLsb + RegFdevMsb * 256);
return Fdev;
}

function C_DEVset(DEV) {
if (DEV > 380.859375) DEV = 380.859375;
if (DEV < 1.586914) DEV = 1.586914;
var deviatn_val;
var bits;
var devlast = 0;
var bitlast = 0;
for (let DEV_E = 0; DEV_E < 8; DEV_E++) {
for (let DEV_M = 0; DEV_M < 8; DEV_M++) {
deviatn_val = (8 + DEV_M) * (Math.pow(2, DEV_E)) * FXOSC_C / 1000 / (Math.pow(2, 17));
bits = DEV_M + (DEV_E << 4);
if (DEV > deviatn_val || ((deviatn_val - DEV)) < (DEV - devlast)) {
devlast = deviatn_val;
bitlast = bits;
}
}
}
return dec2hex(bitlast);
}

function C_FREQOFFread(FSCTRL0) {
var off = ( parseInt(FSCTRL0, 16) );
if (off > 127) {
off -= 255;
}
off = (FXOSC_C / 16384 * off);
return off;
}

function C_FIFOread(FIFOTHR) {
return parseInt(FIFOTHR, 16) & 0b00001111;
}

function SX_FIFOread(FIFOTHR) {
return parseInt(FIFOTHR, 16) & 0b00001111;
}

function SX_SyncToC(S1,S2,S3,S4,S5,S6,S7,S8) {
const SyncIn=[S1,S2,S3,S4,S5,S6,S7,S8],SyncOut=["",""];
var SyncCnt=0;
for(let n=0;n<SyncIn.length;n++)"AA"!==SyncIn[n]&&(1==++SyncCnt?SyncOut[0]=SyncIn[n]:2==SyncCnt&&(SyncOut[1]=SyncIn[n]));
return SyncOut;
}

function saveFile(data,filename) {
console.log('saveFile');
// Convert the text to BLOB.
const textToBLOB = new Blob([data], { type: 'text/plain' });
let newLink = document.createElement("a");
newLink.download = filename;

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

const p=`RegBitrate`;
const q=`RegFdev`;
const r=`RegFrf`;
const s=`RegListen`;
const t=`Reserved`;
const u=`RegDioMapping`;
const v=`RegIrqFlags`;
const w=`RegRxTimeout`;
const x=`RegPreamble`;
const y=`RegSyncValue`;
const z=`RegAesKey`;

const C_RegN = ['IOCFG2','IOCFG1','IOCFG0','FIFOTHR','SYNC1','SYNC0','PKTLEN','PKTCTRL1','PKTCTRL0','ADDR','CHANNR','FSCTRL1','FSCTRL0','FREQ2','FREQ1','FREQ0','MDMCFG4','MDMCFG3','MDMCFG2','MDMCFG1','MDMCFG0','DEVIATN','MCSM2','MCSM1','MCSM0','FOCCFG','BSCFG','AGCCTRL2','AGCCTRL1','AGCCTRL0','WOREVT1','WOREVT0','WORCTRL','FREND1','FREND0','FSCAL3','FSCAL2','FSCAL1','FSCAL0','RCCTRL1','RCCTRL0','FSTEST','PTEST','AGCTEST','TEST2','TEST1','TEST0'];
const SX_RegN = ['RegFifo','RegOpMode','RegDataModul',`${p}Msb`,`${p}Lsb`,`${q}Msb`,`${q}Lsb`,`${r}Msb`,`${r}Mid`,`${r}Lsb`,'RegOsc1','RegAfcCtrl','RegLowBat',`${s}1`,`${s}2`,`${s}3`,'RegVersion','RegPaLevel','RegPaRamp','RegOcp',`${t}14`,`${t}15`,`${t}16`,`${t}17`,'RegLna','RegRxBw','RegAfcBw','RegOokPeak','RegOokAvg','RegOokFix','RegAfcFei','RegAfcMsb','RegAfcLsb','RegFeiMsb','RegFeiLsb','RegRssiConfig','RegRssiValue',`${u}1`,`${u}2`,`${v}1`,`${v}2`,'RegRssiThresh',`${w}1`,`${w}2`,`${x}Msb`,`${x}Lsb`,'RegSyncConfig',`${y}1`,`${y}2`,`${y}3`,`${y}4`,`${y}5`,`${y}6`,`${y}7`,`${y}8`,'RegPacketConfig1','RegPayloadLength','RegNodeAdrs','RegBroadcastAdrs','RegAutoModes','RegFifoThresh','RegPacketConfig2',`${z}1`,`${z}2`,`${z}3`,`${z}4`,`${z}5`,`${z}6`,`${z}7`,`${z}8`,`${z}9`,`${z}10`,`${z}11`,`${z}12`,`${z}13`,`${z}14`,`${z}15`,`${z}16`,'RegTemp1','RegTemp2','RegTestLna','RegTestTcxo','RegTestllBw','RegTestDagc','RegTestAfc'];
const C_XML = ['        <Register>', '        </Register>','            <Name>','</Name>','            <Value>','</Value>'];