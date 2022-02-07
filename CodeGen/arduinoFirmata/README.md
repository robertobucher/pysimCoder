# Arduino Firmata protocol for pysimCoder

## Prerequisites
Install the [Arduino IDE](https://www.arduino.cc/en/software) and the upload StandardFirmata on your Arduino (Arduino IDE > File > Examples > Firmata > StandardFirmata).

---
## Tools

#### SimpleBlockBuilder
Node.js cli tool to create pysimCoder blocks quicker.

Prerequisites:
- Node.js (see package.json for version requirements)
- npm or yarn (see package.json for version requirements)

Usage:
- Navigate to pysimCoder/CodeGen/arduinoFirmata/tools/SimpleBlockBuilder
- If necessary edit the `config.js` file
- Run `npm run start` or `yarn start`
- Generated files will be outputed `config.pysimCoder.home` if present, otherwise it uses `PYSUPSICTRL` env variable


