let rpcWSS = require("rpc-websockets").Server;
const SERVER_OPTIONS = {
    port: 4000,
    host: "localhost"
};

const server = new rpcWSS(SERVER_OPTIONS);
console.log(`Server running on ws://${SERVER_OPTIONS.host}:${SERVER_OPTIONS.port}`);
server.event("A");
server.event("B");
server.event("C");

console.log("Enter A, B or C to generate a corresponding event");
process.stdin.setEncoding("utf8");
process.stdin.on("readable", () => {
    let chuck = null;
    while ((chuck = process.stdin.read()) !== null) {
        let trimmed = chuck.trim().toUpperCase();
        if (trimmed === "A" || trimmed === "B" || trimmed === "C") {
            console.log(`Event ${trimmed} was emitted`);
            server.emit(trimmed, `Event ${trimmed}`);
        } else {
            console.log("Input ignored.Event was not recognized");
        }
    }
});