var http = require('http');

let hMapper = (r)=>{
    let rc = '';
    for(key in r.headers){
        rc+='<h3>'+key+':'+r.headers[key]+'</h3>';
    }
    return rc;
}


http.createServer(function(request,response){
    let b = ''; 
    request.on('data',str=>{b+=str;console.log('data',b)})
    response.writeHead(200,{'content-type':'text/html'});
    request.on('end',()=>response.end(
        '<!DOCTYPE html> <html><head></head>'+
        '<body>'+
        '<h1>Request Structure</h1>'+
        '<h2>'+
        'Method: '+request.method+
        '</h2>'+
        '<h2>'+
        'URI: '+request.url+
        '</h2>'+
        '<h2>'+
        'Protocol Version: '+request.httpVersion+
        '<h2>'+
        'Headers:'+
        '</h2>'+hMapper(request)+
        '<h2>'+
        'Request Body: '+b+
        '</h2>'+
        '</html>'

    ))
}).listen(3000);

console.log("Server running at http://localhost:3000/");