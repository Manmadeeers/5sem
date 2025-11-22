const http = require('http');
const fs = require('fs');

const file = fs.createWriteStream('Server-info.txt');

const options = {
    host:'localhost',
    path:'/download/server-info.txt',
    port:5000,
    method:'GET',
};


const request = http.request(options,(response)=>{
    response.pipe(file);
});

request.on('error',(err)=>{
    console.log('Error: ',err);
});
request.end();