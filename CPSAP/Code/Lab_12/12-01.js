const fs = require('fs');
const http = require('http');
const path = require('path');
const url = require('url');
const PORT = 5000;
const STUDENTS_PATH = path.join(__dirname,'StudentsList.json');

const serverFunction = (req,res)=>{
    const method = req.method;
    const parsed = url.parse(req.url);
    const pathname = parsed.pathname;
    if(method=='GET'){
        switch(pathname){
            case '/':
                if(fs.existsSync(STUDENTS_PATH)){
                    res.writeHead(200,{'content-type':'application/json'});
                    res.end(JSON.stringify(fs.readFileSync(STUDENTS_PATH)))
                }
                else{
                    fs.open('./StudentsList.json','w',(e,file)=>{
                        if(e){
                            console.error('Error occured: ',e);
                        }
                        else{
                            console.log("File was not found. File created");
                        }
                    });
                    res.writeHead(201,{'content-type':'application/json'});
                    res.end(JSON.stringify({}));
                }
            break;

        }
    }
    else if(method=='POST'){

    }
    else if(method=='PUT'){

    }
    else if(method=='DELETE'){

    }
    else{
        res.writeHead(405,{'content-type':'text/plain'});
        res.end('405 Method Not Alowed');
    }
};
const server = http.createServer(serverFunction);
server.listen(PORT);