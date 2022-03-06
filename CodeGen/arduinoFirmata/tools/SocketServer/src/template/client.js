const init = () => {
  refresh();
};
(() => {
  init();
})();

/**
 * @typedef {Object} Resolver
 * @property {string} selector
 * @property {(TemplateData) => any} mappingFn
 */

/**
 * @type {Object.<string, Resolver>}
 */
const dataResolver = {
  activeClient: {
    selector: '#__holder__activeClient',
    mappingFn: (dataResponse) => {
      if (dataResponse.activeClientId && !shared.useClient) {
        setActive(dataResponse.activeClientId);
      }

      if (shared.useClient === shared.templateData?.activeClientId && !dataResponse.activeClientId) {
        setActive(null);
      }

      return dataResponse.activeClientId;
    },
  },
  clientList: {
    selector: '#__holder__clientList',
    mappingFn: (dataResponse) => {
      const d = dataResponse.clients || [];
      return `<ul>${d.map((c) => `<li>${c} - <button onclick="setActive('${c}')">use</button></li>`).join('')}</ul>`;

    },
  },
  serverData: {
    selector: '#__holder__serverData',
    mappingFn: (dataResponse) => {
      return JSON.stringify(dataResponse, null, '\t');
    },
  },
  useClient: {
    selector: '#__holder__useClient',
    mappingFn: (dataResponse) => {
      return `${shared.useClient}`;
    },
  },
  plotData: {
    selector: '#__holder__plotData',
    mappingFn: (dataResponse) => {
      if (!shared.useClient) return '';
      if (!dataResponse.data || !dataResponse.data[shared.useClient]?.dataIn) return '';

      const dataToPlot = dataResponse.data[shared.useClient]?.dataIn;
      const plotData = dataToPlot.map(item => {
        const [output, executionTime, input] = item;
        return {
          x: executionTime,
          y1: output,
          y2: input,
        };
      });

      const plotlyConf = {
        data: [
          {
            x: plotData.map(item => item.x),
            y: plotData.map(item => Math.max(item.y1, 0)),
            type: 'scatter',
            name: 'output (C)',
          },
          {
            x: plotData.map(item => item.x),
            y: plotData.map(item => item.y2 * (100 / 255)),
            type: 'scatter',
            name: 'input (%)',
            yaxis: 'y2',
          },
        ],
        layout: {
          title: 'Plot',
          xaxis: {
            title: 'execution time',
          },
          yaxis: {title: 'temperature in C'},
          yaxis2: {
            title: 'input in % (0-255)',
            titlefont: {color: 'rgb(148, 103, 189)'},
            tickfont: {color: 'rgb(148, 103, 189)'},
            overlaying: 'y',
            side: 'right',
            range: [0, 105],
          },
        },
      };

      //
      Plotly.newPlot(document.getElementById('__holder__plotData'), plotlyConf);

      return '';
    },
  },
};

const shared = {};

window.setActive = (client) => {
  shared.useClient = client;

  const resolver = dataResolver.useClient;
  const $element = $(resolver.selector);
  const data = resolver.mappingFn(shared.templateData);
  $element.html(data);

  // const resolver2 = dataResolver.plotData;
  // const $element2 = $(resolver2.selector);
  // const data2 = resolver2.mappingFn(shared.templateData);
  // $element2.html(data2);

};

window.sendArray = () => {
  const array = $('#__holder__dataToSend').val();
  console.log('sendArray', JSON.parse(array));
  // if (!shared.useClient) return;

  const data = {
    clientId: shared.useClient,
    dataOut: JSON.parse(array),
  };


  $.ajax({
    url: '/api/send',
    type: 'POST',
    contentType: 'application/json',
    data: JSON.stringify(data),
    success: (d) => {
      alert('sendArray success', d);
    },
    error: (e) => {
      alert('sendArray error', e);
    },
  });
};


function refresh() {
  // ajax get index html
  $.ajax({
    url: 'api/data',
    /**
     * @param {TemplateData} response
     */
    success: function (response) {
      Object.values(dataResolver).forEach((resolver) => {
        const $element = $(resolver.selector);
        const data = resolver.mappingFn(response);

        if (data) {
          $element.html(data);
        }
      });

      shared.templateData = response;

      if (!shared.useClient) {
        shared.useClient = response.activeClientId;
      }


      setTimeout(function () {
        refresh();
      }, 1000);
    },
  });
}

