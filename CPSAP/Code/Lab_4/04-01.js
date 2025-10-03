var http = require('http');
var fs = require('fs');
var url = require('url');
var data_module = require('./Db_module');
const PORT = 5000;

var db = new data_module.DB();

db.on('GET', async(request, response) => {
    console.log("DB.GET triggered");
    response.writeHead(200,{'content-type':'application/json;charset=utf-8'});
    response.end(JSON.stringify(db.select()))
});


db.on('POST', async (request, response) => {
    console.log("DB.POST triggered");
    request.on('data',data=>{
        let dat = JSON.parse(data);
        db.insert(dat);
        response.writeHead(200,{'content-type':'application/json;charset=utf-8'});
        response.end(JSON.stringify(dat));
    });
});


db.on('PUT',async(request,response)=>{
    console.log("DB.PUT triggered");
    request.on('data',data=>{
        let dat = JSON.parse(data);
        db.update(dat);
        response.writeHead(200,{'content-type':'application/json;charset=utf-8'});
        response.end(JSON.stringify(db.select()));
    })
})


// db.on('DELETE',(request,response)=>{
//     console.log("DB.PUT triggered");
//     request.on('data',data=>{
//         let parsed = JSON.parse(data);
//         db.put(parsed);
//         response.end(JSON.stringify(db.get()));
//     });
// });




const server = http.createServer(function (request,response){
    if(url.parse(request.url).pathname==="/api/db"){
        db.emit(request.method,request,response);
    }
    else{
        response.writeHead(404,{'content-type':'text/html'});
        response.end(
            "<h1>404 Not Found</h1>"
        );
    }
});

server.listen(PORT);


console.log("Server running at http://localhost:5000/api/db");

