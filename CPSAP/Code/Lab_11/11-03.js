const ws = require('ws');
const PORT = 4000;
const MESSAGE_INTERVAL = 15000;
const PING_INTERVAL = 5000;
const PONG_WAIT = 200;
const SERVER_OPTIONS = {
    port: PORT,
    host: 'localhost',
    path: '/broadcast'
}
let messagesCounter = 0;
const server = new ws.Server(SERVER_OPTIONS);
const aliveMap = new Map();

server.on('connection', (socket) => {
    aliveMap.set(socket, true);

    socket.on('pong', (data) => {
        aliveMap.set(socket, true);
        console.log('Client pong: ', data.toString());
    });

    socket.on('close', () => {
        aliveMap.delete(socket);
        console.log('client disconnected');
    });

    socket.on('error', (err) => {
        aliveMap.delete(socket);
        console.error(`Error occured: ${err}`);
    });
});

setInterval(() => {
    server.clients.forEach((client) => {
        if (client.readyState == ws.OPEN) {
            client.send(`11-03-Server: ${++messagesCounter}`);
        }
    });
}, MESSAGE_INTERVAL);

setInterval(() => {
    server.clients.forEach((client) => {
        if (client.readyState != ws.OPEN) {
            return;
        }

        const alive = aliveMap.get(client);
        if (alive == false) {
            console.log('Terminating unresponsive client');
            aliveMap.delete(client);
            return client.terminate();
        }

        aliveMap.set(client, false);
        client.ping();
    });

    setTimeout(() => {
        const aliveCount = Array.from(server.clients).filter(c => c.readyState == ws.OPEN && aliveMap.get(c) == true).length;
        console.log(`Alive connections: ${aliveCount}`);
    }, PONG_WAIT)
}, PING_INTERVAL);



console.log(`Server listening at ws://${SERVER_OPTIONS.host}:${SERVER_OPTIONS.port}${SERVER_OPTIONS.path}`);

