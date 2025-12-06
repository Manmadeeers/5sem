const fs = require('fs');
const http = require('http');
const path = require('path');
const url = require('url');
const rws = require('rpc-websockets');
const PORT = 5000;

const STUDENTS_PATH = path.join(__dirname, 'StudentsList.json');

//TODO: async implementation of server function

//TODO: notifications implementation via RPC websockets

const HandleRootGetEP = (req, res) => {
    let jsonContents = fs.readFileSync('./StudentsList.json');
    const parsedStudents = JSON.parse(jsonContents);
    //console.log(parsedStudents);
    if (!Array.isArray(parsedStudents)) {
        res.writeHead(500, { 'content-type': 'application/json' });
        res.end(JSON.stringify({ error: "Invalid file format. Array was expected" }));
    }
    if (parsedStudents.length == 0) {
        res.writeHead(201, { 'content-type': 'application/json' });
        res.end(JSON.stringify(parsedStudents));
    }
    res.writeHead(200, { 'content-type': 'application/json' });
    res.end(JSON.stringify(parsedStudents));
}

const HandleIdGetEP = (req, res, id) => {
    const idNum = Number(id);
    if (!Number.isInteger(idNum) || idNum < 0) {
        res.writeHead(400, { 'content-type': 'application/json' });
        res.end(JSON.stringify({ error: "Id value was invalid" }));
        return;
    }

    try {
        let jsonContents = fs.readFileSync(STUDENTS_PATH);
        const parsedStudents = JSON.parse(jsonContents);
        if (!Array.isArray(parsedStudents)) {
            res.writeHead(500, { 'content-type': 'application/json' });
            res.end(JSON.stringify({ error: "Ivalid file format. Array was expected" }));
            return;
        }
        const student = parsedStudents.find(s => {
            if (s && Object.prototype.hasOwnProperty.call(s, 'id')) {
                return Number(s.id) === idNum;
            }
            return false;
        });
        if (!student) {
            res.writeHead(404, { 'content-type': 'application/json' });
            res.end(JSON.stringify({ error: `Student with id ${idNum} not found` }));
            return;
        }
        res.writeHead(200, { 'content-type': 'application/json' });
        res.end(JSON.stringify(student));
    }
    catch (err) {
        res.writeHead(500, { 'content-type': 'application/json' });
        res.end(JSON.stringify({ error: `Failed to parse JSON from StudentsList.json. Error: ${err}` }));
    }
}

const HandleRootPostEP = (req, res) => {
    let reqBody = '';
    req.on('data', (chunk) => {
        reqBody += chunk;
    });
    req.on('end', () => {

        let reqStudent;
        try {
            reqStudent = JSON.parse(reqBody.toString());
        }
        catch (err) {
            res.writeHead(400, { 'content-type': 'application/json' });
            res.end(JSON.stringify({ error: `Could not parse JSON from request body. Error: ${err}` }));
            return;
        }

        if (!reqStudent.id || !reqStudent.Name || !reqStudent.bday || !reqStudent.speciality) {
            res.writeHead(400, { 'content-type': 'application/json' });
            res.end(JSON.stringify({ error: "Student transmitted in request body has a corrupted structure" }));
            return;
        }

        let studentsArray = [];
        if (fs.existsSync(STUDENTS_PATH)) {
            let filetext = fs.readFileSync(STUDENTS_PATH);
            studentsArray = JSON.parse(filetext);
        }

        let duplicate = false;
        studentsArray.forEach(student => {
            if (student.id == reqStudent.id) {
                duplicate = true;
            }
        });
        if (duplicate) {
            res.writeHead(400, { 'content-type': 'application/json' });
            res.end(JSON.stringify({ error: "Duplicate found. Could not add this element to a file" }));
            return;
        }
        studentsArray.push(reqStudent);
        fs.writeFileSync(STUDENTS_PATH, JSON.stringify(studentsArray, null, 2));
        res.writeHead(201, { 'content-type': 'application/json' });
        //TODO: send notification to a client here
        res.end(JSON.stringify(reqStudent));
    });

    req.on('error', (err) => {
        console.error("Request error occured: ", err);
        return;
    });
}


const HandleRootPutEP = (req, res) => {
    let reqBody = '';
    req.on('data', (chunk) => {
        reqBody += chunk;
    });
    req.on('end', () => {
        if (reqBody == '') {
            res.writeHead(400, { 'content-type': 'application/json' });
            res.end(JSON.stringify({ error: "Bad request. BOdy was null" }));
            return;
        }
        let reqStud;
        try {
            reqStud = JSON.parse(reqBody);
        }
        catch (err) {
            res.writeHead(500, { 'content-type': 'application/json' });
            res.end(JSON.stringify({ error: `Failed to parse JSON from request body. Error: ${err}` }));
        }
    });

    req.on('error', (err) => {
        res.writeHead(500, { 'content-type': 'application/json' });
        res.end(JSON.stringify({ error: `Request failed. Error: ${err}` }));
    })
}

const serverFunction = (req, res) => {

    const method = req.method;
    const parsed = url.parse(req.url);
    const pathname = parsed.pathname;
    console.log(`Request came. Method: ${req.method}. Pathname: ${pathname}`);
    if (method == 'GET') {
        if (pathname == '/') {
            HandleRootGetEP(req, res);
        }
        else if (pathname.startsWith('/') && pathname.split('/').length > 0) {
            const idStr = pathname.slice(1);
            HandleIdGetEP(req, res, idStr);
        }
        else {
            res.writeHead(400, { 'content-type': 'application/json' });
            res.end(JSON.stringify({ error: "Bad request. There's no such endpoint for GET request" }));
        }
    }
    else if (method == 'POST') {
        if (pathname == '/') {
            HandleRootPostEP(req, res);
        }
        else {
            res.writeHead(400, { 'content-type': 'application/json' });
            res.end(JSON.stringify({ error: "Bad request. There's no such endpoint for POST request" }));
        }
    }
    else if (method == 'PUT') {
        if(pathname=='/'){
            HandleRootPutEP(req,res);
        }
        else{
            res.writeHead(400,{'content-type':'application/json'});
            res.end(JSON.stringify({error:"Bad request. There's no such endpoint"}));
        }
    }
    else if (method == 'DELETE') {
        console.log('DELETE');
    }
    else {
        res.writeHead(405, { 'content-type': 'text/plain' });
        res.end('405 Method Not Alowed');
    }
};
const server = http.createServer(serverFunction);
server.listen(PORT);


console.log(`Server running on http://localhost:${PORT}/`);