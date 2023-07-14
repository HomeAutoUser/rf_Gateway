var store = document.querySelector(':root');  /* Get the CSS root element */
var value = getComputedStyle(store);          /* Get the styles (properties and values) for the root */
var color1 = value.getPropertyValue('--rssi_good');
var color2 = value.getPropertyValue('--rssi_bad');

var js = document.createElement("script");
js.src = '/all.js';
document.head.appendChild(js);

function onMessage(event) {
 console.log('received message: ' + event.data);

 if (event.data.includes('RAW,') ) {
  var time = new Date().toLocaleTimeString();
  const obj=event.data.split(',');

  document.getElementById("MODE").innerHTML = obj[1];
  // Update Data Table
  var table = document.getElementById("dataTable");
  var row = table.insertRow(1);  // Add after headings
  var cell1 = row.insertCell(0);
  var cell2 = row.insertCell(1);
  var cell3 = row.insertCell(2);
  var cell4 = row.insertCell(3);
  // Time
  cell1.innerHTML = time;
  cell1.style.textAlign = "center";
  // RAW
  cell2.innerHTML = obj[2];
  cell2.style.fontFamily = "Courier New,Lucida Console";
  // RSSI
  cell3.innerHTML = obj[3];
  if(obj[3] <= -80) {
   cell3.style.color = color2;
   cell3.style.textAlign = "right";
  }else if(obj[3] > -50) {
   cell3.style.color = color1;
   cell3.style.textAlign = "right";
  }else{
   cell3.style.textAlign = "right";
  }
  cell4.innerHTML = obj[4];
  cell4.style.textAlign = "right";
 } else if (event.data.includes('MODE,')) {
  const obj=event.data.split(',');
  document.getElementById("MODE").innerHTML = obj[1];
 }
}
