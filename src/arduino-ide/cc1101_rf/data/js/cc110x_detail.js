/*
  JavaScript for website cc110x detail information /cc110x_detail - cc110x_rf_Gateway
  Copyright (c) 2022 <HomeAutoUser & elektron-bbs>
  URL: https://github.com/HomeAutoUser/cc1101_rf_Gateway
*/

var js = document.createElement("script");
js.src = '/all.js';
document.head.appendChild(js);

var store = document.querySelector(':root');        /* Get the CSS root element */
var value = getComputedStyle(store);                /* Get the styles (properties and values) for the root */
var color1 = value.getPropertyValue('--val_std');
var color2 = value.getPropertyValue('--val_diff');

// Show the received message 
function onMessage(event) {
  console.log('received message: ' + event.data);

  if(event.data.includes('Time') ) {
    var obj = JSON.parse(event.data);
    var name;         // to read value from HTML
    var obj_n;        // value from websocket for new value
    var reg_val;

    for ( i=0; i<= 46; i++ ) {    /* register values 00 - 2E (0 - 46) */
      const num = i;
      const hex = num.toString(16);
      if (i < 16) {
        reg_val = '0' + hex;
      } else {
        reg_val = hex;
      }

      name = '0x' + reg_val.toUpperCase();    // notation HTML 
      obj_n = reg_val.toUpperCase();          // access to object from web socket

      /* compare old value < > HTML value */
      if(document.getElementsByName(name)[0].value != obj[obj_n]) {
        document.getElementsByName(name)[0].value = document.getElementsByName(name)[0].value.replace(document.getElementsByName(name)[0].value, obj[obj_n]);
        document.getElementsByName(name)[0].placeholder = obj[obj_n];
        document.getElementsByName(name)[0].style.color = color2;
      } else {
        document.getElementsByName(name)[0].style.color = color1;
      }

      if (i == 18) { /* 0x12: MDMCFG2 - Modulation */
        var val = (parseInt(obj[obj_n], 16) & "01110000") >> 4;
        document.getElementById('mod').innerHTML = MOD_FORMAT[val];
        document.querySelector('#modulation').value = val;
      }
    }
  }
}

/* if website complete loaded, added texts */
document.onreadystatechange = function () {
  if (document.readyState == 'complete') {
    for ( i=0; i<regExplanation.length; i++ ) {
      document.getElementById('n' + i).innerHTML = regExplanation[i];
    }

    /* SYNC_MODE | CC1101_readReg(0x12, READ_BURST) & 0x07 ; */
    //document.getElementById('SYNC_MODE').innerHTML = SYNC_MODE[0];
  }
}

/* - - Detailed description register - - */
const regExplanation = [
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
  'Modem Configuration 2 (Modulation,Sync_Mod, ...)',
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

const MOD_FORMAT = ['2-FSK', 'GFSK', '', 'ASK/OOK', '4-FSK', '', '', 'MSK'];

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

/* - - END - - Detailed description register - - */