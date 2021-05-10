"use strict"

var socketio = require('socket.io');
var io;

var dgram = require('dgram');
var fs = require('fs');

exports.listen = function(server) {
    io = socketio.listen(server);
    io.set('log level 1');

    io.sockets.on('connection', function(socket) {
        handleCommand(socket);
    });
} 

function handleCommand(socket) {
    socket.on('prime', function(data) {
        console.log('prime command: ' + data);

        var PORT = 12345;
        var HOST = '127.0.0.1';
        var buffer = new Buffer(data);

        var client = dgram.createSocket('udp4');
        client.send(buffer, 0, buffer.length, PORT, HOST, function(err, bytes) {
            if(err){
                throw err;
            }
            console.log('UDP message sent to ' + HOST + ':' + PORT);
        });

        client.on('listening', function() {
            var address = client.address();
            console.log("UDP Client: listening on " + address.address + ":" + address.port);
        });

        client.on('message', function(message, remote) {
            console.log("UDP Client: message Rx" + remote.address + ":" + remote.port + ' - ' + message);

            var reply = message.toString('utf-8');
            socket.emit('commandReply', reply);

            client.close();

        });

        client.on('UDP Client: close', function() {
            console.log("closed");
        })

        client.on('UDP Client: error', function(err) {
            console.log("error: " + err);
        })
    })

    socket.on('updateTime',  function(){
        fs.readFile('/proc/uptime', 'utf8', function(err, data){
            if(err){
                console.log("Unable to ReadFile: " + err);
            } else {
                var uptime = Number(data.split(' ')[0]);
                var hours = Math.floor((uptime/3600));
                var min = Math.floor((uptime%3600)/60);
                var sec = Math.floor(uptime%60);
                socket.emit('commandReply', "time " + hours +":" + min + ":" + sec)
            }    
        })
       ;
    })
}