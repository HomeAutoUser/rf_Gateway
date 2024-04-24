function C_FREQread(reg1,reg2,reg3) {
 var ret = parseInt(reg1, 16) * 256;
 ret = (ret + parseInt(reg2, 16) ) * 256;
 ret = (ret + parseInt(reg3, 16) );
 ret = parseInt(FXOSC_C * ret / 65536);
 return ret;
}

function SX_FREQread(RegFrfMsb,RegFrfMid,RegFrfLsb) {
 var ret = parseInt(RegFrfMsb, 16) * 256;
 ret = (ret + parseInt(RegFrfMid, 16) ) * 256;
 ret = (ret + parseInt(RegFrfLsb, 16) );
 ret = parseInt(Fstep_SX * ret);
 return ret;
}

function C_FREQset(freq) {
 var ret = ['','',''];
 var val = parseInt(Math.round(freq / FXOSC_C * 65536));
 ret[0] = parseInt(freq / FXOSC_C).toString(16);
 ret[1] = parseInt((val % 65536) / 256).toString(16);
 ret[2] = parseInt(val % 256).toString(16);
 return ret;
}

function SX_FREQset(freq) {
 var ret = ['','',''];
 var f = parseInt(freq / Fstep_SX);
 ret[0] = (parseInt( f / 65536 )).toString(16);
 ret[1] = (parseInt((f % 65536) / 256 )).toString(16);
 ret[2] = (parseInt( f % 256 )).toString(16);
 return ret;
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
 var ret = [];
 for (var bwExp = 3; bwExp >= 0; bwExp--) {
  for (var bwMant = 3; bwMant >= 0; bwMant--) {
   var RxBw = (FXOSC_C / (8 * (4 + bwMant) * (2 ** bwExp)));
   ret.push(RxBw);
  }
 }
 return ret;
}

function SX_BWsteps(RegDataModul) {
 var ret = [];
 for (var bwExp = 7; bwExp >= 0; bwExp--) {
  for (var bwMant = 2; bwMant >= 0; bwMant--) {
   var RxBw = FXOSC_SX / ((16 + bwMant * 4) * (2 ** (bwExp + 2 + ((parseInt(RegDataModul, 16) & 0b00011000) >> 3) )));
   ret.push(RxBw);
  }
 }
 return ret;
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
 // need CHANBW_E & CHANBW_M so set MDMCFG4 complete
 // unchecked -> detail_rfm69.js
 var ret = [];
 var bits1 = 0;
 var bits2 = 0;
 var bw1 = 0;
 var bw2 = 0;
 var c10 = 0;

 if(RxBw == undefined) {
	RxBw = 0; // min value check or 0 
 }

 exit_loops:
	for (var e = 0; e < 4; e++) {
		for (var m = 0; m < 4; m++) {
      bits2 = bits1;
      bits1 = (e << 6) + (m << 4);
      bw2 = bw1;
      bw1 = (FXOSC_C / 1000) / (8 * (4 + m) * (1 << e));
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
 
 if (DRATE < 24.7955) {
	DRATE = 24.7955;
 } else if (DRATE > 1621830) {
	DRATE = 1621830;
 }

 var DRATE_E = DRATE * ( 2**20 ) / FXOSC_C * 1.0;
 DRATE_E = Math.log(DRATE_E) / Math.log(2);
 DRATE_E = Math.trunc(DRATE_E);
 var DRATE_M = (DRATE * (2**28) / (FXOSC_C * 1.0 * (2**DRATE_E))) - 256;
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
 ret.push(DRATE_E.toString(16));
 ret.push(DRATE_M.toString(16));
 return ret;
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

function C_FREQOFFread(FSCTRL0) {
 var off = ( parseInt(FSCTRL0, 16) );
 if (off > 127) {
	off -= 255;
 }
 off = (FXOSC_C / 16384 * off);
 return off;
}

function C_FIFOread(FIFOTHR) {
 // ToDo needed ???
 return parseInt(FIFOTHR, 16) & 0b00001111;
}

function SX_FIFOread(FIFOTHR) {
 // ToDO needed ???
 return parseInt(FIFOTHR, 16) & 0b00001111;
}
