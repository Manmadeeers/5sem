//broadcast client
const ws = require('ws');
const wss = new ws('ws://localhost:5000/broadcast');

if(process.argv.length<3){
    console.error("Usage: node 10-03a.js <ClientId>");
    process.exit(1);
}
const clientId = process.argv[2];

wss.on('open',()=>{
    wss.on('message',(message)=>{
        console.log(`Received from broadcast server: ${message}`);
        wss.send(`Hello from client identified by: ${clientId}`);
    });
});