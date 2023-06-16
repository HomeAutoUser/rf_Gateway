/*
  JavaScript for website cc110x detail information /cc110x_detail - cc110x_rf_Gateway
  Copyright (c) 2022 <HomeAutoUser & elektron-bbs>
  URL: https://github.com/HomeAutoUser/cc1101_rf_Gateway
*/

var js = document.createElement("script");
js.src = '/all.js';
document.head.appendChild(js);

// Show the received message 
function onMessage(event) {
  console.log('received message: ' + event.data);

  /*  {"ToggleTime":"30000", "activated_mode_nr":"11", "activated_mode_name":"Lacrosse_mode1", "00":"01", "01":"2E", "02":"2E", "03":"41", "04":"2D", "05":"D4", "06":"05", "07":"80", "08":"00", "09":"00", "0A":"00", "0B":"06", "0C":"00", "0D":"21", "0E":"65", "0F":"6A", "10":"89", "11":"5C", "12":"02", "13":"22", "14":"F8", "15":"56", "16":"07", "17":"00", "18":"18", "19":"15", "1A":"6C", "1B":"43", "1C":"68", "1D":"91", "1E":"87", "1F":"6B", "20":"F8", "21":"B6", "22":"11", "23":"EA", "24":"2A", "25":"00", "26":"1F", "27":"41", "28":"00", "29":"01", "2A":"2E", "2B":"2E", "2C":"45", "2D":"2D", "2E":"D4"}  */
  if(event.data.includes('ToggleTime') ) {
    var obj = JSON.parse(event.data);
    //location.reload(); // Werte schwinden dadurch wieder

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

      //alert(name);
      //alert(obj[obj_n]);
      /* compare old value < HTML value | document.getElementsByName(name)[0].value > with new value < websocket value | obj[obj_n] > */
    }
  }
}

/* if website complete loaded, added texts */
document.onreadystatechange = function () {
  if (document.readyState == "complete") {
    for ( i=0; i<regExplanation.length; i++ ) {
      document.getElementById('n' + i).innerHTML = regExplanation[i];
    }
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
]

/* - - END - - Detailed description register - - */