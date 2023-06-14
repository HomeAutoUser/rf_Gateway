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

    location.reload();

    /* Detect If the Browser Tab has focus */
    if (document.hasFocus()) {
      console.log('Tab is active')
    } else {
      console.log('Tab is inactive')
    }
  }
}
