/*
 * SPDX-FileCopyrightText: Sebastiano Barezzi
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

/**
 * Peugeot 208 cluster manager.
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
		state.leftIndicator,
		state.rightIndicator
	);

	sendFuelGauge();

	sendInformationalMessage();

	sendTripMeter(
		state.tripMeterMeters
	);

	sendDashboardLights(
		state.sidelights,
		state.lowBeams,
		state.highBeams,
		state.frontFogLights,
		state.rearFogLights,
		state.rightIndicator,
		state.leftIndicator,
		state.gear,
		state.blinkingGear,
		state.autoGearSelection,
		state.sportMode,
		state.pressBrakePedalLightStatus,
		state.secondPassengerAirbagDisabled,
		state.serviceLight,
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
		state.highEngineCoolantTemperature,
		state.automaticWipersEnabled,
		state.engineFault,
		state.batteryNotChargingLightStatus,
		state.sportAndWinterGearBlink,
		state.tcStatus,
		state.absStatus,
		state.startAndStopLightStatus,
		state.powerSteeringWarningLight,
		state.airbagWarningLight,
		state.lowBeamsWarningLight,
		state.waterInFuelFilterWarningLight
	);

	sendServiceLight(
		state.carServiceStatus,
		state.serviceCounterKm
	);
}
