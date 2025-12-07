const net = require('net');
const HOST = 'localhost';
const PORT = 2000;

const client = new net.Socket();
client.connect(PORT,HOST,()=>{
    console.log(`Client connected. Address: ${client.remoteAddress},Port: ${client.remotePort}`);
});

client.write('Hello from client');

client.on('data',(data)=>{
    console.log(`Received message: ${data}`);
    client.destroy();
});

client.on('close',()=>{
    console.log('Client closed');
});

client.on('error',(err)=>{
    console.log(`Error occured. Error:${err}`);
});