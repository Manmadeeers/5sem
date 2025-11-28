const ws = require('ws');
const fs = require('fs');
const path = require('path');
const WSServerPort = 4000;
const UPLOAD_DIR = path.join(__dirname,'upload');

if(!fs.existsSync(UPLOAD_DIR)){
    fs.mkdirSync(UPLOAD_DIR,{recursive:true});
}

const WSoptions = {
    port: WSServerPort,
    host: 'localhost',
    path: '/upload'
};

const WSserver = new ws.Server(WSoptions);
let fileCounter = 0;

WSserver.on('connection',(serv)=>{
    const wsStream = ws.createWebSocketStream(serv,{encoding:'utf-8'});
    const savePath = path.join(UPLOAD_DIR,`File${++fileCounter}.txt`);
    const wfile = fs.createWriteStream(savePath);
    wsStream.pipe(wfile);

    wfile.on('finish',()=>{
        serv.send("File delivered and saved into 'upload' directory");
    });
    wfile.on('error',(err)=>{
        console.error("Error occured during file delivery: ",err);
        serv.send(`Error occured during file delivery: ${err}`);
    });
});

console.log(`WebSocket server listenint at: ws://${WSoptions.host}:${WSoptions.port}${WSoptions.path}`);



