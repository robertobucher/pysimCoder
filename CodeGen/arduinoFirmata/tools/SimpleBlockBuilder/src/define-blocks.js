const prompt = require("prompts").prompt;

const CONST = require("./constants.js");

const INITIAL = "define-blocks__initial";

/**
 *
 * @param {bbApp} app
 * @returns {Promise<void>}
 */
module.exports = async (app) => {
  const chalk = (await import("chalk")).default;

  const libraries = app.libraries;

  const Configstore = (await import("configstore")).default;
  const conf = new Configstore(CONST.CONFIG_STORE.DEFINE_BLOCKS);

  const questions = [
    {
      initial: conf.get(INITIAL) || "",
      type: "autocomplete",
      name: "library",
      message: "Select a library",
      limit: 5,
      choices: libraries.map((lib) => ({ title: lib, value: lib })),
      suggest: suggestFn,
      fallback: function (val) {
        return `Library with this name doesn't exist, ${chalk.cyan(
          "press enter to create new library"
        )}`;
      },
      onRender: function () {
        // warn - do not use arrow function - we cannot lose context
        if (this.suggestions?.length) {
          return;
        }

        this.value = this.input;
      },
      onState: function () {
        if (this.done) {
          conf.set(INITIAL, this.value);
        }
      },
    },
    // TODO - 6. 2. 2022 - select icon
    // {
    //   type: "autocomplete",
    //   name: "blockIcon",
    //   message: "Enter block icon",
    //   choices: icons.map((icon) => ({ title: icon, value: icon })),
    //   ....
    // },
    {
      type: "text",
      name: "functionName",
      message: `Enter ${chalk.cyan(
        "function name"
      )} (do not use spaces as this will be used as a block name and c function name)
`,
      validate: validateFunctionName,
    },
    {
      type: "text",
      name: "blockName",
      message: `Enter ${chalk.cyan(
        "block name"
      )} - defaults to function name if not specified`,
    },
    {
      type: "list",
      name: "functionParameters",
      message: `Type ${chalk.cyan(
        "function parameter names and values "
      )} as comma separated list \n(e.g. paramName:'defaultValueAsString', numberParam:123) - leave empty if no parameters\n`,
      validate: validateFunctionParameters,
    },
    {
      type: "text",
      name: "blockDescription",
      // TODO - 7. 2. 2022 - create help file in `pysimCoder\resources\blocks\rcpBlk\help`
      message:
        `Enter ${chalk.cyan("block description")} - optional - ` +
        chalk.red("IGNORED -> help file is not implemented yet"),
    },
    {
      type: "number",
      name: "blockInputs",
      message: `Enter ${chalk.cyan("number of block inputs")}`,
      initial: 0,
    },
    {
      type: "confirm",
      name: "blockInputCountSettable",
      message: `Is block ${chalk.cyan("input count settable")}?`,
      initial: false,
    },
    {
      type: "number",
      name: "blockOutputs",
      message: `Enter ${chalk.cyan("number of block outputs")}`,
      initial: 0,
    },
    {
      type: "confirm",
      name: "blockOutputCountSettable",
      message: `Is block ${chalk.cyan("output count settable?")}`,
      initial: false,
    },
  ];

  const check = async (questions, op) => {
    const fixed = fixData(op);
    const confirm = (
      await prompt({
        type: "confirm",
        name: "confirm",
        message:
          `Are these data correct? ${chalk.red("Press Ctrl+C to abort")}\n` +
          chalk.cyan(JSON.stringify(fixed, null, 2)),
        initial: true,
      })
    ).confirm;

    if (!confirm) {
      questions.forEach((q) => {
        q.initial = fixed[q.name];

        if (q.name === "functionParameters") {
          q.initial = fixed[q.name].split("|");
        }
      });

      const newOutput = await prompt(questions);
      return check(questions, newOutput);
    }

    return fixed;
  };

  const output = await prompt(questions);
  const finalData = await check(questions, output);

  return transformData(finalData);
};

const transformData = (data) => {
  //   transform data into
  //   \begin{description}
  // \item[``lib''] the name of the tab for the block library (example
  //       ``tab'':``linear'')
  // \item [``name''] the default name of the block
  // \item [``ip''] number of inputs
  // \item [``op''] number of outputs
  // \item [``stin''] flag which indicates if the number of inputs can be modified
  // \item [``stout''] flag which indicates if the number of outputs can be modified
  // \item [``icon'']the name of the ``.svg'' file with the icon of the block
  // \item [``param''] the parameters of the block
  // \end{description}

  let param = data.functionName;
  if (data.functionParameters) {
    param += `|${data.functionParameters}`;
  }

  return {
    lib: data.library,
    name: data.blockName,
    ip: data.blockInputs,
    op: data.blockOutputs,
    stin: data.blockInputCountSettable,
    stout: data.blockOutputCountSettable,
    param: param,
    desc: data.blockDescription,
  };
};

const fixData = (data) => {
  if (!data.blockName) {
    data.blockName = data.functionName;
  }

  if (Array.isArray(data.functionParameters)) {
    data.functionParameters = data.functionParameters
      ?.filter((param) => !!param?.trim())
      ?.join("|")
      ?.trim();
  }

  return data;
};

const suggestFn = (input, choices) => {
  return choices.filter((choice) =>
    choice.title.toLowerCase().includes(input.toLowerCase())
  );
};

const validateFunctionName = (val) => {
  if (val.length === 0) {
    return "Function name cannot be empty";
  }

  if (val.indexOf(" ") !== -1) {
    return "Function name cannot contain spaces";
  }

  return true;
};

const validateFunctionParameters = (val) => {
  if (val.length === 0) {
    return true;
  }

  const params = val.split(",");

  for (let i = 0; i < params.length; i++) {
    const param = params[i];

    const tmp = param.split(":");
    if (tmp.length !== 2) {
      return "Invalid parameter format";
    }

    const [name, value] = tmp;

    if (name.length === 0) {
      return "Parameter name cannot be empty";
    }

    if (value.length === 0) {
      return "Parameter value cannot be empty";
    }
  }

  return true;
};

// convert this python code
//from supsisim.RCPblk import RCPblk
// from scipy import size
//
// def setup_af(pin, pout):
//     """
//
//     Call:   setup_af(pin, pout)
//
//     Parameters
//     ----------
//        pin: connected input port(s)
//        pout: connected output port(s)
//
//     Returns
//     -------
//        blk: RCPblk
//
//     """
//
//     blk = RCPblk('setup_af', pin, pout, [0,0], 0, [], [])
//     return blk
const genfunction = (data) => {
  return;
  `
from supsisim.RCPblk import RCPblk
from scipy import size

def ${data.functionName}(pin, pout):
    blk = RCPblk('${data.functionName}', pin, pout, [0,0], 0, [], [])
    return blk  
`;
};
