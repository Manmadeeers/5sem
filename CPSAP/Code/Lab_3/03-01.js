var http = require('http')
var fs = require('fs');


const serverFunction = function(request,response){
    if(request.url==="/"){
        let html = fs.readFileSync('./index.html');
        response.writeHead(200,{"content-type":"text/html;charset=utf-8"});
        response.end(html);
    }
    else{
        response.writeHead(404,{"content-type":"text/html"});
        response.end("<h1>404 Not Found</h1>")
    }
}


const server = http.createServer(serverFunction);

server.listen(5000);

console.log("Server running at http://localhost:5000");