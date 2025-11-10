const fs = require('fs');
const path = require('path');

const MIME = {
  html: 'text/html',
  css: 'text/css',
  js: 'text/javascript',
  png: 'image/png',
  docx: 'application/msword',
  json: 'application/json',
  xml: 'application/xml',
  mp4: 'video/mp4'
};

function createStaticHandler(staticRootRelative) {
  const staticRoot = path.resolve(process.cwd(), staticRootRelative);

  return async function handleStatic(request, response) {
    if (request.method !== 'GET') {
      response.writeHead(405, { 'Content-Type': 'text/plain; charset=utf-8' });
      response.end('405 Method Not Allowed');
      return;
    }

    const host = request.headers.host || 'localhost';
    const url = new URL(request.url, `http://${host}`);
    let pathname = decodeURIComponent(url.pathname);
    let ext = path.extname(pathname).slice(1).toLowerCase();
    if (pathname.startsWith('/')) pathname = pathname.slice(1);


    if (!ext || !MIME[ext]) {
      response.writeHead(404, { 'Content-Type': 'text/plain; charset=utf-8' });
      response.end('404 Method Is Not Static');
      return;
    }

    const absPath = path.resolve(staticRoot, pathname);


    if (!absPath.startsWith(staticRoot + path.sep) && absPath !== staticRoot) {
      response.writeHead(404, { 'Content-Type': 'text/plain; charset=utf-8' });
      response.end('404 Not Found');
      return;
    }

    try {
      const stat = await fs.promises.stat(absPath);
      if (!stat.isFile()) {
        response.writeHead(404, { 'Content-Type': 'text/plain; charset=utf-8' });
        response.end('404 Not Found');
        return;
      }

      if (ext === 'mp4' && request.headers.range) {
        const range = request.headers.range;
        const bytes = range.replace(/bytes=/, '').split('-');
        const start = parseInt(bytes[0], 10);
        const end = bytes[1] ? parseInt(bytes[1], 10) : stat.size - 1;
        if (isNaN(start) || isNaN(end) || start > end) {
          response.writeHead(416);
          response.end();
          return;
        }
        const chunkSize = (end - start) + 1;
        response.writeHead(206, {
          'Content-Range': `bytes ${start}-${end}/${stat.size}`,
          'Accept-Ranges': 'bytes',
          'Content-Length': chunkSize,
          'Content-Type': MIME[ext]
        });
        const stream = fs.createReadStream(absPath, { start, end });
        stream.pipe(response);
        stream.on('error', (err) => {
          console.error('Stream error:', err);
          if (!response.headersSent) response.writeHead(500);
          response.end('500 Internal Server Error');
        });
        return;
      }

      response.writeHead(200, {
        'Content-Type': MIME[ext],
        'Content-Length': stat.size
      });

      const readStream = fs.createReadStream(absPath);
      readStream.pipe(response);
      readStream.on('error', (err) => {
        console.error('Read stream error:', err);
        if (!response.headersSent) response.writeHead(500, { 'Content-Type': 'text/plain; charset=utf-8' });
        response.end('500 Internal Server Error');
      });
    } catch (err) {
      if (err.code === 'ENOENT' || err.code === 'ENOTDIR') {
        response.writeHead(404, { 'Content-Type': 'text/plain; charset=utf-8' });
        response.end('404 Not Found');
      } else {
        console.error('FS error:', err);
        response.writeHead(500, { 'Content-Type': 'text/plain; charset=utf-8' });
        response.end('500 Internal Server Error');
      }
    }
  };
}

module.exports = { createStaticHandler };