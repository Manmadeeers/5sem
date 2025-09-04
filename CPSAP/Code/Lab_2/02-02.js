const PORT = 5000;
var http = require('http');
var fs = require('fs');
const { hostname } = require('os');


const server = http.createServer(function (request, response) {
    const fname = './pic.png';
    let png = null;

    if (request.url === "/html") {
        let html = fs.readFileSync('./index.html');
        response.writeHead(200, { 'content-type': 'text/html;charset=utf-8' });
        response.end(html);
        
    }
    else if(request.url==="/png"){
        fs.stat(fname,(err,stat)=>{
            if(err){
                console.log('Error:',err);
            }
            else{
                png = fs.readFileSync(fname);
                response.writeHead(200,{'content-type':'image/png','content-length':stat.size});
                response.end(png,'binary');
            }
        })
        
    }
    else{
        response.writeHead(404,{'content-type':'text/plain'});
        response.end('404 Not Found');
    }

});

server.listen(PORT,)



console.log("Server running at https://localhost:5000");
