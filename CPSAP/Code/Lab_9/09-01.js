const http = require('http');

let options = {
    host:'localhost',
    path:'/get',
    port:5000,
    method:'GET'
}

const request = http.request(options,(response)=>{
    console.log('Response status code: ',response.statusCode);
    console.log('Response message: ',response.statusMessage);
    console.log('Server IP address: ',response.socket.remoteAddress);
    console.log('Server port: ',response.socket.remotePort);

    let responseBody = '';
    response.on('data',(chunk)=>{
        responseBody+=chunk;
    });
    response.on('end',()=>{
        console.log('Response body: ',responseBody);
    });
});

request.on('error',(err)=>{
    console.error('Error: ',err);
});
request.end();