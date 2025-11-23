const ws = require('ws');
const socket = new ws.WebSocket('ws:/localhost:4000/wsserver');

let counter = 0;

socket.on('open', () => {
    counter = 0;
    console.log('socket.onopen');
    setInterval(() => { socket.send(`10-01-client: ${++counter}`); }, 3000);

    setTimeout(() => {
        socket.close();
    }, 25000);

    socket.onclose = (e) => {
        console.log('socket.onclose', e);
    }
    socket.onmessage = (e) => {
        console.log('socket.onmessage: ', e.data);
    }
    socket.onerror = function (error) {
        console.error("Error: ", error.message);
    }
});
