const express = require('express');
const ejs = require('ejs');

const SHARED = require('./src/shared.js');
const CONFIG = require('./src/config.js');
const tcp = require('./src/tcp.js');
const unixClient = require('./src/unix-clinet.js');
const {EventData} = require('./src/model.js');


const app = express();
app.use(express.json());
app.get('/', function (req, res) {
  ejs.renderFile('src/template/index.ejs', SHARED.templateData.recalc(),
    {}, function (err, template) {
      if (err) {
        throw err;
      } else {
        res.end(template);
      }
    });
});

app.get('/client.js', function (req, res) {
  res.sendFile('src/template/client.js', {root: __dirname});
});

app.get('/api/data', function (req, res) {
  res.json(SHARED.templateData.recalc());
});

// [post] api/send with json as data
app.post('/api/send', function (req, res) {
  const data = req.body;

  if (!data.clientId) {
    res.status(400).send('clientId is required');
    return;
  }

  if (SHARED.templateData.activeClientId !== data.clientId) {
    res.status(400).send('clientId is not active');
    return;
  }

  console.log('[post] api/send', data);
  SHARED.bus.emit('onDataOut', new EventData('onDataOut', data.clientId, CONFIG.port.tcp, data.dataOut));

  res.json({status: 'ok'});
});

app.listen(CONFIG.port.http, function (error) {
  if (error) {
    console.error(error);
  } else {
    console.info('Express\tserver started on port ' + CONFIG.port.http);
  }

  // eventually we can create TCP sockets on demand :)
  const s1 = tcp(CONFIG.port.tcp);

  SHARED.bus.subscribe('onClientConnect', function (data) {
    const c1 = unixClient(CONFIG.port.sock, function (data) {
      const dataLength = 8;
      const totalLength = data.data.length;
      const buffer = Buffer.alloc(dataLength * totalLength);

      for (let i = 0; i < totalLength; i++) {
        buffer.writeDoubleLE(data.data[i], i * dataLength);
      }

      return buffer;
    });


    SHARED.templateData.activeClientId = data.clientId;
    SHARED.templateData.data[data.clientId] = {
      client: c1,
      dataIn: [],
      dataOut: [],
      eventLog: [{
        text: 'onClientConnect',
        data: [data.clientId, data.port],
      }],
    };

  });

  SHARED.bus.subscribe('onClientLeave', function (input) {
    const data = SHARED.templateData.data[input.clientId];

    data.client.end();
    data.client = null;
    data.eventLog.push({text: 'disconnected'});

    if (SHARED.templateData.activeClientId === input.clientId) {
      console.warn('onClientLeave', input);
      SHARED.templateData.activeClientId = null;
    }
  });

  SHARED.bus.subscribe('onDataIn', function (input) {
    SHARED.templateData.data[input.clientId].dataIn.push(input.data);
    SHARED.templateData.data[input.clientId].eventLog.push({
      text: 'onDataIn',
      data: input.data,
    });
  });

  // SHARED.bus.subscribe('onDataOut', function (input) {
  //   console.log('onDataOut', input);
  // });
});

