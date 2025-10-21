var http = require('http');
var fs = require('fs');
var nodemailer = require('nodemailer');
var url = require('url');
var querystring = require('querystring');
const PORT = 5000;

const transporter = nodemailer.createTransport({
    service: 'gmail',
    auth: {
        user: 'philmade6@gmail.com',
        pass: 'qrro xauj awnt zgsp'
    }
});



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

        let reply_flag = false;
        request.on('end', () => {
            const { from, to, message } = querystring.parse(content);

            console.log(`Retrieved from: ${from}`);

            console.log(`Retrieved to: ${to}`);

            console.log(`Retrieved message: ${message}`);

            const mailOptions = {
                from: from,
                to: to,
                subject: 'Lab6 sendmail message',
                text: message
            }

            transporter.sendMail(mailOptions, (err, info) => {
                if (err) {
                    console.log("Error occured: ",err);
                    response.writeHead(500,{'content-type':'text/html;charset=utf-8'});
                    response.end(
                        "<h1>500 Failed to send an email</h1>"+
                        `${err}`
                    );
                }
                if(info){
                    console.log("Mail successfully sent");
                    response.writeHead(200,{'content-type':'text/html;charset=utf-8'});
                    response.end(
                        "<h1>200 Mail successfully sent</h1>"+
                        `Accepted: ${info.accepted}`+
                        `Envelope: ${info.envelope}`+
                        `Message ID: ${info.messageId}`+
                        `Pending: ${info.pending}`+
                        `Rejected: ${info.rejected}`+
                        `Response${info.response}`
                    );
                }

            })
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

