const udp = require('dgram');
const HOST = '127.0.0.1';
const PORT = 3000;
let client = udp.createSocket('udp4');


let message = 'Client message\0';

client.on('message', msg => {
    console.log(`Получено сообщение: ${msg.toString()} (${msg.length} байт)`);
    client.close();
});

client.send(message, PORT, HOST, error => {
    if (error) {
        console.error(error.message);
        client.close();
    }
    else {
        console.log('Сообщение отправлено серверу.');
    }
});

client.on('error', error => {
    console.error(error.message);
    client.close();
});

client.on('close', () => { console.log('Closed'); });