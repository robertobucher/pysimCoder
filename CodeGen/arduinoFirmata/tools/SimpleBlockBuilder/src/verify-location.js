const prompt = require("prompts").prompt;

const CONST = require("./constants.js");

module.exports = async function () {
  const chalk = (await import("chalk")).default;

  let location = CONST.CONFIG.pysimCoder.home || process.env[CONST.PSC_ENV];

  if (!location) {
    console.log(
      chalk.red(
        "Error: No install path specified. \nCheck your environment or specify path in config.json."
      )
    );
    return null;
  }

  if (CONST.CONFIG.app.ignoreLocation) {
    return location;
  }

  const response = (
    await prompt({
      type: "confirm",
      name: "value",
      message:
        "Your pysimCoder home is set to: " +
        chalk.cyan(location) +
        "\nIs this correct?",
      inital: true,
    })
  ).value;

  return response ? location : null;
};
