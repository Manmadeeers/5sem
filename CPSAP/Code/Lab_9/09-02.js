const http = require('http');
const qs = require('querystring');


let params = qs.stringify({x:10,y:20});
let path = `/get-query?${params}`;

let options = {
    host:'localhost',
    path:path,
    port:5000,
    method:'GET'
};

const request = http.request(options,(response)=>{

    console.log("Response status code: ",response.statusCode);
    let responseBody = '';
    response.on('data',(chunk)=>{
        responseBody+=chunk;
    });
    response.on('end',()=>{
        console.log("Response body: ",responseBody);
    });
});

request.on('error',(err)=>{
    console.error('Error: ',err);
});
request.end();