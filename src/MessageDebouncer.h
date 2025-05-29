/*
 * SPDX-FileCopyrightText: Sebastiano Barezzi
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <Arduino.h>
#include <mcp2515.h>

class MessageDebouncer {
public:
	MessageDebouncer(uint8_t delayMs) : debounceDelayMs(delayMs) {}

	bool sendMessageWithDebounce(MCP2515 &mcp2515, struct can_frame &frame) {
		if (shouldUpdate()) {
			mcp2515.sendMessage(&frame);
			return true; // Message sent
		}
		return false; // Message not sent due to debounce
	}

private:
	uint8_t debounceDelayMs;
	bool sentAnyMessage = false;
	uint32_t lastPoll = 0;

	bool shouldUpdate() {
		if (!sentAnyMessage) {
			sentAnyMessage = true;
			lastPoll = millis();
			return true; // First message, always send
		}

		uint32_t currentTime = millis();
		if (currentTime - lastPoll > debounceDelayMs) {
			lastPoll = currentTime;
			return true;
		}

		return false;
	}
};
