const RPC = require('rpc-websockets').Client;
const ws = new RPC('ws://localhost:3000');

ws.on('open', () => {
    console.log('connected');
    ws.subscribe('server-notification');
});

ws.on('server-notification', (notification) => {
    console.log('new notification: ')
    console.log(`   id: ${notification.id}`);
    console.log(`   msg: ${notification.msg}`);
    console.log(`   time: ${new Date(notification.timestamp).toString()}`);
});

ws.on('close', () => {
    console.log('connection closed');
});