const http = require('http');


let jsonRequest = JSON.stringify({x:1,y:2,z:'something'});

let options = {
    host:'localhost',
    path:'/post-json',
    post:5000,
    method:'POST',
    headers:{
        'content-type':'application/json',
        'accept':'application/json'
    }
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