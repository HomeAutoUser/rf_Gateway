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

  /*  {"ToggleTime":"30000"} */
  if(event.data.includes('ToggleTime') ) {
    var obj = JSON.parse(event.data);
    //location.reload(); // Werte schwinden dadurch wieder
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