var http = require('http');
var url = require('url');
var fs = require('fs');
const PORT = 5000;

function factorial(n) {
    if (isNaN(n)) {
        return null;
    }
    if (n < 0) {
        return null;
    }
    if (n == 0) {
        return 1;
    }
    return n * factorial(n - 1);
}

const serverFunction = function (request, response) {
    const parsedUrl = url.parse(request.url, true);

    const parsedNum = parseInt(parsedUrl.query.k);

    if (request.method==="GET"&&parsedUrl.pathname==="/fact"&&!isNaN(parsedNum)) {
       // console.log(parsedNum);
        const processedNum = factorial(parsedNum);
        //console.log(processedNum);
        if (processedNum != null) {
            response.writeHead(200, { 'content-type': 'application/json;charset=utf-8' });
            response.end(JSON.stringify({
                k: parsedNum,
                fact: processedNum
            }));
        }
        else{
            response.writeHead(400,{'content-type':'application/json;charset=utf-8'});
            response.end(JSON.stringify({
                error:"Factoriated number was less then 0 or not a number"
            }));
        }

    }
    else if(request.method==="GET"&&request.url==="/fact"){
        let html = fs.readFileSync("./factorial.html");

        response.writeHead(200,{'content-type':'text/html'});
        response.end(html);
    }
    else{
        response.writeHead(404,{'content-type':'text/html'});
        response.end(
            '<h1>404 Not Found</h1>'
        );
    }
}

const server = http.createServer(serverFunction);

server.listen(PORT);

console.log("Server running at http://localhost:5000/fact");


//time passed in one folder: 645ms
//time passed in two folders: 752ms(1st folder), 494ms(2nd folder)
//time passed in three folders: 465ms(1st folder), 707ms(2nd folder), 623ms(3rd folder)