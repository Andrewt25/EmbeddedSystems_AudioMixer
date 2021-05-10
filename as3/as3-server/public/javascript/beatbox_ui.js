"var strict";

var socket = io.connect();
$(document).ready(function() {

    window.setInterval(function() {sendPrimeCommand('getVolume')}, 1000);
    window.setInterval(function() {sendPrimeCommand('getMode')}, 1000);
    window.setInterval(function() {sendPrimeCommand('getBpm')}, 1000);
    window.setInterval(function() {
        socket.emit('updateTime')
    }, 1000);

    $('#volumeUp').click(function() {
        var msg = sendPrimeCommand("volUp");
    });
    $('#volumeDown').click(function() {
        sendPrimeCommand("volDown");
    });


    $('#bpmUp').click(function() {
        var msg = sendPrimeCommand("bpmUp");
    });
    $('#bpmDown').click(function() {
        sendPrimeCommand("bpmDown");
    });


    $('#modeNone').click(function() {
        sendPrimeCommand("mode 0");
    });
    $('#modeRock1').click(function() {
        sendPrimeCommand("mode 1");
    });
    $('#modeRock2').click(function() {
        sendPrimeCommand("mode 2");
    });


    $('#base').click(function() {
        sendPrimeCommand("base");
    });
    $('#hihat').click(function() {
        sendPrimeCommand("hihat");
    });
    $('#snare').click(function() {
        sendPrimeCommand("snare");
    });

    socket.on("commandReply", function(reply){
        var msg = reply.split(' ');
        var updateId = msg[0];
        switch(updateId){
            case 'volume':
                $('#volumeid').val(msg[1]);
                break;
            case 'bpm':
                $('#bpmid').val(msg[1]);
                break;
            case 'mode':
                var display = msg.slice(1, msg.length).join(' ');
                $('#modeid').text(display);
                break;
            case 'time':
                $('#timeid').text(msg[1]);
        }
    })
})

function sendPrimeCommand(message) {
    socket.emit('prime', message);
}