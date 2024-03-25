﻿var js = document.createElement("script");
js.src = '/js/all.js';
document.head.appendChild(js);

function onMessage(event) {
 console.log('received message: ' + event.data);
}

function inputfile() {
  const fileSelector = document.getElementById('inputfile');
  fileSelector.addEventListener('change', (event) => {
    //console.log("change");
    const fileList = event.target.files;

    var reader = new FileReader();
    var regPosAdr = '';
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
          //console.log(lines[i]);
          regPosAdr = lines[i].search("0x");
          regAdr = lines[i].substr(regPosAdr+2,2);
          regValTxt = lines[i].substr(regPosAdr+4);
          //console.log(regAdr);
          regVal = regValTxt.search("0x");
          regVal = regValTxt.substr(regVal+2,2)
          //console.log(regVal);
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
