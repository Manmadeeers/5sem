const ws = require('ws');
const fs = require('fs');
const path = require('path');
const socket = new ws("ws://localhost:4000/download");
const DOWNLOAD_FILE_NAME = 'Downloaded.txt';
const DOWNLOADED_PATH = path.join(__dirname, DOWNLOAD_FILE_NAME);

if (!fs.existsSync(DOWNLOADED_PATH)) {
    fs.writeFileSync(DOWNLOADED_PATH, '');
}

socket.on('open', () => {
    const wsStream = ws.createWebSocketStream(socket, { encoding: 'utf-8' });
    const fsStream = fs.createWriteStream(DOWNLOADED_PATH);
    wsStream.pipe(fsStream);
});