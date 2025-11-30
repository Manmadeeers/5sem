const WebSocket = require('ws');
const ws = new WebSocket('ws://localhost:4000/broadcast');
if (process.argv.length < 3) {
  console.error("Usage: node 11-03a.js <ClientName>");
  process.exit(1);
}
const CLIENT_NAME = process.argv[2];

ws.on('open', () => console.log('connected'));
ws.on('message', m => console.log('Message:', m.toString()));
ws.on('ping', (data) => {
  console.log('received ping:', data ? "" : data);
  ws.pong(`Pong: ${CLIENT_NAME}`);
});
ws.on('close', () => console.log('closed'));
ws.on('error', console.error);