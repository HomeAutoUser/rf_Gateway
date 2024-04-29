var js = document.createElement("script");
js.src = '/js/all.js';
document.head.appendChild(js);
var js2 = document.createElement("script");
js2.src = '/js/functions.js';
document.head.appendChild(js2);

function WebSocket_MSG(event) {
 console.log('received message: ' + event.data);
}

function inputfile() {
  const fileSelector = document.getElementById('inputfile');
  fileSelector.addEventListener('change', (event) => {
    const fileList = event.target.files;
    var reader = new FileReader();
    var regAdrPos = '';
    var regAdr = '';
    var regValTxt = '';
    var regVal = '';
    var regTxt = '';

    reader.onload = function(){
      var text = reader.result;
      const lines = reader.result.split('\n');
      if (!lines[0].match(/#Type.*Register\sName.*Address\[Hex\].*Value\[Hex\]/g)) {
        return;
      }

      for (let i = 0; i < lines.length; i++) {
        if (lines[i].match(/REG/g)) {
          regAdrPos = lines[i].search("0x");
          regAdr = lines[i].substr(regAdrPos+2,2);
          regValTxt = lines[i].substr(regAdrPos+4);
          regVal = regValTxt.search("0x");
          regVal = regValTxt.substr(regVal+2,2)
          regTxt += regAdr;
          regTxt += regVal;
        }
        if (i == 85) {  // security point | software export 85 registers
          break;
        }
      }
      //console.log(regTxt);
      websocket.send('imp,SX,'+regTxt);
    };
    reader.readAsText(fileList[0]);
  });

  fileSelector.addEventListener('cancel', (event) => {
    //console.log("cancel");
  });
}

function SXimpFromCC() {
  var input = document.getElementsByName('imp')[0].value;
  var inputTXT = '';
  const Sync = [];
  const Freq = [];
  const Drate = [];
  var Dev = '';

  input = input.slice(1);     // cut first [
  input = input.slice(0, -1); // cut last ]

  const reg = input.split(',');
  for ( let i=0; i<reg.length; i++ ){
    // Examination of specific registers
    var adr = reg[i].substring(1, 3);
    if(SX_allowCC.includes(adr)) {
      // all Sync register, once failed = no Sync set
      if(adr == '04' || adr == '05') {
        Sync.push(reg[i].substring(1, 5));
      }

      // all Frequency register, once failed = no Frequency set
      if(adr == '0D' || adr == '0E' || adr == '0F') {
        Freq.push(reg[i].substring(1, 5));
      }
      // all Datarate register, once failed = no Datarate set
      if(adr == '10' || adr == '11') {
        Drate.push(reg[i].substring(1, 5));
      }
      // Deviation
      if(adr == '15') {
        Dev += reg[i].substring(1, 5);
      }
    }
  }

  if (Sync.length == 2) {
    inputTXT += Sync[0]+',';
    inputTXT += Sync[1]+',';
  }

  if (Freq.length == 3) {
    var val = cCalFreq(Freq[0].substring(2, 4), Freq[1].substring(2, 4), Freq[2].substring(2, 4), 26);
    alert('freq cc110x: ' + val);
    alert('cal rfm register');
    inputTXT += Freq[0]+',';
    inputTXT += Freq[1]+',';
    inputTXT += Freq[2]+',';
  }

  if (Drate.length == 2) {
    inputTXT += Drate[0]+',';
    inputTXT += Drate[1]+',';
  }

  if(Dev != '') {
    inputTXT += Dev;
  }

  if(inputTXT.charAt(inputTXT.length-1) ==',') {
    inputTXT = inputTXT.slice(0, -1);
  }

  if (inputTXT != '') {
    document.getElementsByName('imp')[0].value = inputTXT;
  }
  return true;
};

const SX_allowCC = [
 // Sync
 '04',
 '05',
 // Frequency
 '0D',
 '0E',
 '0F',
 // Datarate & Bandwidth
 '10',
 '11',
 // Deviation
 '15',
];