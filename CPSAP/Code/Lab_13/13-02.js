let net = require('net');

let HOST = '127.0.0.1';
let PORT = 40000;

let client = new net.Socket();
client.connect(PORT, HOST, ()=>console.log('Client connected:' + client.remoteAddress + ': ' + client.remotePort));

client.write('Hello\0');

client.on('data', (data) => {console.log(data.toString()); client.destroy();});

client.on('close', () => {console.log('client closed')});

client.on('error', () => {console.log('client error');})