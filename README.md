# SimpleSimHubArduino

![Hero](assets/hero.jpg)

Simple Arduino project for SimHub to car clusters

## Supported clusters

| Manufacturer | Model | Years | Part number | Information |
| ------------ | ----- | ----- | ----------- | ------ |
| Peugeot | 208 I | 2012 - 2019 | 9801906680 | [Here](src/clusters/peugeot_208_i/README.md) |

## BOM

- Arduino
- MCP2515 CAN module
- Jumper wires
- Your cluster (usually runs on 12V, so an additional power supply should be needed)

## MCP2515 pinout

Refer to [this](https://github.com/autowp/arduino-mcp2515?tab=readme-ov-file#can-shield)

## Instructions

1. Install [SimHub](https://www.simhubdash.com/)
2. Install [Arduino IDE](https://www.arduino.cc/en/software)
3. Install [Arduino MCP2515 library (autowp-mcp2515)](https://github.com/autowp/arduino-mcp2515)
4. Download this repository
5. Open the project in Arduino IDE
6. Enable the cluster you want to use in `SHCustomProtocol.h`
7. Connect your cluster, the Arduino and the MCP2515 CAN module according to the wiring instructions
   in this README and the cluster's one
8. Upload the code to your Arduino
9. Open SimHub and configure the custom protocol message (see below)
10. Enjoy

## SimHub custom protocol message

```ncalc
''
+ isnull([DataCorePlugin.GameData.TemperatureUnit], 'Celcius') + ';'
+ isnull([DataCorePlugin.GameData.OilPressureUnit], 'Bar') + ';'
+ isnull([DataCorePlugin.GameData.FuelUnit], 'Liters') + ';'
+ isnull([DataCorePlugin.GameData.SpeedLocalUnit], 'KMH') + ';'
+ isnull([DataCorePlugin.GameData.EngineIgnitionOn], '1') + ';'
+ isnull([DataCorePlugin.GameData.EngineStarted], '1') + ';'
+ format([DataCorePlugin.GameData.Rpms], '0') + ';'
+ format([DataCorePlugin.GameData.SpeedKmh], '0') + ';'
+ format([DataCorePlugin.GameData.WaterTemperature], '0') + ';'
+ format([DataCorePlugin.GameData.AirTemperature], '0') + ';'
+ format([DataCorePlugin.GameData.FuelPercent], '0') + ';'
+ format([DataCorePlugin.GameData.SessionOdo], '0') + ';'
+ isnull([DataCorePlugin.GameData.InstantConsumption_L100KM], '0') + ';'
+ format([DataCorePlugin.GameData.MaxSpeedKmh], '0') + ';'
+ format([DataCorePlugin.GameData.StintOdo], '0') + ';'
+ isnull([DataCorePlugin.Computed.Fuel_CurrentLapConsumption], '0') + ';'
+ isnull([DataCorePlugin.Computed.Fuel_LastLapConsumption], '0') + ';'
+ isnull([DataCorePlugin.GameData.Gear], 'N') + ';'
+ format([DataCorePlugin.GameData.TurnIndicatorLeft], '0') + ';'
+ format([DataCorePlugin.GameData.TurnIndicatorRight], '0') + ';'
+ if(changed(1000, [DataCorePlugin.GameData.TCActive]), '1', '0') + ';'
+ if(changed(1000, [DataCorePlugin.GameData.ABSActive]), '1', '0') + ';'
```

## License

This project is licensed under the GPL v3.0 License or later, except for the following components:

- Cluster commands are licensed under the Apache License 2.0, to let other people use the reverse
  engineered commands however they want
- The SimHub custom protocol message source code is unlicensed, but it hasn't been modified at all,
  except for `SHCustomProtocol.h`, which has been modified to just read the custom message and
  storing values in the `State` struct

Friendly reminder that GPL v3.0 taints the whole project, so shall you use this project with
modifications, you must release the whole project.

Luckily for you, pull requests are welcome, so you can contribute to this project (like adding
support for new clusters) without worrying about licensing issues :)
