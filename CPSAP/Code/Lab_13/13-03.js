let net = require('net');

let HOST = '0.0.0.0';
let PORT = 40000;

let sum = 0;

let server = net.createServer();
server.on('connection', sock => {

    console.log('Server connected: ' + sock.remoteAddress + ': ' + sock.remotePort);

    sock.on('data', (data) => {
        console.log('Server DATA: ', data, sum);
        sum += data.readInt32LE();        
    });

    let buf = Buffer.alloc(4);
    setInterval(() => {buf.writeInt32LE(sum, 0); sock.write(buf)}, 5000);

    sock.on('close', () => console.log('socket closed: ', sock.remoteAddress + ' ' + sock.remotePort))
    sock.on('error', () => console.log('socket error: ', sock.remoteAddress + ' ' + sock.remotePort))

})

server.on('listening', () => {console.log('TCP-server ', HOST, PORT);});
server.on('error', (e) => {console.log('TCP-сервер error', e);});
server.listen(PORT, HOST);