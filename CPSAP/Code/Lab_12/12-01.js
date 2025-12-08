const fs = require('fs');
const http = require('http');
const path = require('path');
const url = require('url');
const rws = require('rpc-websockets').Server;
const PORT = 5000;
const RPC_PORT = 3000;
const STUDENTS_PATH = path.join(__dirname, 'StudentsList.json');
const COPY_TIMEOUT = 2000;
const rpc = new rws({
    port:RPC_PORT,
    host:'localhost'
});

rpc.event('server-notification');

const SendNotifications = (notificationMessage)=>{
    let notification = {
        id:Math.random()*10,
        notificationMessage,
        timestamp:Date.now()
    }

    //console.log("Notification sent. Notification message: ",notification);
    rpc.emit('server-notification',notification);
}

rpc.on('conntection',()=>{
    console.log('client connected');
});

rpc.on('disconnection',()=>{
    console.log('client disconnected');
});

fs.watch('./',(event,f)=>{
    if(f&&DateFormatParser(f.split('_')[0])){
        console.log("f split: ",f.split('_')[0]);
        SendNotifications(`Backup file changed. File:${f}, event:${event}`);
    }
});

fs.watch(STUDENTS_PATH,(event,f)=>{
    if(f){
        SendNotifications(`StudentsList.json file changed. Event:${event}`);
    }
});

const FormatHelper = (n) => {
    return n.toString().padStart(2, '0');
}

const DateFormatter = (date = new Date()) => {
    const Y = date.getFullYear();
    const M = FormatHelper(date.getMonth() + 1);
    const D = FormatHelper(date.getDate());
    const H = FormatHelper(date.getHours());
    const m = FormatHelper(date.getMinutes());
    const S = FormatHelper(date.getSeconds());

    return `${Y}${M}${D}${H}${m}${S}`;
}

const DateFormatParser = (dateStr) => {
    if (typeof (dateStr) != 'string') {
        return null;
    }
    if (dateStr.length != 14) {
        return null;
    }
    const Year = dateStr.slice(0, 4);
    const Month = dateStr.slice(4, 6);
    const Day = dateStr.slice(6, 8);
    const Hours = dateStr.slice(8, 10);
    const Minutes = dateStr.slice(10, 12);
    const Seconds = dateStr.slice(12, 14);

    const parsedDate = `${Year}${Month}${Day}${Hours}${Minutes}${Seconds}`;
    return parsedDate;
}

const CompareDates = (fileDate, parsedDate) => {
    const DateYear = fileDate.slice(0, 4);
    const ParsedYear = parsedDate.slice(0, 4);

    const DateMonth = fileDate.slice(4, 6);
    const ParsedMonth = parsedDate.slice(4, 6);

    const DateDay = fileDate.slice(6, 8);
    const ParsedDay = parsedDate.slice(6, 8);

    const DateHours = fileDate.slice(8, 10);
    const ParsedHours = parsedDate.slice(8, 10);

    const DateMinutes = fileDate.slice(10, 12);
    const ParsedMinutes = parsedDate.slice(10, 12);

    const DateSeconds = fileDate.slice(12, 14);
    const ParsedSeconds = parsedDate.slice(12, 14);

    if (ParsedYear > DateYear) {
        return -1
    }
    else if (ParsedYear < DateYear) {
        return 1;
    }
    else {
        if (ParsedMonth > DateMonth) {
            return -1;
        }
        else if (ParsedMonth < DateMonth) {
            return 1;
        }
        else {
            if (ParsedDay > DateDay) {
                return -1;
            }
            else if (ParsedDay < DateDay) {
                return 1;
            }
            else {
                if (ParsedHours > DateHours) {
                    return -1;
                }
                else if (ParsedHours < DateHours) {
                    return 1;
                }
                else {
                    if (ParsedMinutes > DateMinutes) {
                        return -1;
                    }
                    else if (ParsedMinutes < DateMinutes) {
                        return 1;
                    }
                    else {
                        if (ParsedSeconds > DateSeconds) {
                            return -1;
                        }
                        else if (ParsedSeconds < DateSeconds) {
                            return 1;
                        }
                        else return 0;
                    }
                }
            }
        }
    }
}

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

        if (!reqStud.id || !reqStud.Name || !reqStud.bday || !reqStud.speciality) {
            res.writeHead(400, { 'content-type': 'application/json' });
            res.end(JSON.stringify({ error: "Bad request. Request body was invalid" }));
            return;
        }

        let studentsArray = [];
        if (fs.existsSync(STUDENTS_PATH)) {
            let filetext = fs.readFileSync(STUDENTS_PATH);
            studentsArray = JSON.parse(filetext);
        }

        let foundIndex = studentsArray.findIndex(s => s.id == reqStud.id);
        if (foundIndex == -1) {
            res.writeHead(404, { 'content-type': 'application/json' });
            res.end(JSON.stringify({ error: "Element not found" }));
        }
        studentsArray[foundIndex] = reqStud;
        fs.writeFileSync(STUDENTS_PATH, JSON.stringify(studentsArray, null, 2));
        res.writeHead(200, { 'content-type': 'application/json' });
        res.end(JSON.stringify(reqStud));
        return;
    });

    req.on('error', (err) => {
        res.writeHead(500, { 'content-type': 'application/json' });
        res.end(JSON.stringify({ error: `Request failed. Error: ${err}` }));
        return;
    });

}

const HandleRootDeleteEP = (req, res, DelID) => {
    const idNum = Number(DelID);

    if (!Number.isInteger(idNum) || idNum < 0) {
        res.writeHead(400, { 'content-type': 'application/json' });
        res.end(JSON.stringify({ error: "Id value was invalid" }));
        return;
    }

    let studentsArray = [];
    try {
        if (fs.existsSync(STUDENTS_PATH)) {
            let filetext = fs.readFileSync(STUDENTS_PATH);
            studentsArray = JSON.parse(filetext);
        }
        else {
            res.writeHead(404, { 'content-type': 'application/json' });
            res.end(JSON.stringify({ error: "Not Found. Resourse does not exist" }));
            return;
        }
    }
    catch (err) {
        res.writeHead(500, { 'content-type': 'application/json' });
        res.end(JSON.stringify({ error: `Failed to parse JSON. Error: ${err}` }));
        return;
    }

    let foundIndex = studentsArray.findIndex(s => s.id == idNum);
    if (foundIndex == -1) {
        res.writeHead(404, { 'content-type': 'application/json' });
        res.end(JSON.stringify({ error: "Not Found. Element with this index does not exist" }));
        return;
    }
    else {
        let delStudent = studentsArray[foundIndex];

        studentsArray.splice(foundIndex, 1);
        fs.writeFileSync(STUDENTS_PATH, JSON.stringify(studentsArray, null, 2));
        res.writeHead(200, { 'content-type': 'application/json' });
        res.end(JSON.stringify(delStudent));
        return;
    }
}

const HandlePostBackupEP = (req, res) => {
    if (fs.existsSync(STUDENTS_PATH)) {
        setTimeout(() => {
            try {
                fs.copyFile(STUDENTS_PATH, `${DateFormatter()}_StudentsList.json`, (err) => {
                    if (err) {
                        console.log(`CopyFile error occured: ${err}`);
                        res.writeHead(500, { 'content-type': 'appliation/json' });
                        res.end(JSON.stringify({ error: `CopyFile failed. Error: ${err}` }));
                        return;
                    }
                    else {
                        console.log('File copied');
                        res.writeHead(200, { 'content-type': 'application/json' });
                        res.end(JSON.stringify({ info: "File copied" }));
                        return;
                    }
                });
            }
            catch (err) {
                res.writeHead(500, { 'content-type': 'application/json' });
                res.end(JSON.stringify({ error: `CopyFile error occured. Error: ${err}` }));
                return;
            }

        }, COPY_TIMEOUT);
    }
    else {
        res.writeHead(404, { 'content-type': 'application/json' });
        res.end(JSON.stringify({ error: "Not Found. Resourse does not exist" }));
        return;
    }
}

const HandleDeleteBackupEP = (req, res, dateStr) => {

    const parsedDate = DateFormatParser(dateStr);
    if (parsedDate == null) {
        res.writeHead(400, { 'content-type': 'application/json' });
        res.end(JSON.stringify({ error: "Bad Request. Date was invalid or of an invalid format" }));
        return;
    }
    else {
        const files = fs.readdirSync('./', { withFileTypes: true }).filter(e => e.isFile() && path.extname(e.name).toLowerCase() == '.json');
        console.log("All .json files: ", files);

        let deletionCounter = 0;

        for (f of files) {
            if (f.name.split('_').length == 2) {
                const fileDate = f.name.split('_')[0];
                if (CompareDates(fileDate, parsedDate) == -1) {
                    try {
                        fs.unlinkSync(path.join(f.parentPath, f.name));
                        console.log(`File named ${f.name} was deleted`);
                        deletionCounter++;
                    }
                    catch (err) {
                        res.writeHead(500, { 'content-type': 'application/json' });
                        res.end(JSON.stringify({ error: `Failed to delete a file. Error: ${err}` }));
                        return;
                    }

                }
            }
        }

        if (deletionCounter == 0) {
            console.log('No files were deleted. All the dates are not older or equal');
            res.writeHead(200, { 'content-type': 'application/json' });
            res.end(JSON.stringify({ info: "No files were deleted" }));
            return;
        }
        else {
            console.log(`${deletionCounter} files deleted`);
            res.writeHead(200, { 'content-type': 'application/json' });
            res.end(JSON.stringify({ info: `${deletionCounter} files were deleted` }));
            return;
        }
    }
}

const HandleGetBackupEP = (req, res) => {
    let filesArray = [];
    const files = fs.readdirSync('./', { withFileTypes: true }).filter(e => e.isFile() && path.extname(e.name).toLowerCase() == '.json');
    console.log("All .json files: ", files);

    for (f of files) {
        if (f.name.split('_').length == 2) {
            const fileDate = f.name.split('_')[0];

            if (DateFormatParser(fileDate) != null) {
                filesArray.push(f.name);
            }

        }
    }

    if (filesArray.length == 0) {
        res.writeHead(204, { 'content-type': 'application/json' });
        res.end(JSON.stringify(filesArray));
        return;
    }
    else {
        res.writeHead(200, { 'content-type': 'application/json' });
        res.end(JSON.stringify(filesArray));
        return;
    }
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
        else if (pathname == '/backup') {
            HandleGetBackupEP(req, res);
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
        else if (pathname == '/backup') {
            HandlePostBackupEP(req, res);
        }
        else {
            res.writeHead(400, { 'content-type': 'application/json' });
            res.end(JSON.stringify({ error: "Bad request. There's no such endpoint for POST request" }));
        }
    }
    else if (method == 'PUT') {
        if (pathname == '/') {
            HandleRootPutEP(req, res);
        }
        else {
            res.writeHead(400, { 'content-type': 'application/json' });
            res.end(JSON.stringify({ error: "Bad request. There's no such endpoint" }));
        }
    }
    else if (method == 'DELETE') {
        if (pathname.startsWith('/') && pathname.split('/').length == 2) {
            const idStr = pathname.slice(1);
            HandleRootDeleteEP(req, res, idStr);
        }
        else if (pathname.startsWith('/') && pathname.split('/').length == 3) {
            const dateStr = pathname.split('/')[2];
            HandleDeleteBackupEP(req, res, dateStr);
            return;
        }
        else {
            res.writeHead(400, { 'content-type': 'application/json' });
            res.end(JSON.stringify({ error: "Bad Request. There's no such endpoint" }));
        }
    }
    else {
        res.writeHead(405, { 'content-type': 'text/plain' });
        res.end('405 Method Not Alowed');
    }
};

const server = http.createServer(serverFunction);
server.listen(PORT);


console.log(`Server running on http://localhost:${PORT}/`);