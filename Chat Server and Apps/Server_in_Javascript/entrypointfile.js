var http = require('http');
var fs = require('fs');
var path = require('path');
let util = require('util');

var WebSocketServer = require('websocket').server;


var server = http.createServer(function (request, response) {
    console.log('request ', request.url);

    var filePath = '.' + request.url;

    if (filePath == './') {
        filePath = './index.html';
    }

    var extname = String(path.extname(filePath)).toLowerCase();
    var mimeTypes = {
        '.html': 'text/html',
        '.js': 'text/javascript',
        '.css': 'text/css',
        '.png': 'image/png',
        '.jpg': 'image/jpg',
    };

    var contentType = mimeTypes[extname] || 'application/octet-stream';

    fs.readFile(filePath, function(error, content) {
        if (error) {
            if(error.code == 'ENOENT') {
                fs.readFile('./404.html', function(error, content) {
                    response.writeHead(404, { 'Content-Type': 'text/html' });
                    response.end(content, 'utf-8');
                });
            }
            else {
                response.writeHead(500);
                response.end('Sorry, check with the site admin for error: ' +error.code+' ..\n');
            }
        }
        else {
            response.writeHead(200, { 'Content-Type': contentType });
            response.end(content, 'utf-8');
        }
    });

});
server.listen(8080, function(){
console.log('Server running at http://127.0.0.1:8080/');
});

wsServer = new WebSocketServer({
    httpServer: server,
    autoAcceptConnections: false
});

let rooms = {};
let connectionToRoomMap = {};            

wsServer.on('request', function(request) {

    var connection = request.accept(null, request.origin);
    console.log(' Connection accepted.');
    connection.on('message', function(message) {
        console.log(message.utf8Data);
        let messageInput = message.utf8Data;
        let parsedMessage = messageInput.split(" ");

        joinRequest = parsedMessage[0];
        roomRequest = parsedMessage[1];

        if(joinRequest === "join"){

                if(roomRequest in rooms){

                    let currentRoom = rooms[roomRequest];
                    currentRoom.user.push(connection);
                    console.log("joined " + roomRequest);
                    for(i = 0;i< currentRoom.messages.length; i++){
                        connection.sendUTF(currentRoom.messages[i]);
                    }
                    connectionToRoomMap[connection] = roomRequest;

                }else{
                    let aroom = { user:[],
                        messages:[]
                        };
                        aroom.user.push(connection);
                        rooms[roomRequest]=aroom;
                        console.log("created and joined " + roomRequest);
                        connectionToRoomMap[connection] = roomRequest;
                }
            }else{
                
                let roomName = connectionToRoomMap[connection];
                console.log(roomName);
                let currentRoom = rooms[roomName];

                let splitMessage = message.utf8Data.split(" ");
                let user = splitMessage[0];
                let rebuildMessage = "";
                for(let i=1; i<splitMessage.length; i++){
                    rebuildMessage += (" " + splitMessage[i])
                }

                let json = JSON.stringify({user:user,message:rebuildMessage});
                currentRoom.messages.push(json);
                console.log("json " + json); 

                for(let user of currentRoom.user){
                    user.sendUTF(json);
                }
            }

    });
    connection.on('close', function(reasonCode, description) {
        console.log(' Peer ' + connection.remoteAddress + ' disconnected.');
    });
});

