"use strict";

var PORT = 515;
var HOST = '127.0.0.1';

var dgram = require('dgram');

exports.listen = function(server) {

	var cApp = dgram.createSocket('udp4');

	console.log("hello");

	cApp.on('listening', function () {
	    var address = cApp.address();
	    console.log('UDP Server listening on ' + address.address + ":" + address.port);
	});

	cApp.on('message', function (message, remote) {
	    console.log(remote.address + ':' + remote.port +' - ' + message);
	    var msgArray = message.toString().split(":");

	    var commandType = msgArray[0];

	    if (commandType == "triggerAlarm") {
	    	var question = msgArray[1];
	    	var answer = msgArray[2];

	    	console.log("Question: " + question);
	    	console.log("Answer: " + answer);

	    }
	    else if (commandType == "stopAlarm") {

	    }

	});

	cApp.bind(PORT, HOST);
};