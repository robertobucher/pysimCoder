# Arduino Firmata protocol for pysimCoder

## Prerequisites

Install the [Arduino IDE](https://www.arduino.cc/en/software) and the upload StandardFirmata on your Arduino (Arduino
IDE > File > Examples > Firmata > StandardFirmata).

---

## New Blocks

- setup - Initializes Firmata on `port='/dev/ttyACM0'` with `baud=57600`
- analogWrite - uses arduino analogWrite function on `pin` with `mode` (firmata supported
  modes: https://github.com/Chiff/FirmataC/blob/master/includes/firmata.h#L7-L13)
- digitalWrite - uses arduino digitalWrite function on `pin`
- pinRead - reads pin value from arduino `pin` with `mode` (firmata supported
  modes: https://github.com/Chiff/FirmataC/blob/master/includes/firmata.h#L7-L13)
- customRead - array from 0 to 6
  - customRead[0] - VL53L0X distance sensor - distance in mm - SYSEX format = `START_SYSEX 0x72 msb lsb END_SYSEX`
  - customRead[1] - **unused at the moment*
  - customRead[2] - **unused at the moment*
  - customRead[3] - **unused at the moment*
  - customRead[4] - **unused at the moment*
  - customRead[5] - **unused at the moment*
  - customRead[6] - **unused at the moment*

Helpers:

- help_HeatShield_temperature - customized block to translate voltage to temperature based on shield
- help_TCLab_temperature - customized block to translate voltage to temperature based on shield

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
- https://github.com/AtmelUniversityFrance/atmel-avr-xmini-boardmanagermodule/wiki
- https://ctms.engin.umich.edu/CTMS/index.php?aux=Activities_RCcircuitA

#### VL53L0X

https://www.instructables.com/Going-Beyond-StandardFirmata-Adding-New-Device-Sup/
https://docs.arduino.cc/hacking/software/FirmataLibrary
https://github.com/pololu/vl53l0x-arduino

## TC Lab

- Main page: http://apmonitor.com/pdc/index.php/Main/ArduinoTemperatureControl
- Test: https://tclab.readthedocs.io/en/latest/notebooks/01_TCLab_Overview.html#TCLab-Architecture
- Arduino IDE firmware - https://github.com/jckantor/TCLab-sketch/blob/master/TCLab-sketch/TCLab-sketch.ino

## Node MCU

- https://github-wiki-see.page/m/xujiaao/android-firmata/wiki/Getting-Started-with-Android-Firmata-and-NodeMcu-Board
