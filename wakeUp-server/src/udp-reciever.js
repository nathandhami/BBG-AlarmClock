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
	});

	cApp.on('message', function (message, remote) {
	    console.log(remote.address + ':' + remote.port +' - ' + message);
	    var msgArray = message.toString().split(":");

	    var commandType = msgArray[0];

	    if (commandType == "triggerAlarm") {
	    	var question = msgArray[1];
	    	var answers;
	    	var isMCQ;

	    	if (msgArray[2]) { 	// MCQ

	    		isMCQ = true;
	    		answers = [msgArray[2], msgArray[3]];
	    		if (msgArray[4]) {
	    			answers.push(msgArray[4]);
	    			answers.push(msgArray[5]);
	    		}

	    	} else {			// Arithemtic
	    		
	    		isMCQ = false;
	    	}

	    	var postData = {
			  	type: isMCQ,
	    		question: question,
	    		options: JSON.stringify(answers),
			}

			var formData = {
				method: 'post',
				body: postData,
				json: true,
				url: 'http://localhost:8088/trigger'
			}

	    	request.post(formData);

	    }
	    else if (commandType == "stopAlarm") {
	    	var formData = {
				method: 'post',
				json: true,
				url: 'http://localhost:8088/stop'
			}

	    	request.post(formData);
	    }

	});

	cApp.bind(PORT, HOST);
};