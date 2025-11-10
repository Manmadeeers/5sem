const http = require('http');
const path = require('path');
const fs = require('fs');
const PORT = 5000;


const serverFunction = function(request,response){
    if(request.method==='GET'){
        let uri = request.url;
        switch(uri){
            case '/connection':
                
        }
    }
    else if(request.method==='POST'){

    }
    else{
        response.writeHead(405,{'content-type':'text/html;charset=utf-8'});
        response.end("<h1>405 Method Not Alowed</h1>");
        return;
    }
}


const server = http.createServer(serverFunction);


console.log(`Server listening at http://localhost:${PORT}/`);