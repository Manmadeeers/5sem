let rcpWSC = require("rpc-websockets").Client;
let ws = new rcpWSC("ws://localhost:4000/");

ws.on("open", () => {
  ws.subscribe("A");

  ws.on("A", (data) => {
    console.log("Event А occured: ", data.toString());
  });
});