const ws = require('ws');
const socketOptions = {
    port:5000,
    host:'localhost',
    path:'/broadcast'
};

const wss = new ws.Server(socketOptions);

wss.on('connection',(socket)=>{
    
    socket.on('message',(message)=>{
       console.log(`Received message: ${message}`);
    });
    setInterval(()=>{
        socket.send('Hello from broadcast server');
    },5000);
});

console.log(`Broadcast server running at http://${socketOptions.host}:${socketOptions.port}${socketOptions.path}`);