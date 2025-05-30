/*
 * SPDX-FileCopyrightText: Sebastiano Barezzi
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

/**
 * Citroen C5 II cluster manager.
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
		state.leftIndicator,
		state.rightIndicator,
		state.gear
	);

	sendDashboardLights(
		state.passengerSeatBeltsStatus,
		state.dieselGlowPlugsLight,
		state.lowFuel,
		state.driverSeatBeltsStatus,
		state.parkingBrakeLightStatus,
		state.stopLight,
		state.serviceLight,
		state.tcStatus,
		state.sidelights,
		state.lowBeams,
		state.highBeams,
		state.frontFogLights,
		state.rearFogLights,
		state.rightIndicator,
		state.leftIndicator,
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
		state.highEngineCoolantTemperature,
		state.checkEngineLightStatus,
		state.sportAndWinterGearBlink,
		state.tcStatus,
		state.absStatus,
		state.batteryNotChargingLightStatus,
		state.automaticParkingBrakeIssue
	);

	sendTripMeter(
		state.tripMeterMeters
	);

	sendServiceLight(
		state.carServiceStatus,
		state.serviceCounterKm
	);
}
