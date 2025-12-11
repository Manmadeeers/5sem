const net = require('net');

let HOST = '127.0.0.1';

let client = new net.Socket();

let buf = Buffer.alloc(4);

let timerId = null;

let PORT = Number(process.argv[2] ? process.argv[2] : 40000);
let x = Number(process.argv[3] ? process.argv[3] : 1);

client.connect(PORT, HOST, () => {
    console.log(`Client connected to ${client.remoteAddress}:${client.remotePort}`);

    timerId = setInterval(() => {
        buf.writeInt32LE(x, 0);
        client.write(buf);
    }, 1000);

    setTimeout(() => {
        clearInterval(timerId);
        client.end();
    }, 20000);
});

client.on('data', data => {
    console.log(`client data: ${data}`);
});

client.on('close', () => {
    console.log('client close');
});

client.on('error', err => {
    console.log(`client error: ${err}`);
});