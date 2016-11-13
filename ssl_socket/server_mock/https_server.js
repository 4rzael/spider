#!/usr/bin/env node

const tls = require('tls');
const fs = require('fs');

const options = {
  key: fs.readFileSync('../spider.key'),
  cert: fs.readFileSync('../spider.crt'),
};

const server = tls.createServer(options, (socket) => {
  console.log('server connected',
              socket.authorized ? 'authorized' : 'unauthorized');
  socket.write('welcome!');
  socket.setEncoding('utf8');
  socket.on('data', (data) => {
    console.log(data.toString())
  })

  var interval = setInterval(() => {try{socket.write('bla')} catch (e) {console.log(e); clearInterval(interval)}}, 1000)
});
server.listen(8000, () => {
  console.log('server started on port 8000');
});
