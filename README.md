# open-dyson-battery
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Open source implementation of Dyson cordless vacuum cleaner battery firmware.

## DISCLAIMER

This project is created for purely educational purposes.
Li-ion batteries are high energy devices that may catch fire or explode 
if mishandled.
Review the code and use it at your own risk.

## Hardware
All older Dyson batteries are based on pic16lf1847 microcontroller with
ISL94208 battery controller.
Both ICs have excellent documentation from the manufacture.

### Dyson V6 battery
Reverse engineered schematic of v6 battery with marking 188002 on the board.
[![v6 schematic](hw/docs/original_bms_v6_188002.svg)](hw/docs/original_bms_v6_188002.svg)

### Dyson V8 battery
This is pretty much the same as v6 with some minor changes.
Reverse engineered schematic of v8 battery with marking 180207 on the board.
[![v8 schematic](hw/docs/original_bms_v8_180207.svg)](hw/docs/original_bms_v8_180207.svg)

## Contributing

Pull requests for new features, bug fixes, and suggestions are welcome!

## License
[MIT](LICENSE)


