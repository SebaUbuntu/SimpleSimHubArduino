/*
 * SPDX-FileCopyrightText: Sebastiano Barezzi
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

/**
 * Peugeot 208 I cluster manager.
 */

#include "../../CanFuzzer.h"
#include "../../Cluster.h"
#include "../../types.h"
#include "commands.h"

// Fuzzing
const canid_t CanFuzzer::startId = 0x400;
const canid_t CanFuzzer::endId = 0x4FF; //CAN_SFF_MASK;
const canid_t CanFuzzer::skipIds[] = {
	0x036,
	0x0B6,
	0x0F6,
	0x128,
	0x161,
	0x168,
	0x1A8,
	0x3E7,
	0x3F6,
};
constexpr size_t CanFuzzer::skipIdsSize = sizeof(CanFuzzer::skipIds);

void Cluster::setup() {
	mcp2515.reset();
	mcp2515.setBitrate(CAN_125KBPS, MCP_8MHZ);
	mcp2515.setNormalMode();
}

void Cluster::updateState(State &state) {
	// Fuzzing
	//CanFuzzer::fuzzIds(mcp2515);

	sendIgnitionAndLighting(
		state.economyModeEnabled,
		state.dashboardLightingEnabled,
		state.dashboardBrightness,
		state.ignitionState
	);

    sendRpmAndSpeed(
		state.rpm,
		state.speedKmh
	);

	sendIgnitionAndCoolantTempAndOdometerAndAmbientTempAndReverseAndTurnSignals(
		state.ignitionState,
		state.darkModeEnabled,
		state.engineCoolantTemperatureCelsius,
		state.odometerKm,
		state.ambientTemperatureCelsius,
		state.headlights,
		state.gear
	);

	sendDashboardLights(
		state.headlights,
		state.blinkingGear,
		state.gear,
		state.autoGearSelection,
		state.sportMode,
		state.pressBrakePedalLightStatus,
		state.secondPassengerAirbagDisabled,
		state.stopWarning,
		state.serviceWarning,
		state.tcStatus,
		state.anyDoorOpen,
		state.driverSeatBeltsStatus,
		state.passengerSeatBeltsStatus,
		state.rearLeftSeatBeltsStatus,
		state.lowFuel,
		state.rearCenterSeatBeltsStatus,
		state.rearRightSeatBeltsStatus,
		state.engineFault,
		state.pressClutchLightStatus
	);

	sendFuelAndOil(
		state.engineOilTemperatureCelsius,
		state.fuelLevelPercentage,
		state.engineOilLevel
	);

	sendWarningLights(
		state.parkingBrakeLightStatus,
		state.engineOilPressureWarning,
		state.engineOilLevel,
		state.highEngineCoolantTemperatureLightStatus,
		state.automaticWipersEnabled,
		state.engineFault,
		state.batteryNotChargingLightStatus,
		state.sportAndWinterGearBlink,
		state.tcStatus,
		state.absStatus,
		state.startAndStopLightStatus,
		state.powerSteeringWarning,
		state.airbagWarning,
		state.lowBeamWarning,
		state.waterInFuelFilterWarning
	);

	sendTripMeter(
		state.currentTrip
	);

	sendServiceLight(
		state.carServiceStatus,
		state.serviceCounterKm
	);

	sendLocalization(
		state.locale
	);
}
