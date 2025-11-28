const ws = require('ws');
const fs = require('fs');
const path = require('path');
const wss = new ws('ws://localhost:4000/upload');

if(process.argv.length<3){
    console.error("Usage: node 11-01a.js <FileName>");
    process.exit(1);
}

const filePath = process.argv[2];
if(!fs.existsSync(filePath)){
    console.error("File not found: ",filePath);
    process.exit(2);
}

const fileName = path.basename(filePath);
const buffer = fs.readFileSync(filePath);

wss.on('open',()=>{
    const headers = {
        fileName,
        size:buffer.length
    }

    wss.send(JSON.stringify(headers));

    wss.send(buffer,{},(err)=>{
        if(err){
            console.error("Error occured: ",err);
            wss.close();
            return;
        }

        console.log("File sent. Filename: ",fileName);
    });
}); 

wss.on('message',(message)=>{
    console.log("Server response: ",message);
});

wss.on('close',()=>{
    console.log("Connection closed");
});
wss.on('error',(err)=>{
    console.log("Error occured: ",err);
});