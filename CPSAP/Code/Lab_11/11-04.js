const ws = require('ws');
const PORT = 4000;
const SERVER_OPTIONS = {
    port:PORT,
    host:'localhost',
    path:'/json'
};
const server = new ws.Server(SERVER_OPTIONS);
let messageCounter = 0;

server.on('connection',(client)=>{
    
    client.on('message',(message)=>{
        let receivedMessage = JSON.parse(message);
        console.log('Message from client: ',receivedMessage);

        client.send(JSON.stringify({server:++messageCounter,client:receivedMessage.client,timestamp: Date.now()}));
    });
});

console.log(`Server running on ws://${SERVER_OPTIONS.host}:${SERVER_OPTIONS.port}${SERVER_OPTIONS.path}`);