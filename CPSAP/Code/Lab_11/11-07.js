const rpcWSS = require("rpc-websockets").Server;
const SERVER_OPTIONS = {
    port: 4000,
    host: "localhost"
};

const server = new rpcWSS(SERVER_OPTIONS);
console.log(`Server running on ws://${SERVER_OPTIONS.host}:${SERVER_OPTIONS.port}`);
console.log("Wait for notification...");

server
  .register("A", (params) => {
    console.log("notification A");
  })
  .public();
server
  .register("B", (params) => {
    console.log("notification B");
  })
  .public();
server
  .register("C", (params) => {
    console.log("notification C");
  })
  .public();