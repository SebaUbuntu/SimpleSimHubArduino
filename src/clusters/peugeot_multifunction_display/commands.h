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
	frame.data[4] = 0x00; // Odometer from start (cm)
	frame.data[5] = 0x00; // Odometer from start (cm)
	frame.data[6] = 0x00; // Fuel consumption
	frame.data[7] = 0x00; // Does nothing

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

	uint32_t odometerData = (odometerKm * 10);

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
	LightStatus passengerSeatBeltsStatus,
	bool dieselGlowPlugsLight,
	bool lowFuel,
	LightStatus driverSeatBeltsStatus,
	LightStatus parkingBrakeLightStatus,
	bool stopWarning,
	bool serviceWarning,
	FeatureStatus tcStatus,
	Headlights &headlights,
	bool automaticParkingBrakeDisabled,
	LightStatus rearLeftSeatBeltsStatus,
	LightStatus rearCenterSeatBeltsStatus,
	LightStatus rearRightSeatBeltsStatus,
	bool blinkingGear,
	Gear gear,
	bool autoGearSelection,
	bool sportMode
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

	frame.can_id = 0x128; // 0x928 is also accepted
	frame.can_dlc = 8;
	frame.data[0] = 0x00
		| (driverSeatBeltsStatus != LightStatus::OFF ? 0x40 : 0x00) // Bit 6: Driver seat belts
		| (parkingBrakeLightStatus == LightStatus::ON ? 0x20 : 0x00) // Bit 5: Parking brake light
		| (lowFuel ? 0x10 : 0x00) // Bit 4: Low fuel light
		| (dieselGlowPlugsLight ? 0x04 : 0x00) // Bit 2: Diesel glow plug light
		| (passengerSeatBeltsStatus != LightStatus::OFF ? 0x02 : 0x00); // Bit 1: Passenger seat belts
	frame.data[1] = 0x00
		| (serviceWarning ? 0x80 : 0x00) // Bit 7: Service light
		| (stopWarning ? 0x40 : 0x00); // Bit 6: Stop light on
	frame.data[2] = 0x00
		| (tcStatus == FeatureStatus::DISABLED ? 0x10 : 0x00) // Bit 5: ESP disabled
		| (tcStatus == FeatureStatus::ACTIVE ? 0x08 : 0x00); // Bit 3: ESP active
	frame.data[3] = 0x00; // Does nothing
	frame.data[4] = 0x00
		| (headlights.sidelights ? 0x80 : 0x00) // Bit 7: Sidelights
		| (headlights.lowBeam ? 0x40 : 0x00) // Bit 6: Low beams
		| (headlights.highBeam ? 0x20 : 0x00) // Bit 5: High beams
		| (headlights.frontFogLights ? 0x10 : 0x00) // Bit 4: Front fog lights
		| (headlights.rearFogLights ? 0x08 : 0x00) // Bit 3: Rear fog lights
		| (headlights.rightIndicator ? 0x04 : 0x00) // Bit 2: Right indicator
		| (headlights.leftIndicator ? 0x02 : 0x00); // Bit 1: Left indicator
	frame.data[5] = 0x00 // TODO: Blinks on 0xF0, 0xE0 and 0x70, it possibly remembers seat belts status
		| (rearLeftSeatBeltsStatus != LightStatus::OFF ? 0x40 : 0x00) // Bit 6: Rear left seat belts
		| (rearCenterSeatBeltsStatus != LightStatus::OFF ? 0x10 : 0x00) // Bit 4: Rear center seat belts
		| (rearRightSeatBeltsStatus != LightStatus::OFF ? 0x04 : 0x00) // Bit 2: Rear right seat belts
		| (automaticParkingBrakeDisabled ? 0x01 : 0x00); // Bit 0: Automatic parking brake disabled
	frame.data[6] = gearData // Bit 7-4: Gear type (P, R, N, D, 1-6), Bit 3-1: D gear number
		| (blinkingGear ? 0x01 : 0x00); // Bit 0: Blinking indicator
	frame.data[7] = 0x00
		| (sportMode ? 0x20 : 0x00) // Bit 5: Sport mode
		| (autoGearSelection ? 0x02 : 0x00) // Bit 1: Auto gear selection for automated manual transmission
		| (gear == Gear::GEAR_HIDDEN ? 0x01 : 0x00); // Bit 0: Hide gear indicator

	mcp2515.sendMessage(&frame);
}

static void sendOilOk(
	EngineOilLevel engineOilLevel
) {
	static MessageDebouncer messageDebouncer(500);
	struct can_frame frame;

	if (!messageDebouncer.shouldUpdate()) {
		return;
	}

	frame.can_id = 0x161; // 0x961 is also accepted
	frame.can_dlc = 7;
	frame.data[0] = 0x00;
	frame.data[1] = 0x00;
	frame.data[2] = 0x00;
	frame.data[3] = 0x00;
	frame.data[4] = 0x00;
	frame.data[5] = 0x00;
	frame.data[6] = engineOilLevel == EngineOilLevel::UNKNOWN ? 0xFF : 0x00; // 0xFF: Oil reading invalid
	//frame.data[7] = 0x00;

	mcp2515.sendMessage(&frame);
}

static void sendWarningLights(
	LightStatus parkingBrakeLightStatus,
	EngineOilLevel engineOilLevel,
	LightStatus highEngineCoolantTemperatureLightStatus,
	LightStatus checkEngineLightStatus,
	bool sportAndWinterGearBlink,
	FeatureStatus tcStatus,
	FeatureStatus absStatus,
	LightStatus batteryNotChargingLightStatus,
	bool automaticParkingBrakeIssue
) {
	static MessageDebouncer messageDebouncer(200);
	struct can_frame frame;

	if (!messageDebouncer.shouldUpdate()) {
		return;
	}

	frame.can_id = 0x168; // 0x968 is also accepted
	frame.can_dlc = 8;
	frame.data[0] = 0x00
		| (highEngineCoolantTemperatureLightStatus != LightStatus::OFF ? 0x80 : 0x00) // Bit 7: High engine coolant temperature
		| (engineOilLevel == EngineOilLevel::NOT_OK ? 0x10 : 0x00) // Bit 4: Engine oil level not ok
		| (parkingBrakeLightStatus == LightStatus::ON ? 0x04 : 0x00); // Bit 2: Parking brake light
	frame.data[1] = 0x00; // Does nothing
	frame.data[2] = 0x00; // Does nothing
	frame.data[3] = 0x00
		| (parkingBrakeLightStatus == LightStatus::ON ? 0x80 : 0x00) // Bit 7: Parking brake light, possibly issues with it
		| (absStatus != FeatureStatus::ENABLED ? 0x20 : 0x00) // Bit 5: ABS disabled
		| (tcStatus == FeatureStatus::DISABLED ? 0x10 : 0x00) // Bit 4: ESP disabled
		| (sportAndWinterGearBlink ? 0x08 : 0x00) // Bit 3: Sport and/or winter gear mode blink
		| (checkEngineLightStatus == LightStatus::ON ? 0x02 : 0x00); // Bit 3: Check engine light
	frame.data[4] = 0x00
		| (checkEngineLightStatus == LightStatus::BLINKING ? 0x10 : 0x00) // Bit 4: Check engine light blink
		| (batteryNotChargingLightStatus == LightStatus::ON ? 0x04 : 0x00) // Bit 2: Battery not charging light
		| (batteryNotChargingLightStatus == LightStatus::ON ? 0x02 : 0x00); // Bit 1: Battery not charging light
	frame.data[5] = 0x00;
	frame.data[6] = 0x00
		| (automaticParkingBrakeIssue ? 0x10 : 0x00) // Bit 4: Automatic parking brake issue
		| (parkingBrakeLightStatus == LightStatus::BLINKING ? 0x08 : 0x00); // Bit 3: Parking brake light blink
	frame.data[7] = 0x00
		| (batteryNotChargingLightStatus == LightStatus::BLINKING ? 0x80 : 0x00) // Bit 7: Battery not charging light blink
		| (checkEngineLightStatus == LightStatus::BLINKING ? 0x40 : 0x00); // Bit 6: Check engine light blink

	mcp2515.sendMessage(&frame);
}

static void sendInformationalMessage() {
	static MessageDebouncer messageDebouncer(200);
	struct can_frame frame;

	if (!messageDebouncer.shouldUpdate()) {
		return;
	}

	frame.can_id = 0x1A1;
	frame.can_dlc = 8;
	frame.data[0] = 0xFF;
	frame.data[1] = 0x00;
	frame.data[2] = 0x00;
	frame.data[3] = 0x00;
	frame.data[4] = 0x00;
	frame.data[5] = 0x00;
	frame.data[6] = 0x00;
	frame.data[7] = 0x00;

	mcp2515.sendMessage(&frame);
}

static void sendTripComputerInfo(
	bool tripButtonPushed,
	uint16_t instantFuelConsumptionLP100Km,
	uint16_t remainingFuelDistanceKm,
	uint16_t remainingTripDistanceKm
) {
	static MessageDebouncer messageDebouncer(1000);
	struct can_frame frame;
	// Static variable to keep track of the trip button push status
	static bool tripButtonPushStatus = tripButtonPushStatus || tripButtonPushed;

	if (!messageDebouncer.shouldUpdate()) {
		return;
	}

	uint16_t instantFuelConsumptionData = round(instantFuelConsumptionLP100Km * 10.0);

	frame.can_id = 0x221;
	frame.can_dlc = 7;
	frame.data[0] = 0x00
		| (tripButtonPushStatus ? 0x08 : 0x00); // Bit 3: Trip (right) push button
	frame.data[1] = instantFuelConsumptionData >> 8 & 0xFF; // Instant fuel consumption in liters per 100 km
	frame.data[2] = instantFuelConsumptionData & 0xFF; // Instant fuel consumption in liters per 100 km
	frame.data[3] = remainingFuelDistanceKm >> 8 & 0xFF; // Remaining range in km
	frame.data[4] = remainingFuelDistanceKm & 0xFF; // Remaining range in km
	frame.data[5] = remainingTripDistanceKm >> 8 & 0xFF; // Remaining trip distance in km
	frame.data[6] = remainingTripDistanceKm & 0xFF; // Remaining trip distance in km
	//frame.data[7] = 0x00;

	mcp2515.sendMessage(&frame);

	// Reset trip button push status after sending the message
	tripButtonPushStatus = false;
}

static struct can_frame sendTripN(
	canid_t canId,
	Trip &trip
) {
	struct can_frame frame;

	uint16_t distanceKm = trip.distanceMeters / 1000;
	uint16_t averageFuelConsumptionData = round(trip.averageFuelConsumptionLP100Km * 10.0);

	// TODO: Is duration really in minutes?

	frame.can_id = canId;
	frame.can_dlc = 7;
	frame.data[0] = trip.averageSpeedKmh; // Average speed in km/h
	frame.data[1] = distanceKm >> 8 & 0xFF; // Trip meter in km
	frame.data[2] = distanceKm & 0xFF; // Trip meter in km
	frame.data[3] = averageFuelConsumptionData >> 8 & 0xFF; // Liters per 100 km
	frame.data[4] = averageFuelConsumptionData & 0xFF; // Liters per 100 km
	frame.data[5] = trip.durationMinutes >> 8 & 0xFF; // Duration in minutes
	frame.data[6] = trip.durationMinutes & 0xFF; // Duration in minutes
	//frame.data[7] = 0x00;

	return frame;
}

static void sendTrip2(
	Trip &trip
) {
	static MessageDebouncer messageDebouncer(1000);
	struct can_frame frame;

	if (!messageDebouncer.shouldUpdate()) {
		return;
	}

	frame = sendTripN(0x261, trip);

	mcp2515.sendMessage(&frame);
}

static void sendTrip1(
	Trip &trip
) {
	static MessageDebouncer messageDebouncer(1000);
	struct can_frame frame;

	if (!messageDebouncer.shouldUpdate()) {
		return;
	}

	frame = sendTripN(0x2A1, trip);

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
