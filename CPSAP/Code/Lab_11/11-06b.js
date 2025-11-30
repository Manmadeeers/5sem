let rcpWSC = require("rpc-websockets").Client;
let ws = new rcpWSC("ws://localhost:4000/");

ws.on("open", () => {
  ws.subscribe("B");

  ws.on("B", (data) => {
    console.log("Event B occured: ", data.toString());
  });
});