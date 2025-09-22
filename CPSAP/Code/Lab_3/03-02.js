var http = require('http');
const PORT = 5000;

const server = http.createServer();

const processFact = (numToFact)=>{
    if(numToFact<0){
        return undefined;
    }
    else if(numToFact===0){
        return 1;
    }
    else{
        return numToFact*processFact(numToFact-1);
    }
}   

server.on('request',(request,response)=>{

})

server.listen(PORT);
console.log("Server running at http://localhost:5000/fact");