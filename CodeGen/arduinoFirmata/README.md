# Arduino Firmata protocol for pysimCoder

## Prerequisites

Install the [Arduino IDE](https://www.arduino.cc/en/software) and the upload StandardFirmata on your Arduino (Arduino
IDE > File > Examples > Firmata > StandardFirmata).

---

## Tools

#### SocketServer

Express server that can be used to communicate with the Arduino.

Creates:

- simple web page to plot data from the Arduino and send commands to the Arduino
- express API
- TCP server that listens on port 5000
- Unix socket client

Prerequisites:

- Node.js (see package.json for version requirements)
- npm or yarn (see package.json for version requirements)

Usage:

- Navigate to pysimCoder/CodeGen/arduinoFirmata/tools/SocketServer
- If necessary edit the `src/config.js` file
- Run `npm run start` or `yarn start`

## Reference

#### VL53L0X

https://www.instructables.com/Going-Beyond-StandardFirmata-Adding-New-Device-Sup/
https://docs.arduino.cc/hacking/software/FirmataLibrary
https://github.com/pololu/vl53l0x-arduino

#### Headers

https://github.com/firmata/arduino/blob/4bd508aca4f2c73ed3528529eeb5c490815b5f9b/Boards.h
https://github.com/gergelytakacs/AutomationShield/blob/master/src/AutomationShield.h
https://github.com/gergelytakacs/AutomationShield/blob/master/src/FloatShield.h
https://github.com/gergelytakacs/AutomationShield/blob/master/src/HeatShield.h
https://github.com/Chiff/FirmataC/include/Firmata.h

#### Misc

- Heatshield - https://github.com/gergelytakacs/AutomationShield/wiki/HeatShield
- https://github.com/firmata/arduino/search?q=PIN_MODE_PWM&type=code
- https://docs.arduino.cc/hacking/software/FirmataLibrary
- https://www.arduino.cc/en/reference/wire
- https://www.instructables.com/Going-Beyond-StandardFirmata-Adding-New-Device-Sup/

## TC Lab

- Main page: http://apmonitor.com/pdc/index.php/Main/ArduinoTemperatureControl
- Test: https://tclab.readthedocs.io/en/latest/notebooks/01_TCLab_Overview.html#TCLab-Architecture
- Arduino IDE firmware - https://github.com/jckantor/TCLab-sketch/blob/master/TCLab-sketch/TCLab-sketch.ino

## Node MCU

- https://github-wiki-see.page/m/xujiaao/android-firmata/wiki/Getting-Started-with-Android-Firmata-and-NodeMcu-Board
