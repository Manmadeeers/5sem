const rpcWSC = require("rpc-websockets").Client;
const ws = new rpcWSC("ws://localhost:4000/");

console.log("Enter A, B or C to send corresponding notification");
process.stdin.setEncoding("utf8");
process.stdin.on("readable", () => {
  let chuck = null;
  while ((chuck = process.stdin.read()) !== null) {
    let trimmed = chuck.trim().toUpperCase();
    if (trimmed === "A" || trimmed === "B" || trimmed === "C") {
      console.log(`Notification ${trimmed} sent`);
      ws.notify(trimmed);
    } else {
      console.log("Input was ignored. Event was not recognized");
    }
  }
});