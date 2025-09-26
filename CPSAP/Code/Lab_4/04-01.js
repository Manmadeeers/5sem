var http = require('http');
var fs = require('fs');
var events = require('events');
var util = require('util');
const PORT = 5000;


const server = http.createServer();

server.listen(PORT);


console.log("Server running at http://localhost:5000/api/db");

