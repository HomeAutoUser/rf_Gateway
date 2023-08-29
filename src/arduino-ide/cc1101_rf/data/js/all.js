var wsurl = 'ws://' + window.location.host + ':81' + window.location.pathname;
var websocket;
window.addEventListener('load', onload);

function onload(event) {
 initWebSocket();
}

function initWebSocket() {
 console.log('Trying to open a WebSocket connection…');
 websocket = new WebSocket(wsurl);
 websocket.onopen = onOpen;
 websocket.onclose = onClose;
 websocket.onmessage = onMessage;
}

function onOpen(event) {
 console.log('Connected ' + window.location.host);
}

function onClose(event) {
 console.log('Disconnected ' + window.location.host);
}
