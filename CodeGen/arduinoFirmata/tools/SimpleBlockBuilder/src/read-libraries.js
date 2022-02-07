const fs = require("fs");
const { resolve } = require("path");
const { readdir } = require("fs").promises;

/**
 * @param {bbApp} app
 * @return {Promise<string[]>}
 */
module.exports = async (app) => {
  const chalk = (await import("chalk")).default;

  console.log("- Reading libraries...");

  const read = `${app.location}/resources/blocks/blocks`;
  const knownLibraries = {};

  for await (const f of getFiles(read)) {
    const isXblk = f.endsWith(".xblk");
    if (!isXblk) continue;

    const raw = fs.readFileSync(f);
    const parsed = JSON.parse(raw);

    if (parsed.lib) {
      knownLibraries[parsed.lib] = true;
    }
  }

  const result = Object.keys(knownLibraries);
  console.log(
    "- Found libraries: [" + chalk.cyan("'" + result.join("', '") + "'") + "]"
  );

  return result;
};

async function* getFiles(dir) {
  const dirents = await readdir(dir, { withFileTypes: true });
  for (const dirent of dirents) {
    const res = resolve(dir, dirent.name);
    if (dirent.isDirectory()) {
      yield* getFiles(res);
    } else {
      yield res;
    }
  }
}
