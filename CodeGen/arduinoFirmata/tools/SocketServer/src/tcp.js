const Net = require('net');

const SHARED = require('./shared.js');
const {EventData} = require('./model.js');

/**
 * @param {string|number} port
 * @returns {module:net.Server}
 */
module.exports = function (port) {

  const tcpServer = new Net.Server();
  tcpServer.listen(port, function () {
    console.log(
      `TCP\tserver started on port ${port}`,
    );
  });


  tcpServer.on('connection', function (socket) {
    const clientId = socket.remoteAddress + ':' + socket.remotePort;
    console.log(
      `TCP\tclient ${clientId} connected to server ${port}`,
    );


    SHARED.bus.emit('onClientConnect', new EventData('onClientConnect', clientId, port));

    socket.on('data', function (chunk) {
      const dataLength = 8;
      const totalLength = chunk.length;

      // console.log(chunk.toString('hex'));

      const data = [];
      for (let i = 0; i < totalLength; i += dataLength) {
        data.push(chunk.readDoubleLE(i));
      }

      SHARED.bus.emit('onDataIn', new EventData('onClientConnect', clientId, port, data));
    });

    socket.on('end', function () {
      SHARED.bus.emit('onClientLeave', new EventData('onClientConnect', clientId, port));
    });

    socket.on('error', function (err) {
      console.error(`Error: ${err}`);
    });
  });

  return tcpServer;
};
