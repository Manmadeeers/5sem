let rpcWSC = require("rpc-websockets").Client;
let ws = new rpcWSC("ws://localhost:4000/");

process.stdin.setEncoding("utf8");
process.stdin.on("readable", () => {
  let chuck = null;
  while ((chuck = process.stdin.read()) !== null) {
    let trimmed = chuck.trim().toUpperCase();
    if (trimmed === "A" || trimmed === "B" || trimmed === "C") {
      ws.notify(trimmed);
    } else {
      console.log("Event was not recognized");
    }
  }
});