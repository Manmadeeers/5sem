const http = require('http');
const fs = require('fs');
const path = require('path');
const PORT = 5000;

const MIME = {
    html: 'text/html;charset=utf-8',
    css: 'text/css',
    js: 'text/javascript',
    png: 'image/png',
    docx: 'application/msword',
    json: 'application/json;charset=utf-8',
    xml: 'application/xml',
    mp4: 'video/mp4'
};

const serverFunction = function (request, response) {
    if (request.method === "GET") {
        
    }
    else {
        response.writeHead(405, { 'contnt-type': 'text/html;charset=utf-8' });
        response.end("<h1>405 Method not alowed</h1>");
    }
}


const server = http.createServer(serverFunction);

server.listen(PORT);

console.log(`Server running at http://localhost:${PORT}`);