/*
 * SPDX-FileCopyrightText: Sebastiano Barezzi
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <mcp2515.h>
#include <stdint.h>
#include "../../MessageDebouncer.h"
#include "../../types.h"

static MCP2515 mcp2515(10);

static void sendIgnitionAndLighting(
	bool economyModeEnabled,
	bool dashboardLightingEnabled,
	uint8_t dashboardBrightness,
	IgnitionState ignitionState
) {
	static MessageDebouncer messageDebouncer(100);
	struct can_frame frame;

	if (!messageDebouncer.shouldUpdate()) {
		return;
	}

	frame.can_id = 0x036;
	frame.can_dlc = 8;
	frame.data[0] = 0x00;
	frame.data[1] = 0x00;
	frame.data[2] = 0x00
		| (economyModeEnabled ? 0x80 : 0x00); // Bit 7: Economy mode enabled
	frame.data[3] = 0x00
		| (dashboardLightingEnabled ? 0x20 : 0x00) // Bit 5: Dashboard lightning enabled
		| (dashboardBrightness > 0x0F ? 0x0F : dashboardBrightness); // Bit 0-3: Brightness for dashboard lightning
	frame.data[4] = 0x00
		| (ignitionState == IgnitionState::OFF ? 0x02 : 0x00) // Bit 0-1: Ignition state
		| (ignitionState == IgnitionState::ON ? 0x01 : 0x00); // 1: On, 2: Off, 3: N/A (dimmed)
	frame.data[5] = 0x00;
	frame.data[6] = 0x00;
	frame.data[7] = 0x00;

	mcp2515.sendMessage(&frame);
}

static void sendRpmAndSpeed(
	int rpm,
	int speedKmh
) {
	static MessageDebouncer messageDebouncer(50);
	struct can_frame frame;

	if (!messageDebouncer.shouldUpdate()) {
		return;
	}

	uint16_t rpmData = rpm << 3;
	uint16_t speedData = speedKmh * 100;

	frame.can_id = 0x0B6;
	frame.can_dlc = 8;
	frame.data[0] = rpmData >> 8 & 0xFF; // RPM
	frame.data[1] = rpmData & 0xFF; // RPM
	frame.data[2] = speedData >> 8 & 0xFF; // Speed (km/h * 100)
	frame.data[3] = speedData & 0xFF; // Speed (km/h * 100)
	frame.data[4] = 0x00; // Does nothing
	frame.data[5] = 0x00; // Does nothing
	frame.data[6] = 0x00; // Does nothing
	frame.data[7] = 0x00; // Used for immobilizer, ignore

	mcp2515.sendMessage(&frame);
}

static void sendIgnitionAndCoolantTempAndOdometerAndAmbientTempAndReverseAndTurnSignals(
	IgnitionState ignitionState,
	bool darkModeEnabled,
	int engineCoolantTemperatureCelsius,
	uint32_t odometerKm,
	int ambientTemperatureCelsius,
	Headlights &headlights,
	Gear gear
) {
	static MessageDebouncer messageDebouncer(500);
	struct can_frame frame;

	if (!messageDebouncer.shouldUpdate()) {
		return;
	}

	uint8_t engineCoolantTemperatureData;
	if (engineCoolantTemperatureCelsius <= 90) {
		engineCoolantTemperatureData = (engineCoolantTemperatureCelsius + 130) / 2;
	} else if (engineCoolantTemperatureCelsius <= 100) {
		engineCoolantTemperatureData = (engineCoolantTemperatureCelsius - 62.5) * 4;
	} else if (engineCoolantTemperatureCelsius <= 110) {
		engineCoolantTemperatureData = (engineCoolantTemperatureCelsius + 200) / 2;
	} else if (engineCoolantTemperatureCelsius <= 130) {
		engineCoolantTemperatureData = (engineCoolantTemperatureCelsius + 946) / 6.7;
	} else {
		engineCoolantTemperatureData = 160; // Clamp to 130 °C
	}

	uint32_t odometerData = odometerKm * 10;
	uint8_t ambientTemperatureData = (ambientTemperatureCelsius / 2.0) - 39.5;

	frame.can_id = 0x0F6;
	frame.can_dlc = 8;
	frame.data[0] = 0x00
		| (darkModeEnabled ? 0x10 : 0x00) // Bit 4: Power off displays
		| (ignitionState == IgnitionState::ON ? 0x08 : 0x00); // Bit 3: Ignition
	frame.data[1] = engineCoolantTemperatureData; // Coolant temperature
	frame.data[2] = odometerData >> 16 & 0xFF; // Odometer
	frame.data[3] = odometerData >> 8 & 0xFF; // Odometer
	frame.data[4] = odometerData & 0xFF; // Odometer
	frame.data[5] = ambientTemperatureData; // Ambient temperature
	frame.data[6] = ambientTemperatureData; // Filtered ambient temperature
	frame.data[7] = 0x00
		| (gear == Gear::GEAR_R ? 0x80 : 0x00) // Bit 7: Reverse gear
		| (headlights.rightIndicator ? 0x02 : 0x00) // Bit 1: Turn right
		| (headlights.leftIndicator ? 0x01 : 0x00); // Bit 0: Turn left

	mcp2515.sendMessage(&frame);
}

static void sendDashboardLights(
	Headlights &headlights,
	bool blinkingGear,
	Gear gear,
	bool autoGearSelection,
	bool sportMode,
	LightStatus pressBrakePedalLightStatus,
	bool secondPassengerAirbagDisabled,
	bool stopWarning,
	bool serviceWarning,
	FeatureStatus tcStatus,
	bool anyDoorOpen,
	LightStatus driverSeatBeltsStatus,
	LightStatus passengerSeatBeltsStatus,
	LightStatus rearLeftSeatBeltsStatus,
	bool lowFuel,
	LightStatus rearCenterSeatBeltsStatus,
	LightStatus rearRightSeatBeltsStatus,
	EngineFault engineFault,
	LightStatus pressClutchLightStatus
) {
	static MessageDebouncer messageDebouncer(200);
	struct can_frame frame;

	if (!messageDebouncer.shouldUpdate()) {
		return;
	}

	uint8_t gearData;
	switch (gear) {
		case Gear::GEAR_HIDDEN:
			gearData = 0xB0;
			break;
		case Gear::GEAR_P:
			gearData = 0x00;
			break;
		case Gear::GEAR_R:
			gearData = 0x10;
			break;
		case Gear::GEAR_N:
			gearData = 0x20;
			break;
		case Gear::GEAR_D:
			gearData = 0x30;
			break;
		case Gear::GEAR_D_1:
			gearData = 0x32;
			break;
		case Gear::GEAR_D_2:
			gearData = 0x34;
			break;
		case Gear::GEAR_D_3:
			gearData = 0x36;
			break;
		case Gear::GEAR_D_4:
			gearData = 0x38;
			break;
		case Gear::GEAR_D_5:
			gearData = 0x3A;
			break;
		case Gear::GEAR_D_6:
			gearData = 0x3C;
			break;
		case Gear::GEAR_1:
			gearData = 0x90;
			break;
		case Gear::GEAR_2:
			gearData = 0x80;
			break;
		case Gear::GEAR_3:
			gearData = 0x70;
			break;
		case Gear::GEAR_4:
			gearData = 0x60;
			break;
		case Gear::GEAR_5:
			gearData = 0x50;
			break;
		case Gear::GEAR_6:
			gearData = 0x40;
			break;
		default:
			gearData = 0xB0;
			break;
	}

	frame.can_id = 0x128;
	frame.can_dlc = 8;
	frame.data[0] = 0x00
		| (headlights.sidelights ? 0x80 : 0x00) // Bit 7: Sidelights
		| (headlights.lowBeam ? 0x40 : 0x00) // Bit 6: Low beam
		| (headlights.highBeam ? 0x20 : 0x00) // Bit 5: High beam
		| (headlights.frontFogLights ? 0x10 : 0x00) // Bit 4: Front fog lights
		| (headlights.rearFogLights ? 0x08 : 0x00) // Bit 3: Rear fog lights
		| (headlights.rightIndicator ? 0x04 : 0x00) // Bit 2: Right indicator
		| (headlights.leftIndicator ? 0x02 : 0x00); // Bit 1: Left indicator
	frame.data[1] = gearData // Bit 7-4: Gear type (P, R, N, D, 1-6), Bit 3-1: D gear number
		| (blinkingGear ? 0x01 : 0x00); // Bit 0: Blinking indicator
	frame.data[2] = 0x00
		| (sportMode ? 0x20 : 0x00) // Bit 5: Sport mode
		| (autoGearSelection ? 0x02 : 0x00); // Bit 1: Auto gear selection for automated manual transmission
	frame.data[3] = 0x00
		| (serviceWarning ? 0x80 : 0x00) // Bit 7: Service warning light
		| (stopWarning ? 0x40 : 0x00) // Bit 6: Stop warning light
		| (secondPassengerAirbagDisabled ? 0x10 : 0x00) // Bit 4: Second passenger airbag disabled
		| (pressBrakePedalLightStatus == LightStatus::BLINKING ? 0x08 : 0x00) // Bit 3: Press brake pedal light blinking
		| (pressBrakePedalLightStatus == LightStatus::ON ? 0x04 : 0x00); // Bit 2: Press brake pedal light
	frame.data[4] = 0x00
		| (anyDoorOpen ? 0x40 : 0x00) // Bit 6: Any door open
		| (tcStatus == FeatureStatus::DISABLED ? 0x04 : 0x00) // Bit 2: ESP disabled
		| (tcStatus == FeatureStatus::ACTIVE ? 0x02 : 0x00); // Bit 1: ESP active
	frame.data[5] = 0x00
		| (lowFuel ? 0x80 : 0x00) // Bit 7: Low fuel light
		| (driverSeatBeltsStatus != LightStatus::OFF ? 0x40 : 0x00) // Bit 6: Rear left seat belts
		| (driverSeatBeltsStatus == LightStatus::BLINKING ? 0x20 : 0x00) // Bit 5: Passenger seat belts blink
		| (passengerSeatBeltsStatus != LightStatus::OFF ? 0x10 : 0x00) // Bit 4: Passenger seat belts
		| (passengerSeatBeltsStatus == LightStatus::BLINKING ? 0x08 : 0x00) // Bit 3: Seat belts blink
		| (rearLeftSeatBeltsStatus != LightStatus::OFF ? 0x01 : 0x00); // Bit 0: Driver seat belts
	frame.data[6] = 0x00
		| (rearLeftSeatBeltsStatus == LightStatus::BLINKING ? 0x80 : 0x00) // Bit 7: Rear right seat belts blink
		| (rearCenterSeatBeltsStatus != LightStatus::OFF ? 0x40 : 0x00) // Bit 6: Rear right seat belts
		| (rearCenterSeatBeltsStatus == LightStatus::BLINKING ? 0x20 : 0x00) // Bit 5: Rear center seat belts blink
		| (rearRightSeatBeltsStatus != LightStatus::OFF ? 0x10 : 0x00) // Bit 4: Rear center seat belts
		| (rearRightSeatBeltsStatus == LightStatus::BLINKING ? 0x08 : 0x00); // Bit 3: Rear left seat belts blink
	frame.data[7] = 0x00
		| (engineFault == EngineFault::MINOR ? 0x80 : 0x00) // Bit 7: Minor engine fault
		| (engineFault == EngineFault::MAJOR ? 0x40 : 0x00) // Bit 6: Minor engine fault
		| (pressClutchLightStatus == LightStatus::BLINKING ? 0x20 : 0x00) // Bit 5: Press clutch blink
		| (pressClutchLightStatus != LightStatus::OFF ? 0x10 : 0x00); // Bit 4: Press clutch light

	mcp2515.sendMessage(&frame);
}

static void sendFuelAndOil(
	int engineOilTemperatureCelsius,
	uint8_t fuelLevelPercentage,
	EngineOilLevel engineOilLevel
) {
	static MessageDebouncer messageDebouncer(500);
	struct can_frame frame;

	if (!messageDebouncer.shouldUpdate()) {
		return;
	}

	uint8_t engineOilTemperatureData = (engineOilTemperatureCelsius / 2.0) - 39.5;

	frame.can_id = 0x161;
	frame.can_dlc = 7;
	frame.data[0] = 0x00;
	frame.data[1] = 0x00;
	frame.data[2] = engineOilTemperatureData; // Engine oil temperature
	frame.data[3] = fuelLevelPercentage; // Fuel (0x00 - 0x64, 0xFF == 0x00)
	frame.data[4] = 0x00;
	frame.data[5] = 0x00;
	frame.data[6] = engineOilLevel == EngineOilLevel::UNKNOWN ? 0xFF : 0x00; // 0xFF: Oil reading invalid
	//frame.data[7] = 0x00;

	mcp2515.sendMessage(&frame);
}

static void sendWarningLights(
	LightStatus parkingBrakeLightStatus,
	bool engineOilPressureWarning,
	EngineOilLevel engineOilLevel,
	LightStatus highEngineCoolantTemperatureLightStatus,
	bool automaticWipersEnabled,
	EngineFault engineFault,
	LightStatus batteryNotChargingLightStatus,
	bool sportAndWinterGearBlink,
	FeatureStatus tcStatus,
	FeatureStatus absStatus,
	LightStatus startAndStopLightStatus,
	bool powerSteeringWarning,
	bool airbagWarning,
	bool lowBeamWarning,
	bool waterInFuelFilterWarning
) {
	static MessageDebouncer messageDebouncer(200);
	struct can_frame frame;

	if (!messageDebouncer.shouldUpdate()) {
		return;
	}

	frame.can_id = 0x168;
	frame.can_dlc = 8;
	frame.data[0] = 0x00
		| (highEngineCoolantTemperatureLightStatus != LightStatus::OFF ? 0x80 : 0x00) // Bit 7: High engine coolant temperature
		| (engineOilLevel == EngineOilLevel::NOT_OK ? 0x10 : 0x00) // Bit 4: Engine oil level not ok
		| (engineOilPressureWarning ? 0x08 : 0x00) // Bit 3: Engine oil pressure warning
		| (parkingBrakeLightStatus == LightStatus::ON ? 0x04 : 0x00); // Bit 2: Parking brake light
	frame.data[1] = 0x00
		| (engineFault == EngineFault::MINOR ? 0x10 : 0x00) // Bit 4: Minor engine fault light
		| (automaticWipersEnabled ? 0x08 : 0x00); // Bit 3: Automatic wipers enabled
	frame.data[2] = 0x00
		| (parkingBrakeLightStatus == LightStatus::ON ? 0x08 : 0x00) // Bit 3: Parking brake light
		| (batteryNotChargingLightStatus != LightStatus::OFF ? 0x03 : 0x00); // Bit 0-1: Battery not charging light
	frame.data[3] = 0x00
		| (absStatus == FeatureStatus::DISABLED ? 0x20 : 0x00) // Bit 5: ABS disabled
		| (tcStatus == FeatureStatus::DISABLED ? 0x10 : 0x00) // Bit 4: ESP disabled
		| (sportAndWinterGearBlink ? 0x08 : 0x00); // Bit 3: Sport and/or winter gear mode blinking
	frame.data[4] = 0x00
		| (waterInFuelFilterWarning ? 0x80 : 0x00) // Bit 7: Water in fuel filter warning light
		| (lowBeamWarning ? 0x40 : 0x00) // Bit 6: Low beams blink
		| (airbagWarning ? 0x20 : 0x00) // Bit 5: Airbag warning light
		| (powerSteeringWarning ? 0x04 : 0x00) // Bit 2: Power steering warning light
		| (startAndStopLightStatus == LightStatus::BLINKING ? 0x02 : 0x00) // Bit 1: Start and Stop light blink
		| (startAndStopLightStatus == LightStatus::ON ? 0x01 : 0x00); // Bit 0: Start and Stop light
	frame.data[5] = 0x00;
	frame.data[6] = 0x00;
	frame.data[7] = 0x00;

	mcp2515.sendMessage(&frame);
}

static void sendTripMeter(
	Trip &currentTrip
) {
	static MessageDebouncer messageDebouncer(100); // Should be 200ms, decreasing to fix no data
	struct can_frame frame;

	if (!messageDebouncer.shouldUpdate()) {
		return;
	}

	// TODO: Contains cruise control as well

	frame.can_id = 0x1A8;
	frame.can_dlc = 8;
	frame.data[0] = 0x00; // Does nothing
	frame.data[1] = 0x00; // Does nothing
	frame.data[2] = 0x00; // Does nothing
	frame.data[3] = 0x00; // Does nothing
	frame.data[4] = 0x00; // Does nothing
	frame.data[5] = currentTrip.distanceMeters >> 16 & 0xFF;
	frame.data[6] = currentTrip.distanceMeters >> 8 & 0xFF;
	frame.data[7] = currentTrip.distanceMeters & 0xFF;

	mcp2515.sendMessage(&frame);
}

static void sendServiceLight(
	CarServiceStatus carServiceStatus,
	uint16_t serviceCounterKm
) {
	static MessageDebouncer messageDebouncer(200); // TODO: Unknown
	struct can_frame frame;

	if (!messageDebouncer.shouldUpdate()) {
		return;
	}

	uint16_t counterData = serviceCounterKm / 20.1168;

	frame.can_id = 0x3E7;
	frame.can_dlc = 8;
	frame.data[0] = 0x00
		| (carServiceStatus == CarServiceStatus::REACHED ? 0x20 : 0x00) //Bit 5: Negative counter
		| (carServiceStatus == CarServiceStatus::CLOSE ? 0x10 : 0x00) // Bit 4: Blinking counter
		| (carServiceStatus == CarServiceStatus::REACHED ? 0x08 : 0x00); // Bit 3: Fixed service icon
	frame.data[1] = 0x00; // Does nothing
	frame.data[2] = 0x00; // Does nothing
	frame.data[3] = (counterData >> 8) & 0xFF;
	frame.data[4] = counterData & 0xFF;
	frame.data[5] = 0x00; // Does nothing
	frame.data[6] = 0x00; // Does nothing
	frame.data[7] = 0x00; // Does nothing

	mcp2515.sendMessage(&frame);
}

static void sendLocalization(
	Locale &locale
) {
	static MessageDebouncer messageDebouncer(1000);
	struct can_frame frame;

	if (!messageDebouncer.shouldUpdate()) {
		return;
	}

	uint8_t unitsData = 0x00;
	switch (locale.timeDisplayMode) { // Bit 7: 12/24-hour mode
		case TimeDisplayMode::MODE_12_HOUR:
			unitsData |= 0x00;
			break;
		case TimeDisplayMode::MODE_24_HOUR:
			unitsData |= 0x80;
			break;
		default:
			unitsData |= 0x00; // Default to 12-hour mode
			break;
	}
	switch (locale.temperatureUnit) { // Bit 5: Temperature unit
		case TemperatureUnit::CELSIUS:
			unitsData |= 0x00;
			break;
		case TemperatureUnit::FAHRENHEIT:
			unitsData |= 0x20;
			break;
		default:
			unitsData |= 0x00; // Default to Celsius
			break;
	}
	switch (locale.pressureUnit) { // Bit 3: Pressure unit
		case PressureUnit::BAR:
			unitsData |= 0x00;
			break;
		case PressureUnit::PSI:
			unitsData |= 0x08;
			break;
		default:
			unitsData |= 0x00; // Default to bar
			break;
	}
	switch (locale.consumptionUnit) { // Bit 2: Consumption unit
		case ConsumptionUnit::VOLUME_PER_DISTANCE:
			unitsData |= 0x00;
			break;
		case ConsumptionUnit::DISTANCE_PER_VOLUME:
			unitsData |= 0x04;
			break;
		default:
			unitsData |= 0x00; // Default to volume per distance
			break;
	}
	switch (locale.volumeUnit) { // Bit 1: Volume unit
		case VolumeUnit::LITERS:
			unitsData |= 0x00;
			break;
		case VolumeUnit::GALLONS:
			unitsData |= 0x02;
			break;
		default:
			unitsData |= 0x00; // Default to liters
			break;
	}
	switch (locale.distanceUnit) { // Bit 0: Distance unit
		case DistanceUnit::KILOMETERS:
			unitsData |= 0x00;
			break;
		case DistanceUnit::MILES:
			unitsData |= 0x01;
			break;
		default:
			unitsData |= 0x00; // Default to kilometers
			break;
	}

	uint8_t languageData = 0x00;
	switch (locale.language) {
		case Language::ENGLISH:
			languageData = 0x01;
			break;
		case Language::FRENCH:
			languageData = 0x00;
			break;
		case Language::GERMAN:
			languageData = 0x02;
			break;
		case Language::SPANISH:
			languageData = 0x03;
			break;
		case Language::ITALIAN:
			languageData = 0x04;
			break;
		case Language::PORTUGUESE:
			languageData = 0x05;
			break;
		case Language::DUTCH:
			languageData = 0x06;
			break;
		case Language::GREEK:
			languageData = 0x07;
			break;
		case Language::BRASILIAN_PORTUGUESE:
			languageData = 0x08;
			break;
		case Language::POLISH:
			languageData = 0x09;
			break;
		case Language::TRADITIONAL_CHINESE:
			languageData = 0x0A;
			break;
		case Language::SIMPLIFIED_CHINESE:
			languageData = 0x0B;
			break;
		case Language::TURKISH:
			languageData = 0x0C;
			break;
		case Language::RUSSIAN:
			languageData = 0x0E;
			break;
		default:
			languageData = 0x01; // Default to English
			break;
	}

	frame.can_id = 0x3F6;
	frame.can_dlc = 7;
	frame.data[0] = 0x00;
	frame.data[1] = 0x00;
	frame.data[2] = 0x00;
	frame.data[3] = 0x00;
	frame.data[4] = 0x00;
	frame.data[5] = unitsData;
	frame.data[6] = languageData; // Bit 0-3: Language
	//frame.data[7] = 0x00;

	mcp2515.sendMessage(&frame);
}
