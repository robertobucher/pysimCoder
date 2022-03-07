const net = require('net');
const SHARED = require('./shared.js');

/**
 * @param {string|number} port
 * @param {function(EventData): Buffer|null} dataOutCallback
 * @returns {module:net.Socket}
 */
module.exports = function (port, dataOutCallback) {
  const conn = net.createConnection(port);
  conn.on('connect', () => {
    console.log(`SOCK\tclient connected to server ${port}`);

    SHARED.bus.subscribe('onDataOut', (d) => {
      let buffer = dataOutCallback(d);
      if (!buffer) {
        return;
      }

      conn.write(buffer);
    });
  });

  conn.on('close', () => {
    console.log(`SOCK\tclient disconnected from server ${port}`);
  });

  return conn;
};
