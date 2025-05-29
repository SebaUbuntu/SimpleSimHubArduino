/*
 * SPDX-FileCopyrightText: Sebastiano Barezzi
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "MessageDebouncer.h"

#include <Arduino.h>

MessageDebouncer::MessageDebouncer(uint16_t delayMs) : debounceDelayMs(delayMs) {}

bool MessageDebouncer::shouldUpdate() {
	uint32_t currentTime = millis();

	if (!sentAnyMessage) [[unlikely]] {
		sentAnyMessage = true;
		lastPoll = currentTime;
		return true; // First message, always send
	}

	if (currentTime - lastPoll > debounceDelayMs) {
		lastPoll = currentTime;
		return true;
	}

	return false;
}
