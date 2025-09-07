const PORT = 5000;
var http = require('http');
var fs = require('fs');

var serverFunction = function (request, response) {
    if (request.url === "/png") {
        
        let pngName = './pic.png';
        let png = null;
        fs.stat(pngName, (err, stat) => {
            if (err) {
                console.log('Error: ', err);
            }
            else {
                png = fs.readFileSync(pngName);
                response.writeHead(200, { 'content-type': 'image/png', 'content-length': stat.size });
                response.end(png);
            }
        })
    }
    else{
        let error = fs.readFileSync('./error.html');
        response.writeHead(404,{'content-type':'text/html'});
        response.end(error);
    }

}

const server = http.createServer(serverFunction);
server.listen(5000);



console.log("Server running at http://localhost:5000/png");
