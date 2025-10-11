var http = require('http');
var fs = require('fs');
var url = require('url');
var rline = require('readline');
var data_module = require('./Db_module');
const { json } = require('stream/consumers');
const PORT = 5000;

const rl = rline.createInterface({
    input: process.stdin,
    output: process.stdout
});

let serverStats = {
    time_start:null,
    time_finish:null,
    request_count:0,
    commit_count:0
}

var db = new data_module.DB();

db.on('GET', async (request, response) => {
    console.log("DB.GET triggered");
    collectStatistics(0);
    response.writeHead(200, { 'content-type': 'application/json;charset=utf-8' });
    response.end(JSON.stringify(db.select()))
});


db.on('POST', async (request, response) => {
    console.log("DB.POST triggered");
    request.on('data', data => {
        let dat = JSON.parse(data);
        db.insert(dat);
        response.writeHead(200, { 'content-type': 'application/json;charset=utf-8' });
        response.end(JSON.stringify(dat));
    });
});


db.on('PUT', async (request, response) => {
    console.log("DB.PUT triggered");
    request.on('data', data => {
        let dat = JSON.parse(data);
        db.update(dat);
        response.writeHead(200, { 'content-type': 'application/json;charset=utf-8' });
        response.end(JSON.stringify(db.select()));
    })
})


db.on('DELETE', async (request, response) => {
    console.log("DB.DELETE triggered");
    let parsedURL = url.parse(request.url, true);
    const id = parsedURL.query.id;
    if (!id) {
        response.writeHead(400, { 'content-type': 'text/html;charset=utf-8' });
        response.end('<h1>400 Bad request</h1>')
    }
    const deletedRow = await db.delete(id);
    response.writeHead(200, { 'content-type': 'application/json;charset=utf-8' });
    response.end(JSON.stringify(deletedRow));
});

db.on('COMMIT', async () => {
    console.log("DB.COMMIT triggered");
    serverStats.commit_count = db.commit();
});


const server = http.createServer(function (request, response) {
    if (request) {
        serverStats.request_count++;
    }
    if (url.parse(request.url).pathname === "/api/db") {
        db.emit(request.method, request, response);
    }
    else if (url.parse(request.url).pathname === "/api/ss") {
        response.writeHead(200, { 'content-type': 'application/json;charset=utf-8' });
        // if(serverStats.time_finish==null){
        //     collectStatistics(0);
        // }
        collectStatistics(0);
        response.end(JSON.stringify(serverStats));
    }
    else if (request.url === "/") {
        let page = fs.readFileSync('./index.html');
        response.writeHead(200, { 'content-type': 'text/html;charset=utf-8' });
        response.end(page);
    }
    else {
        response.writeHead(404, { 'content-type': 'text/html' });
        response.end(
            "<h1>404 Not Found</h1>"
        );
    }
});

server.listen(PORT);
console.log("Server running at http://localhost:5000/");
console.log("Server statistics available at http://localhost:5000/api/ss");

let stopTimeout;
const stopServer = (delay) => {
    stopTimeout = setTimeout(() => {
        console.log("Timeout passed. Exiting the application...");
        process.exit(0);

    }, delay);
    stopTimeout.unref();
}

let commitInterval;
const startCommiting = (interval) => {
    commitInterval = setInterval(() => {
        db.emit('COMMIT');
        console.log("Commit operation executed");
    }, interval);
    commitInterval.unref();
}

let statisticsTimeout;
const collectStatistics = (collDelay) => {
    serverStats.time_start = new Date(Date.now()).toISOString();
    statisticsTimeout = setTimeout(() => {
        serverStats.time_finish = new Date(Date.now()).toISOString();
        console.log("Statistic collected");
        console.log(serverStats);
    }, collDelay);

    statisticsTimeout.unref();
}


const changeState = () => {
    rl.question('-->', (input) => {
        input.trim().toLowerCase();

        if (input == 'exit') {
            console.log("Exiting the application...");
            process.exit(0);
        }

        let command = input.slice(0, 2);
        let flag = input.slice(3);

        let invalid_flag = false;

        if (isNaN(flag)) {
            console.log("Invalid flag");
            invalid_flag = true;
            changeState();
        }

        let command_NoFlag = false;
        if (flag == "") {
            command_NoFlag = true;
            console.log(`No flag for ${command} command`);
        }


        if (!invalid_flag) {
            switch (command) {
                case 'sd':
                    clearTimeout(stopTimeout);
                    let duration = parseInt(flag);

                    if (flag) {
                        if (!isNaN(duration)) {
                            stopServer(parseInt(flag));
                        }
                    }
                    else {
                        clearTimeout(stopTimeout);
                        console.log("Server stop aborted");
                    }

                    break;
                case 'sc':
                    clearInterval(commitInterval);
                    let interval = parseInt(flag);
                    if (flag) {
                        if (!isNaN(interval)) {
                            startCommiting(interval);
                        }
                    }
                    else {
                        clearInterval(commitInterval);
                        console.log("Commit execution aborted");
                    }

                    break;
                case 'ss':
                    clearTimeout(statisticsTimeout);
                    let statTimeout = parseInt(flag);
                    if (flag) {
                        if (!isNaN(statTimeout)) {
                            collectStatistics(statTimeout);
                        }
                    }
                    else {
                        clearTimeout(statTimeout);
                        console.log("Statistics collection aborted");
                    }
                    break;
            }

        }



        changeState();
    });
}

changeState();


