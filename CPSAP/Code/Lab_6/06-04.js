const http = require('http');
// const {send} = require('m0603_mnmd');   //for local repository
const {send} = require('/Users/Manmade/AppData/Roaming/npm/node_modules/m0603_mnmd');   //for global repository
const querystring = require('querystring');
const fs = require('fs');
const PORT = 5000;


const serverFunction = function (request, response) {
    if (request.url === "/" && request.method === "GET") {
        response.writeHead(200, { 'content-type': 'text/html;charset=utf-8' });
        response.end(fs.readFileSync('./m0603.html'));
    }
    else if (request.url === "/" && request.method === "POST") {
        console.log("Attempted to send a message");

        let content = '';
        request.on('data', chunk => {
            console.log("Chunk: ", chunk.toString());
            content += chunk.toString();
        });

        console.log(`Retrieved content: ${content}`);

        request.on('end', () => {
            const { message, password } = querystring.parse(content);

            console.log("Retrieved message: " + message);
            console.log("Retrieved password: " + password);

            if (message) {
                send(message, password)
                    .then(info => {
                        response.writeHead(200, { 'content-type': 'text/html;charset=utf-8' });
                        response.end("<h1>200 Mail successfully sent</h1>");
                    })
                    .catch(err => {
                        response.writeHead(500, { 'content-type': 'text/html;charset=utf-8' });
                        response.end("<h1>500 Failed to send an email</h1>");
                    })
            }



        });

    }
    else {
        response.writeHead(404, { 'content-type': 'text/html;charset=utf-8' });
        response.end("<h1>404 Not Found</h1>");
    }
}

const server = http.createServer(serverFunction);

server.listen(PORT);


console.log(`Server running at http://localhost:${PORT}/`);
