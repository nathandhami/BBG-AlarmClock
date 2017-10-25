'use strict';

const nconf = require('nconf');

// nconf Configuratiton
nconf.argv({
  'p': {
    'alias': 'http:port',
    'describe': 'The port to listen on',
  },
});

nconf.file('src/nconf/config.json');

nconf.defaults({
  'http': {
    'port': 3000,
  },
});

module.exports = nconf;
