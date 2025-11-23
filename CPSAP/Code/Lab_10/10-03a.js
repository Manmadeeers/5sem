const ws = require('ws');
const wss = new ws('ws://localhost:5000/broadcast');

wss.on('open',()=>{
    setInterval(()=>{
        wss.send('Hello from broadcast client');
    },3000);

    wss.on('message',(message)=>{
        console.log(`Received broadcast message: ${message}`);
    });
});