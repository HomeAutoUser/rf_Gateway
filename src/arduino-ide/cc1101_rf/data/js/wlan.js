document.onreadystatechange = function () {
 if (document.readyState == 'complete') {
  var p = "^[\\d]{1,3}\\.[\\d]{1,3}\\.[\\d]{1,3}\\.[\\d]{1,3}$";
  var t = "Input: only [\d]{1,3} . [\d]{1,3} . [\d]{1,3} . [\d]{1,3}";

  document.getElementsByName('dns')[0].maxLength = "15";
  document.getElementsByName('dns')[0].pattern = p;
  document.getElementsByName('dns')[0].title = t;
  document.getElementsByName('gw')[0].maxLength = "15";
  document.getElementsByName('gw')[0].pattern = p;
  document.getElementsByName('gw')[0].title = t;
  document.getElementsByName('hiddenssid')[0].maxLength = "32";
  document.getElementsByName('hiddenssid')[0].pattern =   "^[_-\\da-zA-Z]{1,32}$";
  document.getElementsByName('hiddenssid')[0].placeholder = "hidden ssid";
  document.getElementsByName('hiddenssid')[0].title = "Input: only [a-zA-Z] [\d] [_-] {1,32}";
  document.getElementsByName('ip')[0].maxLength = "15";
  document.getElementsByName('ip')[0].pattern = p;
  document.getElementsByName('ip')[0].title = t;
  document.getElementsByName('pw')[0].maxLength = "64";
  document.getElementsByName('pw')[0].placeholder = "current password";
  document.getElementsByName('pw')[0].setAttribute('autocomplete', 'current-password');
  document.getElementsByName('sn')[0].maxLength = "15";
  document.getElementsByName('sn')[0].pattern = p;
  document.getElementsByName('sn')[0].title = t;
 }
}

function wlan_conn() {
  document.getElementById("StatCon").innerHTML = "- new wlan settings -";
}

function wlan_wps() {
  document.getElementById("StatWPS").innerHTML = "- WPS starting -";
}
