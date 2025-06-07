/*
 * SPDX-FileCopyrightText: Sebastiano Barezzi
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <Arduino.h>
#include "src/Cluster.h"
#include "src/StateHolder.h"
#include "src/types.h"

// Uncomment which cluster you wanna control
//#include "src/clusters/citroen_c5_ii/Cluster.h"
//#include "src/clusters/peugeot_208_i/Cluster.h"
//#include "src/clusters/peugeot_3008_i/Cluster.h"
//#include "src/clusters/peugeot_multifunction_display/Cluster.h"
// End selection

class SHCustomProtocol {
private:

public:

	/*
	CUSTOM PROTOCOL CLASS
	SEE https://github.com/zegreatclan/SimHub/wiki/Custom-Arduino-hardware-support

	GENERAL RULES :
		- ALWAYS BACKUP THIS FILE, reinstalling/updating SimHub would overwrite it with the default version.
		- Read data AS FAST AS POSSIBLE in the read function
		- NEVER block the arduino (using delay for instance)
		- Make sure the data read in "read()" function READS ALL THE DATA from the serial port matching the custom protocol definition
		- Idle function is called hundreds of times per second, never use it for slow code, arduino performances would fall
		- If you use library suspending interrupts make sure to use it only in the "read" function when ALL data has been read from the serial port.
			It is the only interrupt safe place

	COMMON FUNCTIONS :
		- FlowSerialReadStringUntil('\n')
			Read the incoming data up to the end (\n) won't be included
		- FlowSerialReadStringUntil(';')
			Read the incoming data up to the separator (;) separator won't be included
		- FlowSerialDebugPrintLn(string)
			Send a debug message to simhub which will display in the log panel and log file (only use it when debugging, it would slow down arduino in run conditions)

	*/

	// Called when starting the arduino (setup method in main sketch)
	void setup() {
		Cluster::setup();

		State &state = StateHolder::getState();

		Cluster::updateState(state);

		state.dashboardLightingEnabled = true;
		state.dashboardBrightness = 0x0F;

		Cluster::updateState(state);
	}

	// Called when new data is coming from computer
	void read() {
		State &state = StateHolder::getState();

		String temperatureUnit = FlowSerialReadStringUntil(';');
		if (temperatureUnit == "Celcius") {
			state.locale.temperatureUnit = TemperatureUnit::CELSIUS;
		} else if (temperatureUnit == "Fahrenheit") {
			state.locale.temperatureUnit = TemperatureUnit::FAHRENHEIT;
		} else if (temperatureUnit == "Kelvin") {
			state.locale.temperatureUnit = TemperatureUnit::KELVIN;
		} else {
			FlowSerialDebugPrintLn("Unknown temperature unit: " + temperatureUnit);
			state.locale.temperatureUnit = TemperatureUnit::CELSIUS;
		}

		String pressureUnit = FlowSerialReadStringUntil(';');
		if (pressureUnit == "Bar") {
			state.locale.pressureUnit = PressureUnit::BAR;
		} else if (pressureUnit == "Kpa") {
			state.locale.pressureUnit = PressureUnit::KPA;
		} else if (pressureUnit == "Psi") {
			state.locale.pressureUnit = PressureUnit::PSI;
		} else {
			FlowSerialDebugPrintLn("Unknown pressure unit: " + pressureUnit);
			state.locale.pressureUnit = PressureUnit::BAR;
		}

		String volumeUnit = FlowSerialReadStringUntil(';');
		if (volumeUnit == "Liters") {
			state.locale.volumeUnit = VolumeUnit::LITERS;
		} else if (volumeUnit == "Gallons") {
			state.locale.volumeUnit = VolumeUnit::GALLONS;
		} else {
			FlowSerialDebugPrintLn("Unknown volume unit: " + volumeUnit);
			state.locale.volumeUnit = VolumeUnit::LITERS;
		}

		String speedUnit = FlowSerialReadStringUntil(';');
		if (speedUnit == "KMH") {
			state.locale.distanceUnit = DistanceUnit::KILOMETERS;
		} else if (speedUnit == "MPH") {
			state.locale.distanceUnit = DistanceUnit::MILES;
		} else {
			FlowSerialDebugPrintLn("Unknown speed unit: " + speedUnit);
			state.locale.distanceUnit = DistanceUnit::KILOMETERS;
		}

		if (state.locale.volumeUnit == VolumeUnit::GALLONS &&
				state.locale.distanceUnit == DistanceUnit::MILES) {
			state.locale.consumptionUnit = ConsumptionUnit::DISTANCE_PER_VOLUME;
		} else {
			state.locale.consumptionUnit = ConsumptionUnit::VOLUME_PER_DISTANCE;
		}

		state.ignitionState = FlowSerialReadStringUntil(';').toInt()
			? IgnitionState::ON
			: IgnitionState::OFF;

		state.engineStarted = !!FlowSerialReadStringUntil(';').toInt();

		state.rpm = FlowSerialReadStringUntil(';').toInt();

		state.speedKmh = FlowSerialReadStringUntil(';').toInt();

		state.engineCoolantTemperatureCelsius = FlowSerialReadStringUntil(';').toInt();

		state.ambientTemperatureCelsius = FlowSerialReadStringUntil(';').toInt();

		state.fuelLevelPercentage = FlowSerialReadStringUntil(';').toInt();

		state.odometerKm = FlowSerialReadStringUntil(';').toInt() / 1000;

		state.instantFuelConsumptionLP100Km = FlowSerialReadStringUntil(';').toFloat();

		state.currentTrip.averageSpeedKmh = FlowSerialReadStringUntil(';').toInt();

		state.currentTrip.distanceMeters = FlowSerialReadStringUntil(';').toInt();

		state.currentTrip.averageFuelConsumptionLP100Km = FlowSerialReadStringUntil(';').toFloat();

		state.lastTrip.averageFuelConsumptionLP100Km = FlowSerialReadStringUntil(';').toFloat();

		String gear = FlowSerialReadStringUntil(';');
		switch (gear[0]) {
			case 'P':
				state.gear = Gear::GEAR_P;
				break;
			case 'R':
				state.gear = Gear::GEAR_R;
				break;
			case 'N':
				state.gear = Gear::GEAR_N;
				break;
			case '1':
				state.gear = Gear::GEAR_1;
				break;
			case '2':
				state.gear = Gear::GEAR_2;
				break;
			case '3':
				state.gear = Gear::GEAR_3;
				break;
			case '4':
				state.gear = Gear::GEAR_4;
				break;
			case '5':
				state.gear = Gear::GEAR_5;
				break;
			case '6':
				state.gear = Gear::GEAR_6;
				break;
			default:
				state.gear = Gear::GEAR_HIDDEN;
				break;
		}

		state.headlights.leftIndicator = !!FlowSerialReadStringUntil(';').toInt();

		state.headlights.rightIndicator = !!FlowSerialReadStringUntil(';').toInt();

		// TODO
		state.tcStatus = FlowSerialReadStringUntil(';').toInt()
			? FeatureStatus::ACTIVE
			: FeatureStatus::ENABLED;

		state.absStatus = FlowSerialReadStringUntil(';').toInt()
			? FeatureStatus::ACTIVE
			: FeatureStatus::ENABLED;
	}

	// Called once per arduino loop, timing can't be predicted, 
	// but it's called between each command sent to the arduino
	void loop() {
		State &state = StateHolder::getState();
		Cluster::updateState(state);
	}

	// Called once between each byte read on arduino,
	// THIS IS A CRITICAL PATH :
	// AVOID ANY TIME CONSUMING ROUTINES !!!
	// PREFER READ OR LOOP METHOS AS MUCH AS POSSIBLE
	// AVOID ANY INTERRUPTS DISABLE (serial data would be lost!!!)
	void idle() {
	}
};
