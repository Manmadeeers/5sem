const http = require('http');
const qs = require('querystring');

let params = qs.stringify({x:10,y:20,z:'something'});

let options = {
    host:'localhost',
    path:`/post-query?${params}`,
    port:5000,
    method:'POST'
};

const request = http.request(options,(response)=>{
    console.log('Response status code: ',response.statusCode);

    let responseBody = '';
    response.on('data',(chunk)=>{
        responseBody+=chunk;
    });

    response.on('end',()=>{
        console.log('Response body: ',responseBody);
    });
});

request.on('error',(err)=>{
    console.log('Error: ',err);
});

request.end();