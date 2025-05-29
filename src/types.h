/*
 * SPDX-FileCopyrightText: Sebastiano Barezzi
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <stdint.h>

enum class IgnitionState {
	OFF = 0,
	ACC = 1,
	ON = 2,
};

enum class CarServiceStatus {
	SAFE = 0,
	CLOSE = 1,
	REACHED = 2,
};

enum class Gear {
	GEAR_HIDDEN = 0,
	GEAR_P = 1,
	GEAR_R = 2,
	GEAR_N = 3,
	GEAR_D = 4,
	GEAR_D_1 = 5,
	GEAR_D_2 = 6,
	GEAR_D_3 = 7,
	GEAR_D_4 = 8,
	GEAR_D_5 = 9,
	GEAR_D_6 = 10,
	GEAR_1 = 11,
	GEAR_2 = 12,
	GEAR_3 = 13,
	GEAR_4 = 14,
	GEAR_5 = 15,
	GEAR_6 = 16,
};

enum class LightStatus {
	OFF = 0,
	ON = 1,
	BLINKING = 2,
};

enum class EngineFault {
	NONE = 0,
	MINOR = 1,
	MAJOR = 2,
};

enum class EngineOilLevel {
	OK = 0,
	NOT_OK = 1,
	UNKNOWN = 2,
};

enum class FeatureStatus {
	ENABLED = 0,
	DISABLED = 1,
	ACTIVE = 2,
};

struct State {
	// Engine / key status
	IgnitionState ignitionState = IgnitionState::OFF;
	bool engineStarted = false;

	// Dashboard cosmetics
	bool economyModeEnabled = false;
	bool dashboardLightingEnabled = false;
	bool darkModeEnabled = false;
	uint8_t dashboardBrightness = 0x00;

	// Gauges
	int rpm = 0;
	int speedKmh = 0;
	int engineCoolantTemperatureCelsius = 0;
	int ambientTemperatureCelsius = 0;
	int fuelLevelPercentage = 0;

	// Odometer / trim meter / car service
	uint32_t odometerKm = 0;
	uint32_t tripMeterMeters = 0;
	CarServiceStatus carServiceStatus = CarServiceStatus::SAFE;
	uint16_t serviceCounterKm = 0;

	// Gear
	Gear gear = Gear::GEAR_HIDDEN;
	bool blinkingGear = false;
	bool autoGearSelection = false;
	bool sportMode = false;

	// Lights
	bool sidelights = false;
	bool lowBeams = false;
	bool highBeams = false;
	bool frontFogLights = false;
	bool rearFogLights = false;
	bool leftIndicator = false;
	bool rightIndicator = false;
	bool dieselGlowPlugsLight = false;
	LightStatus pressBrakePedalLightStatus = LightStatus::OFF;
	bool secondPassengerAirbagDisabled = false;
	bool stopLight = false;
	bool serviceLight = false;
	bool anyDoorOpen = false;
	LightStatus driverSeatBeltsStatus = LightStatus::OFF;
	LightStatus passengerSeatBeltsStatus = LightStatus::OFF;
	LightStatus rearLeftSeatBeltsStatus = LightStatus::OFF;
	LightStatus rearCenterSeatBeltsStatus = LightStatus::OFF;
	LightStatus rearRightSeatBeltsStatus = LightStatus::OFF;
	bool lowFuel = false;
	LightStatus pressClutchLightStatus = LightStatus::OFF;
	EngineFault engineFault = EngineFault::NONE;

	// Warning lights
	LightStatus checkEngineLightStatus = LightStatus::OFF;
	LightStatus parkingBrakeLightStatus = LightStatus::OFF;
	bool automaticParkingBrakeDisabled = false;
	bool automaticParkingBrakeIssue = false;
	EngineOilLevel engineOilLevel = EngineOilLevel::OK;
	bool highEngineCoolantTemperature = false;
	bool automaticWipersEnabled = false;
	LightStatus batteryNotChargingLightStatus = LightStatus::OFF;
	bool sportAndWinterGearBlink = false;
	LightStatus startAndStopLightStatus = LightStatus::OFF;
	bool powerSteeringWarningLight = false;
	bool airbagWarningLight = false;
	bool lowBeamsWarningLight = false;
	bool waterInFuelFilterWarningLight = false;

	// Security features
	FeatureStatus tcStatus = FeatureStatus::ENABLED;
	FeatureStatus absStatus = FeatureStatus::ENABLED;
};
