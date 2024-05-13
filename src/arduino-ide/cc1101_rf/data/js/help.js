var js = document.createElement("script");
js.src = '/js/all.js';
document.head.appendChild(js);

function WebSocket_MSG(event) {
 console.log(`received message: ` + event.data);

 if(event.data == 'Connected') {
  websocket.send('help');
 }

 if (event.data.includes('CC110x,no') || event.data.includes('RFM69,no')) {
  document.getElementById("StatCC110x").innerHTML = "";
 }
}
