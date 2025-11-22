const http = require('http');


let requestObj = {
    "_comment":"Request:lab9",
    "x":1,
    "y":10,
    "s":"message",
    "m":["a","b","c"],
    "o":{"surename":"Ivanov","name":"Ivan"}
};

let jsonRequest = JSON.stringify(requestObj);

let options = {
    host:'localhost',
    path:'/post-json',
    port:5000,
    method:'POST',
    headers:{
        'content-type':'application/json',
        'accept':'application/json',
    }
};

const request = http.request(options,(response)=>{
    console.log('Response status code: ',response.statusCode);
    let responseBody = '';
    response.on('data',(chunk)=>{
        responseBody+=chunk;
    });
    response.on('end',()=>{
        console.log('Response body: ',JSON.parse(responseBody));
    });
});


request.on('error',(err)=>{
    console.error(err);
});
request.end(jsonRequest);