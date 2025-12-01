const ws = require('ws');
const socketOptions = {
    port: 5000,
    host: 'localhost',
    path: '/broadcast'
};

const wss = new ws.Server(socketOptions);

wss.on('connection', (socket) => {

    setTimeout(() => {
        wss.clients.forEach((client) => {
            if (client.readyState == ws.OPEN) {
                client.send("Hello from broadcast server");
            }
        });
    }, 5000);

    socket.on('message', (message) => {
        console.log(`Received from client: ${message}`);
    });

});

console.log(`Broadcast server running at http://${socketOptions.host}:${socketOptions.port}${socketOptions.path}`);