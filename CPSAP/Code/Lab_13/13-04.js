const { time } = require('console');
const { connect } = require('http2');
const net = require('net');
const HOST = 'localhost';
const PORT = 2000;

const client = new net.Socket();
let buff = new Buffer.alloc(4);
let timerId = null;

client.connect(PORT,HOST,()=>{
    console.log("Client connected: ",client.remoteAddress+ ' '+client.remotePort);
    let k=0;

    timerId = setInterval(()=>{
        client.write((buff.writeInt32LE(k++,0),buff))
    },1000);

    setTimeout(()=>{
        clearInterval(timerId);
        client.end();
    },20000);
});

client.on('data',(data)=>{
    console.log('Received data: ',data.readInt32LE());
});

client.on('close',()=>{
    console.log('Client closed');
});

client.on('error',(err)=>{
    console.log("Client error: ",err);
});