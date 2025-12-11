let net = require('net');

let HOST = '0.0.0.0';
let PORT = 40000;

let connections = new Map();

let server = net.createServer();

let h = (server) => {
    return (socket) => {
        let serverInterval = null;
        console.log('Server connected: ', socket.remoteAddress + ': ' + socket.remotePort);

        socket.id = (new Date()).toISOString();

        connections.set(socket.id, 0);
        console.log('Socket ID: ', socket.id);

        server.getConnections((e, c) => {
            if (!e) {
                console.log('connected: ', socket.remoteAddress + ': ' + socket.remotePort + c);
                for (let [key, value] of connections) {
                    console.log(key, value);
                }
            }
        });

        socket.on('data', (data) => {
            console.log(`data: ${socket.remoteAddress}:${socket.remotePort} ` + data.readInt32LE());
            connections.set(socket.id, connections.get(socket.id) + data.readInt32LE());
            console.log(`sum: ${connections.get(socket.id)}`);
        });

        let buf = Buffer.alloc(4);

        serverInterval = setInterval(() => {
            buf.writeInt32LE(connections.get(socket.id), 0);
            socket.write(buf);
        }, 5000);

        socket.on('error', err => {
            console.log(`ERROR: ${socket.remoteAddress}:${socket.remotePort} ` + err.message);
            clearInterval(serverInterval);
            connections.delete(socket.id)
        });

        socket.on('close', () => {
            console.log(`CLOSED: ${socket.remoteAddress}:${socket.remotePort} ` + socket.id);
            clearInterval(serverInterval);
            connections.delete(socket.id);
        });
    }
}

server.on('connection', h(server));

server.on('listening', () => {console.log('TCP-server ', HOST, PORT);});
server.on('error', (e) => {console.log('TCP-server error', e);});
server.listen(PORT, HOST);