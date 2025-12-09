const net = require('net');
const HOST = 'localhost';
const PORT = 2000;
let serverSUM = 0;

const SERVER = net.createServer();

SERVER.on('connection',(socket)=>{
    console.log(`Server connected: ${socket.remoteAddress} ${socket.remotePort}`);

    socket.on('data',(chunk)=>{
        console.log("Server data: ",chunk,serverSUM);
        serverSUM+=chunk.readInt32LE();
    });

    let buff = Buffer.alloc(4);
    setInterval(()=>{
        buff.writeInt32LE(serverSUM,0);
        socket.write(buff);
    },5000);

    socket.on('close',()=>{
        console.log('Server closed: ',socket.remoteAddress+' '+socket.remotePort);
    });

    socket.on('error',(err)=>{
        console.log("Error occured: ",err);
    });
});

SERVER.on('listening',()=>{
    console.log("TCP Server: ",HOST+ ' '+ PORT);
});

SERVER.on('error',(err)=>{
    console.log("Server error: ",err);
});

SERVER.listen(PORT,HOST);