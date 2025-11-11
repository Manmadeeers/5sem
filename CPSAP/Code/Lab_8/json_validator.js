const isJson = (headers,header,mime)=>{
    let rc = false;

    let h = headers[header];
    if(h){
        rc = h.indexOf(mime)>=0;
    }
    return rc;
}

exports.isJsonAccept = (hs)=>isJson(hs,'accept','application/json;charset=utf-8');