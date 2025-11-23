const http = require('http');
const ws = require('ws');
const fs = require('fs');
const HTTPServerPort = 3000;
const WSServerPort = 4000;

let socketCounter = 0;

const WSoptions = {
    port: WSServerPort,
    host: 'localhost',
    path: '/wsserver'
};

const serverFunction = (request, response) => {
    if (request.method == 'GET') {
        if (request.url == '/start') {
            let responseBody = fs.readFileSync('./StartWS.html');
            response.writeHead(200, { 'content-type': 'text/html' });
            response.end(responseBody);
        }
        else {
            response.writeHead(400, { 'content-type': 'text/html' });
            response.end('<h1>400 Bad Request</h1>');
        }
    }
    else {
        response.writeHead(405, { 'content-type': 'text/html' });
        response.end('<h1>405 Method Not Alowed</h1>');
    }
}

const HTTPserver = http.createServer(serverFunction);
HTTPserver.listen(HTTPServerPort);

const WSserver = new ws.Server(WSoptions);
WSserver.on('connection', (serv) => {
    socketCounter = 0;
    let clientMessageNumber = '';

    setInterval(() => {
        socketCounter++;
        serv.send(`10-01-server: ${clientMessageNumber}->${socketCounter}`);
    }, 5000);

    serv.on('message', (message) => {
        socketCounter++;
        clientMessageNumber = message.slice("10-01-client: ".length, message.length).toString();

        console.log(`WebSocket server received a message: ${message}`);
        serv.send(`Server got the message (${message})`);
    });
});

WSserver.on('error', (err) => {
    console.log(`WebSocket server error: ${err}`);
});

console.log(`HTTP server listening at http://localhost:${HTTPServerPort}/start`);
console.log(`WebSocket server listening at ws:/${WSserver.options.host}:${WSServerPort}/${WSserver.options.path}`);