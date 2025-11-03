const http = require('http');
const fs = require('fs');
const path = require('path');
const PORT = 5000;



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