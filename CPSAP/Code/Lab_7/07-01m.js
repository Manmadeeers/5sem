const http = require('http');
const fs = require('fs');
const path = require('path');


const MIME = {
    html: 'text/html;charset=utf-8',
    css: 'text/css',
    js: 'text/javascript',
    png: 'image/png',
    docx: 'application/msword',
    json: 'application/json;charset=utf-8',
    xml: 'application/xml',
    mp4: 'video/mp4'
};


function createStaticHandler(staticRootRelative){
    
}