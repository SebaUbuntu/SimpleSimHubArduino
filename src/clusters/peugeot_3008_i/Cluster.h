/*
 * SPDX-FileCopyrightText: Sebastiano Barezzi
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

/**
 * '09 Peugeot 3008 manager.
 */

#include "../../Cluster.h"
#include "../../types.h"
#include "commands.h"

void Cluster::setup() {
	mcp2515.reset();
	mcp2515.setBitrate(CAN_125KBPS, MCP_8MHZ);
	mcp2515.setNormalMode();
}

void Cluster::updateState(State &state) {
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
		state.dieselGlowPlugsLight,
		state.lowFuel,
		state.parkingBrakeLightStatus,
		state.anyDoorOpen,
		state.stopWarning,
		state.serviceWarning,
		state.tcStatus,
		state.pressBrakePedalLightStatus,
		state.parkAssistLightStatus,
		state.headlights,
		state.automaticParkingBrakeDisabled
	);

	sendFuelAndOil(
		state.fuelLevelPercentage,
		state.engineOilLevel
	);

	sendWarningLights(
		state.parkingBrakeLightStatus,
		state.engineOilLevel,
		state.highEngineCoolantTemperatureLightStatus,
		state.checkEngineLightStatus,
		state.tcStatus,
		state.absStatus,
		state.batteryNotChargingLightStatus,
		state.airbagWarning,
		state.automaticParkingBrakeIssue
	);

	sendTripMeter(
		state.currentTrip
	);

	sendLocalization(
		state.locale
	);
}
