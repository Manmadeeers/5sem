var http = require('http');
var fs = require('fs');


http.createServer(function(request,response){
    let html = fs.readFileSync('./index.html');
    response.writeHead(200,{'content-type':'text/html;charset=utf-8'});
    response.end(html);
}).listen(5000);

console.log("Server running at http://localhost:5000")