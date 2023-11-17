var js = document.createElement("script");
js.src = '/js/all.js';
document.head.appendChild(js);

function onMessage(event) {
  console.log('received message: ' + event.data);

  if(event.data == 'Connected') {
    websocket.send('help');
  }

  if (event.data.includes('CC1101,no') ) {
    document.getElementById("StatCC1101").innerHTML = "";
  }
}
