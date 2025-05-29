/*
 * SPDX-FileCopyrightText: Sebastiano Barezzi
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <stdint.h>

enum class TimeDisplayMode {
	MODE_12_HOUR = 0,
	MODE_24_HOUR = 1,
};

enum class TemperatureUnit {
	CELSIUS = 0,
	FAHRENHEIT = 1,
	KELVIN = 2,
};

enum class PressureUnit {
	BAR = 0,
	KPA = 1,
	PSI = 2,
};

enum class ConsumptionUnit {
	VOLUME_PER_DISTANCE = 0, // e.g. liters per 100 km
	DISTANCE_PER_VOLUME = 1, // e.g. km per liter
};

enum class VolumeUnit {
	LITERS = 0,
	GALLONS = 1,
};

enum class DistanceUnit {
	KILOMETERS = 0,
	MILES = 1,
};

enum class Language {
	ENGLISH = 0,
	FRENCH = 1,
	GERMAN = 2,
	SPANISH = 3,
	ITALIAN = 4,
	PORTUGUESE = 5,
	DUTCH = 6,
	GREEK = 7,
	BRASILIAN_PORTUGUESE = 8,
	POLISH = 9,
	TRADITIONAL_CHINESE = 10,
	SIMPLIFIED_CHINESE = 11,
	TURKISH = 12,
	RUSSIAN = 13,
};

struct Locale {
	TimeDisplayMode timeDisplayMode = TimeDisplayMode::MODE_24_HOUR;
	TemperatureUnit temperatureUnit = TemperatureUnit::CELSIUS;
	PressureUnit pressureUnit = PressureUnit::BAR;
	ConsumptionUnit consumptionUnit = ConsumptionUnit::VOLUME_PER_DISTANCE;
	VolumeUnit volumeUnit = VolumeUnit::LITERS;
	DistanceUnit distanceUnit = DistanceUnit::KILOMETERS;
	Language language = Language::ENGLISH;
};

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

struct Trip {
	/**
	 * @brief Average speed in km/h. 0-255.
	 */
	uint8_t averageSpeedKmh = 0;

	/**
	 * @brief Distance traveled in meters.
	 */
	uint16_t distanceMeters = 0;

	/**
	 * @brief Fuel consumption in liters per 100 km.
	 */
	float averageFuelConsumptionLP100Km = 0;

	/**
	 * @brief Duration of the trip in minutes.
	 */
	uint16_t durationMinutes = 0;
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

struct Headlights {
	bool sidelights : 1;
	bool lowBeam : 1;
	bool highBeam : 1;
	bool frontFogLights : 1;
	bool rearFogLights : 1;
	bool leftIndicator : 1;
	bool rightIndicator : 1;
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
	// Locale
	Locale locale = {};

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
	int engineOilTemperatureCelsius = 0;
	int ambientTemperatureCelsius = 0;
	uint8_t fuelLevelPercentage = 0;

	// Odometer
	uint32_t odometerKm = 0;

	// Instant consumption
	float instantFuelConsumptionLP100Km = 0;
	uint16_t remainingFuelDistanceKm = 0;
	uint16_t remainingTripDistanceKm = 0;

	// Trips
	Trip currentTrip = {};
	Trip lastTrip = {};

	// Car service
	CarServiceStatus carServiceStatus = CarServiceStatus::SAFE;
	uint16_t serviceCounterKm = 0;

	// Gear
	Gear gear = Gear::GEAR_HIDDEN;
	bool blinkingGear = false;
	bool autoGearSelection = false;
	bool sportMode = false;

	// Headlights
	Headlights headlights = {};

	// Warning lights
	LightStatus checkEngineLightStatus = LightStatus::OFF;
	EngineFault engineFault = EngineFault::NONE;
	LightStatus parkingBrakeLightStatus = LightStatus::OFF;
	EngineOilLevel engineOilLevel = EngineOilLevel::OK;
	LightStatus highEngineCoolantTemperatureLightStatus = LightStatus::OFF;
	LightStatus batteryNotChargingLightStatus = LightStatus::OFF;
	LightStatus startAndStopLightStatus = LightStatus::OFF;
	bool powerSteeringWarning = false;
	bool airbagWarning = false;
	bool lowBeamWarning = false;
	bool waterInFuelFilterWarning = false;
	bool automaticParkingBrakeIssue = false;
	bool sportAndWinterGearBlink = false;
	bool stopWarning = false;
	bool serviceWarning = false;
	bool anyDoorOpen = false;
	bool lowFuel = false;
	bool engineOilPressureWarning = false;

	// Indicator lights
	bool dieselGlowPlugsLight = false;
	LightStatus pressBrakePedalLightStatus = LightStatus::OFF;
	LightStatus parkAssistLightStatus = LightStatus::OFF;
	bool secondPassengerAirbagDisabled = false;
	LightStatus pressClutchLightStatus = LightStatus::OFF;
	bool automaticParkingBrakeDisabled = false;
	bool automaticWipersEnabled = false;

	// Seat belts status
	LightStatus driverSeatBeltsStatus = LightStatus::OFF;
	LightStatus passengerSeatBeltsStatus = LightStatus::OFF;
	LightStatus rearLeftSeatBeltsStatus = LightStatus::OFF;
	LightStatus rearCenterSeatBeltsStatus = LightStatus::OFF;
	LightStatus rearRightSeatBeltsStatus = LightStatus::OFF;

	// Security features
	FeatureStatus tcStatus = FeatureStatus::ENABLED;
	FeatureStatus absStatus = FeatureStatus::ENABLED;
};
