var http = require('http');
var fs = require('fs');

var server = http.createServer(function (request, response) {
    if (request.url === "/html") {
        let html = fs.readFileSync('./index.html');
        response.writeHead(200, { 'content-type': 'text/html;charset=utf-8' });
        response.end(html);
    }
    else{
        let error = fs.readFileSync('./error.html');
        response.writeHead(404,{'content-type':'text/html;charser=utf-8'});
        response.end(error);
    }

})
server.listen(5000);

console.log("Server running at http://localhost:5000/html")