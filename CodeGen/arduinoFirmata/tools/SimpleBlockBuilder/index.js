#! /usr/bin/env node

// Module imports
const fs = require("fs");

// App imports
const CONST = require("./src/constants.js");

// Commands
const verifyLocation = require("./src/verify-location.js");
const readLibraries = require("./src/read-libraries.js");
const defineBlocks = require("./src/define-blocks.js");

/**
 * @typedef {Object} bbApp
 * @property location: string
 * @property libraries: string[]
 */
module.exports = (async () => {
  // Prompts user to check location of pysimCoder home
  this.location = await verifyLocation();
  if (!this.location) {
    return CONST.helpers.onFail("Invalid pysimCoder home location");
  }

  // Reads libraries
  this.libraries = await readLibraries(this);

  // Defines blocks - (`defBlocks` command)
  const blockDefiniton = defineBlocks(this);

  return {
    location: this.location,
    libraries: this.libraries,
  };
})();
