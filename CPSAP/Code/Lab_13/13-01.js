const net = require('net');
const HOST = 'localhost';
const PORT = 2000;

net.createServer((socket)=>{
    console.log(`Server connected. Address: ${socket.remoteAddress}. Port: ${socket.remotePort}`);
    socket.on('data',(data)=>{
        console.log(`Server received: ${data}`);
        socket.write(`ECHO: ${data}`);
    });

    socket.on('close',()=>{
        console.log(`Server closed. Address: ${socket.remoteAddress}. Port: ${socket.remotePort}`);
    });
}).listen(PORT,HOST);

console.log(`TCP Server Created. Port:${PORT}, HOST:${HOST}`);