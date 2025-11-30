let rcpWSC = require("rpc-websockets").Client;
let ws = new rcpWSC("ws://localhost:4000/");

ws.on("open", () => {
  ws.subscribe("C");

  ws.on("C", (data) => {
    console.log("Event C occured: ", data.toString());
  });
});