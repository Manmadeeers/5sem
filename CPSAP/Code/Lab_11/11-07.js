let rpcWSS = require("rpc-websockets").Server;

let server = new rpcWSS({ port: 4000, host: "localhost" });

server
  .register("A", (params) => {
    console.log("notify A");
  })
  .public();
server
  .register("B", (params) => {
    console.log("notify B");
  })
  .public();
server
  .register("C", (params) => {
    console.log("notify C");
  })
  .public();