const {EventBus, TemplateData} = require('./model.js');

module.exports = {
  bus: new EventBus(),
  templateData: new TemplateData(),
};
