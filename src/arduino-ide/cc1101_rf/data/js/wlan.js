document.onreadystatechange = function () {
if (document.readyState == 'complete') {
var p = "^[\\d]{1,3}\\.[\\d]{1,3}\\.[\\d]{1,3}\\.[\\d]{1,3}$";
var t = "Input: only [\d]{1,3} . [\d]{1,3} . [\d]{1,3} . [\d]{1,3}";

let obj = document.getElementsByName('dns')[0];
obj.maxLength = 15;
obj.pattern = p;
obj.title = t;
obj = document.getElementsByName('gw')[0];
obj.maxLength = 15;
obj.pattern = p;
obj.title = t;
obj = document.getElementsByName('hiddenssid')[0];
obj.maxLength = 32;
obj.pattern =   "^[_-\\da-zA-Z]{1,32}$";
obj.title = "Input: only [a-zA-Z] [\d] [_-] {1,32}";
obj = document.getElementsByName('ip')[0];
obj.maxLength = 15;
obj.pattern = p;
obj.title = t;
obj = document.getElementsByName('pw')[0];
obj.maxLength = 64;
obj.setAttribute('autocomplete', 'current-password');
obj = document.getElementsByName('sn')[0];
obj.maxLength = 15;
obj.pattern = p;
obj.title = t;
}
}

function wlan_conn() {
document.getElementById('StatCon').innerHTML = `- new wlan settings -`;
setTimeout(function(){window.location.href='wlan'},45000);
}

function wlan_wps() {
document.getElementById('StatCon').innerHTML = `- WPS starting -`;
setTimeout(function(){window.location.href='wlan'},45000);
}
