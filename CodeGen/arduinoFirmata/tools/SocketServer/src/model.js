/**
 * @property {('onDataIn'|'onDataOut'|'onClientConnect'|'onClientLeave')} type
 * @property {string} clientId
 * @property {number} port
 * @property {number[]} data - data[0] = exectution time,data[1] = mode (0: heatshield, 1: tclab), data[2..] = mode dependent
 */
class EventData {
  type;
  clientId;
  port;
  data = [];

  /**
   * @param {('onDataIn'|'onDataOut'|'onClientConnect'|'onClientLeave')} type
   * @param {string} clientId
   * @param {number} port
   * @param {number[]} data - data[0] = exectution time,data[1] = mode (0: heatshield, 1: tclab), data[2..] = mode dependent
   */
  constructor(type, clientId, port, data = []) {
    this.type = type;
    this.clientId = clientId;
    this.port = port;
    this.data = data;
  }
}


class EventBus {
  events = {
    onDataIn: [],
    onDataOut: [],
    onClientConnect: [],
    onClientLeave: [],
  };

  /**
   * @param {('onDataIn'|'onDataOut'|'onClientConnect'|'onClientLeave')} eventName
   * @param {(data: EventData) => void} callback
   */
  subscribe(eventName, callback) {
    if (!this.events[eventName]) {
      this.events[eventName] = [];
    }
    this.events[eventName].push(callback);
  }

  emit(eventName, data) {
    if (this.events[eventName]) {
      this.events[eventName].forEach(function (callback) {
        callback(data);
      });
    }
  }
}


/**
 * @typedef {Object} ClientData
 * @property {module:net.Socket} client
 * @property {number[][]} dataIn
 * @property {number[][]} dataOut
 * @property {EventData[]} eventLog
 */

/**
 * @typedef {Object} TemplateData
 * @property {Object.<string, ClientData>} data - key is ClientId
 * @property {string|null} activeClientId
 * @property {string[]} clients
 */
class TemplateData {
  data = {};
  activeClientId = null;
  clients = [];

  recalc = () => ({
    data: this.data,
    activeClientId: this.activeClientId,
    clients: this.getClients(),
  });

  getClients = () => Object.keys(this.data || {});
}

module.exports = {
  EventData,
  EventBus,
  TemplateData,
};
