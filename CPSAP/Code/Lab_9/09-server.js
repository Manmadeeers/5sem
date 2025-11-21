const http = require('http');
const { json } = require('stream/consumers');
const url = require('url');
const { parseString } = require('xml2js');
const DESIRED_PORT = 5000;

const HandleNotFound = (response) => {
    response.writeHead(404, { 'content-type': 'text/html;charset=utf-8' });
    response.end("<h1>404 Not Found</h1>");
    return;
}

const HandleNotAlowed = (response) => {
    response.writeHead(405, { 'content-type': 'text/html;charset=utf-8' });
    response.end("<h1>405 Method not alowed. Use GET requests only</h1>");
    return;
}

const HandleFirstGet = (response) => {
    response.writeHead(200, { 'content-type': 'text/html;charset=utf-8' });
    response.end("<h1>GET request received</h1>");
    return;
}

const HandleGetQuery = (response, query) => {
    if (query.x && query.y) {
        const x = parseFloat(query.x);
        const y = parseFloat(query.y);

        const respObj = {
            math: x + y
        };

        response.writeHead(200, { 'content-type': 'application/json' });
        response.end(JSON.stringify(respObj));
    }
    else {
        response.writeHead(400, { 'content-type': 'text/html' });
        response.end("<h1>400 bad request</h1>");
    }
    return;
}

const HandlePostQuery = (response, query) => {
    if(query.x&&query.y&&query.z){
        let respObj = {
            x:parseFloat(query.x),
            y:parseFloat(query.y),
            z:query.z
        };

        
        response.writeHead(200,{'content-type':'application/json'});
        response.end(JSON.stringify(respObj));
    }
    else{
        response.writeHead(400,{'content-type':'text/html'});
        response.end("<h1>400 Bad Request</h1>");
    }
    return;
}

const HandlePostJsonEndpoint = (request,response)=>{
    let requestBody = '';
    request.on('data',(chunk)=>{
        requestBody+=chunk;
    });
    request.on('end',()=>{
        let jsonRequest = JSON.parse(requestBody);
        const x = parseFloat(jsonRequest.x);
        const y = parseFloat(jsonRequest.y);
        const sum = x+y;
        response.writeHead(200,{'content-type':'application/json'});
        response.end(JSON.stringify({sum:sum,str:jsonRequest.z}));
    });
}

const serverFunction = function (request, response) {

    const parsed = url.parse(request.url, true);
    const query = parsed.query;
    const pathname = parsed.pathname;
    console.log(pathname);

    if (request.method === 'GET') {
        switch (true) {
            case pathname == '/get':
                HandleFirstGet(response);
                break;
            case pathname == '/get-query':
                HandleGetQuery(response, query);
                break;
            default:
                HandleNotFound(request,response);
                break;
        }
    }
    else if (request.method === 'POST') {
        switch (true) {
            case pathname == '/post-query':
                HandlePostQuery(response, query);
                break;
                case pathname == '/post-json':
                    HandlePostJsonEndpoint(response);
                    break;

            default:
                HandleNotFound(response);
                break;
        }
    }
    else {
        HandleNotAlowed(response);
    }
}

const server = http.createServer(serverFunction);
server.listen(DESIRED_PORT);

console.log(`Server running at http://localhost:${DESIRED_PORT}/`);