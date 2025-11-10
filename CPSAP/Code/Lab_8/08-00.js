const http = require('http');
const path = require('path');
const fs = require('fs');
const url = require('url');
const { Socket } = require('dgram');
const { setFips } = require('crypto');
const PORT = 5000;
const SERVER_KILL_TIMEOUT = 10000;//10seconds in ms

const connections = new Set();



const serverFunction = function (request, response) {
    const parsed = url.parse(request.url, true);
    const pathname = parsed.pathname;
   // console.log("Pathname: " + pathname);
    const query = parsed.query;

    const parts = pathname.split('/').filter(p => p.length > 0);

    if (request.method === 'GET') {
        if (pathname == '/') {
            const page = fs.readFileSync('./index.html');
            response.writeHead(200, { 'content-type': 'text/html' });
            response.end(page);
        }
        else if (pathname == '/connection') {
            if (!query.set) {
                response.writeHead(200, { 'content-type': 'application/json;charsete=utf-8' });
                response.end(JSON.stringify(server.keepAliveTimeout));
            }
            else {
                if (!isNaN(query.set) && isFinite(query.set)) {
                    server.keepAliveTimeout = parseInt(query.set)
                    response.writeHead(200, { 'content-type': 'application/json;charset=utf-8' });
                    response.end(JSON.stringify(server.keepAliveTimeout));
                }
            }
        }
        else if (pathname == '/headers') {
            response.setHeader('X-Mnmd-Header', 'SomeData');
            response.writeHead(200, { 'content-type': 'application/json;charset=utf-8' });

            const responseHeaders = response.getHeaders();
            let JSON_response = {
                Req: request.headers,
                Res: responseHeaders
            }

            response.end(JSON.stringify(JSON_response));
        }
        else if (pathname == '/parameter') {

            if (query.x && query.y) {
                let x = query.x;
                let y = query.y;
                if (!isNaN(x) && !isNaN(y)) {
                    let JSON_response = {
                        Sum: parseFloat(x) + parseFloat(y),
                        Substraction: x - y,
                        Multiplication: x * y,
                        Division: x / y
                    };
                    response.writeHead(200, { 'content-type': 'application/json;charset=utf-8' });
                    response.end(JSON.stringify(JSON_response));
                }
                else {
                    response.writeHead(400, { 'content-type': 'text/html;charset=utf-8' });
                    response.end("<h1>400 Bad request(One or More Parameters Was Not a Number)</h1>");
                }
            }
        }
        else if (parts.length == 3) {
            if (parts[0] == 'parameter') {
                const xRaw = parts[1];
                const yRaw = parts[2];
                if (!isNaN(xRaw) && !isNaN(yRaw)) {
                    const x = Number(xRaw);
                    const y = Number(yRaw);
                    let JSON_response = {
                        Sum: x + y,
                        Substraction: x - y,
                        Multiplication: x * y,
                        Division: x / y
                    }
                    response.writeHead(200, { 'content-type': 'application/json;charset=utf-8' });
                    response.end(JSON.stringify(JSON_response));
                }
                else {
                    console.log(`xRaw: ${xRaw}; yRaw: ${yRaw}`);
                    response.writeHead(200, { 'content-type': 'application/json;charset=utf-8' });
                    response.end(JSON.stringify(request.url));
                }
            }
        }
        else if (pathname == '/close') {
            response.writeHead(200, { 'content-type': 'application/json;charset=utf-8' });
            response.end(JSON.stringify("Server will be killed in 10 seconds"));

            console.log("Shutdown scheduled. Server porcess will be killed in 10 seconds");
            shutdownTimer = setTimeout(() => {
                console.log("Shutdown started...");
                server.close(err=>{
                    if(err){
                        console.error("Failed to close server",err.message);
                        process.exit(1);
                    }
                    else{
                        console.log("Server closed");
                        for(const socket of connections){
                            try{
                                socket.destroy()
                            }
                            catch(err){
                                console.warn(err.message);
                            }
                        }
                        setTimeout(()=>{
                            console.log("Timeout passed. Server is shutting down");
                            process.exit(0);
                        },SERVER_KILL_TIMEOUT);
                    }
                });
                
            });
        }
        else if(pathname=='/socket'){
            const sock = request.socket||request.connection;
            let JSON_response = {
                Server:{
                    IP:sock.localAddress,
                    Port:sock.localPort
                },
                Client:{
                    IP:sock.remoteAddress,
                    Port:sock.remotePort
                }
            };
            response.writeHead(200,{'content-type':'application/json;charset=utf-8'});
            response.end(JSON.stringify(JSON_response));
        }
        else if(pathname=='/req-data'){
            
        }
        else {
            response.writeHead(400, { 'content-type': 'text/html;charset=utf-8' });
            response.end("<h1>404 Not Found</h1>");
        }

    }
    else {
        response.writeHead(405, { 'content-type': 'text/html;charset=utf-8' });
        response.end("<h1>405 Method Not Alowed</h1>");
    }
}


const server = http.createServer(serverFunction);

server.listen(PORT);
server.on('error', (err) => {
    console.log('Server error: ', err.message);
})

server.on('connection',(socket)=>{
    connections.add(socket);
    socket.on('close',()=>{
        connections.delete(socket);
    });
});

console.log(`Server listening at http://localhost:${PORT}/`);
