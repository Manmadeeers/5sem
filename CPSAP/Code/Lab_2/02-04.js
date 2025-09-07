var http = require('http');
var fs = require('fs');
const SURENAME = 'Филипюк';
const NAME = 'Илья';
const PATRONIMIC = 'Андреевич';


var serverFunction = function (request, response) {
    if (request.url === "/xmlhttprequest") {
        let xmlhttprequest = fs.readFileSync('./xmlhttprequest.html');
        response.writeHead(200, { 'content-type': 'text-html;charset=utf-8' });
        response.end(xmlhttprequest);
    }
    else if (request.url === "/api/name") {
        response.writeHead(200, { 'content-type': 'text/plain; charset=utf-8' });
         response.end(SURENAME+' '+ NAME+' '+PATRONIMIC);
    }
    else {
        let error = fs.readFileSync('./error.html');
        response.writeHead(404, { 'content-type': 'text/html; charset=utf-8' })
        response.end(error);
    }
}

const server = http.createServer(serverFunction);

server.listen(5000);

console.log("Server running at http://localhost:5000/xmlhttprequest");