const APP_NAME = "pysimcoder-blockbuilder";

module.exports = {
  APP_NAME,

  PSC_ENV: "PYSUPSICTRL",
  PSC_C_TEMPLATE: "CodeGen/LinuxRT/devices/template.c.tmp",

  CONFIG: require("../config.js"),
  CONFIG_STORE: {
    DEFINE_BLOCKS: APP_NAME + "__defineBlocks",
  },

  helpers: {
    onFail: async function (err) {
      const chalk = (await import("chalk")).default;

      console.log(chalk.red(err));
      process.exit(-1);
    },
  },
};
