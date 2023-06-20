/*
  JavaScript for alls website´s
  Copyright (c) 2022 <HomeAutoUser & elektron-bbs>
  URL: https://github.com/HomeAutoUser/cc1101_rf_Gateway

  --- Data update via Websocket ---

*/

// Your IP address instead of ${window.location.hostname}
var wsurl = 'ws://' + window.location.host + ':81' + window.location.pathname;
var websocket;

// Add a listener to initialize websocket on page load
window.addEventListener('load', onload);

// On page load, initialize websocket
function onload(event) {
  initWebSocket();
}

// Setup all the required callback functions for the websocket events
function initWebSocket() {
  console.log('Trying to open a WebSocket connection…');
  websocket = new WebSocket(wsurl);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage;
}

// Connected
function onOpen(event) {
  console.log('Connected ' + window.location.host);
}

// Disconnected
function onClose(event) {
  console.log('Disconnected ' + window.location.host);
}
