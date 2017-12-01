"use strict";

var request = require("request");

var PORT = 9088;
var HOST = '127.0.0.1';

var dgram = require('dgram');

exports.listen = function(server) {

	var cApp = dgram.createSocket('udp4');

	cApp.on('listening', function () {
	    var address = cApp.address();
	    console.log('UDP Server listening on ' + address.address + ":" + address.port);
	    cApp.emit("triggerAlarm");
	});

	cApp.on('message', function (message, remote) {
	    console.log(remote.address + ':' + remote.port +' - ' + message);
	    var msgArray = message.toString().split(":");

	    var commandType = msgArray[0];

	    if (commandType == "triggerAlarm") {
	    	var question = msgArray[1];
	    	var answers;
	    	var qType;

	    	console.log("Question: " + question);

	    	if (msgArray[2]) { 	// MCQ

	    		qType = 0;
	    		answers = [msgArray[2], msgArray[3]];
	    		if (msgArray[4]) {
	    			answers.push(msgArray[4]);
	    			answers.push(msgArray[5]);
	    		}

	    		console.log("Answers: " + answers);

	    	} else {			// Arithemtic
	    		
	    		qType = 1;
	    	}

	    	request.get("http://localhost:8088/trigger");

	    }
	    else if (commandType == "stopAlarm") {

	    }

	});

	cApp.bind(PORT, HOST);
};