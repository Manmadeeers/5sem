const udp = require('dgram');
const PORT = 3000;

let server = udp.createSocket('udp4');

server.on('error', error => {
    console.error(error.message);
    server.close();
});

server.on('message', (message, rinfo) => {
   console.log(`От клиента получено сообщение: ${message}`);
   console.log(`Получено ${message.length} байт от ${rinfo.address}:${rinfo.port}`);

   server.send('ECHO:' + message, rinfo.port, rinfo.address, (err) => {
       if (err) {
           console.error(err);
           server.close();
       } else {
           console.log("Данные отправлены клиенту");
       }
   })
});

server.on('listening', () => {
    console.log('Сервер слушает порт: ' + server.address().port);
    console.log('IP-адрес сервера: ' + server.address().address);
    console.log('Семейство ' + server.address().family);
});

server.on('close', () => {
    console.log('Server: сокет закрыт');
});

server.bind(PORT);