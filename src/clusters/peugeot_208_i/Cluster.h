/*
 * SPDX-FileCopyrightText: Sebastiano Barezzi
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

/**
 * Peugeot 208 I cluster manager.
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
		state.darkModeEnabled,
		state.dashboardBrightness
	);

	//sendOilOk();

	sendRpmAndSpeed(
		state.rpm,
		state.speedKmh
	);

	sendIgnitionAndCoolantTempAndOdometerAndAmbientTempAndReverseAndTurnSignals(
		state.ignitionState,
		state.engineCoolantTemperatureCelsius,
		state.odometerKm,
		state.ambientTemperatureCelsius,
		state.gear,
		state.headlights
	);

	sendFuelGauge();

	sendInformationalMessage();

	sendTripMeter(
		state.currentTrip
	);

	sendDashboardLights(
		state.headlights,
		state.gear,
		state.blinkingGear,
		state.autoGearSelection,
		state.sportMode,
		state.pressBrakePedalLightStatus,
		state.secondPassengerAirbagDisabled,
		state.serviceWarning,
		state.anyDoorOpen,
		state.driverSeatBeltsStatus,
		state.passengerSeatBeltsStatus,
		state.rearLeftSeatBeltsStatus,
		state.rearCenterSeatBeltsStatus,
		state.rearRightSeatBeltsStatus,
		state.lowFuel,
		state.pressClutchLightStatus,
		state.engineFault
	);

	sendOilOk(
		state.engineOilLevel
	); // TODO check whether order matters

	sendWarningLights(
		state.parkingBrakeLightStatus,
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

	sendServiceLight(
		state.carServiceStatus,
		state.serviceCounterKm
	);
}
