var wsurl = 'ws://' + window.location.host + ':81' + window.location.pathname;
var websocket;
window.addEventListener('load', Website_loaded);

var FXOSC_C;
var FXOSC_SX;
var Fstep_SX;

function Website_loaded(event) {
 WebSocket_Init();
}

function WebSocket_Init() {
 console.log('Trying to open a WebSocket connection…');
 websocket = new WebSocket(wsurl);
 websocket.onopen = WebSocket_open;
 websocket.onclose = WebSocket_close;
 websocket.onmessage = onMessage;
}

function WebSocket_open(event) {
 console.log('Connected ' + window.location.host);

 if (document.getElementById("FXc")) {
   FXOSC_C = document.getElementById("FXc").textContent;
   FXOSC_SX = document.getElementById("FXr").textContent;
   Fstep_SX = FXOSC_SX / 524288;
 }
}

function WebSocket_close(event) {
 console.log('Disconnected ' + window.location.host);
}
