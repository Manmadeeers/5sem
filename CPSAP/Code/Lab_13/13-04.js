let net = require('net');

let HOST = '127.0.0.1';
let PORT = 40000;

let client = new net.Socket();
let buf = new Buffer.alloc(4);
let timerId = null;

client.connect(PORT, HOST, ()=>{
    console.log('Client connected: ', client.remoteAddress + ' ' + client.remotePort);
    let k = 0;
    timerId = setInterval(() => {client.write((buf.writeInt32LE(k++, 0), buf))}, 1000);
    setTimeout(() => {clearTimeout(timerId); client.end();}, 20000);
})

client.on('data', (data) => {console.log('client data: ', data.readInt32LE());});

client.on('close', () => {console.log('client close');});
client.on('error', (e) => {console.log('client error', e);});