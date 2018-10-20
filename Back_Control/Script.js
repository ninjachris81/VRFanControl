var conn = new WebSocket('ws://' + location.hostname + ':81/');

setInterval(function() {
	if (conn.readyState==WebSocket.OPEN) {
		conn.send('refresh');
	}
}, 2000);

function sendData(elem, cmd) {
	if (conn.readyState==WebSocket.OPEN) {
		conn.send('@' + cmd + elem.previousElementSibling.id.substr(5) + '' + parseInt(elem.previousElementSibling.innerHTML));
	}
}

function sendCmd(cmd, mod, v) {
	if (conn.readyState==WebSocket.OPEN) {
		conn.send('@' + cmd + mod + v);
	}
}

conn.onerror = function (error) {
  console.error('WS ', error);
};

conn.onmessage = function (e) {
  if (e.data.length==4 && e.data.charAt(0)=='@') {
	var mod = e.data.charAt(2);
	
    var eid = 'eid_' + e.data.charAt(1,1) + mod;
	var value = e.data.charCodeAt(3,3) - 48;
	
	if (document.getElementById(eid)) {
		document.getElementById(eid).innerHTML = value;
	} else {
		console.error('Unknown eid: ', eid);
	}
  } else if (e.data.length>4 && e.data.charAt(0)=='[') {
	  var data = JSON.parse(e.data);
	  for (var i=0;i<data.length;i++) {
		  document.getElementById('f' + i).innerHTML = data[i];
	  }
  } else {
    console.error('WS ', 'invalid data: ', e.data);
  }
};