/*
 * SPDX-FileCopyrightText: Sebastiano Barezzi
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

/**
 * Peugeot multifunction display manager.
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
		state.passengerSeatBeltsStatus,
		state.dieselGlowPlugsLight,
		state.lowFuel,
		state.driverSeatBeltsStatus,
		state.parkingBrakeLightStatus,
		state.stopWarning,
		state.serviceWarning,
		state.tcStatus,
		state.headlights,
		state.automaticParkingBrakeDisabled,
		state.rearLeftSeatBeltsStatus,
		state.rearCenterSeatBeltsStatus,
		state.rearRightSeatBeltsStatus,
		state.blinkingGear,
		state.gear,
		state.autoGearSelection,
		state.sportMode
	);

	sendOilOk(
		state.engineOilLevel
	);

	sendWarningLights(
		state.parkingBrakeLightStatus,
		state.engineOilLevel,
		state.highEngineCoolantTemperatureLightStatus,
		state.checkEngineLightStatus,
		state.sportAndWinterGearBlink,
		state.tcStatus,
		state.absStatus,
		state.batteryNotChargingLightStatus,
		state.automaticParkingBrakeIssue
	);

	sendInformationalMessage();

	sendTripComputerInfo(
		false, // tripButtonPushed
		state.instantFuelConsumptionLP100Km,
		state.remainingFuelDistanceKm,
		state.remainingTripDistanceKm
	);

	sendTrip2(state.lastTrip);

	sendTrip1(state.currentTrip);

	sendLocalization(
		state.locale
	);
}
