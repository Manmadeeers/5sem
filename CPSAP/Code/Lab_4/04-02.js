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


db.on('DELETE',async(request,response)=>{
    console.log("DB.DELETE triggered");
    let parsedURL = url.parse(request.url,true);
    const id = parsedURL.query.id;
    if(!id){
        response.writeHead(400,{'content-type':'text/html;charset=utf-8'});
        response.end('<h1>400 Bad request</h1>')
    }
    const deletedRow = await db.delete(id);
    response.writeHead(200,{'content-type':'application/json;charset=utf-8'});
    response.end(JSON.stringify(deletedRow));
});


const server = http.createServer(function (request,response){
    if(url.parse(request.url).pathname==="/api/db"){
        db.emit(request.method,request,response);
    }
    else if(request.url==="/"){
        let page = fs.readFileSync('./index.html');
        response.writeHead(200,{'content-type':'text/html;charset=utf-8'});
        response.end(page);
    }
    else{
        response.writeHead(404,{'content-type':'text/html'});
        response.end(
            "<h1>404 Not Found</h1>"
        );
    }
});

server.listen(PORT);


console.log("Server running at http://localhost:5000/");