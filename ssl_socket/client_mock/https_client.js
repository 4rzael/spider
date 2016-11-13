#!/usr/bin/env node

const tls = require('tls');
const fs = require('fs');

const options = {
  // Necessary only if the server uses the self-signed certificate
  ca: [ fs.readFileSync('../spider.crt') ]
};

const socket = tls.connect(process.argv[2], options, () => {
  console.log('client connected',
              socket.authorized ? 'authorized' : 'unauthorized');
  process.stdin.pipe(socket);
  process.stdin.resume();
});
socket.setEncoding('utf8');
socket.on('data', (data) => {
  console.log(data);
});

socket.on('end', () => {
  console.log('=== end ===')
  server.close();
});
