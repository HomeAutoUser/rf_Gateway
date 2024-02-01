var js = document.createElement("script");
js.src = '/js/all.js';
document.head.appendChild(js);

var store = document.querySelector(':root');
var value = getComputedStyle(store);
var color1 = value.getPropertyValue('--val_std');
var color2 = value.getPropertyValue('--val_diff');
var status_html = false;
var status_websocket = false;
var mod_list = false;
var bw_list = false;

function onMessage(event) {
 console.log('received message: ' + event.data);

 if(event.data == 'Connected') {
  status_websocket = 1;
  check();
 }

 if(event.data.includes(',detail,')) {
  const obj=event.data.split(',');
  var REGISTER_MAX = 46;
  var name;
  var hex;
  var Freq;

  for (i=0; i<= REGISTER_MAX; i++) {
   name = 'r' + i;
   if (i <= 15) {
    hex = '0' + i.toString(16)
   } else {
    hex = i.toString(16)
   }

   let element = document.getElementsByName(name)[0];
   document.getElementById('s' + i).innerHTML = '0x' + hex.toUpperCase() + '&ensp;' + Explan_short[i];

   element.maxLength = "2";
   element.pattern = "^[\\da-fA-F]{1,2}$";
   element.placeholder = element.value.replace(element.value, obj[i]);
   element.setAttribute('size', '2');
   element.title = "Input: 2 characters in hexadecimal";

   if (element.value != obj[i]) {
    element.value = element.value.replace(element.value, obj[i]);
    element.placeholder = obj[i];
    element.style.color = color2;
   } else {
    element.style.color = color1;
   }
  }

  // 0x06: PKTLEN 0x08: PKTCTRL0
  var val = ( parseInt(obj[8], 16) & 0b00000011 ) >> 4;
  document.getElementById('PKTCTRL0').innerHTML = LENGTH_CONFIG[val];
  let span = document.getElementById("PKTLEN");
  if(val == 0) {
   span.textContent = "current: bytes " + parseInt(obj[6], 16) + "  |  nibbles " + parseInt(obj[6], 16) * 2 + "  |  bits " + parseInt(obj[6], 16) * 8;
  } else {
   span.textContent = "current: disabled";
  }

  // 0x0C: FSCTRL0 
  if(document.getElementsByName('afc')[0].checked) {
   var off = ( parseInt(obj[12], 16) );
   if (off > 127) {
    off -= 255;
   }
   off = (26000000 / 16384 * off / 1000);
   document.getElementById('n' + 12).innerHTML = Explan[12] + ' (Freq. offset ' + off.toFixed(0) + ' kHz)';
  }

  // 0x0D: FREQ2 0x0E: FREQ1 0x0F: FREQ0
  Freq = parseInt(obj[13], 16) * 256;
  Freq = (Freq + parseInt(obj[14], 16) ) * 256;
  Freq = (Freq + parseInt(obj[15], 16) );
  Freq = (26 * Freq) / 65536;
  document.getElementById('FREQis').innerHTML = Freq.toFixed(3);
  document.getElementById('p2').maxLength = "6";
  document.getElementById('p2').pattern = "^-?[\\d]{1,3}(\\.[\\d]{1,3})?$";
  Freq = Freq - (obj[49] * 1);
  document.getElementById('FREQ').innerHTML = Freq.toFixed(3);
  document.getElementById('p1').maxLength = "7";
  document.getElementById('p1').pattern = "^[\\d]{3}(\\.[\\d]{1,3})?$";
  document.getElementsByName('freq')[0].value = Freq.toFixed(3);

  // 0x10: MDMCFG4 0x11: MDMCFG3
  var CHANBW_M = (parseInt(obj[16], 16) & 0b00110000) >> 4;
  var CHANBW_E = (parseInt(obj[16], 16) >> 6);
  var RxBwComp = (26000000 / (8 * (4 + CHANBW_M) * (2 ** CHANBW_E)) / 1000).toFixed(2);
  var selectElement = document.getElementById('bandw');
  if(!bw_list) {
   bw_list = true;
   for (var bwExp = 3; bwExp >= 0; bwExp--) {
    for (var bwMant = 3; bwMant >= 0; bwMant--) {
     var RxBw = (26000000 / (8 * (4 + bwMant) * (2 ** bwExp)) / 1000).toFixed(2);
     selectElement.add(new Option(RxBw));
    }
   }
  }
  selectElement.value = RxBwComp;
  document.getElementById('CHANBW').innerHTML = RxBwComp + ' kHz';

  var DRATE_E = parseInt(obj[16], 16);
  var DRATE_M = parseInt(obj[17], 16);
  var DRATE = ( ( (256 + DRATE_M) * (2 ** (DRATE_E & 15)) ) * 26000000.0 / (2 ** 28) / 1000.0 ).toFixed(2);
  document.getElementById('DRATE').innerHTML = DRATE + ' kBaud';
  document.getElementsByName('datarate')[0].value = DRATE;
  document.getElementById('p4').maxLength = "6";
  document.getElementById('p4').pattern = "^[\\d]{1,4}(\\.[\\d]{1,2})?$";

  // 0x12: MDMCFG2
  if (!mod_list) {
   mod_list = true;
   var selectElement = document.getElementById('modulation');
   for (var j = 0; j<=7; j++) {
    if (MOD[j] != '') {
     selectElement.add(new Option(MOD[j]));
    }
   }
  }
  var val = (parseInt(obj[18], 16) & 0b01110000) >> 4;
  document.getElementById('MOD_FORMAT').innerHTML = MOD[val];
  document.getElementById('modulation').value = MOD[val];
  var val = (parseInt(obj[18], 16) & 0b00000111);
  document.getElementById('SYNC_MODE').innerHTML = SYNC_MODE[val];

  // 0x13: MDMCFG1
  var val = (parseInt(obj[19], 16) & 0b01110000) >> 4;
  document.getElementById('MDMCFG1').innerHTML = 'minimum ' + NUM_PREAMBLE[val] + ' preamble bytes to be transmitted configured in MDMCFG1 register';

  // 0x15: DEVIATN
  var DEVIATN = parseInt(obj[21], 16);
  DEVIATN = (8 + (DEVIATN & 7)) * ( 2 ** ((DEVIATN >> 4) & 7) ) * 26000.0 / ( 2 ** 17);
  document.getElementById('DEVIATN').innerHTML = DEVIATN.toFixed(2) + ' kHz';
  document.getElementsByName('deviation')[0].value = DEVIATN.toFixed(2);
  document.getElementById('p5').maxLength = "5";
  document.getElementById('p5').pattern = "^[\\d]{1,3}(\\.[\\d]{1,2})?$";

  document.getElementById('state').innerHTML = obj[obj.length - 2] + ' values readed &#10004;';
 }  // END - event.data.includes(',detail,')
}   // END function onMessage(event)


document.onreadystatechange = function () {
 if (document.readyState == 'complete') {

  for ( i=0; i<Explan.length; i++ ) {
   document.getElementById('n' + i).innerHTML = Explan[i];
   var id = 't' + i;

   if (ExplanAdd[i] != '') {
     document.getElementById(id).innerHTML = '&#128712;';

    /* <!-- The Modal -->     https://www.w3schools.com/howto/tryit.asp?filename=tryhow_css_modal */
    var div = document.createElement("div");
    div.id = "Mo"+i;
    div.className = "mod";
    document.getElementById(id).appendChild(div);

    var div2 = document.createElement("div");
    div.appendChild(div2);
    div2.className = "mod-cont";

    var Span = document.createElement("span");
    Span.className = "close";
    Span.innerHTML = "&times;";
    div2.appendChild(Span);

    var p = document.createElement("p");
    p.innerHTML = Explan_short[i];

    div2.appendChild(p);
    div2.insertAdjacentHTML( 'beforeend', ExplanAdd[i] );
   }
  }

  status_html = 1;
  check();
  var lastmodal;

  window.onclick = function(event) {
   var id = event.target.id.match(/t\d+/i);
   var nr = event.target.id.substr(1);
   var cla = event.target.className;
   var modal = document.getElementById("Mo" + nr);   // Get the modal

   if (ExplanAdd[nr] != ''){
    if(id && modal) {
     modal.style.display = "block";    // When the user clicks the button, open the modal
     lastmodal = modal;
    }
    if(cla && cla == 'close'){
     lastmodal.style.display = "none"; // When the user clicks on <span> (x), close the modal
    }
   }
  }
 }  // END - document.readyState == 'complete'
}   // END - document.onreadystatechange = function ()


function check(){
 if (status_html && status_websocket) {
  websocket.send('detail');
 }
}


const Explan = [
'GDO2 Output Pin Configuration',
'GDO1 Output Pin Configuration',
'GDO0 Output Pin Configuration',
'RX FIFO and TX FIFO Thresholds',
'Sync Word, High Byte',
'Sync Word, Low Byte',
'Packet Length',
'Packet Automation Control (Preamble quality, ...)',
'Packet Automation Control (Packet length config, ...)',
'Device Address',
'Channel Number',
'Frequency Synthesizer Control',
'Frequency Synthesizer Control',
'Frequency Control Word, High Byte',
'Frequency Control Word, Middle Byte',
'Frequency Control Word, Low Byte',
'Modem Configuration 4 (Datarate, Bandwidth)',
'Modem Configuration 3 (Datarate)',
'Modem Configuration 2 (Modulation, Sync_Mod, ...)',
'Modem Configuration 1 (Num_Preamble, ...)',
'Modem Configuration 0',
'Modem Deviation Setting',
'Main Radio Control State Machine Configuration 2',
'Main Radio Control State Machine Configuration 1',
'Main Radio Control State Machine Configuration 0',
'Frequency Offset Compensation Configuration',
'Bit Synchronization Configuration',
'AGC Control 2',
'AGC Control 1',
'AGC Control 0',
'High Byte Event0 Timeout',
'Low Byte Event0 Timeout',
'Wake On Radio Control',
'Front End RX Configuration',
'Front End TX Configuration',
'Frequency Synthesizer Calibration 3',
'Frequency Synthesizer Calibration 2',
'Frequency Synthesizer Calibration 1',
'Frequency Synthesizer Calibration 0',
'RC Oscillator Configuration 1',
'RC Oscillator Configuration 0',
'Frequency Synthesizer Calibration Control',
'Production Test',
'AGC Test',
'Various Test Settings 2',
'Various Test Settings 1',
'Various Test Settings 0'
];

const Explan_short = [
'IOCFG2',
'IOCFG1',
'IOCFG0',
'FIFOTHR',
'SYNC1',
'SYNC0',
'PKTLEN',
'PKTCTRL1',
'PKTCTRL0',
'ADDR',
'CHANNR',
'FSCTRL1',
'FSCTRL0',
'FREQ2',
'FREQ1',
'FREQ0',
'MDMCFG4',
'MDMCFG3',
'MDMCFG2',
'MDMCFG1',
'MDMCFG0',
'DEVIATN',
'MCSM2',
'MCSM1',
'MCSM0',
'FOCCFG',
'BSCFG',
'AGCCTRL2',
'AGCCTRL1',
'AGCCTRL0',
'WOREVT1',
'WOREVT0',
'WORCTRL',
'FREND1',
'FREND0',
'FSCAL3',
'FSCAL2',
'FSCAL1',
'FSCAL0',
'RCCTRL1',
'RCCTRL0',
'FSTEST',
'PTEST',
'AGCTEST',
'TEST2',
'TEST1',
'TEST0',
];

const tabPre = '<table><tr><td>Bit No.</td><td>Name</td><td>Reset</td><td>R/W</td><td>Description</td></tr><tr><td>';
const tabEnd = '</td></tr></table>';

const ExplanAdd = [
tabPre + '7</td><td>&nbsp;</td><td>0x00</td><td>R0</td><td>Not used</td></tr><tr><td>6</td><td>GDO2_INV</td><td>0x00</td><td>R/W</td><td>Invert output, i.e. select active low (1) / high (0)</td></tr><tr><td>5</td><td>GDO2_CFG[5:0]</td><td>0x29</td><td>R/W</td><td>Default is CHP_RDYn.' + tabEnd,
tabPre + '7</td><td>GDO_DS</td><td>0x00</td><td>R/W</td><td>Set high (1) or low (0) output drive strength on the GDO pins.</td></tr><tr><td>6</td><td>GDO1_INV</td><td>0x00</td><td>R/W</td><td>Invert output, i.e. select active low (1) / high (0)</td></tr><tr><td> 5:0</td><td>GDO1_CFG[5:0]</td><td>0x2e</td><td>R/W</td><td>Default is 3-state.' + tabEnd,
tabPre + '7</td><td>TEMP_SENSOR_ENABLE</td><td>0x00</td><td>R/W</td><td>Enable analog temperature sensor. Write 0 in all other register bits when using temperature sensor.</td></tr><tr><td>6</td><td>GDO0_INV</td><td>0x00</td><td>R/W</td><td>Invert output, i.e. select active low (1) / high (0)</td></tr><tr><td>5:0</td><td>GDO0_CFG[5:0]</td><td>0x3f</td><td>R/W</td><td>Default is CLK_XOSC/192. It is recommended to disable the clock output in initialization, in order to optimize RF performance.' + tabEnd,
tabPre + '7</td><td></td><td>0x00</td><td>R/W</td><td>Reserved , write 0 for compatibility with possible future extensions</td></tr><tr><td>6</td><td>ADC_RETENTION</td><td>0x00</td><td>R/W</td><td>...</td></tr><tr><td>5:4</td><td>CLOSE_IN_RX [1:0]</td><td>0x00</td><td>R/W</td><td>...</td></tr><tr><td>3:0</td><td>FIFO_THR[3:0]</td><td>0x07</td><td>R/W</td><td>Set the threshold for the TX FIFO and RX FIFO.<br>The threshold is exceeded when the number of bytes in the FIFO is equal to or higher than the threshold value. <table><tbody><tr><td>Setting </td><td>Bytes in TX FIFO</td><td>Bytes in RX FIFO</td></tr><tr><td>0 (0000)</td><td>61</td><td>4</td></tr><tr><td>1 (0001)</td><td>57</td><td>8</td></tr><tr><td>2 (0010)</td><td>53</td><td>12</td></tr><tr><td>3 (0011)</td><td>49</td><td>16</td></tr><tr><td>4 (0100)</td><td>45 </td><td>20</td></tr><tr><td>5 (0101)</td><td>41 </td><td>24 </td></tr><tr><td>6 (0110)</td><td>37</td><td>28</td></tr><tr><td>7 (0111)</td><td>33</td><td>32</td></tr><tr><td>8 (1000)</td><td>29</td><td>36</td></tr><tr><td>9 (1001)</td><td>25</td><td>40</td></tr><tr><td>10 (1010)</td><td>21</td><td>44</td></tr><tr><td>11 (1011)</td><td>17</td><td>48</td></tr><tr><td>12 (1100)</td><td>13</td><td>52</td></tr><tr><td>13 (1101)</td><td>9</td><td>56</td></tr><tr><td>14 (1110)</td><td>5</td><td>60</td></tr><tr><td>15 (1111)</td><td>1</td><td>64</td></tr></tbody></table>' + tabEnd,
'',
'',
'Indicates the packet length when fixed packet length mode is enabled.<br>If variable packet length mode is used, this value indicates the maximum packet length allowed.<br><br><span id="PKTLEN">xxx</span>',
tabPre + '7:5</td><td>PQT[2:0]</td><td>0x00</td><td>R/W</td><td>Preamble quality estimator threshold. The preamble quality estimator increases an internal counter by one each time a bit is received that is different from the previous bit, and decreases the counter by 8 each time a bit is received that is the same as the last bit. A threshold of 4∙PQT for this counter is used to gate sync word detection. When PQT=0 a sync word is always accepted.</td></tr><tr><td>4</td><td></td><td>0x00</td><td>R0</td><td>Not Used.</td></tr><tr><td>3</td><td>CRC_AUTOFLUSH</td><td>0x00</td><td>R/W</td><td>Enable automatic flush of RX FIFO when CRC is not OK. This requires that only one packet is in the RXIFIFO and that packet length is limited to the RX FIFO size.</td></tr><tr><td>2</td><td>APPEND_STATUS</td><td>0x01</td><td>R/W</td><td>When enabled, two status bytes will be appended to the payload of the packet. The status bytes contain RSSI and LQI values, as well as CRC OK.</td></tr><tr><td>1:0</td><td>ADR_CHK[1:0]</td><td>0x00</td><td>R/W</td><td>Controls address check configuration of received packages.<br><br>Values:<br>0 : No address check<br>1 : Address check, no broadcast<br>2 : Address check and 0 (0x00) broadcast<br>3 : Address check and 0 (0x00) and 255 (0xFF) broadcast' + tabEnd,
tabPre + '7</td><td></td><td>0x00</td><td>R0</td><td>Not used</td></tr><tr><td>6</td><td>WHITE_DATA</td><td>0x01</td><td>R/W</td><td>Turn data whitening on / off.<br><br>Values:<br>0 : Whitening off<br>1 : Whitening on</td></tr><tr><td>5:4</td><td>PKT_FORMAT[1:0]</td><td>0x00</td><td>R/W</td><td>Format of RX and TX data<br><br>Values:<br>0 : Normal mode, use FIFOs for RX and TX<br>1 : Synchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins<br>2 : Random TX mode; sends random data using PN9 generator. Used for test. Works as normal mode, setting 0 (00), in RX<br>3 : Asynchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins</td></tr><tr><td>3</td> <td></td><td>0x00</td><td>R0</td><td>Not used</td></tr><tr><td>2</td><td>CRC_EN</td><td>0x01</td><td>R/W</td><td>Values:<br>1 : CRC calculation in TX and CRC check in RX enabled<br>0 : CRC disabled for TX and RX</td></tr><tr><td>1:0</td><td>LENGTH_CONFIG[1:0]</td><td>0x01</td><td>R/W</td><td>Configure the packet length<br><br>Values:<br>0 : Fixed packet length mode. Length configured in PKTLEN register<br>1 : Variable packet length mode. Packet length configured by the first byte after sync word<br>2: Infinite packet length mode<br>3: Reserved' + tabEnd,
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
tabPre + '7</td><td>FEC_EN</td><td>0x00</td><td>R/W</td><td>Enable Forward Error Correction (FEC) with interleaving for packet payload<br>(Only supported for fixed packet length mode, i.e. PKTCTRL0.LENGTH_CONFIG =0)<br><br>Values:<br>0 : Disable<br>1 : Enable</td></tr><tr><td>6:4</td><td>NUM_PREAMBLE[2:0]</td><td>0x02</td><td>R/W</td><td>Sets the minimum number of preamble bytes to be transmitted<br><br>Values:<br>0 : 2<br>1 : 3<br>2 : 4<br>3 : 6<br>4 : 8<br>5 : 12<br>6 : 16<br>7 : 24<br></td></tr><tr><td>3:2</td><td></td><td>0x00</td><td>R0</td><td>Not used</td></tr><tr><td>1:0</td><td>CHANSPC_E[1:0]</td><td>0x02</td><td>R/W</td><td>2 bit exponent of channel spacing' + tabEnd,
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
'',
''
];

const LENGTH_CONFIG = [
'Fixed packet length. Length configured in PKTLEN register',
'Variable packet length. Length configured by the first byte after sync word',
'Infinite packet length',
'Reserved'
];

const MOD = ['2-FSK', 'GFSK', '', 'ASK/OOK', '4-FSK', '', '', 'MSK'];
const NUM_PREAMBLE = ['2', '3', '4', '6', '8', '12', '16', '24'];

const SYNC_MODE = [
'No preamble/sync',
'15/16 sync word bits detected',
'16/16 sync word bits detected',
'30/32 sync word bits detected',
'No preamble/sync, carrier-sense above threshold',
'15/16 + carrier-sense above threshold',
'16/16 + carrier-sense above threshold',
'30/32 + carrier-sense above threshold'
];
