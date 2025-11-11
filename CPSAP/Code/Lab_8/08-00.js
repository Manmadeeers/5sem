const http = require('http');
const path = require('path');
const fs = require('fs');
const url = require('url');
const qs = require('querystring');
const jsonValidator = require('./json_validator');
const xmlParser = require('xml2js');
const xmlBuilder = require('xmlbuilder');
const xmlHelper = require('./xml_helper');
const { type } = require('os');
const PORT = 5000;
const SERVER_KILL_TIMEOUT = 10000;//10seconds in ms
const connections = new Set();
let connection_count = 0;



const serverFunction = function (request, response) {

    console.log(`Request URL: ${request.url}`);
    const parsed = url.parse(request.url, true);
    const pathname = parsed.pathname;
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
                server.close(err => {
                    if (err) {
                        console.error("Failed to close server", err.message);
                        process.exit(1);
                    }
                    else {
                        console.log("Server closed");
                        for (const socket of connections) {
                            try {
                                socket.destroy()
                            }
                            catch (err) {
                                console.warn(err.message);
                            }
                        }
                        setTimeout(() => {
                            console.log("Timeout passed. Server is shutting down");
                            process.exit(0);
                        }, SERVER_KILL_TIMEOUT);
                    }
                });

            });
        }
        else if (pathname == '/socket') {
            const sock = request.socket || request.connection;
            let JSON_response = {
                Server: {
                    IP: sock.localAddress,
                    Port: sock.localPort
                },
                Client: {
                    IP: sock.remoteAddress,
                    Port: sock.remotePort
                }
            };
            response.writeHead(200, { 'content-type': 'application/json;charset=utf-8' });
            response.end(JSON.stringify(JSON_response));
        }
        else if (pathname == '/req-data') {
            let buffer = '';
            let req_sequence = new Array(0);

            request.on('data', (data) => {
                console.log(`Got chunk with length: ${data.length}`);
                buffer += data;
                req_sequence.push(data.length);
            });
            request.on('end', () => {
                let JSON_response = {
                    Buffer_length: buffer.length,
                    Length_sequence: req_sequence
                }
                console.log(`Final buffer length: ${buffer.length}`);
                response.writeHead(200, { 'content-type': 'application/json;charset=utf-8' });
                response.end(JSON.stringify(JSON_response));
            });

        }
        else if (pathname == '/resp-status') {
            if (!query.code || !query.mess) {
                response.writeHead(400, { 'content-type': 'text/html;charset=utf-8' });
                response.end("<h1>400 Bad Request</h1>");
            }
            else {
                response.statusCode = query.code;
                response.statusMessage = query.mess;
                response.writeHead(query.code, { 'content-type': 'application/json;charset=utf-8' });
                response.end(JSON.stringify(query.mess));
            }
        }
        else if (pathname == '/formparameter') {
            let param_page = fs.readFileSync('./form-parameter.html');
            response.writeHead(200, { 'content-type': 'text/html;charset=utf-8' });
            response.end(param_page);
        }
        else if (pathname == '/upload') {

        }
        else {
            response.writeHead(404, { 'content-type': 'text/html;charset=utf-8' });
            response.end("<h1>404 Not Found</h1>");
        }

    }
    else if (request.method == 'POST') {
        if (pathname == '/formparameter') {
            let JSON_response = new Object();
            let param_result = '';
            request.on('data', (data) => {
                param_result += data;
            });
            request.on('end', () => {
                let qs_parsed = qs.parse(param_result);
                for (let key in qs_parsed) {
                    JSON_response[key] = qs_parsed[key];
                }
                response.writeHead(200, { 'content-type': 'application/json;charset=utf-8' });
                response.end(JSON.stringify(JSON_response));
            })

        }
        else if (pathname == '/json') {
            let json_req = '';
            request.on('data', (data) => {
                json_req += data;
            });
            request.on('end', () => {
                try {
                    let json_parsed = JSON.parse(json_req);
                    console.log(`retrieved JSON: ${JSON.stringify(json_parsed)}`);
                    if (jsonValidator.isJsonAccept(request.headers)) {
                        let JSON_response = {
                            _comment: "Response: lab8",
                            x_plus_y: Number(json_parsed.x) + Number(json_parsed.y),
                            concatination_s_0: String(json_parsed.s).concat(": ").concat(String(json_parsed.o.surename)).concat(' ').concat(String(json_parsed.o.name)),
                            length_m: json_parsed.m.length
                        }
                        response.writeHead(200, { 'content-type': 'application/json;charset=utf-8' });
                        response.end(JSON.stringify(JSON_response));
                    }
                    else {
                        response.writeHead(400, { 'content-type': 'text/html;charset=uft-8' });
                        response.end("<h1>400 Bad Request. JSON was invalid</h1>");
                    }

                }
                catch (err) {
                    response.writeHead(400, { 'content-type': 'text/html;charset=utf-8' });
                    response.end(`<h1>400 Bad JSON. Error: ${err.message}</h1>`);
                }

            });
        }
        else if (pathname == '/xml') {
            let xml_req = '';
            request.on('data', (data) => {
                xml_req += data;
            });
            request.on('end', () => {
                try {
                    const xmlParsed = xmlParser.parseString(xml_req, { explicitArray: false, explicitRoot: false, attrkey: '$' });

                    const xs = xmlHelper.normalize(parsed.x);
                    const ms = xmlHelper.normalize(parsed.m);

                    const sum = xs.map(x => {
                        if (x && typeof x === 'object' && x.$ && x.$.value != undefined) {
                            return Number(x.$.value);
                        }
                        if (x && typeof x === 'object' && x.value != undefined) {
                            return Number(x.value);
                        }
                        return Number(x);
                    }).reduce((acc, n) => acc + (Number.isFinite(n) ? n : 0), 0);

                    const concat = ms.map(m => {
                        if (m && typeof m === 'object' && m.$ && x.$.value != undefined) {
                            return String(m.$.value);
                        }
                        if (m && typeof m === 'object' && m.value != undefined) {
                            return String(m.value);
                        }
                        return String(m);
                    }).join('');

                    const XML_response = {
                        response: {
                            sum: sum.toString(),
                            concat: concat
                        }
                    };

                    const xmlResponse = xmlBuilder.create(XML_response, { version: '1.0', encoding: 'UTF-8', standalone: true });

                    response.writeHead(200,{'content-type':'application/xml'});
                    response.end(xmlResponse);
                }
                catch (err) {
                    console.log(err);
                    response.writeHead(400,{'content-type':'text/html;charset=utf-8'});
                    response.end("<h1>400 Bad Request. XML was invalid");
                }


            });
        }
        else if (pathname == '/files') {

        }
        else if (pathname == '/upload') {

        }
        else {
            response.writeHead(404, { 'content-type': 'text/html;charset=utf-8' });
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

server.on('connection', (socket) => {
    console.log(`Connection #${++connection_count}. KeepAliveTimeout: ${server.keepAliveTimeout}`);
    connections.add(socket);
    socket.on('close', () => {
        connections.delete(socket);
    });
});

server.on('timeout', (socket) => {
    console.log(`timeout: ${server.timeout}`);
});

console.log(`Server listening at http://localhost:${PORT}/`);
