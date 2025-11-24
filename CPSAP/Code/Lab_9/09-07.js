const http = require('http');
const fs = require('fs');

const bound = "----mnmd";
let requestBody = `--${bound}\r\n`;
requestBody += 'content-disposition:attachment; name="uploadFile"; filename="MyFile.png"\r\n';
requestBody += 'content-type:application/octet-stream\r\n\r\n';

const options = {
    host: 'localhost',
    path: '/image',
    port: 5000,
    method: 'POST',
    headers: {
        'content-type': 'multipart/form-data; boundary=' + bound,
    }
}


const request = http.request(options, (response) => {
    console.log('Response status code: ', response.statusCode);
    let responseBody = '';
    response.on('data', (chunk) => {
        responseBody += chunk;
    });
    response.on('end', () => {
        console.log('Response body: ', responseBody);
    });
});

request.on('error', (err) => {
    console.error('Error occured: ', err);
});
request.write(requestBody);
let stream = fs.ReadStream('./MyFile.png');
stream.on('data', (chunk) => {
    request.write(chunk);
});
stream.on('end', () => {
    request.end(`\r\n--${bound}--\r\n`);
});