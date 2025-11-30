const ws = require('ws');
const fs = require('fs');
const path = require('path');
const PORT = 4000;
const DOWNLOAD_DIRECTORY = path.join(__dirname,'download');
const SERVER_OPTIONS = {
    port: PORT,
    host: 'localhost',
    path: '/download'
}

const socket = new ws.Server(SERVER_OPTIONS);
socket.on('connection',(client)=>{
    const downalodPath = path.join(DOWNLOAD_DIRECTORY,'FileToDownload.txt');
    const wsStream = ws.createWebSocketStream(client,{encoding:'utf-8'});
    const fsStream = fs.createReadStream(downalodPath);
    fsStream.pipe(wsStream);
});

console.log(`WS server running on ws://${SERVER_OPTIONS.host}:${SERVER_OPTIONS.port}${SERVER_OPTIONS.path}`);