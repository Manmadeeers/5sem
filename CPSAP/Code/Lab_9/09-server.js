const http = require('http');
const url = require('url');
const { XMLParser, XMLBuilder } = require('fast-xml-parser');
const formidable = require('formidable');
const path = require('path');
const fs = require('fs');
const DESIRED_PORT = 5000;
const staticDirectory = "./static-server";
const parser = new XMLParser(
    {
        ignoreAttributes: false,
        attributeNamePrefix: '',
    }
);
const builder = new XMLBuilder(
    {
        ignoreAttributes: false,
        attributeNamePrefix: '',
        format: true,
        suppressEmptyNode: true,
    }
);

const HandleNotFound = (response) => {
    response.writeHead(404, { 'content-type': 'text/html;charset=utf-8'});
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
    if (query.x && query.y && query.z) {
        let respObj = {
            x: parseFloat(query.x),
            y: parseFloat(query.y),
            z: query.z
        };


        response.writeHead(200, { 'content-type': 'application/json' });
        response.end(JSON.stringify(respObj));
    }
    else {
        response.writeHead(400, { 'content-type': 'text/html' });
        response.end("<h1>400 Bad Request</h1>");
    }
    return;
}

const HandlePostJsonEndpoint = (request, response) => {
    let requestBody = '';
    request.on('data', (chunk) => {
        requestBody += chunk;
    });
    request.on('end', () => {
        let jsonRequest = JSON.parse(requestBody);
        if (jsonRequest) {
            let jsonResponse = {
                _comment: "Response: lab9",
                x_plus_y: Number(jsonRequest.x) + Number(jsonRequest.y),
                concatination_s_0: String(jsonRequest.s).concat(": ").concat(String(jsonRequest.o.surename)).concat(' ').concat(String(jsonRequest.o.name)),
                length_m: jsonRequest.m.length
            };
            response.writeHead(200, { 'content-type': 'application/json' });
            response.end(JSON.stringify(jsonResponse));
        }
        else {
            response.writeHead(400, { 'content-type': 'text/html' });
            response.end("<h1>400 Bad Request</h1>");
        }
    });
    return;
}

const HandlePostXmlEndpoint = (request, response) => {
    let xmlRequest = '';
    request.on('data', (chunk) => {
        xmlRequest += chunk;
    });
    request.on('end', () => {
        if (xmlRequest.length > 0) {
            const parsedXML = parser.parse(xmlRequest);
            if (!parsedXML.request) {
                response.writeHead(400, { 'content-type': 'text/plain' });
                response.end("400 Bad request. Failed to parse <request/> tag");

            }

            let requestId = Number(parsedXML.request.id);

            let arrayX = parsedXML.request.x;
            let arrayY = parsedXML.request.m;

            if (!arrayX) {
                arrayX = [];
            }
            if (!arrayY) {
                arrayY = [];
            }

            let sumX = arrayX.reduce((accum, element) => {
                let value = Number(element.value);
                return accum + value;
            }, 0);

            let sumY = arrayY.map(element => element.value).join('');

            const xmlRaw = {
                response:
                {
                    id: (requestId + 5).toString(),
                    request: requestId,
                    sum:
                    {
                        element: "X",
                        result: sumX.toString(),
                    },
                    concat:
                    {
                        element: 'M',
                        result: sumY,
                    },
                },
            }
            const xmlResponse = builder.build(xmlRaw);
            response.writeHead(200, { 'content-type': 'application/xml' });
            response.end(xmlResponse);
        }
        else {
            response.writeHead(400, { 'content-type': 'text/plain' });
            response.end("400 Bad Request");

        }
    });
    return;
}

const HandlePostFileEndpoint = (request, response) => {
    const form = new formidable.IncomingForm({ multiples: true, keepExtensions: true });
    form.headers = request.headers;
    form.parse(request, (err, fields, files) => {
        if (err) {
            response.writeHead(400, { 'content-type': 'text/plain' });
            response.end("400 Bad Request. Error occured: ", err);
        }

        let uploaded_file = files.uploadFile[0];
        if (!uploaded_file) {
            response.writeHead(400, { 'content-type': 'text/plain' });
            response.end("400 Bad Request. Files not attached");
        }

        let new_file_path = path.join(staticDirectory, uploaded_file.originalFilename);
        fs.rename(uploaded_file.filepath, new_file_path, err => {
            if (err) {
                response.writeHead(400, { 'content-type': 'text/palin' });
                response.end(`400 Bad Request.Error occured: ${err}`);
            }
        });


        response.writeHead(200, { 'content-type': 'text/plain' });
        response.end(`200 Success. File uploaded at ${new_file_path}.\nFile text: ${fs.readFileSync(new_file_path)}`);
    });
    form.on('error', (err) => {
        response.writeHead(400, { 'content-type': 'text/plain' });
        response.end('400 Bad Requset. Error: ', err);
    })
    return;
}

const HandlePostImageEndpoint = (request, response) => {
    const form = new formidable.IncomingForm({ multiples: true, keepExtensions: true });
    form.headers = request.headers;

    form.parse(request, (err, fields, files) => {
        if (err) {
            response.writeHead(400, { 'content-type': 'text/plain' });
            response.end('400 Bad Request.Error: ', err);
        }

        let uploaded_file = files.uploadFile[0];
        if (!uploaded_file) {
            response.writeHead(400, { 'content-type': 'text/plain' });
            response.end("400 Bad Request. Files not attached");
        }

        let new_file_path = path.join(staticDirectory, uploaded_file.originalFilename);
        fs.rename(uploaded_file.filepath, new_file_path, err => {
            if (err) {
                response.writeHead(400, { 'content-type': 'text/palin' });
                response.end(`400 Bad Request.Error occured: ${err}`);
            }
        });

        response.writeHead(200,{'content-type':'text/plain'});
        response.end(`200 Success. File uploaded at ${new_file_path}`,);

    });
    form.on('error',(err)=>{
        response.writeHead(400,{'content-type':'text/plain'});
        response.end('400 Bad Request. Formidable error: ',err);
    });
    return;
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
                HandleNotFound(response);
                break;
        }
    }
    else if (request.method === 'POST') {
        switch (true) {
            case pathname == '/post-query':
                HandlePostQuery(response, query);
                break;
            case pathname == '/post-json':
                HandlePostJsonEndpoint(request, response);
                break;
            case pathname == '/post-xml':
                HandlePostXmlEndpoint(request, response);
                break;
            case pathname == '/file':
                HandlePostFileEndpoint(request, response);
                break;
            case pathname == '/image':
                HandlePostImageEndpoint(request, response);
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