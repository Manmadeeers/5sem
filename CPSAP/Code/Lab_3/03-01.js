var http = require('http');
const { stdin } = require('process');
var readline = require('readline');

const PORT = 5000;

const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
})

let appState = "norm";

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
                <h1 id="state-display">${appState}</h1>
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

server.listen(PORT, () => {
    console.log(`Server running at http://localhost:${PORT}`);
});

const changeState = () => {
    rl.question(`Current state is "${appState}". Enter new state (norm, stop, idle, exit): `, (input) => {

        input = input.trim().toLowerCase();

        if (input === 'exit') {

            console.log('Exiting the application...');

            rl.close();

            process.exit(0);

        } else if (['norm', 'stop', 'idle'].includes(input)) {

            appState = input;

            console.log(`State changed to: ${appState}`);

        } else {

            console.log(`Invalid input: "${input}". State not changed.`);

        }

        changeState();

    });
}

changeState();  