var http = require('http');
var fs = require('fs');
var sendmail = require('sendmail')({ 
    silent: false,
    smtpPort:1025,
    smtpHost:"127.0.0.1",
 });
var url = require('url');
const { resourceUsage, send } = require('process');
var querystring = require('querystring');
const PORT = 5000;



const serverFunction = function (request, response) {
    if (request.url === "/" && request.method === "GET") {
        response.writeHead(200, { 'content-type': 'text/html;charset=utf-8' });
        response.end(fs.readFileSync('./index.html'));
    }
    else if (request.url === "/" && request.method === "POST") {
        console.log("Attempted to send a message");

        let content = '';
        request.on('data', chunk => {
            console.log("Chunk: ", chunk.toString());
            content += chunk.toString();
        });

        console.log(`Retrieved content: ${content}`);

        let mail_from;
        let mail_to;
        let mail_message;
        request.on('end', () => {
            const { from, to, message } = querystring.parse(content);
            mail_from = from;
            console.log(`Retrieved from: ${from}`);
            mail_to = to;
            console.log(`Retrieved to: ${to}`);
            mail_message = message;
            console.log(`Retrieved message: ${message}`);


            sendmail({
                from: from,
                to: to,
                subject: "Lab6 sendmail message",
                html: `<p>${message}</p>`,
            }, function (err, reply) {
                if(err){
                    console.log("Error occured:", err);
                    response.writeHead(500,{'content-type':'text/html;charset=utf-8'});
                    response.end("<h1>500 Internal Server Error</h1>");
                }
                if(reply){
                    console.log("Got a reply",reply);
                    response.writeHead(200,{'content-type':'text/html;charset=utf-8'});
                    response.end("<h1>200 Email Successfully sent</h1>");
                }
            });
        });

    }
    else {
        response.writeHead(404, { 'content-type': 'text/html;charset=utf-8' });
        response.end("<h1>404 Not Found</h1>");
    }
}

const server = http.createServer(serverFunction);


server.listen(PORT);


console.log("Server running at http://localhost:5000/");

