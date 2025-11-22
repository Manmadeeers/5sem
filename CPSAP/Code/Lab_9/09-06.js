const http = require('http');
const fs = require('fs');
const { buffer } = require('stream/consumers');

let bound = "----mnmd";
let body = `--${bound}\r\n`;
body += 'content-disposition:form-data; name="uploadFile"; filename="MyFile.txt"\r\n';
body += 'content-type:text/plain\r\n\r\n';
body += fs.readFileSync("./MyFile.txt");
body += `\r\n--${bound}--\r\n`;

let options = {
    host: 'localhost',
    path: '/file',
    port: 5000,
    method: 'POST',
    headers: {
        'content-type': 'multipart/form-data; boundary=' + bound,
        'content-length':body.length
    }
};



const request = http.request(options, (response) => {
    console.log('Response status code: ',response.statusCode);
    let responseBody = '';
    response.on('data',(chunk)=>{
        responseBody+=chunk;
    });
    response.on('end',()=>{
        console.log('Response body: ',responseBody);
    })
});

request.on('error', (err) => {
    console.error(err);
});

request.end(body);