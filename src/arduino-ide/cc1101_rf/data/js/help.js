/*
  JavaScript for website Help /help - cc110x_rf_Gateway
  Copyright (c) 2022 <HomeAutoUser & elektron-bbs>
  URL: https://github.com/HomeAutoUser/cc1101_rf_Gateway
*/


var js = document.createElement("script");
js.src = '/all.js';
document.head.appendChild(js);

// Show the received message 
function onMessage(event) {
  console.log('received message: ' + event.data);

  if(event.data == 'Connected') {
    websocket.send('help');
  }

  if (event.data.includes('CC1101":"no') ) {
    document.getElementById("StatCC1101").innerHTML = "";
  }
}
