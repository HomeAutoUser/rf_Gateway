var js = document.createElement("script");
js.src = '/js/all.js';
document.head.appendChild(js);
var js2 = document.createElement("script");
js2.src = '/js/functions.js';
document.head.appendChild(js2);

var obj;
var fileName;

let stateCheck = setInterval(() => {
  if (document.readyState === 'complete' && websocket.readyState == 1) {
    websocket.send('detail');
    clearInterval(stateCheck);  // document ready
  }
}, 50);

function WebSocket_MSG(event) {
 console.log('received message: ' + event.data);
 
  if(event.data.includes(',detail,')) {
   obj=event.data.split(',');
   fileName=obj[obj.length - 2].replaceAll(' ','_');
  }
}

function saveData(variant){
 var fns;
 var txt;
 if (variant == 'h') {
  txt = 'const uint8_t ' + fileName + '[] PROGMEM = {\n';
  txt += '  // CC110x register values for ' + obj[obj.length - 2] + '\n';
  for (let i=0; i<= 46; i++) {
    txt += '  0x' + obj[i] + ', // 0x' + dec2hex(i) + ' - ' + C_RegN[i] + '\n';
  }
  txt += '}; // END CC110x ' +  fileName + ' register values\n';
  fns = 'CC110x_' + fileName + '.h';
 } else if (variant == 'C') {
  txt = '<dcpanelconfiguration>\n';
  txt += '    <registersettings>\n';
  for (i=0; i<= 46; i++) {
    txt += C_XML[0] + '\n';
    txt += C_XML[2] + C_RegN[i] + C_XML[3] + '\n';
    txt += C_XML[4] + '0x' + obj[i] + C_XML[5] + '\n';
    txt += C_XML[1] + '\n';
  }
  txt += '    </registersettings>\n';
  txt += '</dcpanelconfiguration>\n';
  fns = 'SmarfRF_' + fileName + '.xml';
 } else {
  alert('development');
  return;
 }

 saveFile(txt,fns);
}