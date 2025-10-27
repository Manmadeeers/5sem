const http = require('http');
const fs = require('fs');
const PORT = 5000;


const serverFunction = function(request,response){
    if(request.method==="GET"){

    }
    else{
        response.writeHead(405,{'c'})
    }
}


const server = http.createServer(serverFunction);

server.listen(PORT);

console.log(`Server running at http://localhost:${PORT}`);