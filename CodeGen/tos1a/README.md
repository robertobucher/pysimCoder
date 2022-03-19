# TOS1A implementation for pysimCoder

## Prerequisites

Tos1A device

- https://www.eas.sk/mod/product/show.php?ID=16

## Block

Params = 4

- Port -
- TimeConstLamp - 0.5s to 10.0s
- TimeConstVent - 0.5s to 10.0s
- TimeConstTemp - 0.5s to 10.0s

---

Inputs = 3

- 0 = temp
- 1 = vent
- 2 = lamp

---

Outputs = 15

temp

- 0 = temp - PCB
- 1 = temp - internal, filtered
- 2 = temp - internal, derived
- 3 = temp - external, filtered
- 4 = temp - external, derived

light

- 5 = light intensity - linear, filtered
- 6 = light intensity - linear, derived
- 7 = light intensity - logarithmic, filtered
- 8 = light intensity - logarithmic, derived
- 9 = lamp - current
- 10 = lamp - voltage

Vent

- 11 = vent - current
- 12 = vent - volatge
- 13 = vent - rpm, filtered
- 14 = vent - rpm, derived



