const net = require('net');

let HOST = '127.0.0.1';
let PORT1 = 40000;
let PORT2 = 50000;

let connections = new Map();

let server1 = net.createServer();
let server2 = net.createServer();

let h = (server) => {
    return (socket) => {
        let serverInterval = null;
        console.log(`server connected: ${socket.remoteAddress}:${socket.remotePort}`);

        socket.id = (new Date()).toISOString();

        connections.set(socket.id, 0);
        console.log('socket ID: ', socket.id);

        server.getConnections((e, c) => {
            if (!e) {
                console.log(`connected: ${socket.remoteAddress}:${socket.remotePort} ` + c);
                for (let [key, value] of connections) {
                    console.log(key, value);
                }
            }
        });

        socket.on('data', (data) => {
            console.log(`data: ${socket.remoteAddress}:${socket.remotePort} ` + data.readInt32LE());
            connections.set(socket.id, `ECHO: ${data.readInt32LE()}`);
            socket.write(connections.get(socket.id));
        });

        socket.on('error', err => {
            console.log(`error: ${socket.remoteAddress}:${socket.remotePort} ` + err.message);
            clearInterval(serverInterval);
            connections.delete(socket.id)
        });

        socket.on('close', () => {
            console.log(`close: ${socket.remoteAddress}:${socket.remotePort} ` + socket.id);
            clearInterval(serverInterval);
            connections.delete(socket.id);
        });
    };
};
server1.on('connection', h(server1));

server1.listen(PORT1, HOST).on('listening', () => {
    console.log(`\nServer is listening: ${HOST}:${PORT1}`)
});

server2.on('connection', h(server2));

server2.listen(PORT2, HOST).on('listening', () => {
    console.log(`\nServer is listening: ${HOST}:${PORT2}`)
});