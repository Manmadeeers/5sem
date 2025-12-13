const http = require('http');
const mssql = require('mssql');
const url = require('url');
const path = require('path');
const PORT = 3000;

const HandleNotAlowed = (request, response) => {
    response.writeHead(405, { 'content-type': 'application/json' });
    response.end(JSON.stringify({ error: "405 Method Not Alowed" }));
    return;
}

const HandleNotFound = (request,response)=>{
    response.writeHead(404,{'content-type':'application/json'});
    response.end(JSON.stringify({error:"404 Not Found"}));
    return;
}

const serverFunction = (request, response) => {
    const parsed = url.parse(request.url);
    const pathname = parsed.pathname;
    let path = pathname.split('/')[1] + '/' + pathname.split('/')[2];
    const method = request.method;

    if (method == 'GET') {
        if(pathname=='/'){

        }
        else{
            HandleNotFound(request,response);
        }
    }
    else if (method == 'POST') {

    }
    else if (method == 'PUT') {

    }
    else if (method == 'DELETE') {

    }
    else {
        HandleNotAlowed(request,response);
    }
}

const server = http.createServer(serverFunction);

server.listen(PORT);

console.log(`Server listening at http://localhost:${PORT}/`);