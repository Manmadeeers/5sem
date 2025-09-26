var http = require('http');
var fs = require('fs');
var url = require('url');
const { error } = require('console');
const PORT = 5000;

function factorial(n, callback) {
    if (isNaN(n) || n < 0) {
        return callback(null);
    }
    if (n === 0) {
        return callback(1);
    }

    let result = 1;

    function calculate(num) {
        result *= num;
        if (num > 1) {
            process.nextTick(() => calculate(num - 1));
        }
        else {
            callback(result);
        }
    }
    calculate(n);
}


const serverFunction = function (request, response) {
    const parsedUrl = url.parse(request.url, true);

    const parsedNum = parseInt(parsedUrl.query.k);

    if (request.method === "GET" && parsedUrl.pathname === "/fact" && !isNaN(parsedNum)) {
        //console.log(parsedNum);
        factorial(parsedNum,(processedNum)=>{
            if(processedNum!=null){
                //console.log(processedNum);
                response.writeHead(200,{'content-type':'application/json;charset=utf-8'});
                response.end(JSON.stringify(
                    {
                        k:parsedNum,
                        fact:processedNum
                    }
                ))
            }
            else{
                response.writeHead(400,{'content-type':'application/json;charset=utf-8'});
                response.end(JSON.stringify({
                    error:"Invalid factorial number"
                }))
            }
        })

    }
    else if (request.method === "GET" && request.url === "/fact") {
        let html = fs.readFileSync("./factorial.html");

        response.writeHead(200, { 'content-type': 'text/html' });
        response.end(html);
    }
    else {
        response.writeHead(404, { 'content-type': 'text/html' });
        response.end(
            '<h1>404 Not Found</h1>'
        );
    }
}

var server = http.createServer(serverFunction);

server.listen(PORT);


console.log("Server running at http://localhost:5000/fact");

//time passed in 1 folder: 573ms
//time passed in 2 folders: 509ms(1st folder), 617ms(2nd folder)
//time passed in 3 folders: 303ms(1st folder), 647ms(2nd folder), 463ms(3rd folder)