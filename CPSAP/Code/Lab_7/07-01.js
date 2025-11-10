const http = require('http');
const { createStaticHandler } = require('./07-01m');

const PORT = process.env.PORT ? Number(process.env.PORT) : 5000;

const staticHandler = createStaticHandler('static');

const server = http.createServer((req, res) => {
  staticHandler(req, res);
});

server.listen(PORT, () => {
  console.log(`Server 07-01 listening at http://localhost:${PORT}/index.html`);
});