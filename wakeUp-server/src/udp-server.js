"use strict";

var socketio = require('socket.io');
var fs = require('fs');
var io;

var dgram = require('dgram');

// Info for connecting to the local process via UDP
var PORT = 12345;
var HOST = '127.0.0.1';

// var server1 = dgram.createSocket('udp4');

// server1.on('listening', function () {
//     var address = server1.address();
//     console.log('UDP Server listening on ' + address.address + ":" + address.port);
// });

// server1.on('message', function (message, remote) {
//     console.log(remote.address + ':' + remote.port +' - ' + message);

// });

exports.listen = function(server) {
	io = socketio.listen(server);
	io.set('log level 1');
	
	io.sockets.on('connection', function(socket) {
		handleCommand(socket);
	});
};

function handleCommand(socket) {

	socket.on('handshake', function(){
		console.log('recieved connection');
	});

	socket.on('initArray', function(data) {
	
		var buffer = new Buffer("initArray:" + data);	

		var client = dgram.createSocket('udp4');
		client.send(buffer, 0, buffer.length, PORT, HOST, function(err, bytes) {
		    if (err) 
		    	throw err;
		    console.log('UDP message sent to ' + HOST +':'+ PORT);
		});
		
		client.on('listening', function () {
		    var address = client.address();
		    console.log('UDP Client: listening on ' + address.address + ":" + address.port);
		});
		// Handle an incoming message over the UDP from the local application.
		client.on('message', function (message, remote) {
		    console.log("UDP Client: message Rx" + remote.address + ':' + remote.port +' - ' + message);
		    
		    var reply = message.toString('utf8')
		    socket.emit('commandReply', reply);
		    
		    client.close();

		});
	});

	socket.on('createAlarm', function(data) {
	
		var buffer = new Buffer("createAlarm:" + data);	
		
		var client = dgram.createSocket('udp4');
		client.send(buffer, 0, buffer.length, PORT, HOST, function(err, bytes) {
		    if (err) 
		    	throw err;
		    console.log('UDP message sent to ' + HOST +':'+ PORT);
		});
		
		client.on('listening', function () {
		    var address = client.address();
		    console.log('UDP Client: listening on ' + address.address + ":" + address.port);
		});
		// Handle an incoming message over the UDP from the local application.
		client.on('message', function (message, remote) {
		    console.log("UDP Client: message Rx" + remote.address + ':' + remote.port +' - ' + message);
		    
		    var reply = message.toString('utf8')
		    socket.emit('commandReply', reply);
		    
		    client.close();

		});
	});

	socket.on('editAlarm', function(data) {
	
		var buffer = new Buffer("editAlarm:" + data);	
		
		var client = dgram.createSocket('udp4');
		client.send(buffer, 0, buffer.length, PORT, HOST, function(err, bytes) {
		    if (err) 
		    	throw err;
		    console.log('UDP message sent to ' + HOST +':'+ PORT);
		});
		
		client.on('listening', function () {
		    var address = client.address();
		    console.log('UDP Client: listening on ' + address.address + ":" + address.port);
		});
		// Handle an incoming message over the UDP from the local application.
		client.on('message', function (message, remote) {
		    console.log("UDP Client: message Rx" + remote.address + ':' + remote.port +' - ' + message);
		    
		    var reply = message.toString('utf8')
		    socket.emit('commandReply', reply);
		    
		    client.close();

		});
	});

	socket.on('deleteAlarm', function(data) {
	
		var buffer = new Buffer("deleteAlarm:" + data.toString());	
		
		var client = dgram.createSocket('udp4');
		client.send(buffer, 0, buffer.length, PORT, HOST, function(err, bytes) {
		    if (err) 
		    	throw err;
		    console.log('UDP message sent to ' + HOST +':'+ PORT);
		});
		
		client.on('listening', function () {
		    var address = client.address();
		    console.log('UDP Client: listening on ' + address.address + ":" + address.port);
		});
		// Handle an incoming message over the UDP from the local application.
		client.on('message', function (message, remote) {
		    console.log("UDP Client: message Rx" + remote.address + ':' + remote.port +' - ' + message);
		    
		    var reply = message.toString('utf8')
		    socket.emit('commandReply', reply);
		    
		    client.close();

		});
	});


	socket.on('changeSound', function(data) {
		
			var buffer = new Buffer("changeSound:" + data.toString());	
			
			var client = dgram.createSocket('udp4');
			client.send(buffer, 0, buffer.length, PORT, HOST, function(err, bytes) {
				if (err) 
					throw err;
				console.log('UDP message sent to ' + HOST +':'+ PORT);
			});
			
			client.on('listening', function () {
				var address = client.address();
				console.log('UDP Client: listening on ' + address.address + ":" + address.port);
			});
			// Handle an incoming message over the UDP from the local application.
			client.on('message', function (message, remote) {
				console.log("UDP Client: message Rx" + remote.address + ':' + remote.port +' - ' + message);
				
				var reply = message.toString('utf8')
				socket.emit('commandReply', reply);
				
				client.close();
	
			});
	});

	socket.on('trigger', function() {

			console.log("GOT IT");
			socket.emit('triggerAlarm');
	});

};
