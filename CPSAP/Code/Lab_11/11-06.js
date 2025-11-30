let rpcWSS = require("rpc-websockets").Server;

let server = new rpcWSS({ port: 4000, host: "localhost" });

server.event("A");
server.event("B");
server.event("C");

process.stdin.setEncoding("utf8");
process.stdin.on("readable", () => {
    let chuck = null;
    while ((chuck = process.stdin.read()) !== null) {
        let trimmed = chuck.trim().toUpperCase();
        if (trimmed === "A" || trimmed === "B" || trimmed === "C") {
            server.emit(trimmed, `Event ${trimmed}`);
        } else {
            console.log("Event was not recognized");
        }
    }
});