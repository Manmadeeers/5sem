var http = require('http');
const { stdin } = require('process');
var readline = require('readline');

const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
})
let app_state = "norm";

const serverFunction = function (request, response) {
    if (request.url === "/") {
        response.writeHead(200, { "content-type": "text/html;charset=utf-8" });
        response.end(
            `<!DOCTYPE html>
        <html lang="en">
            <head>
                <meta charset="UTF-8">
                <meta name="viewport" content="width=device-width, initial-scale=1.0">
                <title>03-01</title>
            </head>
            <body>
                <h1 id="state-display">${app_state}</h1>
            </body>
        </html>`
        );
    }
    else {
        response.writeHead(404, { "content-type": "text/html" });
        response.end("<h1>404 Not Found</h1>")
    }
}


const server = http.createServer(serverFunction);

server.listen(5000);

process.stdin.setEncoding('utf-8');
process.stdin.on('readable', () => {
    let chunk = null;
    while ((chunk = process.stdin.read()) != null) {

    }
})
console.log("Server running at http://localhost:5000");