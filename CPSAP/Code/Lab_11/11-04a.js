const ws = require('ws');
if(process.argv.length<3){
    console.error("Usage: node 11-04a.js <ClientName>");
    process.exit(1);
}
const MESSAGE_INTERVAL = 5000;
const CLIENT_NAME = process.argv[2];
const client = new ws("ws://localhost:4000/json");

client.on('open',()=>{
    client.on('message',(message)=>{
        console.log('Received from server: ',JSON.parse(message));
    });
    setInterval(()=>{
        client.send(JSON.stringify({client:CLIENT_NAME,timestamp:Date.now()}));
    },MESSAGE_INTERVAL);
});
